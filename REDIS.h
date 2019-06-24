#pragma once
#include <hiredis/hiredis.h>
#include <string>
#include <vector>

class REDIS{
public:
    REDIS(std::string host,int port,int timeout);
    ~REDIS();

    REDIS(const REDIS &) = delete;
    REDIS &operator=(const REDIS &) = delete;
    REDIS(REDIS &&) = delete;
    REDIS &operator=(REDIS &&) = delete;

    bool exeCommand(const std::string &cmd,std::vector<std::string>& res);
private:
    redisContext *conn;
    redisReply *reply;
};