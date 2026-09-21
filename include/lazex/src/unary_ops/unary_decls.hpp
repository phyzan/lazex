#ifndef LAZEX_UNARY_DECLS_HPP
#define LAZEX_UNARY_DECLS_HPP

#include "../rules.hpp"

/**
 * @brief Declare a new unary function node type and its overloaded free function.
 *
 * Expands to:
 * 1. A struct `OP<T, Arg>` inheriting `Unary<OP<T,Arg>, T, Arg>` and
 *    `CustomUnaryEvaluator<T>`, with `tag = TAG`
 * 2. A free function `FUNC(U&&)` constrained to any expression type, returning
 *    OP<detail::lazy_value_type<U>, std::decay_t<U>>(std::forward<U>(arg))`.
 *
 * @param FUNC     The function name (e.g. `abs`, `sqrt`).
 * @param OP       The node struct name (e.g. `Abs`, `Sqrt`).
 * @param TAG      The dispatch tag type (e.g. `ABS`, `SQRT`).
 */
#define LAZEX_DEFINE_UNARY_OP(FUNC, OP, TAG)                             \
template<typename T, typename Arg>                                              \
struct OP : public Unary<OP<T, Arg>, T, Arg>, public CustomUnaryEvaluator<T> {                \
    using Base = Unary<OP<T, Arg>, T, Arg>;                                     \
    using tag = TAG;                                                             \
    using Base::Base;                                                            \
};                                                                              \
                                                                                \
\
template<typename U>                                                            \
requires (                                                                      \
    requires { typename std::decay_t<U>::lazy_value_type; } &&                         \
    traits::isLazyExpr<std::decay_t<U>, typename std::decay_t<U>::lazy_value_type>                 \
)                                                                               \
LAZEX_FORCE_INLINE auto FUNC(U&& arg) {                                               \
    using T = detail::lazy_value_type<U>;                                              \
    auto arg_expr = make_expr<T>(std::forward<U>(arg));                         \
    return OP<T, std::decay_t<decltype(arg_expr)>>(std::move(arg_expr));        \
}

/**
 * @brief Open a specialisation block for unary function rules for type `Type`.
 *
 * Usage:
 * @code
 *   LAZEX_SPECIALIZE_FUNCTIONS(MyType) {
 *       using T = MyType;
 *       using Base = UnaryEvaluator<CustomUnaryEvaluator<T>, T>;
 *       using Base::evaluate; using Base::eval_rule;
 *       LAZEX_EVALUATE_FUNC(T, a, NEG, T) { out = my_neg(a); }
 *   };
 * @endcode
 *
 * @param Type The arithmetic type to specialise for.
 */
#define LAZEX_SPECIALIZE_FUNCTIONS(Type)\
template<>\
struct CustomUnaryEvaluator<Type> : public UnaryEvaluator<CustomUnaryEvaluator<Type>, Type>

/**
 * @brief Declare an `evaluate` overload for a specific unary function and argument type.
 *
 * Generates a `static LAZEX_FORCE_INLINE void evaluate(tag, T& out, Pool<T> workers, const ARG& arg)`
 * declaration inside a `LAZEX_SPECIALIZE_FUNCTIONS` block.
 *
 * @param T    The arithmetic value type.
 * @param arg  Name for the argument parameter.
 * @param tag  The operation tag type (e.g. `ABS`, `SQRT`, `NEG`).
 * @param ARG  The C++ type of the argument (typically `T`).
 */
#define LAZEX_EVALUATE_FUNC(T, arg, tag, ARG)\
LAZEX_FORCE_INLINE static void evaluate(tag, T& out, Pool<T> workers, const ARG& arg)


