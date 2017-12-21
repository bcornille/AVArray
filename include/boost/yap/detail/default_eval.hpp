#ifndef BOOST_YAP_DETAIL_DEFAULT_EVAL_HPP_INCLUDED
#define BOOST_YAP_DETAIL_DEFAULT_EVAL_HPP_INCLUDED

#include <boost/yap/algorithm_fwd.hpp>
#include <boost/yap/operators.hpp>

#include <boost/hana/transform.hpp>

#include <cassert>


namespace boost { namespace yap {

    namespace detail {

        struct nonexistent_transform {};
        inline nonexistent_transform transform_expression (...) { return {}; }

        template <typename I, typename T>
        decltype(auto) eval_placeholder (I, T && arg)
        {
            static_assert(
                I::value == 1,
                "The current placeholder's index is out of bounds.  Did you perhaps call "
                "evaluate() with too few arguments?"
            );
            return static_cast<T &&>(arg);
        }

#ifdef BOOST_NO_CONSTEXPR_IF

        template <typename T, typename ...Ts>
        decltype(auto) eval_placeholder (hana::llong<1>, T && arg, Ts && ... args)
        { return static_cast<T &&>(arg); }

        template <typename I, typename T, typename ...Ts>
        decltype(auto) eval_placeholder (I, T && arg, Ts && ... args)
        { return eval_placeholder(hana::llong<I::value - 1>{}, static_cast<Ts &&>(args)...); }

#else

        template <typename I, typename T, typename ...Ts>
        decltype(auto) eval_placeholder (I, T && arg, Ts && ... args)
        {
            if constexpr (I::value == 1) {
                return static_cast<T &&>(arg);
            } else {
                return eval_placeholder(hana::llong<I::value - 1>{}, static_cast<Ts &&>(args)...);
            }
        }

#endif

        template <long long I, typename ...T>
        decltype(auto) eval_terminal (placeholder<I>, T && ... args)
        { return eval_placeholder(hana::llong_c<I>, static_cast<T &&>(args)...); }

        template <typename T, typename ...Ts>
        decltype(auto) eval_terminal (T && value, Ts && ... args)
        { return static_cast<T &&>(value); }

#ifdef BOOST_NO_CONSTEXPR_IF

        template <typename Expr, typename ...T>
        decltype(auto) default_eval_expr (Expr && expr, T && ... args);

        template <expr_kind Kind>
        struct default_eval_expr_impl;

        template <>
        struct default_eval_expr_impl<expr_kind::expr_ref>
        {
            template <typename Expr, typename ...T>
            decltype(auto) operator() (Expr && expr, T && ... args)
            { return default_eval_expr(::boost::yap::deref(static_cast<Expr &&>(expr)), static_cast<T &&>(args)...); }
        };

        template <>
        struct default_eval_expr_impl<expr_kind(-1)>
        {
            template <typename Expr, typename ...T>
            decltype(auto) operator() (Expr && expr, T && ... args)
            { return transform_expression(static_cast<Expr &&>(expr), static_cast<T &&>(args)...); }
        };

        template <>
        struct default_eval_expr_impl<expr_kind::terminal>
        {
            template <typename Expr, typename ...T>
            decltype(auto) operator() (Expr && expr, T && ... args)
            { return eval_terminal(::boost::yap::value(static_cast<Expr &&>(expr)), static_cast<T &&>(args)...); }
        };

#define BOOST_YAP_UNARY_OPERATOR_CASE(op_name)                          \
        template <>                                                     \
        struct default_eval_expr_impl<expr_kind:: op_name>              \
        {                                                               \
            template <typename Expr, typename ...T>                     \
            decltype(auto) operator() (Expr && expr, T && ... args)     \
            {                                                           \
                using namespace hana::literals;                         \
                return eval_ ## op_name(                                \
                    default_eval_expr(static_cast<Expr &&>(expr).elements[0_c], static_cast<T &&>(args)...) \
                );                                                      \
            }                                                           \
        };

