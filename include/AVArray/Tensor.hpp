#include <boost/yap/yap.hpp>
#include "AVArray.hpp"

template<boost::yap::expr_kind Kind, typename Tuple>
struct TensorExpr;

template<typename Expr1, typename Expr2>
struct TensorProduct;

template<typename T, int D>
struct Tensor;

struct TakeNth
{
	template<typename T, int D>
	boost::yap::terminal<TensorExpr, T>
	operator()(const boost::yap::terminal<TensorExpr, AVArray<T, D>> &expr)
	{
		T x = boost::yap::value(expr).data(n);
		return boost::yap::make_terminal<TensorExpr, T>(std::move(x));
	}

	int n;
};

struct TakeSlice
{
	template<typename T, int D>
	boost::yap::terminal<TensorExpr, AVArray<T, D - 1>>
	operator()(const boost::yap::terminal<TensorExpr, AVArray<T, D>> &expr)
	{
		auto x = boost::yap::value(expr)[n];
		return boost::yap::make_terminal<TensorExpr, AVArray<T, D - 1>>(std::move(x));
	}

	template<typename T>
	boost::yap::terminal<TensorExpr, T>
	operator()(const boost::yap::terminal<TensorExpr, AVArray<T, 1>> &expr)
	{
		auto x = boost::yap::value(expr)[n];
		return boost::yap::make_terminal<TensorExpr, T>(std::move(x));
	}

	int n;
};

struct GetShape
{
	template<typename T, int D>
	std::array<int, D>
	operator()(const boost::yap::terminal<TensorExpr, AVArray<T, D>> &expr)
	{
		return boost::yap::value(expr).shape();
	}
};

struct inner_product_tag {};

template<boost::yap::expr_kind RightKind, typename RightTuple>
struct InnerProductTransform
{
	template<boost::yap::expr_kind LeftKind, typename LeftTuple>
	auto operator()(const TensorExpr<LeftKind, LeftTuple> &left_expr)
	{ return boost::yap::make_expression<TensorExpr,
		boost::yap::expr_kind::call>(inner_product_tag{}, left_expr, right_expr); }

	const TensorExpr<RightKind, RightTuple> &right_expr;
};

template<int Dleft, int Dright>
auto combineShapes(std::array<int, Dleft> sl, std::array<int, Dright> sr)
{
	typedef std::array<int, Dleft + Dright - 2> Shape;
	Shape sout;
	for (int i = 0; i < Dleft - 1; ++i)
	{
		sout[i] = sl[i];
	}
	for (int i = 1; i < Dright; ++i)
	{
		sout[Dleft - 2 + i] = sr[i];
	}
	return sout;
}

// template<typename T, int D>
// AVArray<T, D> mult(T value, const AVArray<T, D> &tin)
// {
// 	AVArray<T, D> tout(tin.shape());
// 	for (int i = 0; i < tin.size(); ++i)
// 	{
// 		tout.data(i) = value*tin.data(i);
// 	}
// 	return tout;
// }

template<typename T, int Dleft, int Dright>
Tensor<T, Dleft + Dright - 2>
eagerInnerProduct(Tensor<T, Dleft> tl, Tensor<T, Dright> tr)
{
	Tensor<T, Dleft + Dright - 2> tout(combineShapes<Dleft, Dright>(tl.shape(), tr.shape()));
	for (int i = 0; i < tl.shape()[0]; ++i)
	{
		tout[i] = eagerInnerProduct(tl[i], tr);
	}
	return tout;
}

template<typename T, int Dright>
Tensor<T, Dright - 1>
eagerInnerProduct(Tensor<T, 1> tl, Tensor<T, Dright> tr)
{
	Tensor<T, Dright - 1> tout(combineShapes<1, Dright>(tl.shape(), tr.shape()));
	for (int i = 0; i < tl.shape()[0]; ++i)
	{
		tout += tl[i]*tr[i];
	}
	return tout;
}

