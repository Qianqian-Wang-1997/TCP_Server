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
  /*if(argc != 4)
  {
    cerr << "client参数错误！" << endl;
    return 0;
  }
  */

  signal(SIGINT, signal_exit); //退出程序

  char *addr = argv[1];
  int port = atoi(argv[2]);
  tcp.setuplink(addr, port);
  string msg = "";

  while (cin >> msg)
  {
    bool sendtoserver = tcp.sendMsg(msg);
    //cout << "sendtoserver:" << sendtoserver<< endl; ///
    string recvMsg = tcp.receive();
    if(recvMsg!="")
    {
      cout << recvMsg << endl;
      recvMsg = "";
    }
    
    cout << "msg1:::" << msg << endl;
    msg = "";
    cout << "msg2:::" << msg << endl;
    sleep(1); //循环发送
  }
  return 0;
}