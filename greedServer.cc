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
#define HEIGHT 19
#define WIDTH 80


#define OWNER_DISSAPER 1
#define CREATE_SAME_NAME 2
#define GETIN_ERROR 3
#define TIME_PUBLISH 4
#define QUIT_ERROR 5
#define ALREADLY_IN_ROOM 6
#define UNKNOW_CMD 7
#define QUIT_CURRENT 8
#define FINSH_COMPELE 9

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
        : topic_(topic),onwer_(onwer),createdTime_(createdTime),grid(nullptr)
          ,ownerScore_(0),rivalScore_(0),isOwnerTurn(true),ownerCanMove(true),rivalCanMove(true)
    {
        grid = new int*[19];
        for(int i = 0;i < 19;++i){
            grid[i] = new int[80];
        }
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


    void publish(const string &content, Timestamp time, ProtobufCodec &codec,int id)
    {
        content_ = content;
        lastPubTime_ = time;
        pubsub::SystemAns ans;
        ans.set_id(OWNER_DISSAPER);
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
    void publishBoardUpdate(int& code, ProtobufCodec &codec)
    {
        pubsub::boardAns ans;
        google::protobuf::Message* MessageToSend_;
        ans.set_code(code);
        pubsub::GridInfo* gi = ans.mutable_g();
        string s = gridToString();
        //std::cout<<s<<std::endl;
        gi->set_grid(s);
        pubsub::clientType ct = cSubscribe;
        gi->set_topic(topic_);
        gi->set_isownertune(isOwnerTurn);
        gi->set_rivalscore(getRivalScore());
        gi->set_ownerscore(getownerScore());
        gi->set_ownercanmove(ownerCanMove);
        gi->set_rivalcanmove(rivalCanMove);
        pubsub::Pos* gpo = gi->mutable_o();
        gridPos owerpos = getOwnerPos();
        gpo->set_x(owerpos.x);
        gpo->set_y(owerpos.y);
        pubsub::Pos* gpr = gi->mutable_r();
        gridPos rivalpos = getRivalPos();
        gpr->set_x(rivalpos.x);
        gpr->set_y(rivalpos.y);
        for (std::set<std::shared_ptr<ClientInfo>>::iterator it = audiences_.begin();
             it != audiences_.end();
             ++it)
        {
            if((*it)->getclientType() == cSubscribe)
            {
                ct = cSubscribe;
            }
            else if((*it)->getclientType() == cRival)
            {
                ct = cRival;
            }
            else
            {
                ct = cOwner;
            }
            gi->set_type(ct);
            MessageToSend_ = &ans;
            codec.send((*it)->conn_, *MessageToSend_);
            //(*it)->conn_->send(message);
        }
    }
    void publishFinalInfo(ProtobufCodec &codec)
    {
        std::cout<<"game over"<<std::endl;
        pubsub::finalInfo fio;
        google::protobuf::Message* MessageToSend_;
        fio.set_ownerscore(ownerScore_);
        fio.set_rivalscore(rivalScore_);
        MessageToSend_ = &fio;
        for (std::set<std::shared_ptr<ClientInfo>>::iterator it = audiences_.begin();
             it != audiences_.end();
             ++it)
        {
            codec.send((*it)->conn_,*MessageToSend_);
        }
    }
    const string getOwner()
    {
        return onwer_;
    }
    void updateownerScore(int& t)
    {
        ownerScore_ += t;
    }
    void updaterivalScore(int& t)
    {
        rivalScore_ += t;
    }
    int& getownerScore()
    {
        return ownerScore_;
    }
    int& getRivalScore()
    {
        return rivalScore_;
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
    void setOwnerPos(int& x,int& y){
        o.x = x;
        o.y = y;
    }
    void setRivalPos(int& x,int& y){
        r.x = x;
        r.y = y;
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
        if(rival_ == "")
        {
            if(mtx.try_lock())
            {
                if(rival_ == "")
                {
                    rival_ = rival;
                    mtx.unlock();
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
    const string& getGridString(){
        return gridString;
    }
    const string& gridToString()
    {
        gridString.clear();
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
    int ownerScore_;
    int rivalScore_;
    string gridString;
    gridPos o, r;
    std::mutex mtx;
public:
    int** grid;
    bool isOwnerTurn;
    bool ownerCanMove;
    bool rivalCanMove;
};



typedef std::shared_ptr<pubsub::SystemQuery> SystemQueryPtr;
typedef std::shared_ptr<pubsub::boardQuery> boardQueryPtr;
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
        dispatcher_.registerMessageCallback<pubsub::boardQuery>(
        std::bind(&GreedServer::onboardQuery, this, _1, _2, _3));
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
    void onboardQuery(const TcpConnectionPtr& conn,
                        const boardQueryPtr& message,
                        Timestamp)
    {
        pubsub::boardAns ans;
        google::protobuf::Message* MessageToSend_;
        pubsub::Pos dxdy = message->x();
        clientType ct = static_cast<pubsub::clientType>(message->type());
        string topic = message->topic();
        int res = isValid(getTopic(topic).grid,ct,topic,dxdy.x(),dxdy.y());
        bool inTheEnd = false;
        if(ct == cOwner)
        {
            bool anyM = anymove(getTopic(topic).grid,getTopic(topic).getOwnerPos().y ,getTopic(topic).getOwnerPos().x);//can move??
            getTopic(topic).ownerCanMove = anyM;
            if(!anyM)
            {
                int trivalscore = getTopic(topic).getRivalScore();
                int townerscore = getTopic(topic).getownerScore();
                if(townerscore <= trivalscore) // over
                {
                    inTheEnd = true;
                    getTopic(topic).publishBoardUpdate(res, codec_);
                    getTopic(topic).publishFinalInfo(codec_);
                }
                else
                {
                    if(getTopic(topic).rivalCanMove) // not yet
                    {
                        getTopic(topic).isOwnerTurn = false;
                    }
                    else // over
                    {
                        inTheEnd = true;
                        getTopic(topic).publishBoardUpdate(res, codec_);
                        getTopic(topic).publishFinalInfo(codec_);
                    }
                    
                }
                
            }
        }else
        {
            bool anyM = anymove(getTopic(topic).grid,getTopic(topic).getRivalPos().y,getTopic(topic).getRivalPos().x);
            getTopic(topic).rivalCanMove = anyM;
            if(!anyM)
            {
                int trivalscore = getTopic(topic).getRivalScore();
                int townerscore = getTopic(topic).getownerScore();
                if(trivalscore < trivalscore)
                {
                    inTheEnd = true;
                    getTopic(topic).publishBoardUpdate(res, codec_);
                    getTopic(topic).publishFinalInfo(codec_);
                }
                else
                {
                    if(getTopic(topic).ownerCanMove)
                    {
                        getTopic(topic).isOwnerTurn = true;
                    }
                    else
                    {
                        inTheEnd = true;
                        getTopic(topic).publishBoardUpdate(res, codec_);
                        getTopic(topic).publishFinalInfo(codec_);
                    }
                    
                }
                
            }
        }
        if(res == 1){//Bad Move
            
            return;
        }
        // else if(res == 0)
        // {
        //     int trivalscore = getTopic(topic).getRivalScore();
        //     int townerscore = getTopic(topic).getownerScore();
        //     pubsub::finalInfo fio;
        //     if(ct == cRival)
        //     {
        //         if(trivalscore <= townerscore)
        //         {
        //             inTheEnd = true;
        //             getTopic(topic).publishFinalInfo(codec_);
        //         }
        //         else 
        //         {
        //             if(getTopic(topic).ownerCanMove)
        //             getTopic(topic).isOwnerTurn = !getTopic(topic).isOwnerTurn;
        //             else {
        //                 inTheEnd = true;
        //                 getTopic(topic).publishFinalInfo(codec_);
        //             }
        //         }
                
        //     }
        //     if(ct == cOwner)
        //     {
        //         if(trivalscore >= townerscore)
        //         {
        //             inTheEnd = true;
        //             getTopic(topic).publishFinalInfo(codec_);
        //         }
        //         else
        //         {
        //             if(getTopic(topic).rivalCanMove)
        //             getTopic(topic).isOwnerTurn = !getTopic(topic).isOwnerTurn;
        //             else{
        //                 inTheEnd = true;
        //                 getTopic(topic).publishFinalInfo(codec_);
        //             }
        //         }
                
        //     }
        // }
        if(!inTheEnd){
            ans.set_code(res);
            getTopic(topic).publishBoardUpdate(res, codec_);
        }else
        {
            getTopic(topic).publish("Game is finsh, tap any key to back cmd model.",Timestamp::now(),codec_,FINSH_COMPELE);
            getTopic(topic).userUnsubscribe();
            topics_.erase(topic);
        }
    }
    int othermove(int** &grid_,int bady, int badx,int x, int y)
    {
        int dy = -1, dx;

        for (; dy <= 1; dy++)
        for (dx = -1; dx <= 1; dx++)
            if ((!dy && !dx) || (dy == bady && dx == badx) ||
            y+dy < 0 && x+dx < 0 && y+dy >= HEIGHT && x+dx >= WIDTH)
            /* don't do 0,0 or bad coordinates */
            continue;
            else {
            int j=y, i=x, d= grid_[y+dy][x+dx];

            if (!d) continue;
            do {		/* "walk" the path, checking */
                j += dy;
                i += dx;
                if (j < 0 || i < 0 || j >= HEIGHT ||
                i >= WIDTH || !grid_[j][i]) break;
            } while (--d);
            if (!d) return 1;	/* if "d" got to 0, *
                        * move was okay.   */
            }
        return 0;			/* no good moves were found */
    }
    bool anymove(int** &grid_, int x, int y)
    {
        int dy = -1, dx;

        for (; dy <= 1; dy++)
        for (dx = -1; dx <= 1; dx++)
            if ((!dy && !dx) || y+dy < 0 || x+dx < 0 || y+dy >= HEIGHT || x+dx >= WIDTH)
            /* don't do 0,0 or bad coordinates */
            continue;
            else {
            int j=y, i=x, d= grid_[y+dy][x+dx];

            if (!d) continue;
            do {		/* "walk" the path, checking */
                j += dy;
                i += dx;
                if (j < 0 || i < 0 || j >= HEIGHT ||
                i >= WIDTH || !grid_[j][i]) break;
            } while (--d);
            if (!d) return true;	/* if "d" got to 0, *
                        * move was okay.   */
            }
        return false;			/* no good moves were found */
    }
    int isValid(int**& grid_, clientType ct, const string& topic, int dx, int dy)
    {
        int distance;
        gridPos ownerpos = getTopic(topic).getOwnerPos();
        gridPos rivalpos = getTopic(topic).getRivalPos();
        int y = (ct == cOwner?ownerpos.x:rivalpos.x);
        int x = (ct == cOwner?ownerpos.y:rivalpos.y);
        distance = (y+dy >= 0 && x+dx >= 0 && y+dy < HEIGHT && x+dx < WIDTH) ?
        grid_[y+dy][x+dx] : 0;
        int score = 0;
        {
        int j = y, i = x, d = distance;

        do {				/* process move for validity */
            j += dy;
            i += dx;
            if (j >= 0 && i >= 0 && j < HEIGHT && i < WIDTH && grid_[j][i])
            continue;	/* if off the screen */
            else if (!othermove(grid_,dy, dx, x, y)) {	/* no other good move */
            j -= dy;
            i -= dx;
            //mvaddch(y, x, ' ');
            while (y != j || x != i) {
                y += dy;	/* so we manually */
                x += dx;	/* print chosen path */
                score++;
                //mvaddch(y, x, ' ');
            }
            //over
            return (0);
            } else {		/* otherwise prevent bad move */
            //botmsg("Bad move.", true,x,y);
            return (1);
            }
        } while (--d);
        }
        do {				/* print good path */
        y += dy;
        x += dx;
        score++;
        grid_[y][x] = 0;
        } while (--distance);
        if(ct == cOwner)
        {
            getTopic(topic).updateownerScore(score);
            getTopic(topic).setOwnerPos(y,x);
            if(getTopic(topic).rivalCanMove)
            {
                getTopic(topic).isOwnerTurn = !getTopic(topic).isOwnerTurn;
            }
        }
        else
        {
            getTopic(topic).updaterivalScore(score);
            getTopic(topic).setRivalPos(y,x);
            if(getTopic(topic).ownerCanMove)
            {
                getTopic(topic).isOwnerTurn = !getTopic(topic).isOwnerTurn;
            }
        }
        return (2);
    }
    void onUnknownMessage(const TcpConnectionPtr& conn,
                        const MessagePtr& message,
                        Timestamp)
    {
        LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
        conn->shutdown();
    }
    void sendGridInfo(const TcpConnectionPtr& conn,const string& topic, clientType ct){
        string s = getTopic(topic).getGridString();
        pubsub::GridInfo gi;
        gi.set_grid(s);
        gi.set_type(static_cast<int>(ct));
        gi.set_topic(topic);
        gi.set_isownertune(getTopic(topic).isOwnerTurn);
        gi.set_rivalscore(getTopic(topic).getRivalScore());
        gi.set_ownerscore(getTopic(topic).getownerScore());
        gi.set_ownercanmove(getTopic(topic).ownerCanMove);
        gi.set_rivalcanmove(getTopic(topic).rivalCanMove);
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
                sendGridInfo(conn, topic, cOwner);
            }
            else
            {
                ans.set_id(CREATE_SAME_NAME);
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
                sendGridInfo(conn, topic,cRival);
            }
            else
            {
                ans.set_id(GETIN_ERROR);
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
                ans.set_id(QUIT_CURRENT);
                ans.set_content("you are a tourist now");
                MessageToSend_ = &ans;
                codec_.send(conn, *MessageToSend_);
            }
            else if(user.getclientType() == cOwner)
            {
                LOG_INFO << conn->name() << " remove topic: " << topic;
                Timestamp now = Timestamp::now();
                doPublish(conn->name(),user.getTopic(),"Owner delete room, you become tourist.",now, OWNER_DISSAPER);
                getTopic(user.getTopic()).userUnsubscribe();
                topics_.erase(user.getTopic());
            }
            else
            {
                ans.set_id(QUIT_ERROR);
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
                sendGridInfo(conn, topic,cSubscribe);
            }
            else
            {
                ans.set_id(ALREADLY_IN_ROOM);
                ans.set_content("you are in a room");
                MessageToSend_ = &ans;
                codec_.send(conn, *MessageToSend_);
            }
        }
        else
        {
            ans.set_id(UNKNOW_CMD);
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
            doPublish(conn->name(),user.getTopic(),"Owner delete room, you become tourist.",now, OWNER_DISSAPER);
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
        doPublish("internal", "utc_time", now.toFormattedString(), now, TIME_PUBLISH);
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
                   int id)
    {
        getTopic(topic).publish(content, time, codec_, id);
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