template<typename T>
T eagerInnerProduct(Tensor<T, 1> tl, Tensor<T, 1> tr)
{
	T tout{};
	for (int i = 0; i < tl.shape()[0]; ++i)
	{
		tout += tl[i]*tr[i];
	}
	return tout;
}

template<typename ...Ts>
auto eval_call(inner_product_tag, Ts && ...t)
{
	return eagerInnerProduct(t...);
}

// template<typename T, int DRight, int DLeft>
// boost::yap::terminal<TensorExpr, AVArray<T, DRight + DLeft - 2>>
// eval_call(inner_product_tag, Tensor<T, DLeft> left_expr, Tensor<T, DRight> right_expr)
// {
// 	AVArray<T, DRight + DLeft - 2> tout(combineShapes(left_expr.shape(), right_expr.shape()), 0.0);
// 	for (int i = 0; i < tout.shape()[0]; ++i)
// 	{
// 		tout[i] = left_expr[i].innerProduct(right_expr);
// 	}
// 	return boost::yap::make_terminal<TensorExpr, AVArray<T, DRight + DLeft - 2>>(std::move(tout));
// }

// template<typename T, int DRight>
// boost::yap::terminal<TensorExpr, AVArray<T, DRight - 1>>
// eval_call(inner_product_tag, Tensor<T, 1> left_expr, Tensor<T, DRight> right_expr)
// {
// 	AVArray<T, DRight - 1> tout(combineShapes(left_expr.shape(), right_expr.shape()), 0.0);
// 	for (int i = 0; i < tout.shape()[0]; ++i)
// 	{
// 		tout += left_expr[i]*right_expr[i];
// 	}
// 	return boost::yap::make_terminal<TensorExpr, AVArray<T, DRight - 1>>(std::move(tout));
// }

// struct InnerProduct
// {
// 	template<typename T, int DRight, int DLeft>
// 	boost::yap::terminal<TensorExpr, AVArray<T, DRight + DLeft - 2>>
// 	operator()(boost::yap::call_tag, inner_product_tag, Tensor<T, DLeft> left_expr, Tensor<T, DRight> right_expr)
// 	{
// 		AVArray<T, DRight + DLeft - 2> tout(combineShapes(left_expr.shape(), right_expr.shape()), 0.0);
// 		for (int i = 0; i < tout.shape()[0]; ++i)
// 		{
// 			tout[i] = left_expr[i].innerProduct(right_expr);
// 		}
// 		return boost::yap::make_terminal<TensorExpr, AVArray<T, DRight + DLeft - 2>>(std::move(tout));
// 	}

// 	template<typename T, int DRight>
// 	boost::yap::terminal<TensorExpr, AVArray<T, DRight - 1>>
// 	operator()(boost::yap::call_tag, inner_product_tag, Tensor<T, 1> left_expr, Tensor<T, DRight> right_expr)
// 	{
// 		AVArray<T, DRight - 1> tout(combineShapes(left_expr.shape(), right_expr.shape()), 0.0);
// 		for (int i = 0; i < tout.shape()[0]; ++i)
// 		{
// 			tout += left_expr[i]*right_expr[i];
// 		}
// 		return boost::yap::make_terminal<TensorExpr, AVArray<T, DRight - 1>>(std::move(tout));
// 	}
// };

// template<boost::yap::expr_kind Kind, typename Tuple>
// struct InnerProduct
// {
// 	template<typename T, int DRight, int DLeft>
// 	boost::yap::terminal<TensorExpr, AVArray<T, DRight + DLeft - 2>>
// 	operator()(const boost::yap::terminal<TensorExpr, AVArray<T, DLeft>> &left_expr)
// 	{
// 		AVArray<T, DRight + DLeft - 2> tout(combineShapes(left_expr.shape(), right_expr.shape()), 2.0);
// 		for (int i = 0; i < tout.shape()[0]; ++i)
// 		{
// 			tout[i] = 100.0; //left_expr[i].innerProduct(right_expr);
// 		}
// 		return boost::yap::make_terminal<TensorExpr, AVArray<T, DRight + DLeft - 2>>(std::move(tout));
// 	}

