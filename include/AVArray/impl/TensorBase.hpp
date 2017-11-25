#include "TensorET.hpp"

#ifndef TENSOR_BASE_HPP
#define TENSOR_BASE_HPP

template<typename Derived, int D>
class TensorBase {
public:
	template<typename OtherDerived>
	Sum<Derived, OtherDerived, D>
	operator+(const TensorBase<OtherDerived>& other)
	{
		return Sum<Derived, OtherDerived, D>(derived(), other.derived());
	}

private:
	Derived& derived() { return static_cast<Derived&>(*this); }

	constexpr int dimension() { return D; }

};

#endif // TENSOR_BASE_HPP
