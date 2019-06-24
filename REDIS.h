#pragma once
#include <hiredis/hiredis.h>
#include <string>

class REDIS{
public:
    REDIS(std::string host,int port,struct timeval timeout);
    ~REDIS();

    REDIS(const REDIS &) = delete;
    REDIS &operator=(const REDIS &) = delete;
    REDIS(REDIS &&) = delete;
    REDIS &operator=(REDIS &&) = delete;

    bool exeCommand(std::string cmd,std::string res);
private:
    redisContext *conn;
    redisReply *reply;
};