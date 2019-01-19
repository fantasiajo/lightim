#include "Buffer.h"

Buffer::Buffer() {
	head = (BufferStrip *)malloc(sizeof(BufferStrip));
	if (!head) {
		std::cout << "malloc head failed." << std::endl;
	}
	head->next = (BufferStrip *)malloc(sizeof(BufferStrip));
	tail = head->next;
	if (!tail) {
		std::cout << "malloc tail failed." << std::endl;
	}
	tail->next = nullptr;
	begin.pstrip = head;
	begin.offset = 0;
	end.pstrip = head;
	end.offset = 0;
	stripnum = 2;
}

Buffer::~Buffer() {
	BufferStrip *p = head, *tmp;
	while (p != tail) {
		tmp = p->next;
		free(p);
		p = tmp;
	}
	free(tail);
}

bool Buffer::empty() {
	return begin.pstrip == end.pstrip && begin.offset == end.offset;
}

int Buffer::length() {
	if (begin.pstrip == end.pstrip) {
		return end.offset - begin.offset;
	}
	int len = STRIP_LEN - begin.offset;
	BufferStrip *p = begin.pstrip->next;
	while (p != end.pstrip) {
		len += STRIP_LEN;
		p = p->next;
	}
	len += end.offset;
	return len;
}

int Buffer::inbuffer(const char *buf, int len) {
	int leftSpace = getLeftSpace(), tmp;
	while (leftSpace < len) {
		tmp = expandSpace();
		if (tmp == -1) return -1;
		leftSpace += tmp;
	}

	//如果当前strip可以容下所有数据，全部填入，如果不能填满当前strip
	int overlen = std::min(STRIP_LEN - end.offset, len);
	strncpy(end.pstrip->data + end.offset, buf, overlen);
	end.offset += overlen;
	if (end.offset == STRIP_LEN) {
		end.pstrip = end.pstrip->next;
		end.offset = 0;
	}

	while ((len - overlen) >= STRIP_LEN) {//当剩余的数据持续大于一个strip大小时
		strncpy(end.pstrip->data, buf + overlen, STRIP_LEN);
		overlen += STRIP_LEN;
		end.pstrip = end.pstrip->next;
		end.offset = 0;
	}
	if (overlen < len) {
		strncpy(end.pstrip->data, buf + overlen, (len - overlen));
		end.offset = len - overlen;
	}
	return len;
}

int Buffer::outbuffer(char *buf, int len) {
	if (length() < len) return -1;
	int overlen = std::min(STRIP_LEN - begin.offset, len);
	strncpy(buf, begin.pstrip->data + begin.offset, overlen);
	begin.offset += overlen;
	if (begin.offset == STRIP_LEN) {
		headToTail();
		begin.pstrip = head;
		begin.offset = 0;
		if (!end.pstrip) {
			end.pstrip = tail;
		}
	}
	while ((len - overlen) >= STRIP_LEN) {
		strncpy(buf + overlen, begin.pstrip->data, STRIP_LEN);
		overlen += STRIP_LEN;
		headToTail();
		begin.pstrip = head;
		begin.offset = 0;
	}
	if (overlen < len) {
		strncpy(buf + overlen, begin.pstrip->data, (len - overlen));
		begin.offset = len - overlen;
	}
	return len;
}

void Buffer::headToTail() {
	tail->next = head;
	head = head->next;
	tail = tail->next;
	tail->next = nullptr;
}

int Buffer::getCapacity() {
	return stripnum * STRIP_LEN;
}

int Buffer::getLeftSpace() {
	if (!end.pstrip) return 0;
	int len = STRIP_LEN - end.offset;
	BufferStrip *p = end.pstrip->next;
	if (p) {
		len += STRIP_LEN;
		p = p->next;
	}
	return len;
}

int Buffer::expandSpace() {
	for (int i = 0; i < stripnum; ++i) {
		tail->next = (BufferStrip*)malloc(sizeof(BufferStrip));
		if (!tail->next) {
			return -1;
		}
		tail = tail->next;
		if (!end.pstrip) {
			end.pstrip = tail;
		}
	}
	tail->next = nullptr;
	stripnum *= 2;
	return stripnum * STRIP_LEN;
}