        BOOST_YAP_UNARY_OPERATOR_CASE(unary_plus) // +
        BOOST_YAP_UNARY_OPERATOR_CASE(negate) // -
        BOOST_YAP_UNARY_OPERATOR_CASE(dereference) // *
        BOOST_YAP_UNARY_OPERATOR_CASE(complement) // ~
        BOOST_YAP_UNARY_OPERATOR_CASE(address_of) // &
        BOOST_YAP_UNARY_OPERATOR_CASE(logical_not) // !
        BOOST_YAP_UNARY_OPERATOR_CASE(pre_inc) // ++
        BOOST_YAP_UNARY_OPERATOR_CASE(pre_dec) // --
        BOOST_YAP_UNARY_OPERATOR_CASE(post_inc) // ++(int)
        BOOST_YAP_UNARY_OPERATOR_CASE(post_dec) // --(int)

#undef BOOST_YAP_UNARY_OPERATOR_CASE

#define BOOST_YAP_BINARY_OPERATOR_CASE(op_name)                         \
        template <>                                                     \
        struct default_eval_expr_impl<expr_kind:: op_name>              \
        {                                                               \
            template <typename Expr, typename ...T>                     \
            decltype(auto) operator() (Expr && expr, T && ... args)     \
            {                                                           \
                using namespace hana::literals;                         \
                return eval_ ## op_name(                                \
                    default_eval_expr(static_cast<Expr &&>(expr).elements[0_c], static_cast<T &&>(args)...), \
                    default_eval_expr(static_cast<Expr &&>(expr).elements[1_c], static_cast<T &&>(args)...) \
                );                                                      \
            }                                                           \
        };

        BOOST_YAP_BINARY_OPERATOR_CASE(shift_left) // <<
        BOOST_YAP_BINARY_OPERATOR_CASE(shift_right) // >>
        BOOST_YAP_BINARY_OPERATOR_CASE(multiplies) // *
        BOOST_YAP_BINARY_OPERATOR_CASE(divides) // /
        BOOST_YAP_BINARY_OPERATOR_CASE(modulus) // %
        BOOST_YAP_BINARY_OPERATOR_CASE(plus) // +
        BOOST_YAP_BINARY_OPERATOR_CASE(minus) // -
        BOOST_YAP_BINARY_OPERATOR_CASE(less) // <
        BOOST_YAP_BINARY_OPERATOR_CASE(greater) // >
        BOOST_YAP_BINARY_OPERATOR_CASE(less_equal) // <=
        BOOST_YAP_BINARY_OPERATOR_CASE(greater_equal) // >=
        BOOST_YAP_BINARY_OPERATOR_CASE(equal_to) // ==
        BOOST_YAP_BINARY_OPERATOR_CASE(not_equal_to) // !=
        BOOST_YAP_BINARY_OPERATOR_CASE(logical_or) // ||
        BOOST_YAP_BINARY_OPERATOR_CASE(logical_and) // &&
        BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_and) // &
        BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_or) // |
        BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_xor) // ^

        template <>
        struct default_eval_expr_impl<expr_kind::comma>
        {
            template <typename Expr, typename ...T>
            decltype(auto) operator() (Expr && expr, T && ... args)
            {
                using namespace hana::literals;
                return eval_comma(
                    default_eval_expr(static_cast<Expr &&>(expr).elements[0_c], static_cast<T &&>(args)...),
                    default_eval_expr(static_cast<Expr &&>(expr).elements[1_c], static_cast<T &&>(args)...)
                );
            }
        };

        BOOST_YAP_BINARY_OPERATOR_CASE(mem_ptr) // ->*
        BOOST_YAP_BINARY_OPERATOR_CASE(assign) // =
        BOOST_YAP_BINARY_OPERATOR_CASE(shift_left_assign) // <<=
        BOOST_YAP_BINARY_OPERATOR_CASE(shift_right_assign) // >>=
        BOOST_YAP_BINARY_OPERATOR_CASE(multiplies_assign) // *=
        BOOST_YAP_BINARY_OPERATOR_CASE(divides_assign) // /=
        BOOST_YAP_BINARY_OPERATOR_CASE(modulus_assign) // %=
        BOOST_YAP_BINARY_OPERATOR_CASE(plus_assign) // +=
        BOOST_YAP_BINARY_OPERATOR_CASE(minus_assign) // -=
        BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_and_assign) // &=
        BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_or_assign) // |=
        BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_xor_assign) // ^=
        BOOST_YAP_BINARY_OPERATOR_CASE(subscript) // []

