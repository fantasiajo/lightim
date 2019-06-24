#include "REDIS.h"
#include "LogManager.h"
#include "Singleton.h"

REDIS::REDIS(std::string host,int port,struct timeval timeout){
    conn = redisConnectWithTimeout(host.c_str(),port,timeout);
    if(!conn || conn->err){
        std::string loginfo("Conection error: ");
        if(conn){
            loginfo.append(conn->errstr);
        }
        else{
            loginfo.append("can't allocate redis context");
        }
        Singleton<LogManager>::instance().logInQueue(LogManager::FATAL_LEVEL,loginfo);
    }
}

REDIS::~REDIS(){
    if(conn){
        redisFree(conn);
    }
}

bool REDIS::exeCommand(std::string cmd, std::string res){

}