// 	template<typename T, int DRight>
// 	boost::yap::terminal<TensorExpr, AVArray<T, DRight - 1>>
// 	operator()(const boost::yap::terminal<TensorExpr, AVArray<T, 1>> &left_expr)
// 	{
// 		AVArray<T, DRight - 1> tout(combineShapes(left_expr.shape(), right_expr.shape()), 3.0);
// 		for (int i = 0; i < tout.shape()[0]; ++i)
// 		{
// 			tout += 1.0; //left_expr[i]*right_expr[i];
// 		}
// 		return boost::yap::make_terminal<TensorExpr, AVArray<T, DRight - 1>>(std::move(tout));
// 	}

// 	const TensorExpr<Kind, Tuple> &right_expr;
// };

template<boost::yap::expr_kind Kind, typename Tuple>
struct TensorExpr
{
	// static_assert(
	// 	Kind != boost::yap::expr_kind::terminal ||
	// 	std::is_same<Tuple, boost::hana::tuple<AVArray>);

	static constexpr boost::yap::expr_kind kind = Kind;

	Tuple elements;

	BOOST_YAP_USER_BINARY_OPERATOR_MEMBER(plus, ::TensorExpr)
    BOOST_YAP_USER_BINARY_OPERATOR_MEMBER(minus, ::TensorExpr)
    BOOST_YAP_USER_BINARY_OPERATOR_MEMBER(multiplies, ::TensorExpr)
    BOOST_YAP_USER_BINARY_OPERATOR_MEMBER(divides, ::TensorExpr)

    // template<boost::yap::expr_kind OtherKind, typename OtherTuple>
    // auto innerProduct(const TensorExpr<OtherKind, OtherTuple> &other)
    // { return boost::yap::evaluate(boost::yap::transform(*this, InnerProduct<OtherKind, OtherTuple>{other})); }

    // template<boost::yap::expr_kind OtherKind, typename OtherTuple>
    // auto innerProduct(const TensorExpr<OtherKind, OtherTuple> &other)
    // {
    // 	return boost::yap::evaluate(
    // 		boost::yap::transform(*this, InnerProductTransform<OtherKind, OtherTuple>{other}));
    // }

    auto operator[](int i) const
    { return boost::yap::evaluate(boost::yap::transform(*this, TakeSlice{i})); }

    auto data(int i) const
    { return boost::yap::evaluate(boost::yap::transform(*this, TakeNth{i})); }

    auto shape() const
    { return boost::yap::evaluate(boost::yap::transform(*this, GetShape{})); }
};

BOOST_YAP_USER_FREE_BINARY_OPERATOR(multiplies, ::TensorExpr)
BOOST_YAP_USER_FREE_BINARY_OPERATOR(divides, ::TensorExpr)

template<typename T, int D>
struct Tensor :
	TensorExpr<
		boost::yap::expr_kind::terminal,
		boost::hana::tuple<AVArray<T, D>>
		>
{
	Tensor()
	{}

	template<typename ...Ints>
	Tensor(int n1, Ints... nD)
	{
		this->elements = boost::hana::tuple<AVArray<T, D>>(AVArray<T, D>(n1, nD...));
	}

	explicit Tensor(AVArray<T, D> && tensor)
	{ this->elements = boost::hana::tuple<AVArray<T, D>>(std::move(tensor)); }

	Tensor& operator=(AVArray<T, D> && tensor)
	{ this->elements = boost::hana::tuple<AVArray<T, D>>(std::move(tensor)); }

	template<boost::yap::expr_kind Kind, typename Tuple>
	Tensor& operator=(const TensorExpr<Kind, Tuple> & rhs)
	{
		AVArray<T, D> &this_avarray = boost::yap::value(*this);
		for (int i = 0; i < this_avarray.size(); ++i)
		{
			this_avarray.data(i) = rhs.data(i);
		}
		return *this;
	}

	template<boost::yap::expr_kind Kind, typename Tuple>
	Tensor& operator+=(const TensorExpr<Kind, Tuple> & rhs)
	{
		AVArray<T, D> &this_avarray = boost::yap::value(*this);
		for (int i = 0; i < this_avarray.size(); ++i)
		{
			this_avarray.data(i) += rhs.data(i);
		}
		return *this;
	}

	template<int Din>
	Tensor<T, D + Din - 2> innerProduct(const Tensor<T, Din> &other)
	{
		Tensor<T, D + Din - 2> tout(combineShapes<D, Din>(this->shape(), other.shape()));
		for (int i = 0; i < tout.shape()[0]; ++i)
		{
			tout[i] = (*this)[i]*other;
		}
		return tout;
	}

	using TensorExpr<
			boost::yap::expr_kind::terminal,
			boost::hana::tuple<AVArray<T, D>>
			>::operator[];

	using TensorExpr<
			boost::yap::expr_kind::terminal,
			boost::hana::tuple<AVArray<T, D>>
			>::data;

	using TensorExpr<
			boost::yap::expr_kind::terminal,
			boost::hana::tuple<AVArray<T, D>>
			>::shape;
};