#undef BOOST_YAP_BINARY_OPERATOR_CASE

        template <>
        struct default_eval_expr_impl<expr_kind::if_else>
        {
            template <typename Expr, typename ...T>
            decltype(auto) operator() (Expr && expr, T && ... args)
            {
                using namespace hana::literals;
                return eval_if_else(
                    default_eval_expr(static_cast<Expr &&>(expr).elements[0_c], static_cast<T &&>(args)...),
                    default_eval_expr(static_cast<Expr &&>(expr).elements[1_c], static_cast<T &&>(args)...),
                    default_eval_expr(static_cast<Expr &&>(expr).elements[2_c], static_cast<T &&>(args)...)
                );
            }
        };

        template <>
        struct default_eval_expr_impl<expr_kind::call>
        {
            template <typename Expr, typename ...T>
            decltype(auto) operator() (Expr && expr, T && ... args)
            {
                decltype(auto) expand_args = [&](auto && element) {
                    return default_eval_expr(
                        static_cast<decltype(element) &&>(element),
                        static_cast<T &&>(args)...
                    );
                };

                return hana::unpack(
                    static_cast<Expr &&>(expr).elements,
                    [expand_args](auto && ... elements) {
                        return eval_call(
                            expand_args(static_cast<decltype(elements) &&>(elements))...
                        );
                    }
                );
            }
        };

        template <typename Expr, typename ...T>
        decltype(auto) default_eval_expr (Expr && expr, T && ... args)
        {
            constexpr bool transform_exists = !std::is_same<
                decltype(transform_expression(static_cast<Expr &&>(expr), static_cast<T &&>(args)...)),
                nonexistent_transform
            >{};
            constexpr expr_kind kind = transform_exists ?
                expr_kind(-1) :
                remove_cv_ref_t<Expr>::kind;
            return default_eval_expr_impl<kind>{}(static_cast<Expr &&>(expr), static_cast<T &&>(args)...);
        }

#else // BOOST_NO_CONSTEXPR_IF

        template <typename Expr, typename ...T>
        decltype(auto) default_eval_expr (Expr && expr, T && ... args)
        {
            constexpr expr_kind kind = remove_cv_ref_t<Expr>::kind;

            using namespace hana::literals;

            if constexpr (
                !std::is_same_v<
                    decltype(transform_expression(static_cast<Expr &&>(expr), static_cast<T &&>(args)...)),
                    nonexistent_transform
                >
            ) {
                return transform_expression(static_cast<Expr &&>(expr), static_cast<T &&>(args)...);
            } else if constexpr (kind == expr_kind::expr_ref) {
                return default_eval_expr(::boost::yap::deref(static_cast<Expr &&>(expr)), static_cast<T &&>(args)...);
            } else if constexpr (kind == expr_kind::terminal) {
                return eval_terminal(::boost::yap::value(static_cast<Expr &&>(expr)), static_cast<T &&>(args)...);
            }

#define BOOST_YAP_UNARY_OPERATOR_CASE(op_name)                          \
            else if constexpr (kind == expr_kind:: op_name) {           \
                return                                                  \
                    eval_ ## op_name(                                   \
                        default_eval_expr(static_cast<Expr &&>(expr).elements[0_c], static_cast<T &&>(args)...) \
                    );                                                  \
            }

            BOOST_YAP_UNARY_OPERATOR_CASE(unary_plus) // +
            BOOST_YAP_UNARY_OPERATOR_CASE(negate) // -
            BOOST_YAP_UNARY_OPERATOR_CASE(dereference) // *
            BOOST_YAP_UNARY_OPERATOR_CASE(complement) // ~
            BOOST_YAP_UNARY_OPERATOR_CASE(address_of) // &
            BOOST_YAP_UNARY_OPERATOR_CASE(logical_not) // !
            BOOST_YAP_UNARY_OPERATOR_CASE(pre_inc) // ++
            BOOST_YAP_UNARY_OPERATOR_CASE(pre_dec) // --
            BOOST_YAP_UNARY_OPERATOR_CASE(post_inc) // ++(int)
            BOOST_YAP_UNARY_OPERATOR_CASE(post_dec) // --(int)

#undef BOOST_YAP_UNARY_OPERATOR_CASE

