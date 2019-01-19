#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>

template <typename T>
class Singleton {
public:
	Singleton();
	~Singleton();

	static T& instance() {
		if (!pT) {
			pT = new T();
		}
		return *pT;
	}
private:
	static std::shared_ptr<T> pT;
};

#endif
