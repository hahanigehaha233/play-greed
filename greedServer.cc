#include<iostream>

#include"codec.h"

#include<muduo/net/EventLoop.h>
#include<muduo/base/Logging.h>
#include<muduo/net/TcpServer.h>

#include<mutex>
#include<map>
#include<set>
#include<stdio.h>
#include<memory>

using namespace muduo;
using namespace::net;

namespace pubsub
{
typedef std::set<string> ConnectionSubscription;


class Topic:public muduo::copyable
{
public:
    Topic(const string &topic,const string &onwer)
        : topic_(topic),onwer_(onwer)
    {
    }

    void add(const TcpConnectionPtr &conn)
    {
        audiences_.insert(conn);
        if (lastPubTime_.valid())
        {
            conn->send(makeMessage());
        }
    }

    void remove(const TcpConnectionPtr &conn)
    {
        audiences_.erase(conn);
    }

    void publish(const string &content, Timestamp time)
    {
        content_ = content;
        lastPubTime_ = time;
        string message = makeMessage();
        for (std::set<TcpConnectionPtr>::iterator it = audiences_.begin();
             it != audiences_.end();
             ++it)
        {
            (*it)->send(message);
        }
    }
    const string getOwner()
    {
        return onwer_;
    }
    const string getRival()
    {
        return rival_;
    }
    const string getTopicName()
    {
        return topic_;
    }
    bool setRival(const string &rival)
    {
        if(rival_ == "")
        {
            if(mtx.try_lock())
            {
                if(rival == "")
                {
                    mtx.unlock();
                    return false;
                } 
                rival_ = rival;
                mtx.unlock();
                return true;
            }
        }
        else
        {
            return false;
        }
    }

private:
    string makeMessage()
    {
        return "info "  + content_ + "\r\n" ;
    }
    string topic_;
    string content_;
    Timestamp lastPubTime_;
    std::set<TcpConnectionPtr> audiences_;
    string onwer_;
    string rival_;
    std::mutex mtx;
};
enum clientType {cTourist, cOwner, cRival, cSubscribe};
class ClientInfo
{
public:
    ClientInfo(const TcpConnectionPtr& conn)
    :   ct_(cTourist),
        conn_(conn),
        name_(conn->name())
    {
    }
    int getclientType()
    {
        return ct_;
    }
    void setclientType(const clientType& ct,const string& topic)
    {
        ct_ = ct;
        topic_ = topic;
    }

    const string& getTopic()
    {
        return topic_;
    }


private:
    TcpConnectionPtr conn_;
    clientType ct_;
    string topic_;
    string name_;
};

class GreedServer: noncopyable
{
public:
    GreedServer(muduo::net::EventLoop *loop,
                const muduo::net::InetAddress &listenAddr)
        : loop_(loop),
          server_(loop, listenAddr, "GreedServer")
    {
        server_.setConnectionCallback(
            std::bind(&GreedServer::onConnection, this, _1));
        server_.setMessageCallback(
            std::bind(&GreedServer::onMessage, this, _1, _2, _3));
        loop_->runEvery(1.0, std::bind(&GreedServer::timePublish, this));
        createTopic("utc_time","root");
    }

    void start()
    {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            conn->setContext(ConnectionSubscription());
            createUser(conn);
        }
        else
        {
            removeUser(conn);
        }
    }

