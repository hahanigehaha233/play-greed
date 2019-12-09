# ifndef _PUB_SUB_H_
# define _PUB_SUB_H_

#include <muduo/net/TcpClient.h>
#include "clientSingle.h"

namespace pubsub
{
using muduo::string;

class PubsubClient{
public:
    typedef std::function<void (PubsubClient*)> ConnectionCallback;
    typedef std::function<void (const string& topic,
                                const string& content,
                                muduo::Timestamp)> SubscribeCallback;
    typedef std::function<void ()> CheckerBoardCallback;

    PubsubClient(muduo::net::EventLoop* loop,
                 const muduo::net::InetAddress& hubAddr,
                 const string& name);
    
    void start();
    void stop();
    bool connected() const;

    bool messageTest(const string& topic);
    int dealCmd(const string& cmd, const string& topic);
    bool subscribe(const string& topic);
    void unsubscribe(const string& topic);
    bool publish(const string& topic, const string& content);
    bool createRoom(const string& topic);
    bool getinRoom(const string& topic);
    void setConnectionCallback(const ConnectionCallback& cb){
        connectionCallback_ = cb;
    }

private:
    /// conn change
    void onConnection(const muduo::net::TcpConnectionPtr& conn); 
    /// get message
    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp receiveTime);
    bool send(const string& message);



private:
    muduo::net::TcpClient client_;
    muduo::net::TcpConnectionPtr conn_;
    ConnectionCallback connectionCallback_;
    SubscribeCallback subscribeCallback_;
    CheckerBoardCallback checkerBoardCallback_;

};// class pubsub
}// namespace pubsub


# endif //_PUB_SUB_H_