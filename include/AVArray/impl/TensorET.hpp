#ifndef TENSOR_ET_HPP
#define TENSOR_ET_HPP

template<typename Derived, int D>
class TensorBase;

template<typename ExpressionType, int D>
class TensorEvaluator : TensorBase<TensorEvaluator, D>
{
	using element_type = ExpressionType::element_type;
	const ExpressionType &expression;

public:
	TensorEvaluator<element_type, D - 1> operator[](int i)
	{
		return TensorEvaluator<element_type, D - 1>(expression[i]);
	}

};

template<typename ExpressionType>
class TensorEvaluator<ExpressionType, 1> :
TensorBase<TensorEvaluator<ExpressionType, 1>, 1>
{
	using value_type = ExpressionType::value_type;
	const ExpressionType &expression;

public:
	value_type operator[](int i)
	{
		return expression[i];
	}

};

template<typename Destination, typename Source>
class TensorAssign : TensorBase<TensorAssign>
{
	using destination_element_type = Destination::element_type;
	using source_element_type = Source::element_type;
	const Destination &destination;
	const Source &source;

public:
	TensorAssign<destination_element_type, source_element_type, D - 1>
	operator[](int i)
	{
		return TensorAssign<destination_element_type, source_element_type,
							D - 1>(destination[i], source[i]);
	}

};

template<typename T_A, typename T_B, int D>
class TensorSum : TensorBase<TensorSum, D>
{
	using element_type_A = T_A::element_type;
	using element_type_B = T_B::element_type;
	const T_A &A;
	const T_B &B;

public:
	TensorSum<element_type_A, element_type_B, D - 1> operator[](int i)
	{
		return TensorSum<element_type_A, element_type_B, D - 1>(A[i], B[i]);
	}

};

template<typename T_A, typename T_B>
class TensorSum<T_A, T_B, 1> : TensorBase<TensorSum<T_A, T_B, 1>, 1>
{
	using value_type = T_A::value_type;
	const T_A &A;
	const T_B &B;

public:
	value_type operator[](int i)
	{
		return A[i] + B[i];
	}

};

#endif // TENSOR_ET_HPP