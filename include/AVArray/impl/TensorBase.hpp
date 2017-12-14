#include "TensorET_forward.hpp"
#include <cassert>

#ifndef TENSOR_BASE_HPP
#define TENSOR_BASE_HPP

template<typename Derived, int D>
class TensorBase {
public:
	template<typename OtherDerived>
	Derived& operator=(const OtherDerived &other)
	{
		assert(derived().getLeadingDim() == other.getLeadingDim());
		for (int i = 0; i < derived().getLeadingDim(); ++i)
		{
			derived()[i] = other[i];
		}
		return derived();
	}

	template<typename OtherDerived>
	TensorSum<Derived, OtherDerived, D>
	operator+(const OtherDerived &other)
	{
		return TensorSum<Derived, OtherDerived, D>(derived(), other);
	}

public:
	Derived& derived() { return static_cast<Derived&>(*this); }

	const Derived& derived() const { return static_cast<const Derived&>(*this); }

	const Derived& const_derived() const { return static_cast<const Derived&>(*this); }

	constexpr int dimension() { return D; }

};

#endif // TENSOR_BASE_HPP