#define BOOST_YAP_BINARY_OPERATOR_CASE(op_name)                         \
            else if constexpr (kind == expr_kind:: op_name) {           \
                return                                                  \
                    eval_ ## op_name(                                   \
                        default_eval_expr(static_cast<Expr &&>(expr).elements[0_c], static_cast<T &&>(args)...), \
                        default_eval_expr(static_cast<Expr &&>(expr).elements[1_c], static_cast<T &&>(args)...) \
                    );                                                  \
            }

            BOOST_YAP_BINARY_OPERATOR_CASE(shift_left) // <<
            BOOST_YAP_BINARY_OPERATOR_CASE(shift_right) // >>
            BOOST_YAP_BINARY_OPERATOR_CASE(multiplies) // *
            BOOST_YAP_BINARY_OPERATOR_CASE(divides) // /
            BOOST_YAP_BINARY_OPERATOR_CASE(modulus) // %
            BOOST_YAP_BINARY_OPERATOR_CASE(plus) // +
            BOOST_YAP_BINARY_OPERATOR_CASE(minus) // -
            BOOST_YAP_BINARY_OPERATOR_CASE(less) // <
            BOOST_YAP_BINARY_OPERATOR_CASE(greater) // >
            BOOST_YAP_BINARY_OPERATOR_CASE(less_equal) // <=
            BOOST_YAP_BINARY_OPERATOR_CASE(greater_equal) // >=
            BOOST_YAP_BINARY_OPERATOR_CASE(equal_to) // ==
            BOOST_YAP_BINARY_OPERATOR_CASE(not_equal_to) // !=
            BOOST_YAP_BINARY_OPERATOR_CASE(logical_or) // ||
            BOOST_YAP_BINARY_OPERATOR_CASE(logical_and) // &&
            BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_and) // &
            BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_or) // |
            BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_xor) // ^

            else if constexpr (kind == expr_kind::comma) {
                return
                    eval_comma(
                        default_eval_expr(static_cast<Expr &&>(expr).elements[0_c], static_cast<T &&>(args)...),
                        default_eval_expr(static_cast<Expr &&>(expr).elements[1_c], static_cast<T &&>(args)...)
                    );
            }

            BOOST_YAP_BINARY_OPERATOR_CASE(mem_ptr) // ->*
            BOOST_YAP_BINARY_OPERATOR_CASE(assign) // =
            BOOST_YAP_BINARY_OPERATOR_CASE(shift_left_assign) // <<=
            BOOST_YAP_BINARY_OPERATOR_CASE(shift_right_assign) // >>=
            BOOST_YAP_BINARY_OPERATOR_CASE(multiplies_assign) // *=
            BOOST_YAP_BINARY_OPERATOR_CASE(divides_assign) // /=
            BOOST_YAP_BINARY_OPERATOR_CASE(modulus_assign) // %=
            BOOST_YAP_BINARY_OPERATOR_CASE(plus_assign) // +=
            BOOST_YAP_BINARY_OPERATOR_CASE(minus_assign) // -=
            BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_and_assign) // &=
            BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_or_assign) // |=
            BOOST_YAP_BINARY_OPERATOR_CASE(bitwise_xor_assign) // ^=
            BOOST_YAP_BINARY_OPERATOR_CASE(subscript) // []

            else if constexpr (kind == expr_kind::if_else) {
                return
                    eval_if_else(
                        default_eval_expr(static_cast<Expr &&>(expr).elements[0_c], static_cast<T &&>(args)...),
                        default_eval_expr(static_cast<Expr &&>(expr).elements[1_c], static_cast<T &&>(args)...),
                        default_eval_expr(static_cast<Expr &&>(expr).elements[2_c], static_cast<T &&>(args)...)
                    );
            }

#undef BOOST_YAP_BINARY_OPERATOR_CASE

            else if constexpr (kind == expr_kind::call) {
                decltype(auto) expand_args = [&](auto && element) {
                    return default_eval_expr(
                        static_cast<decltype(element) &&>(element),
                        static_cast<T &&>(args)...
                    );
                };

                return hana::unpack(
                    static_cast<Expr &&>(expr).elements,
                    [expand_args](auto && ... elements) {
                        return eval_call(
                            expand_args(static_cast<decltype(elements) &&>(elements))...
                        );
                    });
            } else {
                assert(false && "Unhandled expr_kind in default_evaluate!");
                return;
            }
        }

#endif // BOOST_NO_CONSTEXPR_IF


        template <typename Expr, typename Tuple, typename Transform>
        decltype(auto) transform_nonterminal (Expr const & expr, Tuple && tuple, Transform && transform);

