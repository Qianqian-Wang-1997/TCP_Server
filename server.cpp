#include "server.h"
#include <pthread.h> //多线程

char server::msg[MAXPACKETSIZE]; //发送的msg
//vector<socketDesc *> server::Message;
bool server::isonline;
int server::last_closed;
int server::num_client;
vector<socketDesc *> server::newSockfd;
std::mutex server::kMutex;


void server::error(string errorMessage)
{
  cerr<< errorMessage << endl;
  exit(1);
}

/* 处理信息 发送回应  argv[0]=port */
void* server::Task(void *x)
{
  struct socketDesc *desc = (struct socketDesc *) x;
  pthread_detach(pthread_self());
  cerr << "open client:" << desc->id << endl;

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
                          newSockfd.end()); //删除已完成的客户端请求
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
          std::lock_guard<std::mutex> guard(kMutex); 
          newSockfd.push_back(desc);
        }
        usleep(700);
    }
    else 
    {
      cerr<<"接收失败！"<<endl;
      return 0;
    }
  }
  if(desc !=NULL)
  {
    cout << sizeof(desc) << endl;
    free(desc); //释放内存
  }
  cerr << "Thread done:" << this_thread::get_id() << endl;
  pthread_exit(NULL);
  return 0;
}

/* 开始接收client*/
int server::setupSocket(int port, vector<int> opts)
{
  //初始化sock状态
  isonline = false;
  last_closed = -1;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serv_addr, 0, sizeof(serv_addr));

  //AF_INET指IPv4地址，SOCK_STREAM指用TCP协议，protocal基本是0，当协议不确定时自动确定协议
  if(sockfd < 0)
  {
    cerr<< "socket building fail！"<<endl;
  }

  for (int i = 0; i < opts.size(); i++)
  {
    int opt = 1;
    if(setsockopt(sockfd, SOL_SOCKET, opts.at(i), (char *)&opts, sizeof(opt))<0) //保证绑定地址可以有多个socket
    {
      cerr << "Setsockopt error!" << endl;
      return -1;
    }
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int temp = ::bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if(listen(sockfd,5) < 0){
		cerr << "Errore listen" << endl;
		return -1;
	}
  
  num_client = 0; //开始接收来到的客户端
  isonline = true;
  return 0;
}

/* 接收客户端 */
void server::Accepted() 
{
  //初始化socket信息
  socklen_t newCliSize = sizeof(cli_addr);
  socketDesc * newCli = new socketDesc;
	cerr<<"wait for guest........."<<endl;
  newCli->socket = accept(sockfd, (struct sockaddr *)&cli_addr, &newCliSize);
  newCli->id = num_client;
  newCli->ip = inet_ntoa(cli_addr.sin_addr);
  newSockfd.push_back(newCli);
  cerr << "new client id:" << newSockfd[num_client]->id <<endl
       << "new client ip:" << newSockfd[num_client]->ip << endl;
  pthread_create(&ServThread[num_client], NULL, &Task, (void *)newSockfd[num_client]);
  isonline = true;
  num_client++;
}

 vector<socketDesc *> server:: GetMessage() //返回已己收到的消息
 {
   std::lock_guard<std::mutex> guard(kMutex);
   return newSockfd;
 }

void server::Send(string msg, int id)
{
  send(newSockfd[id]->socket, msg.c_str(), msg.length(), 0); //c_str() covert string to char*
}

void server::Clean(int id)
{
  newSockfd[id]->message = ""; //clear nemory
  memset(msg, 0, MAXPACKETSIZE);//初始化msg
}

void server::Detach(int id) //清初socket之前传来的内容 准备接收下一次收到的消息
{
  close(newSockfd[id]->socket);
  newSockfd[id]->ip = "";
  newSockfd[id]->id = -1;
  newSockfd[id]->message = "";
}

bool server::isOnline() //判断client是否在线 
{
  return isonline;
}
string server::GetIpAddr(int id) //返回处理的socket的ip，便于查看
{
  return newSockfd[id]->ip;
}

void server::closed()   //关闭socket 
{
  cout << "close socket" << endl;
  close(sockfd);
}

int server::GetLastClosed() //返回最后处理的socket id
{
  return last_closed;
}
