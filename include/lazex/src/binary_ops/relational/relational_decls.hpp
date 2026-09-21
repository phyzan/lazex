#ifndef LAZEX_RELATIONAL_DECLS_HPP
#define LAZEX_RELATIONAL_DECLS_HPP


#include "../binop_decls.hpp"


/**
 * @brief Generates a relational operator that produces a lazy `Comparison` node.
 *
 * Expands to a template `operator OP(L&&, R&&)` constrained by `LAZEX_REQUIREMENT`.
 * It calls `make_expr<T>()` on both operands to canonicalise them (promoting raw
 * values to `RefType` or `OtherType` atoms), deduces the node types from the
 * resulting expressions, and returns `ClassName<T, LExprType, RExprType>`.
 *
 * @param OP        The operator symbol (e.g. `==`, `>`).
 * @param ClassName The concrete `Comparison`-derived type (e.g. `Eq`, `Gt`).
 */
#define LAZEX_DEFINE_RELATIONAL_OP(OP, ClassName)\
template<typename L, typename R>\
requires LAZEX_REQUIREMENT(L, R)\
LAZEX_FORCE_INLINE auto operator OP(L&& lhs, R&& rhs){\
    using T = lazex::detail::value_typeOf<L, R>;\
    auto lhs_expr = make_expr<T>(std::forward<L>(lhs));\
    auto rhs_expr = make_expr<T>(std::forward<R>(rhs));\
    return ClassName<T, std::decay_t<decltype(lhs_expr)>, std::decay_t<decltype(rhs_expr)>>(std::move(lhs_expr), std::move(rhs_expr));\
}


namespace lazex::tags{

// ====================== COMPARISON OPERATOR TAGS =======================
/// @brief Base tag for all boolean/comparison operations.
struct BOOL_TAG : public Tag{};

/// @brief Tag for equality comparison `==`.
struct EQ : public BOOL_TAG{};
/// @brief Tag for inequality comparison `!=`.
struct NEQ : public BOOL_TAG{};
/// @brief Tag for greater-than comparison `>`.
struct GT : public BOOL_TAG{};
/// @brief Tag for less-than comparison `<`.
struct LT : public BOOL_TAG{};
/// @brief Tag for greater-or-equal comparison `>=`.
struct GE : public BOOL_TAG{};
/// @brief Tag for less-or-equal comparison `<=`.
struct LE : public BOOL_TAG{};

} // namespace lazex::tags

namespace lazex::traits{

template<typename Arg>
concept isBoolTag = std::is_base_of_v<lazex::tags::BOOL_TAG, std::decay_t<Arg>>;

}

namespace lazex::detail{

template<typename T>
struct BooleanEvaluator;

template<typename Derived, typename T, typename L, typename R>
struct Comparison;

template<typename T, typename L, typename R>
struct Eq;

template<typename T, typename L, typename R>
struct Neq;

template<typename T, typename L, typename R>
struct Gt;

template<typename T, typename L, typename R>
struct Lt;

template<typename T, typename L, typename R>
struct Ge;

template<typename T, typename L, typename R>
struct Le;


// Comparison type getters for relational operators
template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::EQ, L, R>{
    using type = lazex::detail::Eq<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::NEQ, L, R>{
    using type = lazex::detail::Neq<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::LT, L, R>{
    using type = lazex::detail::Lt<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::GT, L, R>{
    using type = lazex::detail::Gt<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::GE, L, R>{
    using type = lazex::detail::Ge<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::LE, L, R>{
    using type = lazex::detail::Le<T, L, R>;
};

} // namespace lazex::detail


#endif // LAZEX_RELATIONAL_DECLS_HPP