#ifdef BOOST_NO_CONSTEXPR_IF

        template <
            typename Expr,
            typename Transform,
            expr_kind Kind,
            bool IsExprRef = Kind == expr_kind::expr_ref,
            bool IsTerminal = Kind == expr_kind::terminal,
            bool IsLvalueRef = std::is_lvalue_reference<Expr>{}
        >
        struct default_transform_expression_impl;

        template <
            typename Expr,
            typename Transform,
            expr_kind Kind,
            bool IsTerminal,
            bool IsLvalueRef
        >
        struct default_transform_expression_impl <
            Expr,
            Transform,
            Kind,
            true,
            IsTerminal,
            IsLvalueRef
        > {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                return ::boost::yap::transform(
                    ::boost::yap::deref(expr),
                    static_cast<Transform &&>(transform)
                );
            }
        };

        template <typename Expr, typename Transform, expr_kind Kind, bool IsLvalueRef>
        struct default_transform_expression_impl <
            Expr,
            Transform,
            Kind,
            false,
            true,
            IsLvalueRef
        > {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            { return static_cast<Expr &&>(expr); }
        };

        template <typename Expr, typename Transform, expr_kind Kind>
        struct default_transform_expression_impl <Expr, Transform, Kind, false, false, true>
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                return transform_nonterminal(
                    expr,
                    expr.elements,
                    static_cast<Transform &&>(transform)
                );
            }
        };

        template <typename Expr, typename Transform, expr_kind Kind>
        struct default_transform_expression_impl <Expr, Transform, Kind, false, false, false>
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                return transform_nonterminal(
                    expr,
                    std::move(expr.elements),
                    static_cast<Transform &&>(transform)
                );
            }
        };

#endif // BOOST_NO_CONSTEXPR_IF

        template <typename Expr, typename Transform, expr_arity Arity, typename = void_t<>>
        struct default_transform_expression_tag;


        // Expression-matching; attempted second.

#ifdef BOOST_NO_CONSTEXPR_IF

        template <typename Expr, typename Transform, typename = detail::void_t<>>
        struct default_transform_expression_expr
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                constexpr expr_kind kind = remove_cv_ref_t<Expr>::kind;
                return default_transform_expression_impl<Expr, Transform, kind>{}(
                    static_cast<Expr &&>(expr),
                    static_cast<Transform &&>(transform)
                );
            }
        };

#else

        template <typename Expr, typename Transform, typename = void_t<>>
        struct default_transform_expression_expr
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                constexpr expr_kind kind = remove_cv_ref_t<Expr>::kind;
                if constexpr (kind == expr_kind::expr_ref) {
                    decltype(auto) ref = ::boost::yap::deref(expr);
                    constexpr expr_kind kind = remove_cv_ref_t<decltype(ref)>::kind;
                    default_transform_expression_tag<decltype(ref), Transform, detail::arity_of<kind>()> transformer;
                    return transformer(ref, static_cast<Transform &&>(transform));
                } else if constexpr (kind == expr_kind::terminal) {
                    return static_cast<Expr &&>(expr);
                } else {
                    if constexpr (std::is_lvalue_reference<Expr>{}) {
                        return transform_nonterminal(
                            expr,
                            expr.elements,
                            static_cast<Transform &&>(transform)
                        );
                    } else {
                        return transform_nonterminal(
                            expr,
                            std::move(expr.elements),
                            static_cast<Transform &&>(transform)
                        );
                    }
                }
            }
        };

#endif // BOOST_NO_CONSTEXPR_IF

        template <typename Expr, typename Transform>
        struct default_transform_expression_expr<
            Expr,
            Transform,
            void_t<decltype(std::declval<Transform>()(std::declval<Expr>()))>
        >
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            { return static_cast<Transform &&>(transform)(static_cast<Expr &&>(expr)); }
        };


        // Tag-matching; attempted first.

        template <typename Expr, typename Transform, expr_arity Arity, typename>
        struct default_transform_expression_tag
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                return default_transform_expression_expr<Expr, Transform>{}(
                    static_cast<Expr &&>(expr),
                    static_cast<Transform &&>(transform)
                );
            }
        };

        template <typename T, typename Transform>
        decltype(auto) terminal_value (T && x, Transform && transform);

