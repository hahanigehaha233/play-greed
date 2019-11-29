#include"codec.h"

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

ParseResult pubsub::parseMessage(Buffer* buf,
                                 string* cmd,
                                 string* topic,
                                 string* content)
{
    ParseResult result = KError;
    const char* crlf = buf->findCRLF();

    if(crlf)
    {
        const char* space = std::find(buf->peek(), crlf, ' ');
        if(space != crlf)
        {
            cmd->assign(buf->peek(), space);
            topic->assign(space+1, crlf);
            buf->retrieveUntil(crlf+2);
            result = KSucces;
        }
        else
        {
            result = KError;
        }
    }
    else
    {
        result = KContinue;
    }
    return result;
}