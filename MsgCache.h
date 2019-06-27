#pragma once

#include "REDIS.h"
#include "Msg.h"
#include <memory>

/* 
 *pop(lpop)<-~~~~~~~~~~~<-push(rpush)
 */

class MsgCache{
public:
    MsgCache(std::string host, int port, int timeout);
    ~MsgCache();

    MsgCache(const MsgCache &) = delete;
    MsgCache(MsgCache &&) = delete;
    MsgCache& operator=(const MsgCache &) = delete;
    MsgCache& operator=(MsgCache &&) = delete;

    bool push(uint32_t id, std::shared_ptr<Msg> pMsg);
    bool pop(uint32_t id);
    int size(uint32_t id);
    bool content(uint32_t id, std::vector<std::string> &msgs);
private:
    REDIS r;
};