<<<<<<< HEAD
#pragma once

=======
>>>>>>> msg cache
#include "REDIS.h"
#include "Msg.h"
#include <memory>


class MsgCache{
public:
<<<<<<< HEAD
    MsgCache();
=======
    MsgCache(std::string host, int port, int timeout);
>>>>>>> msg cache
    ~MsgCache();

    MsgCache(const MsgCache &) = delete;
    MsgCache(MsgCache &&) = delete;
    MsgCache& operator=(const MsgCache &) = delete;
    MsgCache& operator=(MsgCache &&) = delete;

    bool peekMsgid(uint32_t id, uint64_t &msgid);
    bool push(uint32_t id, std::shared_ptr<Msg> pMsg);
    bool pop(uint32_t id);
    int size(uint32_t id);
<<<<<<< HEAD
    bool content(uint32_t id, std::vector<std::string> &msgs);
=======
>>>>>>> msg cache
private:
    REDIS r;
};