namespace lazex::tags{

// ====================== UNARY FUNCTION TAGS ===========================

/// @brief Tag for unary negation `-x`.
struct NEG : public Tag{};
/// @brief Tag for `abs(x)`.
struct ABS : public Tag{};
/// @brief Tag for `sqrt(x)`.
struct SQRT : public Tag{};
/// @brief Tag for `exp(x)`.
struct EXP : public Tag{};
/// @brief Tag for `log(x)`.
struct LOG : public Tag{};
/// @brief Tag for `sin(x)`.
struct SIN : public Tag{};
/// @brief Tag for `cos(x)`.
struct COS : public Tag{};
/// @brief Tag for `tan(x)`.
struct TAN : public Tag{};
/// @brief Tag for `cot(x)`.
struct COT : public Tag{};
/// @brief Tag for `sec(x)`.
struct SEC : public Tag{};
/// @brief Tag for `csc(x)`.
struct CSC : public Tag{};
/// @brief Tag for `asin(x)`.
struct ASIN : public Tag{};
/// @brief Tag for `acos(x)`.
struct ACOS : public Tag{};
/// @brief Tag for `atan(x)`.
struct ATAN : public Tag{};
/// @brief Tag for `acot(x)`.
struct ACOT : public Tag{};
/// @brief Tag for `asec(x)`.
struct ASEC : public Tag{};
/// @brief Tag for `acsc(x)`.
struct ACSC : public Tag{};
/// @brief Tag for `sinh(x)`.
struct SINH : public Tag{};
/// @brief Tag for `cosh(x)`.
struct COSH : public Tag{};
/// @brief Tag for `tanh(x)`.
struct TANH : public Tag{};
/// @brief Tag for `erf(x)`.
struct ERF : public Tag{};

} // namespace lazex::tags

namespace lazex::detail{



template<typename Derived, typename T, typename Arg> struct Unary;
template<typename T, typename Arg>
struct Neg;
template<typename T, typename Arg>
struct Abs;
template<typename T, typename Arg>
struct Sqrt;
template<typename T, typename Arg>
struct Exp;
template<typename T, typename Arg>
struct Log;
template<typename T, typename Arg>
struct Sin;
template<typename T, typename Arg>
struct Cos;
template<typename T, typename Arg>
struct Tan;
template<typename T, typename Arg>
struct Cot;
template<typename T, typename Arg>
struct Sec;
template<typename T, typename Arg>
struct Csc;
template<typename T, typename Arg>
struct Asin;
template<typename T, typename Arg>
struct Acos;
template<typename T, typename Arg>
struct Atan;
template<typename T, typename Arg>
struct Acot;
template<typename T, typename Arg>
struct Asec;
template<typename T, typename Arg>
struct Acsc;
template<typename T, typename Arg>
struct Sinh;
template<typename T, typename Arg>
struct Cosh;
template<typename T, typename Arg>
struct Tanh;
template<typename T, typename Arg>
struct Erf;




// Unary type getter
template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::NEG, Arg>{
    using type = lazex::detail::Neg<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::ABS, Arg>{
    using type = lazex::detail::Abs<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::SQRT, Arg>{
    using type = lazex::detail::Sqrt<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::EXP, Arg>{
    using type = lazex::detail::Exp<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::LOG, Arg>{
    using type = lazex::detail::Log<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::SIN, Arg>{
    using type = lazex::detail::Sin<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::COS, Arg>{
    using type = lazex::detail::Cos<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::TAN, Arg>{
    using type = lazex::detail::Tan<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::COT, Arg>{
    using type = lazex::detail::Cot<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::SEC, Arg>{
    using type = lazex::detail::Sec<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::CSC, Arg>{
    using type = lazex::detail::Csc<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::ASIN, Arg>{
    using type = lazex::detail::Asin<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::ACOS, Arg>{
    using type = lazex::detail::Acos<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::ATAN, Arg>{
    using type = lazex::detail::Atan<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::ACOT, Arg>{
    using type = lazex::detail::Acot<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::ASEC, Arg>{
    using type = lazex::detail::Asec<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::ACSC, Arg>{
    using type = lazex::detail::Acsc<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::SINH, Arg>{
    using type = lazex::detail::Sinh<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::COSH, Arg>{
    using type = lazex::detail::Cosh<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::TANH, Arg>{
    using type = lazex::detail::Tanh<T, Arg>;
};

template<typename T, typename Arg>
struct TypeGetter<T, lazex::tags::ERF, Arg>{
    using type = lazex::detail::Erf<T, Arg>;
};

    
} // namespace lazex::detail

#endif // LAZEX_UNARY_DECLS_HPP