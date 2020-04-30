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
  }
  return true; //消息发送成功
}

/* 返回收到的信息 */
string client::receive(int size)
{
  char temp[size];
  memset(&temp[0], 0, sizeof(temp));
  if(recv(sock, temp, size, 0) < 0)
  {
    cerr << "receive fail!" << endl;
    return NULL;
  }

  string reply; 
  reply = temp;
  return reply; //返回server接收成功信息
}

void client::Close()
{
  close(sock);
}