#include<iostream>
#include"curses.h"

#include"pubsub.h"
#include"codec.h"
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

#define ME '@'
#define RIVAL '#'


void onUnknownMessage(const TcpConnectionPtr& conn, const MessagePtr& message, Timestamp)
{
    LOG_INFO << "onUnknownMessage: " << message->GetTypeName() << message->DebugString();
}
PubsubClient::PubsubClient(EventLoop* loop,
                           const InetAddress& hubAddr,
                           const string& name)
    : client_(loop, hubAddr, name)
      ,dispatcher_(std::bind(&onUnknownMessage,_1,_2,_3))
      ,codec_(std::bind(&ProtobufDispatcher::onProtobufMessage,&dispatcher_,_1,_2,_3))
{
    dispatcher_.registerMessageCallback<pubsub::SystemAns>(
        std::bind(&PubsubClient::onSystemAns, this, _1, _2, _3));
    dispatcher_.registerMessageCallback<pubsub::ShowInfo>(std::bind(&PubsubClient::onRoomInfo,this,_1,_2,_3));
    dispatcher_.registerMessageCallback<pubsub::GridInfo>(std::bind(&PubsubClient::onGridInfo,this,_1,_2,_3));
    client_.setConnectionCallback(
        std::bind(&PubsubClient::onConnection, this, _1));
    client_.setMessageCallback(
        std::bind(&ProtobufCodec::onMessage,&codec_,_1,_2,_3));
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_CYAN, COLOR_BLACK);	
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);

        attribs[0] = COLOR_PAIR(1);
        attribs[1] = COLOR_PAIR(2);
        attribs[2] = COLOR_PAIR(3);
        attribs[3] = COLOR_PAIR(4);
        attribs[4] = COLOR_PAIR(5);
        attribs[5] = COLOR_PAIR(1) | A_BOLD;
        attribs[6] = COLOR_PAIR(2) | A_BOLD;
        attribs[7] = COLOR_PAIR(3) | A_BOLD;
        attribs[8] = COLOR_PAIR(4) | A_BOLD;
    }
}


void PubsubClient::printError(const string& str)
{
  init_pair(1,COLOR_RED,COLOR_BLACK);
  attron(COLOR_PAIR(1));
  const char* s = str.c_str();
  mvprintw(22, 0, s);
  move(LINES-1,0);
  attroff(COLOR_PAIR(1));
  refresh();
}
void PubsubClient::onSystemAns(const TcpConnectionPtr& conn, const SystemAnsPtr& message, Timestamp)
{
    printError(message->content());
}

void PubsubClient::showRoomInfo(const ShowRoomInfoPtr& message)
{
    move(0,0);
    printw("%s         |%s         |%s      |%s","Room","Owner","Irval","CreatedTime");
    for(int i = 0;i < message->ri_size();++i)
    {
        const pubsub::RoomInfo ri = message->ri(i);
        if(ri.name() == "utc_time"){
            continue;
        }
        move(i+1,0);
        int colon =  ri.owner().find("#");
        string ownername = ri.owner().substr(colon + 1,ri.owner().size() - 1);
        colon =  ri.rival().find("#");
        string rivalname = ri.rival().substr(colon + 1,ri.owner().size() - 1);
        colon = ri.createdtime().find(".");
        string time = ri.createdtime().substr(0,colon);
        printw("%-10s   |%-10s    |%-7s    |%-10s",ri.name().c_str(),ownername.c_str(),rivalname.c_str(),time.c_str());
    }
    move(LINES - 1,0);
    refresh();
}
void PubsubClient::onGridInfo(const TcpConnectionPtr& conn, const GridInfoPtr& message, Timestamp)
{
    string grid = message->grid();
    int c = 0;
    attron(A_BOLD);
    for(int i = 0;i < 19;++i){
        for(int j = 0;j < 80;++j,++c){
            if(grid[c] - '0' == 0){
                continue;
            }
            attron(attribs[grid[c] - '0']);
            mvaddch(i+3,j, grid[c]);
            attroff(attribs[grid[c] - '0']);
        }
    }
    attroff(A_BOLD);
    //addstr(grid.c_str());
    pubsub::Pos o = message->o();
    attron(A_BLINK);
    mvaddch(o.x() + 3, o.y(), ME);
    attroff(A_BLINK);
    pubsub::Pos r = message->r();
    attron(A_REVERSE);
    mvaddch(r.x() + 3,r.y(),RIVAL);
    attroff(A_REVERSE);
    move(LINES - 1,0);
    refresh();
}
void PubsubClient::onRoomInfo(const TcpConnectionPtr& conn, const ShowRoomInfoPtr& message, Timestamp)
{
    showRoomInfo(message);
}

void PubsubClient::start()
{
    client_.connect();
}

void PubsubClient::stop()
{
    client_.disconnect();
}

bool PubsubClient::connected() const
{
    return conn_ && conn_->connected();
}
int PubsubClient::dealCmd(char* s)
{
    string str = s;
    int colon = str.find(" ");
    string cmd;
    string topic;
    if (colon == string::npos)
    {
        if(str == "show" || str == "quit")
        {
            cmd = str;
            topic = "";
        }
    }
    else
    {
        cmd = str.substr(0,colon);
        topic = str.c_str() + colon + 1;
    }
    pubsub::SystemQuery query;
    query.set_cmd(cmd);
    query.set_topic(topic);
    messageToSend_ = &query;
    codec_.send(conn_, *messageToSend_);
    return 0;
}
void printCheckerboard()
{
    std::cout<<"checkerboard info"<<std::endl;
}

void PubsubClient::onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn_ = conn;
    }
    else
    {
        conn_.reset();
    }
    if(connectionCallback_)
    {
        connectionCallback_(this);
    }
}


bool PubsubClient::send(const string& message)
{
  bool succeed = false;
  if (conn_ && conn_->connected())
  {
    conn_->send(message);
    succeed = true;
  }
  return succeed;
}