# ifndef _PUB_SUB_H_
# define _PUB_SUB_H_

#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include "ProtobufCodec.h"
#include "dispatcher.h"
#include "info.pb.h"

namespace pubsub
{
using muduo::string;
typedef std::shared_ptr<pubsub::SystemAns> SystemAnsPtr;
typedef std::shared_ptr<pubsub::ShowInfo> ShowRoomInfoPtr;

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
    //int dealCmd(const string& cmd, const string& topic);
    int dealCmd(char* s);
    void printError(const string& str);
    void showRoomInfo(const ShowRoomInfoPtr& message);
    bool subscribe(const string& topic);
    void unsubscribe(const string& topic);
    bool publish(const string& topic, const string& content);
    bool createRoom(const string& topic);
    bool getinRoom(const string& topic);
    void setConnectionCallback(const ConnectionCallback& cb){
        connectionCallback_ = cb;
    }
    //void onUnknownMessage(const  muduo::net::TcpConnectionPtr& conn, const MessagePtr& message, muduo::Timestamp);
    void onSystemAns(const muduo::net::TcpConnectionPtr& conn, const SystemAnsPtr& message, muduo::Timestamp);
    void onRoomInfo(const muduo::net::TcpConnectionPtr& conn, const ShowRoomInfoPtr& message, muduo::Timestamp);

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
    ProtobufCodec codec_;
    ProtobufDispatcher dispatcher_;
    ConnectionCallback connectionCallback_;
    SubscribeCallback subscribeCallback_;
    CheckerBoardCallback checkerBoardCallback_;
    google::protobuf::Message* messageToSend_;

};// class pubsub
}// namespace pubsub


# endif //_PUB_SUB_H_