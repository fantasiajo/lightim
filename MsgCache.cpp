#include "MsgCache.h"
#include "util.h"
#include "EventLoop.h"
#include <iostream>

MsgCache::~MsgCache(){

}

bool MsgCache::peekMsgid(uint32_t id, uint64_t &msgid){
    std::string str;
    if(r.queuePeekHead(std::to_string(id),str)){
        msgid = ntohll(*(uint64_t*)(str.c_str()+Msg::headerLen));
        return true;
    }
    return false;
}

bool MsgCache::push(uint32_t id, std::shared_ptr<Msg> pMsg){
    return r.queuePush(std::to_string(id),pMsg->getBuf(),pMsg->getLen());
}

bool MsgCache::pop(uint32_t id){
    return r.queuePop(std::to_string(id));
}

int MsgCache::size(uint32_t id){
    if(ploop->isInLoopThread()){
        return r.queueLen(std::to_string(id));
    }
    else{
        std::cerr << "bomb" << std::endl;
        exit(111);
    }
}

bool MsgCache::content(uint32_t id, std::vector<std::string> &msgs){
    return r.queueContent(std::to_string(id),msgs);
}