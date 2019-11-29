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
        std::bind(PubsubClient::onConnection, this, _1));
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
    return conn_ && conn->connected();
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
        connectionCallback(this);
    }
}

void PubsubClient::onMessage(const TcpConnectionPtr& conn,
                             Buffer* buf,
                             Timestamp receiveTime)
{
    ParseResult result = KSucces;
    while(result == KSucces)
    {
        string cmd;
        string topic;
        string content;
        result = parseMessage(buf, &cmd, &topic, &content);
        if(result == KSucces)
        {
            if(cmd == "msg" && subscribeCallback_)
            {
                subscribeCallback_(topic, content, receiveTime);
            }
        }
        else if (result == KError)
        {
            conn->shutdown();
        }
    }
}

bool PubSubClient::send(const string& message)
{
  bool succeed = false;
  if (conn_ && conn_->connected())
  {
    conn_->send(message);
    succeed = true;
  }
  return succeed;
}