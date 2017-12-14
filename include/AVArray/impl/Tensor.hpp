#include "TensorRef.hpp"
#include <algorithm>

#ifndef TENSOR_HPP
#define TENSOR_HPP

#ifndef MULT
#define MULT

template<typename Arg,
	typename std::enable_if<std::is_integral<Arg>::value>::type * = nullptr>
constexpr int mult(Arg n1, Arg n2) { return n1*n2; }

template<typename Arg, typename ...Ts,
	typename std::enable_if<std::is_integral<Arg>::value>::type * = nullptr>
constexpr int mult(Arg n1, Ts... rest)
{
	return n1*mult(rest...);
}

#endif // MULT

template<typename T, int D>
class Tensor : public TensorBase<Tensor<T, D>, D> {
public:
	typedef T value_type;
	typedef TensorRef<T, D - 1> element_type;
	typedef std::array<int, D> shape_type;
	typedef std::array<int, D - 1> sub_shape_type;

private:
	value_type* storage;
	shape_type dims;

public:
	constexpr int getStride() const
	{
		int stride = 1;
		for (int i = 1; i < D; ++i)
		{
			stride *= dims[i];
		}
		return stride;
	}

	constexpr int getSize(const shape_type& in_dims) const
	{
		int size = 1;
		for(const int& i : in_dims) {
			size *= i;
		}
		return size;
	}

	constexpr int getSize() const
	{
		int size = 1;
		for (const int& i : dims)
		{
			size *= i;
		}
		return size;
	}

	constexpr int getLeadingDim() const
	{
		return dims[0];
	}

	sub_shape_type makeSubShape() const
	{
		sub_shape_type sub_shape;
		std::copy(dims.cbegin() + 1, dims.cend(), sub_shape.begin());
		return sub_shape;
	}

public:
	using TensorBase<Tensor<T, D>, D>::operator=;
	using TensorBase<Tensor<T, D>, D>::operator+;

	Tensor() : storage(nullptr), dims{0} {}

	template<typename ...Ints>
	Tensor(int n1, Ints... nD) :
		storage(new value_type[mult(n1, nD...)]), dims{n1, nD...}
	{
		static_assert(1 + sizeof...(nD) == D,
			"Dimension of variadic constructor is wrong.");
		assert(mult(n1, nD...) > 0);
	}

	Tensor(const Tensor &other) : storage(new value_type[getSize(other.dims)]),
		dims(other.dims)
	{
		for (int i = 0; i < getSize(); ++i)
		{
			storage[i] = other.storage[i];
		}
	}

	Tensor(Tensor && other) : storage(other.storage), dims(other.dims)
	{
		other.storage = nullptr;
		other.dims = {0};
	}

	Tensor& operator=(const Tensor &other)
	{
		dims = other.dims;
		storage = new value_type[getSize()];
		for (int i = 0; i < getSize(); ++i)
		{
			storage[i] = other.storage[i];
		}
	}

	Tensor& operator=(Tensor &&other)
	{
		storage = other.storage;
		dims = other.dims;
		other.storage = nullptr;
		other.dims = {0};
	}

	~Tensor() noexcept
	{
		delete[] storage;
	}

	inline element_type operator[](int i)
	{
		assert(i < dims[0]);
		return element_type(storage + i*getStride(), makeSubShape());
	}

	inline const element_type operator[](int i) const
	{
		assert(i < dims[0]);
		return element_type(storage + i*getStride(), makeSubShape());
	}

	template<typename ...Ints>
	inline value_type& operator()(Ints... nD)
	{
		static_assert(sizeof...(nD) == D,
			"Dimension of variadic accessor is wrong.");
		shape_type indices{nD...};
		assert(indices[0] < dims[0]);
		int offset = indices[0];
		for (int i = 1; i < D; ++i)
		{
			assert(indices[i] < dims[i]);
			offset = offset*dims[i] + indices[i];
		}
		return storage[offset];
	}

	template<typename ...Ints>
	inline const value_type& operator()(Ints... nD) const
	{
		static_assert(sizeof...(nD) == D,
			"Dimension of variadic accessor is wrong.");
		shape_type indices{nD...};
		assert(indices[0] < dims[0]);
		int offset = indices[0];
		for (int i = 1; i < D; ++i)
		{
			assert(indices[i] < dims[i]);
			offset = offset*dims[i] + indices[i];
		}
		return storage[offset];
	}

	inline value_type& data(int i)
	{
		assert(i < getSize());
		return storage[i];
	}

	inline const value_type& data(int i) const
	{
		assert(i < getSize());
		return storage[i];
	}

	inline TensorRef<T, D> ref()
	{
		return TensorRef<T, D>(storage, dims);
	}

	inline const TensorRef<T, D> ref() const
	{
		return TensorRef<T, D>(storage, dims);
	}
};

template<typename T>
class Tensor<T, 1> : public TensorBase<Tensor<T, 1>, 1> {
public:
	typedef T value_type;
	typedef std::array<int, 1> shape_type;

private:
	value_type* storage;
	shape_type dims;

public:
	Tensor() : storage(nullptr), dims{0} {}

	Tensor(int n1) :
		storage(new value_type[n1]), dims{n1}
	{
		assert(n1 > 0);
	}

	Tensor(const Tensor &other) : storage(new value_type[other.dims[0]]),
		dims(other.dims)
	{
		for (int i = 0; i < dims[0]; ++i)
		{
			storage[i] = other.storage[i];
		}
	}

	Tensor(Tensor && other) : storage(other.storage), dims(other.dims)
	{
		other.storage = nullptr;
		other.dims = {0};
	}

	Tensor& operator=(const Tensor &other)
	{
		dims = other.dims;
		storage = new value_type[dims[0]];
		for (int i = 0; i < dims[0]; ++i)
		{
			storage[i] = other.storage[i];
		}
	}

	Tensor& operator=(Tensor &&other)
	{
		storage = other.storage;
		dims = other.dims;
		other.storage = nullptr;
		other.dims = {0};
	}

	~Tensor() noexcept
	{
		delete[] storage;
	}

	inline value_type& operator[](int i)
	{
		assert(i < dims[0]);
		return storage[i];
	}

	inline const value_type& operator[](int i) const
	{
		assert(i < dims[0]);
		return storage[i];
	}

	inline value_type& operator()(int i)
	{
		assert(i < dims[0]);
		return storage[i];
	}

	inline const value_type& operator()(int i) const
	{
		assert(i < dims[0]);
		return storage[i];
	}
};

#endif // TENSOR_HPP