#ifdef BOOST_NO_CONSTEXPR_IF

        template <typename T, typename Transform, expr_kind Kind>
        struct terminal_value_expr_impl
        {
            decltype(auto) operator() (T && x, Transform && transform)
            { return static_cast<T &&>(x); }
        };

        template <typename T, typename Transform>
        struct terminal_value_expr_impl<T, Transform, expr_kind::terminal>
        {
            decltype(auto) operator() (T && x, Transform && transform)
            {
                default_transform_expression_tag<T, Transform, detail::arity_of<expr_kind::terminal>()> transformer;
                // This temporary is necessary.  The transform here may
                // create a new object, and we don't want value_impl<>()
                // to leak a reference to it.
                auto retval = value_impl<true>(
                    transformer(static_cast<T &&>(x), static_cast<Transform &&>(transform))
                );
                return retval;
            }
        };

        template <typename T, typename Transform>
        struct terminal_value_expr_impl<T, Transform, expr_kind::expr_ref>
        {
            decltype(auto) operator() (T && x, Transform && transform)
            {
                return terminal_value(
                    ::boost::yap::deref(static_cast<T &&>(x)),
                    static_cast<Transform &&>(transform)
                );
            }
        };

        template <typename T, typename Transform, bool IsExpr>
        struct terminal_value_impl_t
        {
            decltype(auto) operator() (T && x, Transform && transform)
            {
                constexpr expr_kind kind = detail::remove_cv_ref_t<T>::kind;
                return terminal_value_expr_impl<T, Transform, kind>{}(
                    static_cast<T &&>(x),
                    static_cast<Transform &&>(transform)
                );
            }
        };

        template <typename T, typename Transform>
        struct terminal_value_impl_t<T, Transform, false>
        {
            decltype(auto) operator() (T && x, Transform && transform)
            { return static_cast<T &&>(x); }
        };

        template <typename T, typename Transform>
        decltype(auto) terminal_value_impl (T && x, Transform && transform)
        {
            return detail::terminal_value_impl_t<T, Transform, is_expr<T>::value>{}(
                static_cast<T &&>(x),
                static_cast<Transform &&>(transform)
            );
        }

#else

        template <typename T, typename Transform>
        decltype(auto) terminal_value_impl (T && x, Transform && transform)
        {
            if constexpr (is_expr<T>::value) {
                constexpr expr_kind kind = remove_cv_ref_t<T>::kind;
                if constexpr (kind == expr_kind::terminal) {
                    default_transform_expression_tag<T, Transform, detail::arity_of<kind>()> transformer;
                    // This temporary is necessary.  The transform here may
                    // create a new object, and we don't want value_impl<>()
                    // to leak a reference to it.
                    auto retval = value_impl<true>(
                        transformer(static_cast<T &&>(x), static_cast<Transform &&>(transform))
                    );
                    return retval;
                } else if constexpr (kind == expr_kind::expr_ref) {
                    return terminal_value(
                        ::boost::yap::deref(static_cast<T &&>(x)),
                        static_cast<Transform &&>(transform)
                    );
                } else {
                    return static_cast<T &&>(x);
                }
            } else {
                return static_cast<T &&>(x);
            }
        }

