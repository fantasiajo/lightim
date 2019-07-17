#pragma once

#include <hiredis/hiredis.h>
#include <string>
#include <vector>

#define REDIS_HOST "127.0.0.1"
#define REDIS_PORT 6379
#define REDIS_TIMEOUT 1

class REDIS{
public:
    REDIS(std::string host = REDIS_HOST, int port = REDIS_PORT, int timeout = REDIS_TIMEOUT);
    ~REDIS();

    REDIS(const REDIS &) = delete;
    REDIS &operator=(const REDIS &) = delete;
    REDIS(REDIS &&) = delete;
    REDIS &operator=(REDIS &&) = delete;

    bool exeCommand(const std::string &cmd,std::vector<std::string>& res);

    /*
     *  head                   tail
     *  pop(lpop)<-~~~~~~~~~~~<-push(rpush)
     */
    bool queuePush(std::string key, char* data, uint16_t len);
    bool queuePop(std::string key);
    int queueLen(std::string key);
    bool queuePeekHead(std::string key, std::string &str);
    bool queueContent(std::string key,std::vector<std::string> &strs);
private:
    redisContext *conn;
    redisReply *reply;
};