    // void removeTopic(const TcpConnectionPtr& conn)
    // {
    //     const ConnectionSubscription &connSub = boost::any_cast<const ConnectionSubscription &>(conn->getContext());
    //         // subtle: doUnsubscribe will erase *it, so increase before calling.
    //     for (ConnectionSubscription::const_iterator it = connSub.begin();
    //         it != connSub.end();)
    //     {
    //         if(conn->name() == getTopic(*it).getOwner())
    //         {
    //             Timestamp now = Timestamp::now();
    //             doPublish(conn->name(),*it,"owner delete room, this room will disaper",now);
    //             topics_.erase(*it);
    //         }
    //         else
    //         {
    //             doUnsubscribe(conn, *it);
    //         }
    //         it++;
    //     }
    // }
    void removeUser(const TcpConnectionPtr& conn)
    {
        std::map<string,std::shared_ptr<ClientInfo>>::iterator it =  users_.find(conn->name());
        ClientInfo user = (*it->second);
        if((user.getclientType() == cOwner))
        {
            Timestamp now = Timestamp::now();
            doPublish(conn->name(),user.getTopic(),"owner delete room, this room will disaper",now);
            //后续工作
            topics_.erase(conn->name());
            return;
        }
        else if(user.getclientType() == cRival)
        {
            getTopic(user.getTopic()).setRival("");
            return;
        }
        else if(user.getclientType() == cSubscribe)
        {
            doUnsubscribe(conn, user.getTopic());
        }
        users_.erase(conn->name());
    }
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp receiveTime)
    {
        ParseResult result = kSuccess;
        while (result == kSuccess)
        {
            string cmd;
            string topic;
            string content;
            result = parseMessage(buf, &cmd, &topic, &content);
            if (result == kSuccess)
            {
                std::map<string,std::shared_ptr<ClientInfo>>::iterator it =  users_.find(conn->name());
                ClientInfo& user = (*it->second);
                if (cmd == "new")
                {
                    if(user.getclientType() == cTourist && createTopic(topic,conn->name()))
                    {
                        user.setclientType(cOwner,topic);
                        LOG_INFO << conn->name() << " create " << topic;
                        doSubscribe(conn, topic);
                        LOG_INFO << conn->name() << " subscribes " << topic;
                    }
                    else
                    {
                        string message = "info same name or you have a room already.\r\n";
                        conn->send(message);
                        result = kError;
                    }
                }
                else if (cmd == "getin")
                {
                    if(user.getclientType() == cTourist && setRival(topic))
                    {
                        user.setclientType(cRival,topic);
                        doSubscribe(conn, topic);
                        LOG_INFO << conn->name() << " getin " << topic;
                        LOG_INFO << conn->name() << " subscribes " << topic;
                    }
                    else
                    {
                        string message = "info can't gein \r\n";
                        conn->send(message);
                        result = kError;
                    }
                }
                else if (cmd == "msg")
                {
                    doPublish(conn->name(), topic, content, receiveTime);
                }
                else if (cmd == "unsub")
                {
                    doUnsubscribe(conn, topic);
                }
                else
                {
                    conn->shutdown();
                    result = kError;
                }
            }
            else if (result == kError)
            {
                conn->shutdown();
            }
        }
    }

    void timePublish()
    {
        Timestamp now = Timestamp::now();
        doPublish("internal", "utc_time", now.toFormattedString(), now);
    }

    void doSubscribe(const TcpConnectionPtr &conn,
                     const string &topic)
    {
        ConnectionSubscription *connSub = boost::any_cast<ConnectionSubscription>(conn->getMutableContext());

        connSub->insert(topic);
        getTopic(topic).add(conn);
    }

    void doUnsubscribe(const TcpConnectionPtr &conn,
                       const string &topic)
    {
        LOG_INFO << conn->name() << " unsubscribes " << topic;
        getTopic(topic).remove(conn);
        // topic could be the one to be destroyed, so don't use it after erasing.
        ConnectionSubscription *connSub = boost::any_cast<ConnectionSubscription>(conn->getMutableContext());
        connSub->erase(topic);
    }

    void doPublish(const string &source,
                   const string &topic,
                   const string &content,
                   Timestamp time)
    {
        getTopic(topic).publish(content, time);
    }
    bool hasnoTopic(const string &topic)
    {
        std::map<string, std::shared_ptr<Topic>>::iterator it = topics_.find(topic);
        return (it == topics_.end());
    }
    bool createTopic(const string &topic,const string& onwer)
    {
        std::pair<std::map<string, std::shared_ptr<Topic>>::iterator, bool> res = topics_.insert(std::pair<string, std::shared_ptr<Topic>>(topic, std::make_shared<Topic>(topic,onwer)));
        return res.second;
    }
    bool createUser(const TcpConnectionPtr &conn)
    {
        std::pair<std::map<string,std::shared_ptr<ClientInfo>>::iterator,bool> res = users_.insert(std::pair<string, std::shared_ptr<ClientInfo>>(conn->name(),std::make_shared<ClientInfo>(conn)));
        return res.second;
    }
    bool setRival(const string &topic)
    {
        return getTopic(topic).setRival(topic);
    }

    Topic &getTopic(const string &topic)
    {
        std::map<string, std::shared_ptr<Topic>>::iterator it = topics_.find(topic);
        return (*it->second);
    }

    EventLoop *loop_;
    TcpServer server_;
    std::map<string, std::shared_ptr<Topic>> topics_;
    std::map<string,std::shared_ptr<ClientInfo>> users_;
};

}// namespace pubsub

int main(int argc, char* argv[])
{
    if(argc > 1)
    {
        uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
        EventLoop loop;
        pubsub::GreedServer server(&loop, InetAddress(port));
        server.start();
        loop.loop();
    }
    else
    {
        printf("Usage: %s pubsub_port [inspect_port]\n", argv[0]);
    }
    return 0;
}