#endif // BOOST_NO_CONSTEXPR_IF

        template <typename T, typename Transform>
        decltype(auto) terminal_value (T && x, Transform && transform)
        {
            return terminal_value_impl(
                static_cast<T &&>(x),
                static_cast<Transform &&>(transform)
            );
        }

        template <typename Expr, typename Transform>
        struct default_transform_expression_tag<
            Expr,
            Transform,
            expr_arity::one,
            void_t<decltype(
                std::declval<Transform>()(
                    detail::tag_for<remove_cv_ref_t<Expr>::kind>(),
                    terminal_value(::boost::yap::get(std::declval<Expr>(), hana::llong<0>{}),
                                   std::declval<Transform>())
                )
            )>
        >
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                using namespace hana::literals;
                return static_cast<Transform &&>(transform)(
                    detail::tag_for<remove_cv_ref_t<Expr>::kind>(),
                    terminal_value(
                        ::boost::yap::get(static_cast<Expr &&>(expr), 0_c),
                        static_cast<Transform &&>(transform)
                    )
                );
            }
        };

        template <typename Expr, typename Transform>
        struct default_transform_expression_tag<
            Expr,
            Transform,
            expr_arity::two,
            void_t<decltype(
                std::declval<Transform>()(
                    detail::tag_for<remove_cv_ref_t<Expr>::kind>(),
                    terminal_value(::boost::yap::left(std::declval<Expr>()),
                                   std::declval<Transform>()),
                    terminal_value(::boost::yap::right(std::declval<Expr>()),
                                   std::declval<Transform>())
                )
            )>
        >
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                return static_cast<Transform &&>(transform)(
                    detail::tag_for<remove_cv_ref_t<Expr>::kind>(),
                    terminal_value(::boost::yap::left(static_cast<Expr &&>(expr)),
                                   static_cast<Transform &&>(transform)),
                    terminal_value(::boost::yap::right(static_cast<Expr &&>(expr)),
                                   static_cast<Transform &&>(transform))
                );
            }
        };

        template <typename Expr, typename Transform>
        struct default_transform_expression_tag<
            Expr,
            Transform,
            expr_arity::three,
            void_t<decltype(
                std::declval<Transform>()(
                    detail::tag_for<remove_cv_ref_t<Expr>::kind>(),
                    terminal_value(::boost::yap::cond(std::declval<Expr>()),
                                   std::declval<Transform>()),
                    terminal_value(::boost::yap::then(std::declval<Expr>()),
                                   std::declval<Transform>()),
                    terminal_value(::boost::yap::else_(std::declval<Expr>()),
                                   std::declval<Transform>())
                )
            )>
        >
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                return static_cast<Transform &&>(transform)(
                    detail::tag_for<remove_cv_ref_t<Expr>::kind>(),
                    terminal_value(::boost::yap::cond(static_cast<Expr &&>(expr)),
                                   static_cast<Transform &&>(transform)),
                    terminal_value(::boost::yap::then(static_cast<Expr &&>(expr)),
                                   static_cast<Transform &&>(transform)),
                    terminal_value(::boost::yap::else_(static_cast<Expr &&>(expr)),
                                   static_cast<Transform &&>(transform))
                );
            }
        };

        template <typename Expr, typename Transform>
        struct transform_call_unpacker
        {
            template <long long ...I>
            auto operator() (
                Expr && expr,
                Transform && transform,
                std::integer_sequence<long long, I...>
            ) -> decltype(
                static_cast<Transform &&>(transform)(
                    call_tag{},
                    terminal_value(
                        ::boost::yap::get(
                            static_cast<Expr &&>(expr),
                            hana::llong_c<I>
                        ),
                        std::declval<Transform>()
                    )...
                )
            ) {
                return static_cast<Transform &&>(transform)(
                    call_tag{},
                    terminal_value(
                        ::boost::yap::get(
                            static_cast<Expr &&>(expr),
                            hana::llong_c<I>
                        ),
                        static_cast<Transform &&>(transform)
                    )...
                );
            }
        };

        template <typename Expr>
        constexpr auto indices_for (Expr const & expr)
        {
            constexpr long long size = decltype(hana::size(expr.elements))::value;
            return std::make_integer_sequence<long long, size>();
        }

        template <typename Expr, typename Transform>
        struct default_transform_expression_tag<
            Expr,
            Transform,
            expr_arity::n,
            void_t<decltype(
                transform_call_unpacker<Expr, Transform>{}(
                    std::declval<Expr>(),
                    std::declval<Transform>(),
                    indices_for(std::declval<Expr>())
                )
            )>
        >
        {
            decltype(auto) operator() (Expr && expr, Transform && transform)
            {
                return transform_call_unpacker<Expr, Transform>{}(
                    static_cast<Expr &&>(expr),
                    static_cast<Transform &&>(transform),
                    indices_for(expr)
                );
            }
        };

        template <
            template<expr_kind, class> class ExprTemplate,
            expr_kind Kind,
            typename OldTuple,
            typename NewTuple
        >
        auto make_expr_from_tuple (ExprTemplate<Kind, OldTuple> const & expr, NewTuple && tuple)
        { return ExprTemplate<Kind, NewTuple>{std::move(tuple)}; }

        template <typename Expr, typename Tuple, typename Transform>
        decltype(auto) transform_nonterminal (Expr const & expr, Tuple && tuple, Transform && transform)
        {
            auto transformed_tuple = hana::transform(
                static_cast<Tuple &&>(tuple),
                [&transform](auto && element) {
                    using element_t = decltype(element);
                    constexpr expr_kind kind = remove_cv_ref_t<element_t>::kind;
                    default_transform_expression_tag<element_t, Transform, detail::arity_of<kind>()> transformer;
                    return transformer(
                        static_cast<element_t &&>(element),
                        static_cast<Transform &&>(transform)
                    );
                }
            );
            return make_expr_from_tuple(expr, std::move(transformed_tuple));
        }

    }

} }

#endif
