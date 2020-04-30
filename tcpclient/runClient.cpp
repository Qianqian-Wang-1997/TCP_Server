#include "client.h"
#include <iostream>
#include <signal.h>

client tcp;

void signal_exit(int x)
{
  tcp.Close();
  exit(0);
}

int main(int argc, char **argv)
{
  if(argc != 4)
  {
    cerr << "client参数错误！" << endl;
    return 0;
  }

  signal(SIGINT, signal_exit); //退出程序

  char *addr = argv[1];
  int port = atoi(argv[2]);
  tcp.setuplink(addr, port);
  while(1)
  {
    tcp.sendMsg(argv[3]);
    string recvMsg = tcp.receive();
    if(recvMsg!="")
    {
      cout << recvMsg << endl;
    }
    sleep(5);
  }
  return 0;
}