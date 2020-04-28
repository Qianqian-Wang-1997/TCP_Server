#include "server.h"
#include <pthread.h> //多线程

char server::msg[MAXPACKETSIZE]; //发送的msg
bool server::isonline; 
int server::last_closed;
int server::num_client;
vector<socketDesc *> server::newSockfd;

void server::error(string errorMessage)
{
  cerr<< errorMessage << endl;
  exit(1);
}

/* 处理信息 发送回应  argv[0]=port */
void* server::Task(void *arg)
{
  struct socketDesc *desc = (struct socketDesc *)arg;
  pthread_detach(pthread_self());//获取现线程
  cerr << "open client:" << desc->id;

  while(1)
  {
    int n;
    n = recv(desc->socket, msg, MAXPACKETSIZE, 0);
    if(n!=-1) //接收成功
    {
        if(n == 0) //消息为空 
        {
          isonline = false;
          cerr << "close client:" << desc->id << endl;
          last_closed = desc->id;
          close(desc->socket);

          int id = desc->id;
          newSockfd.erase(remove_if(newSockfd.begin(), newSockfd.end(), [id](socketDesc *device) { return device->id == id; }),
                          newSockfd.end()); //删除已完成的客户端请求 报错报错
          if(num_client>0)
          {
            num_client--;
          }
          break;
        }
        else
        {
          msg[n] = 0;
          desc->message = string(msg);
          std::lock_guard<std::mutex> guard(mutex); ///存疑 原为Kmutex
          Message.push_back(desc);
        }

    } else 
    {
      cerr<<"接收失败！"<<endl;
    }
  }
  if(desc !=NULL)
  {
    free(desc);//释放内存
  }
  cerr << "Thread done:" << this_thread::get_id() << endl;
  pthread_exit(NULL);
  return 0;
}

/* 开始接收client*/
int server::setupSocket(int port, vector<int> opts)
{

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serv_addr, 0, sizeof(serv_addr));

  //AF_INET指IPv4地址，SOCK_STREAM指用TCP协议，protocal基本是0，当协议不确定时自动确定协议
  if(sockfd < 0){
    cerr<< "socket building fail！"<<endl;
  }
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 

  int iresult = 0;
  iresult = ::bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
  if (iresult<0)
  {
    error("bind fail!");
  }
  num_client = 0; //开始接收来到的客户端
  isonline = true;
  return 0;
}

/* 接收客户端 */
void server::Accepted() 
{
  socklen_t newCliSize = sizeof(cli_addr);
  socketDesc * newCli = new socketDesc;
  newCli->socket = accept(sockfd, (struct sockaddr *)&cli_addr, &newCliSize);
  newCli->id = num_client;
  newCli->ip = inet_ntoa(cli_addr.sin_addr);
  newSockfd.push_back(newCli);
  cerr << "new client id:" << newSockfd[num_client]->id 
       << "new client ip:" << newSockfd[num_client]->ip << endl;
  pthread_create(&ServThread[num_client], NULL, &Task, (void *)newSockfd[num_client]);
  isonline = true;
  num_client++;
}

 vector<socketDesc *> server:: GetMessage() //返回已己收到的消息
 {
   std::lock_guard<std::mutex> guard(mutex);
   return Message;
 }

void server::Send(string msg, int id)
{
  send(newSockfd[id]->socket, msg.c_str(), msg.length(), 0); //c_str() covert string to char*
}

void server::Clean(int id)
{
  Message[id]->message = ""; //clear nemory
  memset(msg, 0, MAXPACKETSIZE);//初始化msg
}

void server::Detach(int id)
{
  close(newSockfd[id]->socket);
  newSockfd[id]->ip = "";
  newSockfd[id]->id = -1;
  newSockfd[id]->message = "";
}
