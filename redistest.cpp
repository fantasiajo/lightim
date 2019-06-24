#include <iostream>
#include <string>
#include "REDIS.h"
#include "ThreadManager.h"
#include "easylogging++.h"
#include "LogManager.h"
#include "Singleton.h"
using namespace std;

INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv)
{
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime %thread %level: %msg");
    Singleton<ThreadManager>::instance().newThread(1,
		std::bind(&LogManager::loop, &Singleton<LogManager>::instance()));
    REDIS r("127.0.0.1",6379,1);
    std::string line;
    while(std::getline(std::cin,line)){
        std::vector<std::string> res;
        if(r.exeCommand(line,res)){
            for(const auto &item:res){
                std::cout << item << std::endl;
            }
        }
    }
    return 0;
}