#include "pubsub.h"
#include <muduo/base/ProcessInfo.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/base/Logging.h>


//#include<iostream>
#include<stdio.h>


#include<curses.h>

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

EventLoop* g_loop = NULL;
string g_cmd;
string g_topic;
string g_content;


class Board{
public:
  Board(const PubsubClient* client)
    : height_(19),width_(80),
      client_(client)
  {

  }
  void mainBoard(){

  }
private:
  int height_;
  int width_;
  const PubsubClient* client_;
};

void connection(PubsubClient* client)
{
  if (client->connected())
  {
    //client->dealCmd(g_cmd,g_topic);
    client->stop();
  }
  else
  {
    g_loop->quit();
  }
}


int main(int argc, char* argv[])
{
    initscr();
    start_color();
    //nodelay(stdscr,true);
    
    Logger::setLogLevel(Logger::ERROR);
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
            InetAddress InetAddress(hostip, port);
            PubsubClient client(g_loop, InetAddress, name);
            client.start();
            char* s = new char[50];
            string line;
            int val = 1;
            while (true)
            {
                echo();
                move(LINES - 1, 0);
                switch (getch())
                {
                case ':':
                  getstr(s);
                  clrtoeol();
                  client.dealCmd(s);
                  break;
                
                case 'g':
                case 'G':
                  move(LINES - 1,0);
                  clrtoeol();
                  if(client.gameModel)
                  {
                    client.printSystemInfo("You are in game model.");
                    curs_set(0);
                    noecho();
                    while((val = client.tunnel(getch())) > 0)
                      continue;
                  }else
                  {
                    client.printSystemInfo("You are not in game model.");
                    break;
                  }
                  curs_set(1);
                  client.printSystemInfo("You are in cmd model.");
                  
                default:
                  move(LINES - 1,0);
                  clrtoeol();
                  break;
                }
                // }else
                // {
                //   move(LINES-1,0);
                //   echo();
                //   getstr(s);
                //   if(s > 0)
                //   client.dealCmd(s);
                //   //move(LINES -2,0);
                //   //clrtoeol();
                // }
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
    getch();
    endwin();
    return 0;
}