#include "TensorBase.hpp"
#include <cassert>

#ifndef TENSOR_REF_HPP
#define TENSOR_REF_HPP

template<typename T, int D>
class TensorRef : TensorBase<TensorRef<T, D>, D> {
	using value_type = T;
	typedef TensorRef<T, D - 1> element_type;

private:
	value_type* storage_ref;
	int& dims[D];

	constexpr int getStride() const
	{
		int stride = 1;
		for(const int& i : dims) {
			stride *= i;
		}
		return stride;
	}

public:
	TensorRef() = delete;
	TensorRef(const TensorRef&) = delete;
	TensorRef(TensorRef&&) = delete;
	TensorRef& operator=(const TensorRef&) = delete;
	TensorRef& operator=(TensorRef&&) = delete;
	~TensorRef() noexcept = default;

	TensorRef(value_type* location, int (&in_dims)[D]) : storage_ref(location),
		dims(in_dims)
	{}

	inline element_type operator[](int i)
	{
		assert(i < dims[0]);
		return element_type(storage_ref + i*getStride(),
			reinterpret_cast<int(&)[D-1]>(dims[1]));
	}

	inline const element_type operator[](int i) const
	{
		assert(i < dims[0]);
		return element_type(storage_ref + i*getStride(),
			reinterpret_cast<int(&)[D-1]>(dims[1]));
	}
};

template<typename T>
class TensorRef<T, 2> : TensorBase<TensorRef<T, 2>, 2> {
	using value_type = T;
	typedef TensorRef<T, 1> element_type;

private:
	value_type* storage_ref;
	int& dims[2];

	constexpr int getStride() const
	{
		return dims[0]*dims[1];
	}

public:
	TensorRef() = delete;
	TensorRef(const TensorRef&) = default;
	TensorRef(TensorRef&&) = default;
	TensorRef& operator=(const TensorRef&) = default;
	TensorRef& operator=(TensorRef&&) = default;
	~TensorRef() = default;

	TensorRef(value_type* location, int (&in_dims)[2]) : storage_ref(location),
		dims(in_dims)
	{}

	inline element_type operator[](int i)
	{
		assert(i < dims[0]);
		return element_type(storage_ref + i*getStride(), dims[1]);
	}

	inline const_element_type operator[](int i) const
	{
		assert(i < dims[0]);
		return element_type(storage_ref + i*getStride(), dims[1]);
	}
};

template<typename T>
class TensorRef<T, 1> : TensorBase<TensorRef<T, 1>, 1> {
	using value_type = T;
	typedef &value_type element_type;
	typedef const &value_type const_element_type;

private:
	value_type* storage_ref;
	int dims;

public:
	TensorRef() = delete;
	TensorRef(const TensorRef&) = delete;
	TensorRef(TensorRef&&) = delete;
	TensorRef& operator=(const TensorRef&) = delete;
	TensorRef& operator=(TensorRef&&) = delete;
	~TensorRef() noexcept = default;

	TensorRef(value_type* location, int in_dims) : storage_ref(location),
		dims(in_dims)
	{}

	inline element_type operator[](int i)
	{
		assert(i < dims);
		return storage_ref[i];
	}

	inline const_element_type operator[](int i) const
	{
		assert(i < dims);
		return storage_ref[i];
	}
};

#endif // TENSOR_REF_HPP
