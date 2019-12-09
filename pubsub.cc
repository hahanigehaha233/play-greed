#include<iostream>

#include"pubsub.h"
#include"codec.h"

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

PubsubClient::PubsubClient(EventLoop* loop,
                           const InetAddress& hubAddr,
                           const string& name)
    : client_(loop, hubAddr, name)
{
    client_.setConnectionCallback(
        std::bind(&PubsubClient::onConnection, this, _1));
    client_.setMessageCallback(
        std::bind(&PubsubClient::onMessage,this,_1,_2,_3));
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
int PubsubClient::dealCmd(const string& cmd,const string& topic)
{
    if(cmd == "new")
    {
        createRoom(topic);
    }
    else if(cmd == "getin")
    {
        getinRoom(topic);
    }
    else if(cmd == "watch")
    {
        subscribe(topic);
    }
    else if(cmd == "msg")
    {
        messageTest(topic);
    }
    else
    {
        std::cout<<"error cmd"<<std::endl;
    }
    return 0;
}
void printCheckerboard()
{
    std::cout<<"checkerboard info"<<std::endl;
}
bool PubsubClient::subscribe(const string& topic)
{
    checkerBoardCallback_ = std::bind(&printCheckerboard);
    string message = "sub " + topic + "\r\n";
    return send(message);
}

void PubsubClient::unsubscribe(const string& topic)
{
    string message = "unsub " + topic + "\r\n";
    send(message);
}
bool PubsubClient::publish(const string& topic, const string& content)
{
    string message = "pub " + topic + "\r\n" + content + "\r\n";
    return send(message);
}
bool PubsubClient::createRoom(const string& topic)
{
    string message = "new "+topic + "\r\n";
    send(message);
}
bool PubsubClient::messageTest(const string& topic)
{
    string message = "msg "+ topic + "\r\n" + "messagetest" + "\r\n";
    return send(message);
}
bool PubsubClient::getinRoom(const string& topic)
{
    string message = "getin "+topic + "\r\n";
    return send(message);
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

void PubsubClient::onMessage(const TcpConnectionPtr& conn,
                             Buffer* buf,
                             Timestamp receiveTime)
{
    ParseResult result = kSuccess;
    while(result == kSuccess)
    {
        string cmd;
        string topic;
        string content;
        result = parseMessage(buf, &cmd, &topic, &content);
        if(result == kSuccess)
        {
            if(cmd == "msg" && checkerBoardCallback_)
            {
                checkerBoardCallback_();
            }
            else if(cmd == "info")
            {
                std::cout<<content<<std::endl;
            }
            else if(cmd == "getin")
            {
                std::cout<<content<<std::endl;
            }
        }
        else if (result == kError)
        {
            conn->shutdown();
        }
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