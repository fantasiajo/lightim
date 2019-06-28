#pragma once 

#include <memory>
#include <mutex>

template <typename T>
class Singleton {
public:
	Singleton() = delete;
	~Singleton() = delete;
	Singleton(const Singleton &) = delete;
	Singleton(Singleton &&) = delete;
	Singleton& operator=(const Singleton &) = delete;
	Singleton& operator=(Singleton&&) = delete;

	template <typename... Args>
	static T& instance(Args&&... args){
		std::call_once(flg,_instance<Args...>,std::forward<Args>(args)...);
		return *pT;
	}

private:
	template <typename... Args>
	static void _instance(Args&&... args){
		pT = std::make_shared<T>(std::forward<Args>(args)...);
	}

	static std::shared_ptr<T> pT;
	static std::once_flag flg;
};

template <typename T>
std::shared_ptr<T> Singleton<T>::pT;

template <typename T>
std::once_flag Singleton<T>::flg;

