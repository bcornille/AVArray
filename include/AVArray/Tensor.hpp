#include <boost/yap/yap.hpp>
#include "AVArray.hpp"

template<boost::yap::expr_kind Kind, typename Tuple>
struct TensorExpr;

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

    auto operator[](int i) const
    { return boost::yap::evaluate(boost::yap::transform(*this, TakeSlice{i})); }

    auto data(int i) const
    { return boost::yap::evaluate(boost::yap::transform(*this, TakeNth{i})); }
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

	using TensorExpr<
			boost::yap::expr_kind::terminal,
			boost::hana::tuple<AVArray<T, D>>
			>::operator[];

	using TensorExpr<
			boost::yap::expr_kind::terminal,
			boost::hana::tuple<AVArray<T, D>>
			>::data;
};
