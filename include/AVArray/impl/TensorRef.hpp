#include "TensorBase.hpp"
#include <cassert>
#include <array>
#include <algorithm>
#include <utility>

#ifndef TENSOR_REF_HPP
#define TENSOR_REF_HPP

template<typename T, int D>
class TensorRef : TensorBase<TensorRef<T, D>, D> {
	typedef T value_type;
	typedef TensorRef<T, D - 1> element_type;
	typedef std::array<int, D> shape_type;
	typedef std::array<int, D - 1> sub_shape_type;

private:
	value_type* storage_ref;
	shape_type dims;

	constexpr int getStride() const
	{
		int stride = 1;
		for (int i = 1; i < D; ++i)
		{
			stride *= dims[i];
		}
		return stride;
	}

	sub_shape_type makeSubShape() const
	{
		sub_shape_type sub_shape;
		std::copy(dims.cbegin() + 1, dims.cend(), sub_shape.begin());
		return sub_shape;
	}

public:
	TensorRef() = delete;
	TensorRef(const TensorRef&) = default;
	TensorRef(TensorRef&&) = default;
	TensorRef& operator=(const TensorRef&) = default;
	TensorRef& operator=(TensorRef&&) = default;
	~TensorRef() = default;

	TensorRef(value_type* location, shape_type in_dims) : storage_ref(location),
		dims(std::forward<shape_type>(in_dims))
	{}

	inline element_type operator[](int i)
	{
		assert(i < dims[0]);
		return element_type(storage_ref + i*getStride(), makeSubShape());
	}

	inline const element_type operator[](int i) const
	{
		assert(i < dims[0]);
		return element_type(storage_ref + i*getStride(), makeSubShape());
	}
};

template<typename T>
class TensorRef<T, 1> : TensorBase<TensorRef<T, 1>, 1> {
	typedef T value_type;
	typedef std::array<int, 1> shape_type;

private:
	value_type* storage_ref;
	shape_type dims;

public:
	TensorRef() = delete;
	TensorRef(const TensorRef&) = default;
	TensorRef(TensorRef&&) = default;
	TensorRef& operator=(const TensorRef&) = default;
	TensorRef& operator=(TensorRef&&) = default;
	~TensorRef() = default;

	TensorRef(value_type* location, shape_type in_dims) : storage_ref(location),
		dims(std::forward<shape_type>(in_dims))
	{}

	inline value_type& operator[](int i)
	{
		assert(i < dims[0]);
		return storage_ref[i];
	}

	inline const value_type& operator[](int i) const
	{
		assert(i < dims[0]);
		return storage_ref[i];
	}
};

#endif // TENSOR_REF_HPP
