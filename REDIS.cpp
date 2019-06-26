#include "REDIS.h"
#include "LogManager.h"
#include "Singleton.h"

REDIS::REDIS(std::string host,int port,int timeout){
    struct timeval to={timeout,0};
    conn = redisConnectWithTimeout(host.c_str(),port,to);
    if(!conn || conn->err){
        std::stringstream loginfo("Conection error: ");
        if(conn){
            loginfo << conn->errstr;
        }
        else{
            loginfo << "can't allocate redis context";
        }
        Singleton<LogManager>::instance().logInQueue(LogManager::FATAL_LEVEL,loginfo.str());
    }
}

REDIS::~REDIS(){
    if(conn){
        redisFree(conn);
    }
}

bool REDIS::exeCommand(const std::string &cmd, std::vector<std::string> &res){
    res.clear();
    reply = static_cast<redisReply *>(redisCommand(conn,cmd.c_str()));
    bool success=true;
    switch(reply->type){
	    case REDIS_REPLY_STATUS:
            Singleton<LogManager>::instance().logInQueue(LogManager::INFO_LEVEL,std::string(reply->str,reply->len));
            break;
        case REDIS_REPLY_ERROR:
            Singleton<LogManager>::instance().logInQueue(LogManager::ERROR_LEVEL,std::string(reply->str,reply->len));
            success=false;
            break;
        case REDIS_REPLY_INTEGER:
            res.push_back(std::to_string(reply->integer));
            break;
        case REDIS_REPLY_NIL:
            break;
        case REDIS_REPLY_STRING:
            res.push_back(std::string(reply->str,reply->len));
            break;
        case REDIS_REPLY_ARRAY:
            for(int i=0;i<reply->elements;++i){
                res.push_back(reply->element[i]->str);
            }
            break;
    };
    freeReplyObject(reply);
    return success;
}

bool REDIS::queuePush(std::string key, char* data, uint16_t len){
    bool success = true;
    reply = static_cast<redisReply *>(redisCommand(conn,"rpush u%s %b",key.c_str(),data,len));
    if(reply->type==REDIS_REPLY_ERROR){
        Singleton<LogManager>::instance().logInQueue(LogManager::ERROR_LEVEL,reply->str);
        success = false;
    }
    freeReplyObject(reply);
    return success;
}

bool REDIS::queuePop(std::string key){
    bool success = true;
    reply = static_cast<redisReply *>(redisCommand(conn,"lpop u%s",key.c_str()));
    if(reply->type==REDIS_REPLY_ERROR){
        Singleton<LogManager>::instance().logInQueue(LogManager::ERROR_LEVEL,reply->str);
        success = false;
    }
    freeReplyObject(reply);
    return success;
}

int REDIS::queueLen(std::string key){
    int res;
    reply = static_cast<redisReply *>(redisCommand(conn,"llen u%s",key.c_str()));
    res = reply->integer;
    freeReplyObject(reply);
    return res;
}








