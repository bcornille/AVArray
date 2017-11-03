#include <cassert>

#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

template<typename T>
class MyVector
{
private:
	unsigned int n_el;
	T* storage;

public:
	MyVector() : n_el(0), storage(nullptr) {}
	MyVector(unsigned int n) : n_el(n), storage(new T[n]) {}
	MyVector(MyVector & other) = default;
	MyVector(MyVector &&other) = default;
	MyVector& operator=(MyVector & other) = default;
	MyVector& operator=(MyVector &&other) = default;
	~MyVector() { delete[] storage; }

	inline T& operator[](unsigned int i)
	{
		assert(i < n_el);
		return storage[i];
	}

	inline T& operator()(unsigned int i)
	{
		assert(i < n_el);
		return storage[i];
	}

	inline unsigned int size() { return n_el; }
};

#endif // MY_VECTOR_HPP