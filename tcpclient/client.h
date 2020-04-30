#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h>

using namespace std;

class client
{
  public:
    client();
    bool setuplink(string address, int port);
    bool sendMsg(string msg);
    string receive(int size = 4096);
    void Close();

  private:
    int sock;
    string address;
    int port;
    struct sockaddr_in server;
};

#endif