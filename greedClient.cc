#include"pubsub.h"
#include <muduo/base/ProcessInfo.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>

#include<iostream>
#include<stdio.h>

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

EventLoop* g_loop = NULL;
string g_cmd;
string g_topic;
string g_content;



void connection(PubsubClient* client)
{
  if (client->connected())
  {
    client->dealCmd(g_cmd,g_topic);
    client->stop();
  }
  else
  {
    g_loop->quit();
  }
}


int main(int argc, char* argv[])
{
    if(argc == 2)
    {
        string hostport = argv[1];
        size_t colon = hostport.find(':');
        if(colon != string::npos)
        {
            string hostip = hostport.substr(0, colon);
            uint16_t port = static_cast<uint16_t>(atoi(hostport.c_str()+colon+1));

            string name = ProcessInfo::username()+"@"+ProcessInfo::hostname();
            name += ":" + ProcessInfo::pidString();

            EventLoopThread loopThread;
            g_loop = loopThread.startLoop();
            PubsubClient client(g_loop, InetAddress(hostip, port), name);
            client.start();
            //client.dealCmd(g_cmd, g_topic);
            string line;
            while (true)
            {
                std::cin>>g_cmd>>g_topic;
                client.dealCmd(g_cmd, g_topic);
            }
            client.stop();
            CurrentThread::sleepUsec(1000*1000);

        }
    }
    else
    {
        printf("Usage: %s hub_ip:port cmd topic\n"
           "Read contents from stdin:\n"
           "  %s hub_ip:port cmd topic -\n", argv[0], argv[0]);
    }
    
    return 0;
}