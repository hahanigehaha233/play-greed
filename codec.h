#ifndef _CODEC_H_
#define _CODEC_H_


#include<muduo/base/Types.h>
#include<muduo/net/Buffer.h>

namespace pubsub
{
using muduo::string;
enum ParseResult
{
    kError,
    kSuccess,
    kContinue,
};

ParseResult parseMessage(muduo::net::Buffer* buf,
                          string* cmd,
                          string* topic,
                          string* content);
}// namespace pubsub

#endif // _CODEC_H_