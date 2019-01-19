#include "IOEventManager.h"

IOEventManager::IOEventManager() {

}

IOEventManager::~IOEventManager() {

}

int IOEventManager::getfd() {
	return fd;
}

uint32_t IOEventManager::getEvents() {
	return events;
}

void IOEventManager::enableReading() {

}

void IOEventManager::enableWriting() {

}

void IOEventManager::disableReading() {

}

void IOEventManager::disableWriting() {

}

void IOEventManager::handleEvent() {

}
