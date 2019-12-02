# ifndef _PUB_SUB_H_
# define _PUB_SUB_H_

#include <muduo/net/TcpClient.h>

namespace pubsub
{
using muduo::string;

class PubsubClient{
public:
    typedef std::function<void (PubsubClient*)> ConnectionCallback;
    typedef std::function<void (const string& topic,
                                const string& content,
                                muduo::Timestamp)> SubscribeCallback;

    PubsubClient(muduo::net::EventLoop* loop,
                 const muduo::net::InetAddress& hubAddr,
                 const string& name);
    
    void start();
    void stop();
    bool connected() const;

    bool subscribe(const string& topic, const SubscribeCallback& cb);
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

};// class pubsub
}// namespace pubsub


# endif //_PUB_SUB_H_