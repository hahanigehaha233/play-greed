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
    void setclientTourist()
    {
        ct_ = cTourist;
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
    TcpConnectionPtr conn_;

private:
    clientType ct_;
    string topic_;
    string name_;
};

enum publishType{pInfo, pMsg};

class Topic:public muduo::copyable
{
public:
    Topic(const string &topic,const string &onwer)
        : topic_(topic),onwer_(onwer)
    {
    }

    void add(const std::shared_ptr<ClientInfo> &ci)
    {
        audiences_.insert(ci);
        if (lastPubTime_.valid())
        {
            ci->conn_->send(makeMessage());
        }
    }

    void remove(const std::shared_ptr<ClientInfo> &ci)
    {
        audiences_.erase(ci);
    }

    void userUnsubscribe()
    {
        for (std::set<std::shared_ptr<ClientInfo>>::iterator it = audiences_.begin();
             it != audiences_.end();
             ++it)
        {
            (*it)->setclientTourist();
            ConnectionSubscription *connSub = boost::any_cast<ConnectionSubscription>((*it)->conn_->getMutableContext());
            //std::cout<<"it++"<<std::endl;
            connSub->erase(topic_);
        }
    }


    void publish(const string &content, Timestamp time, const publishType &pt)
    {
        content_ = content;
        lastPubTime_ = time;
        string message;
        if(pt == pMsg)
        {
            message = makeMessage();
        }
        else
        {
            message = makeSysMessage();
        }
        for (std::set<std::shared_ptr<ClientInfo>>::iterator it = audiences_.begin();
             it != audiences_.end();
             ++it)
        {
            (*it)->conn_->send(message);
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
    string makeSysMessage()
    {
        return "info "  + content_ + "\r\n" ;
    }
    string makeMessage()
    {
        return "msg " + topic_ + "\r\n" + content_ + "\r\n";
    }
    string topic_;
    string content_;
    Timestamp lastPubTime_;
    std::set<std::shared_ptr<ClientInfo>> audiences_;
    string onwer_;
    string rival_;
    std::mutex mtx;
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
            doPublish(conn->name(),user.getTopic(),"Owner delete room, you become tourist.",now, pInfo);
            getTopic(user.getTopic()).userUnsubscribe();
            topics_.erase(user.getTopic());
        }
        else if(user.getclientType() == cRival)
        {
            getTopic(user.getTopic()).setRival("");
            user.setclientTourist();
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
                        LOG_INFO << conn->name() << " subscribes " << topic;
                        doSubscribe(conn, topic);
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
                    doPublish(conn->name(), topic, content, receiveTime, pMsg);
                }
                else if (cmd == "unsub")
                {
                    doUnsubscribe(conn, topic);
                }
                else if (cmd == "sub")
                {
                    if(user.getclientType() == cTourist)
                    {
                        LOG_INFO << conn->name() << " subscribes " << topic;
                        user.setclientType(cSubscribe, topic);
                        doSubscribe(conn, topic);
                    }
                    else
                    {
                        string message = "info you are in a room\r\n";
                        conn->send(message);
                        result = kError;
                    }
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
        doPublish("internal", "utc_time", now.toFormattedString(), now, pMsg);
    }

    void doSubscribe(const TcpConnectionPtr &conn,
                     const string &topic)
    {
        ConnectionSubscription *connSub = boost::any_cast<ConnectionSubscription>(conn->getMutableContext());
        connSub->insert(topic);
        getTopic(topic).add(getClientInfo(conn->name()));
    }

    void doUnsubscribe(const TcpConnectionPtr &conn,
                       const string &topic)
    {
        LOG_INFO << conn->name() << " unsubscribes " << topic;
        getTopic(topic).remove(getClientInfo(conn->name()));
        // topic could be the one to be destroyed, so don't use it after erasing.
        ConnectionSubscription *connSub = boost::any_cast<ConnectionSubscription>(conn->getMutableContext());
        connSub->erase(topic);
    }

    void doPublish(const string &source,
                   const string &topic,
                   const string &content,
                   Timestamp time,
                   const publishType &pt)
    {
        getTopic(topic).publish(content, time, pt);
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

    std::shared_ptr<ClientInfo> &getClientInfo(const string& name)
    {
        std::map<string,std::shared_ptr<ClientInfo>>::iterator it = users_.find(name);
        return it->second;
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