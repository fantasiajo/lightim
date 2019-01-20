#pragma once

int createEventFd() {
	int fd = eventfd(0);
	if (fd == -1) {
		//log.err
	}
	return fd;
}
