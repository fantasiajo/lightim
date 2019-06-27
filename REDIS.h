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

    bool queuePush(std::string key, char* data, uint16_t len);
    bool queuePop(std::string key);
    int queueLen(std::string key);
    bool queueContent(std::string key,std::vector<std::string> &strs);
private:
    redisContext *conn;
    redisReply *reply;
};