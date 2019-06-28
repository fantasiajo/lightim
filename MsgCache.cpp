#include "MsgCache.h"

MsgCache::MsgCache(){

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

bool MsgCache::content(uint32_t id, std::vector<std::string> &msgs){
    return r.queueContent(std::to_string(id),msgs);
}