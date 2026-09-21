#ifndef LAZEX_UNARY_OPS_HPP
#define LAZEX_UNARY_OPS_HPP

#include <iostream>
#include "unary_decls.hpp"



namespace lazex::detail{


/**
 * @brief Default evaluation policy for unary operations.
 */
template<typename Derived, typename T>
struct UnaryEvaluator : NodalEvaluator<Derived, T> {

    template<lazex::traits::isTag tag, typename Arg>
    inline static void evaluate(tag, T& /*out*/, Pool<T> /*workers*/, const Arg& /*a*/){
        static_assert(false, "UnaryEvaluator::evaluate must be specialised for each performed operation");
    }

    // Bypassing the `eval_rule` dispatch for unary operations, for clarity only. Performance should not change.
    template<lazex::traits::isTag tag, lazex::traits::isLazyExpr<T> Branch>
    LAZEX_FORCE_INLINE static void eval_rule(tag, T& out, Pool<T> workers, const Branch& arg){
        if constexpr (lazex::traits::isNode<Branch, T>) {
            Derived::evaluate(tag{}, out, workers, arg.eval_impl(out, workers));
        } else {
            Derived::evaluate(tag{}, out, workers, get_value(arg));
        }
    }
};


/**
 * @brief Primary unary-operation rules for type `T`.
 *
 * Analogous to `CustomBinaryEvaluator`.  Specialise (via `LAZEX_SPECIALIZE_FUNCTIONS`) to
 * provide custom `evaluate` overloads for unary functions such as `abs`, `sqrt`,
 * `neg`, etc.
 *
 * @tparam T The arithmetic value type to specialise for.
 */
template<typename T>
struct CustomUnaryEvaluator : public UnaryEvaluator<CustomUnaryEvaluator<T>, T>{};



template<typename Derived, typename T, typename Arg>
struct Unary : public Node<Derived, T, Arg>{

    static_assert(traits::isLazyExpr<Arg, T>, "Unary node argument must be an expression");

    using Base = Node<Derived, T, Arg>;
    using branch_t = std::tuple<Arg>;
    using Base::Base;
};



/// @brief `e.g. Abs<T, Arg>` node and lazy `abs(U&&)` overload

LAZEX_DEFINE_UNARY_OP(operator-, Neg, lazex::tags::NEG)
LAZEX_DEFINE_UNARY_OP(abs, Abs, lazex::tags::ABS)
LAZEX_DEFINE_UNARY_OP(sqrt, Sqrt, lazex::tags::SQRT)
LAZEX_DEFINE_UNARY_OP(exp, Exp, lazex::tags::EXP)
LAZEX_DEFINE_UNARY_OP(log, Log, lazex::tags::LOG)
LAZEX_DEFINE_UNARY_OP(sin, Sin, lazex::tags::SIN)
LAZEX_DEFINE_UNARY_OP(cos, Cos, lazex::tags::COS)
LAZEX_DEFINE_UNARY_OP(tan, Tan, lazex::tags::TAN)
LAZEX_DEFINE_UNARY_OP(cot, Cot, lazex::tags::COT)
LAZEX_DEFINE_UNARY_OP(sec, Sec, lazex::tags::SEC)
LAZEX_DEFINE_UNARY_OP(csc, Csc, lazex::tags::CSC)
LAZEX_DEFINE_UNARY_OP(asin, Asin, lazex::tags::ASIN)
LAZEX_DEFINE_UNARY_OP(acos, Acos, lazex::tags::ACOS)
LAZEX_DEFINE_UNARY_OP(atan, Atan, lazex::tags::ATAN)
LAZEX_DEFINE_UNARY_OP(acot, Acot, lazex::tags::ACOT)
LAZEX_DEFINE_UNARY_OP(asec, Asec, lazex::tags::ASEC)
LAZEX_DEFINE_UNARY_OP(acsc, Acsc, lazex::tags::ACSC)
LAZEX_DEFINE_UNARY_OP(sinh, Sinh, lazex::tags::SINH)
LAZEX_DEFINE_UNARY_OP(cosh, Cosh, lazex::tags::COSH)
LAZEX_DEFINE_UNARY_OP(tanh, Tanh, lazex::tags::TANH)
LAZEX_DEFINE_UNARY_OP(erf, Erf, lazex::tags::ERF)

template<typename F>
requires lazex::traits::isNode<F, typename std::decay_t<F>::lazy_value_type>
std::ostream& operator<<(std::ostream& os, const F& expr){
    return os << expr.eval_worker();
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const LazyType<T>& expr){
    return os << get_value(expr);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const RefType<T>& expr){
    return os << get_value(expr);
}

} // namespace lazex::detail


namespace lazex{

using lazex::detail::operator<<;

using lazex::detail::abs, 
      lazex::detail::sqrt,
      lazex::detail::exp,
      lazex::detail::log,
      lazex::detail::sin,
      lazex::detail::cos,
      lazex::detail::tan,
      lazex::detail::cot,
      lazex::detail::sec,
      lazex::detail::csc,
      lazex::detail::asin,
      lazex::detail::acos,
      lazex::detail::atan,
      lazex::detail::acot,
      lazex::detail::asec,
      lazex::detail::acsc,
      lazex::detail::sinh,
      lazex::detail::cosh,
      lazex::detail::tanh,
      lazex::detail::erf;

} // namespace lazex


#endif // LAZEX_UNARY_OPS_HPP