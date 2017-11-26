#include "Tensor.hpp"

#ifndef TENSOR_ET_HPP
#define TENSOR_ET_HPP

template<typename Destination, typename Source, int D>
class TensorAssign : public TensorBase<TensorAssign<Destination, Source, D>, D>
{
public:
	using value_type = typename TensorAssign<Destination, Source, D - 1>::value_type;
	using element_type = TensorAssign<Destination, Source, D - 1>;
	using destination_element_type = typename Destination::element_type;
	using source_element_type = typename Source::element_type;
public:
	const Destination &destination;
	const Source &source;

public:
	TensorAssign(const Destination &_dest, const Source &_source) :
		destination(_dest), source(_source)
	{}

	TensorAssign<destination_element_type, source_element_type, D - 1>
	operator[](int i)
	{
		return TensorAssign<destination_element_type, source_element_type,
							D - 1>(destination[i], source[i]);
	}

};

template<typename Destination, typename Source>
class TensorAssign<Destination, Source, 1> :
	public TensorBase<TensorAssign<Destination, Source, 1>, 1>
{
public:
	using value_type = void;
protected:
	const Destination &destination;
	const Source &source;

public:
	value_type operator[](int i)
	{
		destination[i] = source[i];
	}

};

template<typename T_A, typename T_B, int D>
class TensorSum : public TensorBase<TensorSum<T_A, T_B, D>, D>
{
public:
	using value_type = typename TensorSum<T_A, T_B, D - 1>::value_type;
	using element_type = TensorSum<T_A, T_B, D - 1>;
	using element_type_A = typename T_A::element_type;
	using element_type_B = typename T_B::element_type;
public:
	const T_A &A;
	const T_B &B;

public:
	TensorSum(const T_A &_A, const T_B &_B) : A(_A), B(_B) {}

	TensorSum<element_type_A, element_type_B, D - 1> operator[](int i)
	{
		return TensorSum<element_type_A, element_type_B, D - 1>(A[i], B[i]);
	}

};

template<typename T_A, typename T_B>
class TensorSum<T_A, T_B, 1> : public TensorBase<TensorSum<T_A, T_B, 1>, 1>
{
public:
	using value_type = typename T_A::value_type;
public:
	const T_A &A;
	const T_B &B;

public:
	value_type operator[](int i)
	{
		return A[i] + B[i];
	}

};


template<typename ExpressionType, int D>
class TensorEvaluator : public TensorBase<TensorEvaluator<ExpressionType, D>, D>
{
public:
	using value_type = typename ExpressionType::value_type;
	using element_type = typename ExpressionType::element_type;
protected:
	const ExpressionType& expression;

public:
	TensorEvaluator<element_type, D - 1> operator[](int i)
	{
		return TensorEvaluator<element_type, D - 1>(expression[i]);
	}

};

template<typename ExpressionType>
class TensorEvaluator<ExpressionType, 1> :
	public TensorBase<TensorEvaluator<ExpressionType, 1>, 1>
{
public:
	using value_type = typename ExpressionType::value_type;
protected:
	const ExpressionType& expression;

public:
	value_type operator[](int i)
	{
		return expression[i];
	}
};

template<typename Destination, typename Source, int D>
class TensorEvaluator<TensorAssign<Destination, Source, D>, D> :
	public TensorBase<TensorEvaluator<TensorAssign<Destination, Source, D>, D>, D>
{
public:
protected:
	TensorEvaluator<Destination, D> evalDest;
	TensorEvaluator<Source, D> evalSource;

public:
	TensorEvaluator<TensorAssign<Destination, Source, D>, D>(
		const Destination &_dest, const Source &_source) :
		evalDest(_dest), evalSource(_source)
	{}

	void assign()
	{
		for (int i = 0; i < evalDest.getSize(); ++i)
		{
			evalDest.data(i) = evalSource.data(i);
		}
	}

};

template<typename T_A, typename T_B, int D>
class TensorEvaluator<TensorSum<T_A, T_B, D>, D> :
	public TensorBase<TensorEvaluator<TensorSum<T_A, T_B, D>, D>, D>
{
public:
	using value_type = typename T_A::value_type;
protected:
	const TensorSum<T_A, T_B, D>& expression;
	TensorEvaluator<T_A, D> evalA = expression.A;
	TensorEvaluator<T_B, D> evalB = expression.B;

public:
	TensorEvaluator<TensorSum<T_A, T_B, D>, D>(
		const TensorSum<T_A, T_B, D> &_expr) :
		expression(_expr)
	{}

	value_type data(int i)
	{
		return evalA.data(i) + evalB.data(i);
	}

};

template<typename T, int D>
class TensorEvaluator<Tensor<T, D>, D> :
	public TensorBase<TensorEvaluator<Tensor<T, D>, D>, D>
{
public:
	using value_type = typename Tensor<T, D>::value_type;
protected:
	const Tensor<T, D> &tensor;

public:
	TensorEvaluator<Tensor<T, D>, D>(const Tensor<T, D> &_tensor) :
		tensor(_tensor)
	{}

	inline value_type& data(int i)
	{
		return tensor.data(i);
	}

	inline const value_type& data(int i) const
	{
		return tensor.data(i);
	}

	inline int getSize()
	{
		return tensor.getSize();
	}

};

template<typename T, int D>
class TensorEvaluator<TensorRef<T, D>, D> :
	public TensorBase<TensorEvaluator<TensorRef<T, D>, D>, D>
{
public:
	using value_type = typename TensorRef<T, D>::value_type;
protected:
	const TensorRef<T, D> &tensor;

public:
	value_type data(int i)
	{
		return tensor.storage_ref[i];
	}

};

#endif // TENSOR_ET_HPP
