#include<iostream>

#include"codec.h"
#include "info.pb.h"
#include "ProtobufCodec.h"
#include "dispatcher.h"

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

#define rnd(x) (int) ((lrand48() % (x))+1)

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

typedef struct gridPos{
    int x;
    int y;
} gridPos;

class Topic:public muduo::copyable
{
public:
    Topic(const string &topic,const string &onwer, const Timestamp& createdTime)
        : topic_(topic),onwer_(onwer),createdTime_(createdTime)
    {
        for(int i = 0;i < 19;++i){
            for(int j = 0;j < 80;++j){
                grid[i][j] = rnd(9);
            }
        }
        o.x = rnd(19) - 1;
        o.y = rnd(80) - 1;
        grid[o.x][o.y] = 0;
        r.x = rnd(19) - 1;
        r.y = rnd(80) - 1;
        grid[r.x][r.y] = 0;
        gridToString();
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


    void publish(const string &content, Timestamp time, ProtobufCodec &codec)
    {
        content_ = content;
        lastPubTime_ = time;
        pubsub::SystemAns ans;
        ans.set_content(content);
        google::protobuf::Message* messageToSend = &ans;
        for (std::set<std::shared_ptr<ClientInfo>>::iterator it = audiences_.begin();
             it != audiences_.end();
             ++it)
        {
            codec.send((*it)->conn_, *messageToSend);
            //(*it)->conn_->send(message);
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
    const gridPos& getOwnerPos(){
        return o;
    }
    const gridPos& getRivalPos(){
        return r;
    }
    void setRivalEmpty()
    {
        if(mtx.try_lock())
        {
            rival_.clear();
            mtx.unlock();
            std::cout<<"sent rival empty : "<<rival_<<std::endl;
        }
    }
    bool setRival(const string &rival)
    {
        std::cout<<"beform set rival:"<<rival_<<std::endl;
        if(rival_ == "")
        {
            if(mtx.try_lock())
            {
                if(rival_ == "")
                {
                    rival_ = rival;
                    mtx.unlock();
                    std::cout<<"after set rival:"<<rival_<<std::endl;
                    return true;
                } 
                mtx.unlock();
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    const string& getGrid(){
        return gridString;
    }
    const string& gridToString()
    {
        for(int i = 0;i < 19;++i){
            for(int j = 0;j < 80;++j){
                gridString += grid[i][j] + '0';
            }
        }
        return gridString;
    }
public: Timestamp createdTime_;
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
    int grid[19][80];
    string gridString;
    gridPos o, r;
    std::mutex mtx;
};


typedef std::shared_ptr<pubsub::SystemQuery> SystemQueryPtr;
class GreedServer: noncopyable
{
public:
    GreedServer(muduo::net::EventLoop *loop,
                const muduo::net::InetAddress &listenAddr)
        : loop_(loop),
          server_(loop, listenAddr, "GreedServer"),
          dispatcher_(std::bind(&GreedServer::onUnknownMessage, this, _1, _2, _3)),
          codec_(std::bind(&ProtobufDispatcher::onProtobufMessage, &dispatcher_, _1, _2, _3))
    {
        dispatcher_.registerMessageCallback<pubsub::SystemQuery>(
        std::bind(&GreedServer::onQuery, this, _1, _2, _3));
        server_.setConnectionCallback(
            std::bind(&GreedServer::onConnection, this, _1));
        server_.setMessageCallback(
            std::bind(&ProtobufCodec::onMessage, &codec_, _1, _2, _3));
        loop_->runEvery(1.0, std::bind(&GreedServer::timePublish, this));
        createTopic("utc_time","root");
    }

    void start()
    {
        server_.start();
    }
    

private:
    void onUnknownMessage(const TcpConnectionPtr& conn,
                        const MessagePtr& message,
                        Timestamp)
    {
        LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
        conn->shutdown();
    }
    void sendGridInfo(const TcpConnectionPtr& conn,const string& topic){
        string s = getTopic(topic).getGrid();
        pubsub::GridInfo gi;
        gi.set_grid(s);
        pubsub::Pos* gpo = gi.mutable_o();
        gridPos owerpos = getTopic(topic).getOwnerPos();
        gpo->set_x(owerpos.x);
        gpo->set_y(owerpos.y);
        pubsub::Pos* gpr = gi.mutable_r();
        gridPos rivalpos = getTopic(topic).getRivalPos();
        gpr->set_x(rivalpos.x);
        gpr->set_y(rivalpos.y);
        google::protobuf::Message* MessageToSend;
        MessageToSend = &gi;
        codec_.send(conn, *MessageToSend);
    }
    void onQuery(const TcpConnectionPtr& conn,
               const SystemQueryPtr& message,
               Timestamp)
    {
        std::map<string,std::shared_ptr<ClientInfo>>::iterator it =  users_.find(conn->name());
        ClientInfo& user = (*it->second);
        string cmd = (*message).cmd();
        string topic = (*message).topic();
        pubsub::SystemAns ans;
        google::protobuf::Message* MessageToSend_;
        if (cmd == "new")
        {
            if(user.getclientType() == cTourist && createTopic(topic,conn->name()))
            {
                user.setclientType(cOwner,topic);
                LOG_INFO << conn->name() << " create " << topic;
                LOG_INFO << conn->name() << " subscribes " << topic;
                doSubscribe(conn, topic);
                sendGridInfo(conn, topic);
            }
            else
            {
                ans.set_content("same name or you have a room already");
                MessageToSend_ = &ans;
                codec_.send(conn,*MessageToSend_);
            }
        }
        else if (cmd == "getin")
        {
            if(user.getclientType() == cTourist && setRival(topic, conn->name()))
            {
                user.setclientType(cRival,topic);
                doSubscribe(conn, topic);
                LOG_INFO << conn->name() << " getin " << topic;
                LOG_INFO << conn->name() << " subscribes " << topic;
                sendGridInfo(conn, topic);
            }
            else
            {
                ans.set_content("can't getin");
                MessageToSend_ = &ans;
                codec_.send(conn, *MessageToSend_);
            }
        }
        else if (cmd == "quit")//unsub
        {
            if((user.getclientType() == cRival || user.getclientType() == cSubscribe) && topic == user.getTopic())
            {
                LOG_INFO << conn->name() << " getout " << topic;
                getTopic(user.getTopic()).setRivalEmpty();
                doUnsubscribe(conn, topic);
            }
            else if(user.getclientType() == cOwner)
            {
                LOG_INFO << conn->name() << " remove topic: " << topic;
                Timestamp now = Timestamp::now();
                doPublish(conn->name(),user.getTopic(),"Owner delete room, you become tourist.",now);
                getTopic(user.getTopic()).userUnsubscribe();
                topics_.erase(user.getTopic());
            }
            else
            {
                ans.set_content("you are not in room or this room name is wrong");
                MessageToSend_ = &ans;
                codec_.send(conn, *MessageToSend_);
            }
            user.setclientTourist();
        }
        else if (cmd == "show")
        {
            auto it = topics_.begin();
            int count = topics_.size();
            pubsub::ShowInfo sif;
            int i = 0;
            for(;it != topics_.end();++it)
            {
                pubsub::RoomInfo * tri = sif.add_ri();
                tri->set_createdtime(getTopic(it->first).createdTime_.toFormattedString());
                tri->set_name(getTopic(it->first).getTopicName());
                tri->set_owner(getTopic(it->first).getOwner());
                tri->set_rival(getTopic(it->first).getRival());
            }
            google::protobuf::Message* MessageToSend = &sif;
            codec_.send(conn,*MessageToSend);
        }
        else if (cmd == "watch")//sub
        {
            if(user.getclientType() == cTourist)
            {
                LOG_INFO << conn->name() << " subscribes " << topic;
                user.setclientType(cSubscribe, topic);
                doSubscribe(conn, topic);
                sendGridInfo(conn, topic);
            }
            else
            {
                ans.set_content("you are in a room");
                MessageToSend_ = &ans;
                codec_.send(conn, *MessageToSend_);
            }
        }
        else
        {
            ans.set_content("unknow cmd");
            MessageToSend_ = &ans;
            codec_.send(conn, *MessageToSend_);
            //conn->shutdown();
        }
    }
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


    void removeUser(const TcpConnectionPtr& conn)
    {
        std::map<string,std::shared_ptr<ClientInfo>>::iterator it =  users_.find(conn->name());
        ClientInfo user = (*it->second);
        if((user.getclientType() == cOwner))
        {
            Timestamp now = Timestamp::now();
            doPublish(conn->name(),user.getTopic(),"Owner delete room, you become tourist.",now);
            getTopic(user.getTopic()).userUnsubscribe();
            topics_.erase(user.getTopic());
        }
        else if(user.getclientType() == cRival)
        {
            getTopic(user.getTopic()).setRivalEmpty();
            user.setclientTourist();
        }
        else if(user.getclientType() == cSubscribe)
        {
            doUnsubscribe(conn, user.getTopic());
        }
        users_.erase(conn->name());
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
                   Timestamp time)
    {
        getTopic(topic).publish(content, time, codec_);
    }
    bool hasnoTopic(const string &topic)
    {
        std::map<string, std::shared_ptr<Topic>>::iterator it = topics_.find(topic);
        return (it == topics_.end());
    }
    bool createTopic(const string &topic,const string& onwer)
    {
        Timestamp now = Timestamp::now();
        std::pair<std::map<string, std::shared_ptr<Topic>>::iterator, bool> res = topics_.insert(std::pair<string, std::shared_ptr<Topic>>(topic, std::make_shared<Topic>(topic,onwer,now)));
        return res.second;
    }
    bool createUser(const TcpConnectionPtr &conn)
    {
        std::pair<std::map<string,std::shared_ptr<ClientInfo>>::iterator,bool> res = users_.insert(std::pair<string, std::shared_ptr<ClientInfo>>(conn->name(),std::make_shared<ClientInfo>(conn)));
        return res.second;
    }
    bool setRival(const string &topic, const string& connName)
    {
        return getTopic(topic).setRival(connName);
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
    ProtobufDispatcher dispatcher_;
    ProtobufCodec codec_;


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