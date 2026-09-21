#ifndef LAZEX_RELATIONAL_HPP
#define LAZEX_RELATIONAL_HPP


#include "../../core.hpp"
#include "relational_decls.hpp"


namespace lazex::detail{



/**
 * @brief Evaluation rules for relational comparisons.
 */
template<typename T>
struct BooleanEvaluator : public BinaryEvaluator<BooleanEvaluator<T>, T>{

    template<traits::isBoolTag tag, typename L, typename R>
    LAZEX_FORCE_INLINE static void evaluate(tag, T& out, Pool<T> /*workers*/, const L& a, const R& b){
        out = get_bool(tag{}, a, b);
    }

    template<traits::isBoolTag tag, typename L, typename R>
    inline static bool get_bool(tag, const L& a, const R& b){
        if constexpr (std::is_same_v<tag, lazex::tags::EQ>) {
            return a == b;
        } else if constexpr (std::is_same_v<tag, lazex::tags::NEQ>) {
            return a != b;
        } else if constexpr (std::is_same_v<tag, lazex::tags::GT>) {
            return a > b;
        } else if constexpr (std::is_same_v<tag, lazex::tags::LT>) {
            return a < b;
        } else if constexpr (std::is_same_v<tag, lazex::tags::GE>) {
            return a >= b;
        } else if constexpr (std::is_same_v<tag, lazex::tags::LE>) {
            return a <= b;
        } else {
            static_assert(traits::isBoolTag<tag>, "Invalid boolean tag");
            return false; // should never reach here
        }
    }

};


template<typename Derived, typename T, typename L, typename R>
struct Comparison : public BinaryOperator<Derived, T, L, R>, public BooleanEvaluator<T>{
    using Base = BinaryOperator<Derived, T, L, R>;
    using Base::Base;

    // some times when `out` is boolean, we need to reserve a temporary for **each** branch, as we cannot pass `out` in one of the branches.
    static constexpr size_t REQUIRED_TEMPORARIES = Base::REQUIRED_TEMPORARIES + (lazex::traits::isNode<L, T> || lazex::traits::isNode<R, T> ? 1 : 0);

    operator bool() const{
        Pool<T> workers = this->reserve_workers();
        if constexpr (lazex::traits::isNode<L, T> && lazex::traits::isNode<R, T>) {
            T& left_out = workers.consume();
            T& left = this->template get<0>().eval_impl(left_out, workers);
            T& right_out = workers.consume();
            T& right = this->template get<1>().eval_impl(right_out, workers);
            return this->get_bool(typename Derived::tag{}, left, right);
        } else if constexpr (lazex::traits::isNode<L, T>) {
            T& out = workers.consume();
            T& left = this->template get<0>().eval_impl(out, workers);
            return this->get_bool(typename Derived::tag{}, left, get_value(this->template get<1>()));
        } else if constexpr (lazex::traits::isNode<R, T>) {
            T& out = workers.consume();
            T& right = this->template get<1>().eval_impl(out, workers);
            return this->get_bool(typename Derived::tag{}, get_value(this->template get<0>()), right);
        } else {
            return this->get_bool(typename Derived::tag{}, get_value(this->template get<0>()), get_value(this->template get<1>()));
        }
    }
};


/// @brief Lazy equal-to comparison node (`lhs == rhs`).  `operator bool()` returns the result.
template<typename T, typename L, typename R>
struct Eq : public Comparison<Eq<T, L, R>, T, L, R>{
    using Base = Comparison<Eq<T, L, R>, T, L, R>;
    using Base::Base;
    using tag = lazex::tags::EQ;
};

/// @brief Lazy not-equal comparison node (`lhs != rhs`).  `operator bool()` returns the result.
template<typename T, typename L, typename R>
struct Neq : public Comparison<Neq<T, L, R>, T, L, R>{
    using Base = Comparison<Neq<T, L, R>, T, L, R>;
    using Base::Base;
    using tag = lazex::tags::NEQ;
};


/// @brief Lazy greater-than comparison node (`lhs > rhs`).  `operator bool()` returns the result.
template<typename T, typename L, typename R>
struct Gt : public Comparison<Gt<T, L, R>, T, L, R>{
    using Base = Comparison<Gt<T, L, R>, T, L, R>;
    using Base::Base;
    using tag = lazex::tags::GT;
};

/// @brief Lazy less-than comparison node (`lhs < rhs`).  `operator bool()` returns the result.
template<typename T, typename L, typename R>
struct Lt : public Comparison<Lt<T, L, R>, T, L, R>{
    using Base = Comparison<Lt<T, L, R>, T, L, R>;
    using Base::Base;
    using tag = lazex::tags::LT;
};

/// @brief Lazy greater-or-equal comparison node (`lhs >= rhs`).  `operator bool()` returns the result.
template<typename T, typename L, typename R>
struct Ge : public Comparison<Ge<T, L, R>, T, L, R>{
    using Base = Comparison<Ge<T, L, R>, T, L, R>;
    using Base::Base;
    using tag = lazex::tags::GE;
};

/// @brief Lazy less-or-equal comparison node (`lhs <= rhs`).  `operator bool()` returns the result.
template<typename T, typename L, typename R>
struct Le : public Comparison<Le<T, L, R>, T, L, R>{
    using Base = Comparison<Le<T, L, R>, T, L, R>;
    using Base::Base;
    using tag = lazex::tags::LE;
};



LAZEX_DEFINE_RELATIONAL_OP(==, Eq)
LAZEX_DEFINE_RELATIONAL_OP(!=, Neq)
LAZEX_DEFINE_RELATIONAL_OP(>, Gt)
LAZEX_DEFINE_RELATIONAL_OP(<, Lt)
LAZEX_DEFINE_RELATIONAL_OP(>=, Ge)
LAZEX_DEFINE_RELATIONAL_OP(<=, Le)

} // namespace lazex::detail


namespace lazex{
    
using lazex::detail::operator==, 
      lazex::detail::operator!=, 
      lazex::detail::operator>, 
      lazex::detail::operator<, 
      lazex::detail::operator>=, 
      lazex::detail::operator<=;

} // namespace lazex


#endif // LAZEX_RELATIONAL_HPP