template<typename T>
struct Tensor<T, 1> :
	TensorExpr<
		boost::yap::expr_kind::terminal,
		boost::hana::tuple<AVArray<T, 1>>
		>
{
	Tensor()
	{}

	Tensor(int n1)
	{
		this->elements = boost::hana::tuple<AVArray<T, 1>>(AVArray<T, 1>(n1));
	}

	explicit Tensor(AVArray<T, 1> && tensor)
	{ this->elements = boost::hana::tuple<AVArray<T, 1>>(std::move(tensor)); }

	Tensor& operator=(AVArray<T, 1> && tensor)
	{ this->elements = boost::hana::tuple<AVArray<T, 1>>(std::move(tensor)); }

	template<boost::yap::expr_kind Kind, typename Tuple>
	Tensor& operator=(const TensorExpr<Kind, Tuple> & rhs)
	{
		AVArray<T, 1> &this_avarray = boost::yap::value(*this);
		for (int i = 0; i < this_avarray.size(); ++i)
		{
			this_avarray.data(i) = rhs.data(i);
		}
		return *this;
	}

	template<boost::yap::expr_kind Kind, typename Tuple>
	Tensor& operator+=(const TensorExpr<Kind, Tuple> & rhs)
	{
		AVArray<T, 1> &this_avarray = boost::yap::value(*this);
		for (int i = 0; i < this_avarray.size(); ++i)
		{
			this_avarray.data(i) += rhs.data(i);
		}
		return *this;
	}

	template<int Din>
	Tensor<T, Din - 1> innerProduct(const Tensor<T, Din> &other)
	{
		Tensor<T, Din - 2> tout(combineShapes<1, Din>(this->shape(), other.shape()));
		for (int i = 0; i < tout.shape()[0]; ++i)
		{
			tout += (*this)[i]*other[i];
		}
		return tout;
	}

	using TensorExpr<
			boost::yap::expr_kind::terminal,
			boost::hana::tuple<AVArray<T, 1>>
			>::operator[];

	using TensorExpr<
			boost::yap::expr_kind::terminal,
			boost::hana::tuple<AVArray<T, 1>>
			>::data;

	using TensorExpr<
			boost::yap::expr_kind::terminal,
			boost::hana::tuple<AVArray<T, 1>>
			>::shape;
};

// template<typename Expr1, typename Expr2>
// decltype(auto) transform_expression(const
// 	boost::yap::expression<
// 		boost::yap::expr_kind::multiplies,
// 		boost::hana::tuple<
// 			Expr1, Expr2
// 			>
// 		> &expr)
// {
// 	return boost::yap::make_terminal(eagerInnerProduct(expr.left(), expr.right()));
// }

// template<typename T, int Dleft, int Dright>
// Tensor<T, Dleft + Dright - 2>
// eval_multiplies(const Tensor<T, Dleft> &tl, const Tensor<T, Dright> &tr)
// {
// 	return eagerInnerProduct(tl, tr);
// }


