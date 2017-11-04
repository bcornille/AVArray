#include <cassert>
#include <iterator>
#include <type_traits>
#include <array>

#ifndef AV_ARRAY_HPP
#define AV_ARRAY_HPP

template<typename Arg, typename std::enable_if<std::is_integral<Arg>::value>::type * = nullptr>
constexpr unsigned int mult(Arg n1, Arg n2) { return n1*n2; }

template<typename Arg, typename ...Ts, typename std::enable_if<std::is_integral<Arg>::value>::type * = nullptr>
constexpr unsigned int mult(Arg n1, Ts... rest)
{
	return n1*mult(rest...);
}

template<typename T, unsigned int D>
class AVArray
{
	typedef T ValueType;
	typedef AVArray<T, D - 1> ElementType;
	typedef std::array<unsigned int, D> Shape;
	typedef std::array<unsigned int, D - 1> SubShape;

private:
	Shape dims;
	SubShape sub_dims;
	ValueType* storage;
	unsigned int element_stride;
	bool owner;

	inline SubShape makeSubShape(Shape in_shape)
	{
		SubShape out_shape;
		for (unsigned int i = 0; i < in_shape.size() - 1; ++i)
		{
			out_shape[i] = in_shape[i + 1];
		}
		return out_shape;
	}

	inline unsigned int getStride(SubShape in_shape)
	{
		unsigned int stride = 1;
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
	AVArray(unsigned int n1, Ints... nD) : dims{n1, nD...},
		sub_dims{nD...}, storage(new ValueType[mult(n1, nD...)]),
		element_stride(mult(1u, nD...)), owner(true)
	{
		static_assert(1 + sizeof...(nD) == D, "Dimension of variadic constructor is wrong.");
		assert(mult(n1, nD...) > 0);
	}

	AVArray(ValueType* location, Shape new_dims) : dims(new_dims),
		sub_dims(makeSubShape(new_dims)), storage(location),
		element_stride(getStride(sub_dims)), owner(false)
	{}

	AVArray(const AVArray &other) = default;
	AVArray(AVArray &&other) = default;
	AVArray& operator=(const AVArray &other) = default;
	AVArray& operator=(AVArray &&other) = default;
	~AVArray()
	{
		if (owner)
		{
			delete[] storage;
		}
	}

	inline ElementType operator[](unsigned int i)
	{
		assert(i < dims[0]);
		return AVArray<T, D - 1>(storage + i*element_stride, sub_dims);
	}
};

template<typename T>
class AVArray<T, 1>
{
	typedef T ValueType;
	typedef T& ElementType;
	typedef std::array<unsigned int, 1> Shape;

private:
	Shape dims;
	ValueType* storage;
	const unsigned int element_stride = 1;
	bool owner;

public:
	AVArray() : dims(0), storage(nullptr), owner(false) {}

	AVArray(unsigned int n1) : dims{n1}, storage(new ValueType[n1]), owner(true)
	{
		assert(n1 > 0);
	}

	AVArray(ValueType* location, Shape new_dims) : dims(new_dims),
		storage(location), element_stride(1), owner(false)
	{}

	AVArray(const AVArray &other) = default;
	AVArray(AVArray &&other) = default;
	AVArray& operator=(const AVArray &other) = default;
	AVArray& operator=(AVArray &&other) = default;
	~AVArray()
	{
		if (owner)
		{
			delete[] storage;
		}
	}

	inline ElementType operator[](unsigned int i)
	{
		assert(i < dims[0]);
		return storage[i];
	}
};

#endif // AV_ARRAY_HPP