#include "MsgCache.h"

MsgCache::MsgCache(std::string host, int port, int timeout)
    :r(host,port,timeout){

}

MsgCache::~MsgCache(){

}

bool MsgCache::push(uint32_t id, std::shared_ptr<Msg> pMsg){
    return r.queuePush(std::to_string(id),pMsg->getBuf(),pMsg->getLen());
}

bool MsgCache::pop(uint32_t id){
    return r.queuePop(std::to_string(id));
}

int MsgCache::size(uint32_t id){
    return r.queueLen(std::to_string(id));
}