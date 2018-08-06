#include <cassert>
#include <type_traits>
#include <array>

#ifndef AV_ARRAY_HPP
#define AV_ARRAY_HPP

template<typename Arg,
	typename std::enable_if<std::is_integral<Arg>::value>::type * = nullptr>
constexpr int mult(Arg n1, Arg n2) { return n1*n2; }

template<typename Arg, typename ...Ts,
	typename std::enable_if<std::is_integral<Arg>::value>::type * = nullptr>
constexpr int mult(Arg n1, Ts... rest)
{
	return n1*mult(rest...);
}

template<typename T, int D>
class AVArray
{
	typedef T ValueType;
	typedef AVArray<T, D - 1> ElementType;
	typedef std::array<int, D> Shape;
	typedef std::array<int, D - 1> SubShape;

private:
	Shape dims;
	SubShape sub_dims;
	ValueType* storage;
	int element_stride;
	bool owner;

	inline SubShape makeSubShape(Shape in_shape) const
	{
		SubShape out_shape;
		for (int i = 0; i < in_shape.size() - 1; ++i)
		{
			out_shape[i] = in_shape[i + 1];
		}
		return out_shape;
	}

	inline int getStride(SubShape in_shape) const
	{
		int stride = 1;
		for(auto&& i : in_shape) {
			stride *= i;
		}
		return stride;
	}

public:
	AVArray() : dims{0}, sub_dims{0}, storage(nullptr), element_stride(0),
		owner(false)
	{}

	template<typename ...Ints>
	AVArray(int n1, Ints... nD) : dims{n1, nD...},
		sub_dims{nD...}, storage(new ValueType[mult(n1, nD...)]),
		element_stride(mult(1, nD...)), owner(true)
	{
		static_assert(1 + sizeof...(nD) == D,
			"Dimension of variadic constructor is wrong.");
		assert(mult(n1, nD...) > 0);
	}

	AVArray(ValueType* location, Shape new_dims) : dims(new_dims),
		sub_dims(makeSubShape(new_dims)), storage(location),
		element_stride(getStride(sub_dims)), owner(false)
	{}

	AVArray(const AVArray &other) : dims(other.dims), sub_dims(other.sub_dims),
		storage(new ValueType[getSize(other.dims)]),
		element_stride(other.element_stride), owner(true)
	{
		for (int i = 0; i < getSize(dims); ++i)
		{
			storage[i] = other.storage[i];
		}
	}

	AVArray(AVArray &&other) : dims(other.dims), sub_dims(other.sub_dims),
		storage(other.storage), element_stride(other.element_stride), owner(other.owner)
	{
		other.owner = false;
	}

	AVArray& operator=(const AVArray &other)
	{
		AVArray temp(other);
		*this = std::move(temp);
		return *this;
	}

	AVArray& operator=(AVArray &&other)
	{
		if (owner)
		{
			delete[] storage;
		}
		dims = other.dims;
		sub_dims = other.sub_dims;
		storage = other.storage;
		element_stride = other.element_stride;
		owner = other.owner;
		other.owner = false;
		return *this;
	}

	virtual ~AVArray() noexcept
	{
		if (owner)
		{
			delete[] storage;
		}
	}

	inline ElementType operator[](int i)
	{
		assert(i < dims[0]);
		return AVArray<T, D - 1>(storage + i*element_stride, sub_dims);
	}

	inline const ElementType operator[](int i) const
	{
		assert(i < dims[0]);
		return AVArray<T, D - 1>(storage + i*element_stride, sub_dims);
	}

	template<typename ...Ints>
	inline ValueType& operator()(Ints... nD)
	{
		static_assert(sizeof...(nD) == D,
			"Dimension of variadic accessor is wrong.");
		Shape indices{nD...};
		assert(indices[0] < dims[0]);
		int offset = indices[0];
		for (int i = 1; i < D; ++i)
		{
			assert(indices[i] < dims[i]);
			offset = offset*dims[i-1] + indices[i];
		}
		return storage[offset];
	}

	template<typename ...Ints>
	inline const ValueType& operator()(Ints... nD) const
	{
		static_assert(sizeof...(nD) == D,
			"Dimension of variadic accessor is wrong.");
		Shape indices{nD...};
		assert(indices[0] < dims[0]);
		int offset = indices[0];
		for (int i = 1; i < D; ++i)
		{
			assert(indices[i] < dims[i]);
			offset = offset*dims[i] + indices[i];
		}
		return storage[offset];
	}

	inline ValueType& data(int i)
	{
		return storage[i];
	}

	inline const ValueType& data(int i) const
	{
		return storage[i];
	}

	inline Shape shape() const { return dims; }

	inline int size() const { return element_stride*dims[0]; }
};

template<typename T>
class AVArray<T, 1>
{
	typedef T ValueType;
	typedef T& ElementType;
	typedef const T& ConstElementType;
	typedef std::array<int, 1> Shape;

private:
	Shape dims;
	ValueType* storage;
	const int element_stride = 1;
	bool owner;

public:
	AVArray() : dims{0}, storage(nullptr), owner(false) {}

	AVArray(int n1) : dims{n1}, storage(new ValueType[n1]), owner(true)
	{
		assert(n1 > 0);
	}

	AVArray(ValueType* location, Shape new_dims) : dims(new_dims),
		storage(location), element_stride(1), owner(false)
	{}

	AVArray(Shape new_dims) : dims(new_dims),
		storage(new ValueType[new_dims[0]]), element_stride(1), owner(true)
	{}

	AVArray(const AVArray &other) : dims(other.dims),
		storage(new ValueType[other.dims[0]]),
		element_stride(other.element_stride), owner(true)
	{
		for (int i = 0; i < dims[0]; ++i)
		{
			storage[i] = other.storage[i];
		}
	}

	AVArray(AVArray &&other) : dims(other.dims),
		storage(other.storage), element_stride(other.element_stride), owner(other.owner)
	{
		other.owner = false;
	}

	AVArray& operator=(const AVArray &other)
	{
		AVArray temp(other);
		*this = std::move(temp);
		return this;
	}

	AVArray& operator=(AVArray &&other)
	{
		if (owner)
		{
			delete[] storage;
		}
		dims = other.dims;
		storage = other.storage;
		element_stride = other.element_stride;
		owner = other.owner;
		other.owner = false;
		return *this;
	}

	virtual ~AVArray() noexcept
	{
		if (owner)
		{
			delete[] storage;
		}
	}

	inline ElementType operator[](int i)
	{
		assert(i < dims[0]);
		return storage[i];
	}

	inline ConstElementType operator[](int i) const
	{
		assert(i < dims[0]);
		return storage[i];
	}

	inline ElementType operator()(int i)
	{
		assert(i < dims[0]);
		return storage[i];
	}

	inline ConstElementType operator()(int i) const
	{
		assert(i < dims[0]);
		return storage[i];
	}

	inline ValueType& data(int i)
	{
		return storage[i];
	}

	inline const ValueType& data(int i) const
	{
		return storage[i];
	}

	inline Shape shape() const { return dims; }

	inline int size() const { return element_stride*dims[0]; }
};

#endif // AV_ARRAY_HPP
