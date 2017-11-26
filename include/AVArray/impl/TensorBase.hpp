#include "TensorET_forward.hpp"

#ifndef TENSOR_BASE_HPP
#define TENSOR_BASE_HPP

template<typename Derived, int D>
class TensorBase {
public:
	template<typename OtherDerived>
	Derived& operator=(const TensorBase<OtherDerived, D> &other)
	{
		TensorEvaluator<TensorAssign<Derived, OtherDerived, D>, D>
			evaluator(const_derived(), other.derived());
		evaluator.assign();
		return derived();
	}

	template<typename OtherDerived>
	TensorSum<Derived, OtherDerived, D>
	operator+(const TensorBase<OtherDerived, D> &other)
	{
		return TensorSum<Derived, OtherDerived, D>(const_derived(), other.derived());
	}

public:
	Derived& derived() { return static_cast<Derived&>(*this); }

	const Derived& derived() const { return static_cast<const Derived&>(*this); }

	const Derived& const_derived() const { return static_cast<const Derived&>(*this); }

	constexpr int dimension() { return D; }

};

#endif // TENSOR_BASE_HPP
