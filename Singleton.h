#pragma once 

#include <memory>

template <typename T>
class Singleton {
public:
	Singleton();
	~Singleton();

	static T& instance() {
		if (!pT) {
			pT.reset(new T());
		}
		return *pT;
	}
private:
	static std::shared_ptr<T> pT;
};

template <typename T>
std::shared_ptr<T> Singleton<T>::pT;

