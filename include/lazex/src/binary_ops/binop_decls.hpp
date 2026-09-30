#ifndef LAZEX_BINOP_DECLS_HPP
#define LAZEX_BINOP_DECLS_HPP


#include "../rules.hpp" // IWYU pragma: keep

/**
 * @brief Constraint: at least one of `L`, `R` is a lazy expression type. The other type may be a lazy expression or a raw value convertible to `T`.
 *
 * Expands to an `&&`-expression that is `true` when `std::decay_t<L>` or
 * `std::decay_t<R>` (or both) expose a `lazy_value_type` alias and derive from
 * `ExprBase<lazy_value_type>`.  Used as a `requires` clause on all arithmetic and
 * relational operator overloads to ensure they only activate for expression
 * operands and do not shadow built-in arithmetic.
 */
#define LAZEX_REQUIREMENT(L, R)\
    ( (requires {typename std::decay_t<L>::lazy_value_type;} && lazex::traits::isLazyExpr<std::decay_t<L>, typename std::decay_t<L>::lazy_value_type> && lazex::traits::isValidType<std::decay_t<R>, typename std::decay_t<L>::lazy_value_type>) || \
      (requires {typename std::decay_t<R>::lazy_value_type;} && lazex::traits::isLazyExpr<std::decay_t<R>, typename std::decay_t<R>::lazy_value_type> && lazex::traits::isValidType<std::decay_t<L>, typename std::decay_t<R>::lazy_value_type>) )


/**
 * @brief Declare an `evaluate` overload for a specific binary operation and operand types.
 *
 * Generates a `static LAZEX_FORCE_INLINE void evaluate(tag, T& out, Pool<T> workers, const LEFT& a, const RIGHT& b)`
 * declaration inside a `LAZEX_SPECIALIZE_OPERATIONS` block.  The body should follow immediately.
 *
 * @param T     The arithmetic value type.
 * @param a     Name for the left value parameter.
 * @param b     Name for the right value parameter.
 * @param LEFT  The C++ type of the left operand (e.g. `T`, `double`, `int`).
 * @param tag   The operation tag type (e.g. `PLUS`, `MUL`).
 * @param RIGHT The C++ type of the right operand.
 */
#define LAZEX_EVALUATE_OPER(T, a, b, LEFT, tag, RIGHT)\
LAZEX_FORCE_INLINE static void evaluate(tag, T& out, Pool<T> workers, const LEFT& a, const RIGHT& b)

/**
 * @brief Open a specialisation block for binary operation rules for type `Type`.
 *
 * Usage:
 * @code
 *   LAZEX_SPECIALIZE_OPERATIONS(MyType) {
 *       using T = MyType;
 *       using Base = BinaryEvaluator<CustomBinaryEvaluator<T>, T>;
 *       using Base::evaluate; using Base::eval_rule;
 *       LAZEX_EVALUATE_OPER(T, a, b, T, PLUS, T) { out = my_add(a, b); }
 *       // ...
 *   };
 * @endcode
 *
 * @param Type The arithmetic type to specialise for.
 */
#define LAZEX_SPECIALIZE_OPERATIONS(Type)\
template<>\
struct CustomBinaryEvaluator<Type> : public BinaryEvaluator<CustomBinaryEvaluator<Type>, Type>


namespace lazex::tags{

// ====================== BINARY OPERATOR TAGS ===========================
/// @brief Tag for addition `+`.
struct PLUS : public Tag{};
/// @brief Tag for subtraction `-`.
struct MINUS : public Tag{};
/// @brief Tag for multiplication `*`.
struct MUL : public Tag{};
/// @brief Tag for division `/`.
struct DIV : public Tag{};
/// @brief Tag for exponentiation `pow`.
struct POW : public Tag{};
/// @brief Tag for `max(x,y)`.
struct MAX : public Tag{};
/// @brief Tag for `min(x,y)`.
struct MIN : public Tag{};

} // namespace lazex::tags

namespace lazex::detail {


template<typename Derived, typename T, typename L, typename R> struct BinaryOperator;
template<typename T, typename L, typename R> struct Add;
template<typename T, typename L, typename R> struct Sub;
template<typename T, typename L, typename R> struct Mul;
template<typename T, typename L, typename R> struct Div;
template<typename T, typename L, typename R> struct Pow;
template<typename T, typename L, typename R> struct MaxLazy;
template<typename T, typename L, typename R> struct MinLazy;

// rules
template<typename Derived, typename T> struct BinaryEvaluator;
template<typename T> struct CustomBinaryEvaluator;



// Binary type getter
template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::PLUS, L, R>{
    using type = lazex::detail::Add<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::MINUS, L, R>{
    using type = lazex::detail::Sub<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::MUL, L, R>{
    using type = lazex::detail::Mul<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::DIV, L, R>{
    using type = lazex::detail::Div<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::POW, L, R>{
    using type = lazex::detail::Pow<T, L, R>;
};

template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::MAX, L, R>{
    using type = lazex::detail::MaxLazy<T, L, R>;
};
template<typename T, typename L, typename R>
struct TypeGetter<T, lazex::tags::MIN, L, R>{
    using type = lazex::detail::MinLazy<T, L, R>;
};

} // namespace lazex::detail



namespace lazex::traits{


template<typename Derived, typename T>
concept isBinOp = requires
    {typename Derived::LhsType; typename Derived::RhsType;} &&
    std::is_base_of_v<lazex::detail::BinaryOperator<Derived, T, typename Derived::LhsType, typename Derived::RhsType>, Derived>;

template<typename Derived, typename T>
concept isAdd = requires { requires isBinOp<Derived, T>; } && std::is_base_of_v<lazex::detail::Add<T, typename Derived::LhsType, typename Derived::RhsType>, Derived>;

template<typename Derived, typename T>
concept isSub = requires { requires isBinOp<Derived, T>; } && std::is_base_of_v<lazex::detail::Sub<T, typename Derived::LhsType, typename Derived::RhsType>, Derived>;

template<typename Derived, typename T>
concept isMul = requires { requires isBinOp<Derived, T>; } && std::is_base_of_v<lazex::detail::Mul<T, typename Derived::LhsType, typename Derived::RhsType>, Derived>;

template<typename Derived, typename T>
concept isDiv = requires { requires isBinOp<Derived, T>; } && std::is_base_of_v<lazex::detail::Div<T, typename Derived::LhsType, typename Derived::RhsType>, Derived>;

template<typename Derived, typename T>
concept isPow = requires { requires isBinOp<Derived, T>; } && std::is_base_of_v<lazex::detail::Pow<T, typename Derived::LhsType, typename Derived::RhsType>, Derived>;

template<typename Derived, typename T>
concept isMin = requires { requires isBinOp<Derived, T>; } && std::is_base_of_v<lazex::detail::MinLazy<T, typename Derived::LhsType, typename Derived::RhsType>, Derived>;

template<typename Derived, typename T>
concept isMax = requires { requires isBinOp<Derived, T>; } && std::is_base_of_v<lazex::detail::MaxLazy<T, typename Derived::LhsType, typename Derived::RhsType>, Derived>;

} // namespace lazex::traits

#endif // LAZEX_BINOP_DECLS_HPP