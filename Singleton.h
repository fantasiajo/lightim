#pragma once 

#include <memory>

template <typename T>
class Singleton {
public:
	Singleton() = delete;
	~Singleton() = delete;
	Singleton(const Singleton &) = delete;
	Singleton(Singleton &&) = delete;
	static T& instance() {
		static T ins;
		return ins;
	}
};

