#pragma once

#include <cstdint>
#include <functional>

typedef std::function<void()> Task;

int createEventFd();

int createEpollFd();

uint64_t htonll(uint64_t);

uint64_t ntohll(uint64_t);
