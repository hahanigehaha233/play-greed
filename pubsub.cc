#include<iostream>
#include"curses.h"

#include"pubsub.h"
#include"codec.h"
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

#define ME '#'
#define RIVAL '@'
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
      ,finalwin(NULL),gameModel(false),isOwnerTune(true),canmove(true)
{
    dispatcher_.registerMessageCallback<pubsub::SystemAns>(
        std::bind(&PubsubClient::onSystemAns, this, _1, _2, _3));
    dispatcher_.registerMessageCallback<pubsub::ShowInfo>(std::bind(&PubsubClient::onRoomInfo,this,_1,_2,_3));
    dispatcher_.registerMessageCallback<pubsub::GridInfo>(std::bind(&PubsubClient::onGridInfo,this,_1,_2,_3));
    dispatcher_.registerMessageCallback<pubsub::boardAns>(std::bind(&PubsubClient::onBoardAns,this,_1,_2,_3));
    dispatcher_.registerMessageCallback<pubsub::finalInfo>(std::bind(&PubsubClient::onFinalInfo,this,_1,_2,_3));
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


void PubsubClient::printSystemInfo(const string& str)
{
  const char* s = str.c_str();
  attron(attribs[1]);
  move(22,0);
  clrtoeol();
  mvprintw(22, 0, s); 
  attroff(attribs[1]);
  move(LINES-1,0);
  refresh();
}

void PubsubClient::printScoreBoard(int& ownerScore, int& rivalScore)
{
    move(19,30);
    if(clientType_ == cOwner){
        attron(A_REVERSE);
        printw("Owner  ");
        attroff(A_REVERSE);
        printw("V S  Rival");
    }
    else if(clientType_ == cRival)
    {
        printw("Owner  V S");
        attron(A_REVERSE);
        printw("  Rival");
        attroff(A_REVERSE);
    }
    else{
        printw("Owner  V S  Rival");
    }


    move(20,30);
    if(ownerScore > rivalScore){
        attron(A_REVERSE);
        printw("%-7d ",ownerScore);
        attroff(A_REVERSE);
        printw(":");
        printw("%7d ",rivalScore);
    }else if(ownerScore < rivalScore)
    {
        printw("%-7d ",ownerScore);
        printw(":");
        attron(A_REVERSE);
        printw("%7d ",rivalScore);
        attroff(A_REVERSE);
    }
    else{
        printw("%-7d ",ownerScore);
        printw(":");
        //attron(A_REVERSE);
        printw("%7d ",rivalScore);
        //attroff(A_REVERSE);
    }
    move(LINES - 1,0);
    
}

void PubsubClient::onSystemAns(const TcpConnectionPtr& conn, const SystemAnsPtr& message, Timestamp)
{
    int id = message->id();
    if(id == OWNER_DISSAPER || id == QUIT_CURRENT)
    {
        clear();
        refresh();
    }
    if(id == QUIT_CURRENT){
        clientType_ = cTourist;
    }
    printSystemInfo(message->content());
}
void PubsubClient::onFinalInfo(const TcpConnectionPtr& conn, const FinalInfoPtr& message, Timestamp)
{
    //printSystemInfo("game over!");
    finalwin = newwin(5,30,7,25);
    box(finalwin, ACS_VLINE, ACS_HLINE);
    int trivalscore = message->rivalscore();
    int townerscore = message->ownerscore();
    printScoreBoard(townerscore, trivalscore);
    bool ownerwin = townerscore > trivalscore;
    if(clientType_ == cOwner)
    {
        if(ownerwin)
        {
            mvwprintw(finalwin, 1, 10, "You Win!");
        }
        else
        {
            mvwprintw(finalwin, 1, 10, "You Loss!");
        }
    }
    else if(clientType_ == cRival)
    {
        if(!ownerwin)
        {
            mvwprintw(finalwin, 1, 10, "You Win!");
        }
        else
        {
            mvwprintw(finalwin, 1, 10, "You Loss!");
        }
    }
    else
    {
        if(ownerwin)
        {
            mvwprintw(finalwin, 1, 10, "Owner Win!");
        }
        else
        {
            mvwprintw(finalwin, 1, 10, "Rival Win!");
        }  
    }
    mvwprintw(finalwin,3,1,"Final score    %d:%d",townerscore,trivalscore);
    //mvwprintw(finalwin,4,5,"%d:%d",townerscore,trivalscore);
	wrefresh(finalwin);
    (void) wgetch(finalwin);
    touchwin(stdscr);
    move(LINES - 1,0);
    refresh();
    clientType_ = cTourist;
    gameModel = false;
    canmove = false;
    
}
void PubsubClient::onBoardAns(const TcpConnectionPtr& conn, const boardAnsPtr& message, Timestamp)
{
    int code = message->code();
    if(code == 0)
    {
        isOwnerTune = message->g().isownertune();
    }
    else if(code == 2)
    {
        //printSystemInfo("Good move");
        string grid = message->g().grid();
        //std::cout<<grid<<std::endl;
        int c = 0;
        attron(A_BOLD);
        for(int i = 0;i < 19;++i){
            for(int j = 0;j < 80;++j,++c){
                if(grid[c] - '0' == 0){
                    mvaddch(i,j, ' ');    
                }
            }
        }
        attroff(A_BOLD);
        isOwnerTune = message->g().isownertune();
        if(clientType_ == cOwner)
        {
            canmove = message->g().ownercanmove();
        }
        else
        {
            canmove = message->g().rivalcanmove();
        }
        pubsub::Pos o = message->g().o();
        pubsub::Pos r = message->g().r();
        owner_x = o.x();
        owner_y = o.y();
        rival_x = r.x();
        rival_y = r.y();

        if(clientType_ == cRival)
        {
            attron(A_BLINK);
            mvaddch(rival_x , rival_y, ME);
            attroff(A_BLINK);

            attron(A_REVERSE);
            mvaddch(owner_x, owner_y, RIVAL);
            attroff(A_REVERSE);
        }
        else if(clientType_ == cOwner)
        {
            attron(A_BLINK);
            mvaddch(owner_x, owner_y, ME);
            attroff(A_BLINK);

            attron(A_REVERSE);
            mvaddch(rival_x, rival_y, RIVAL);
            attroff(A_REVERSE);
        }
        else{
            attron(A_REVERSE);
            mvaddch(owner_x, owner_y, ME);
            attroff(A_REVERSE);

            attron(A_REVERSE);
            mvaddch(rival_x, rival_y, RIVAL);
            attroff(A_REVERSE);
        }
            move(LINES - 1,0);
            int os = message->g().ownerscore();
            int rs = message->g().rivalscore();
            printScoreBoard(os,rs);
        }
    // if(code == 1){
    //     printSystemInfo("Bad Move");
    // }
    if(isOwnerTune){
        printSystemInfo("Now is Owner tune!");
    }
    else
    {
        printSystemInfo("Now is Rival tune!");
    }
    
    refresh();
}

void PubsubClient::movecur()
{
    if(clientType_ == cOwner)
    {
        move(owner_y, owner_x - 1);
    }
    else
    {
        move(rival_y,rival_x - 1);
    }
    refresh();
}

int PubsubClient::tunnel(chtype cmd)
{
    int dy, dx, distance;
    if(!gameModel) return (-1);
    if(!canmove){
        printSystemInfo("No place can move!");
        return(1);
    }
    switch (cmd)
    { /* process user command */
    case '4':
        dy = 0;
        dx = -1;
        break;
    case '2':
        dy = 1;
        dx = 0;
        break;
    case '8':
        dy = -1;
        dx = 0;
        break;
    case '6':
        dy = 0;
        dx = 1;
        break;
    case '1':
        dy = 1;
        dx = -1;
        break;
    case '3':
        dy = dx = 1;
        break;
    case '7':
        dy = dx = -1;
        break;
    case '9':
        dy = -1;
        dx = 1;
        break;
    case 27:
        return (-1);
    case '?':
        //help();
        return (1);
    case '\14':
    case '\22':           /* ^L or ^R (redraw) */
        wrefresh(curscr); /* falls through to return */
    default:
        return (1);
    }
    if(isOwnerTune && clientType_ == cRival){
        printSystemInfo("Not you tune yet!");
        return (1);
    }
    if(!isOwnerTune && clientType_ == cOwner){
        printSystemInfo("Not you tune yet!");
        return (1);
    }
    pubsub::boardQuery query;
    query.set_type(static_cast<int>(clientType_));
    pubsub::Pos* mp =  query.mutable_x();
    mp->set_x(dx);
    mp->set_y(dy);
    query.set_topic(topic);
    messageToSend_ = &query;
    codec_.send(conn_, *messageToSend_);
    return (1);
}

void PubsubClient::updatexy(int x, int y)
{
    if(clientType_ == cOwner)
    {
        owner_x = y;
        owner_y = x;
    }
    else
    {
        rival_x = y;
        rival_y = x;
    }
    
}
void PubsubClient::botmsg(const string& msg, bool backcur, int x, int y)
/* 
 * botmsg() writes "msg" at the middle of the bottom line of the screen.
 * Boolean "backcur" specifies whether to put cursor back on the grid or
 * leave it on the bottom line (e.g. for questions).
 */
{
    const char* cmsg = msg.c_str();
    mvaddstr(23, 40, cmsg);
    clrtoeol();
    if (backcur) 
	move(y, x);
    refresh();
    //havebotmsg = true;
}
int PubsubClient::othermove(int bady, int badx,int x, int y)
/* 
 * othermove() checks area for an existing possible move.  bady and
 * badx are direction variables that tell othermove() they are
 * already no good, and to not process them.  I don't know if this
 * is efficient, but it works!
 */
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

void PubsubClient::showRoomInfo(const ShowRoomInfoPtr& message)
{
    clear();
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
    mvaddstr(LINES - 2,0,"Room Infomation update!");
    move(LINES - 1,0);
    refresh();
}
void PubsubClient::onGridInfo(const TcpConnectionPtr& conn, const GridInfoPtr& message, Timestamp)
{
    clientType_ = static_cast<pubsub::clientType>(message->type());
    if(clientType_ == cOwner || clientType_ == cRival)
    {   
        gameModel = true;
    }
    topic = message->topic();
    string grid = message->grid();
    printSystemInfo("Board init finsh!");
    int c = 0;
    attron(A_BOLD);
    for(int i = 0;i < 19;++i){
        for(int j = 0;j < 80;++j,++c){
            if(grid[c] - '0' == 0){
                continue;
            }
            attron(attribs[grid[c] - '1']);
            grid_[i][j] = grid[c] - '0';
            mvaddch(i,j, grid[c]);
            attroff(attribs[grid[c] - '1']);
        }
    }
    attroff(A_BOLD);
    pubsub::Pos o = message->o();
    pubsub::Pos r = message->r();
    isOwnerTune = message->isownertune();
    if(clientType_ == cOwner)
    {
        canmove = message->ownercanmove();
    }
    else
    {
        canmove = message->rivalcanmove();
    }
    
    owner_x = o.x();
    owner_y = o.y();
    rival_x = r.x();
    rival_y = r.y();
    if(clientType_ == cRival)
    {
        attron(A_BLINK);
        mvaddch(rival_x , rival_y, ME);
        attroff(A_BLINK);

        attron(A_REVERSE);
        mvaddch(owner_x, owner_y, RIVAL);
        attroff(A_REVERSE);
    }
    else if(clientType_ == cOwner)
    {
        attron(A_BLINK);
        mvaddch(owner_x, owner_y, ME);
        attroff(A_BLINK);

        attron(A_REVERSE);
        mvaddch(rival_x, rival_y, RIVAL);
        attroff(A_REVERSE);
    }
    else{
        attron(A_REVERSE);
        mvaddch(owner_x, owner_y, ME);
        attroff(A_REVERSE);

        attron(A_REVERSE);
        mvaddch(rival_x, rival_y, RIVAL);
        attroff(A_REVERSE);
    }
    int rs = message->rivalscore();
    int os = message->ownerscore();
    printScoreBoard(os,rs);
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