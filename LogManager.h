#pragma once
#include <vector>
#include <mutex>
#include <chrono>
#include "easylogging++.h"
#include <thread>
#include <unistd.h>

class LogManager {
public:
	enum LOG_TYPE{INFO_LEVEL,ERROR_LEVEL,FATAL_LEVEL,DEBUG_LEVEL};
	typedef struct {
		LOG_TYPE type;
		std::string content;
	}ONELOG;

	LogManager() = default;
	~LogManager() = default;
	LogManager(const LogManager &) = delete;
	LogManager(LogManager &&) = delete;
	LogManager & operator=(const LogManager &) = delete;
	LogManager & operator=(LogManager &&) = delete;

	void logInQueue(const LOG_TYPE &type, const std::string &content) {
		std::unique_lock<std::mutex> lck(mtxLogs);
		tmplog.type = type;
		tmplog.content = content;
		logs.push_back(tmplog);
	}
	void loop() {
		while (true) {
			std::vector<ONELOG> tmpLogs;
			{
				std::lock_guard<std::mutex> lck(mtxLogs);
				swap(tmpLogs, logs);
			}
			for (const auto &item : tmpLogs) {
				switch (item.type) {
				case LOG_TYPE::INFO_LEVEL:
					LOG(INFO) << item.content;
					break;
				case LOG_TYPE::ERROR_LEVEL:
					LOG(ERROR) << item.content;
					break;
				case LOG_TYPE::DEBUG_LEVEL:
					LOG(DEBUG) << item.content;
					break;
				case LOG_TYPE::FATAL_LEVEL:
					LOG(FATAL) << item.content;
					sleep(5);
					exit(1);
					break;
				default:
					break;
				}
			}
			//std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
private:
	std::vector<ONELOG> logs;
	std::mutex mtxLogs;
	ONELOG tmplog;
};
