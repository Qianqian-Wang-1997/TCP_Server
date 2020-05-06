#include "client.h"
using namespace std;

/* 数据初始化 */
client::client()
{
  sock = -1;
  port = 0;
  address = "";
}

bool client::setuplink(string address, int port)
{
  if(sock == -1)
  {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
      cerr << "create socket fail!" << endl;
      return false;
    }
  }
  if((in_addr_t)inet_addr(address.c_str())== -1) //获取主机名字和地址信息
  {
    struct hostent *host = gethostbyname(address.c_str()); //获取域名信息
    if(!host)
    {
      cerr << "getiing IP address fail! " << endl;
      exit(0);
    }
    struct in_addr **host_addr_list = (struct in_addr **) host->h_addr_list; //获取IP地址信息
    for (int i = 0; host_addr_list[i]; i++) //传递host地址
    {
      server.sin_addr = *host_addr_list[i];
    }
  }
  else
  {
    server.sin_addr.s_addr = inet_addr(address.c_str());
  }
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  if(connect(sock, (struct sockaddr*)&server, sizeof(server))<0)
  {
    cerr << "connect fail!" << endl;
    return false;
  }
  return true;
}

 /* 向服务器发送信息 查看信息是否发送成功*/
bool client::sendMsg(string msg)
{
  if(sock>0)
  {
    if(send(sock, msg.c_str(), strlen(msg.c_str()), 0 ) < 0)
    {
      cerr << "Send fail!" << endl;
      return false;
    }
  }
  else
  {
    return false;
    cout << "false!!" << endl;
  }
  cout << "true！！！" << endl;
  return true; //消息发送成功
}

/* 返回收到的信息(时间戳) */
string client::receive(int size)
{
  cout << "revceive 1"<< endl;
  char temp[size];
  memset(&temp[0], 0, sizeof(temp));
  cout << "revceive 2"<< endl;

  int ack = recv(sock, temp, size, 0); // 第一次返回收到字节数 8 即时间戳长度
  cout <<"ack:::"<< ack <<endl; //debug

  if (ack < 0) //sleep后在这里消失
  {
    cerr << "receive fail!" << endl;
    return 0;
  }
  string reply;
  reply = temp;
  cout << "reply::: " << reply << endl; //debug 
  return reply; //返回server接收成功信息(时间戳)
}

void client::Close()
{
  close(sock);
}
