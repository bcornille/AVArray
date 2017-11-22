#include "TensorRef.hpp"

#ifndef TENSOR_HPP
#define TENSOR_HPP

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
class Tensor : TensorBase<Tensor<T, D>, D> {
	using value_type = T;
	typedef TensorRef<T, D - 1> element_type;

private:
	value_type* storage;
	int dims[D];

	constexpr int getStride() const
	{
		int stride = 1;
		for(const int& i : dims) {
			stride *= i;
		}
		return stride;
	}

public:
	Tensor() = delete;
	Tensor(const Tensor &) = delete;

	Tensor(Tensor && other)
	Tensor& operator=(const Tensor &) = delete;
	Tensor& operator=(Tensor &&) = delete;
	~Tensor() noexcept
	{
		delete[] storage;
	}
};

#endif // TENSOR_HPP
