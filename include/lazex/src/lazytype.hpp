#ifndef LAZEX_LAZYTYPE_HPP
#define LAZEX_LAZYTYPE_HPP


#include <vector>
#include "core.hpp"


#define LAZEX_REF static_cast<lazex::detail::copy_const_t<std::remove_reference_t<decltype(*this)>, T>*>(this)

namespace lazex::detail{

/**
 * @brief Owning lazy variable — the primary user-facing storage type.
 * @tparam T The arithmetic value type.
 */
template<typename T>
struct LazyType : public detail::Atom<LazyType<T>, T>, public T{
    
    // Main constructors and assignment operators
    LazyType() = default;
    LazyType(const LazyType&) = default;
    LazyType(LazyType&&) noexcept = default;
    LazyType& operator=(const LazyType&) = default;
    LazyType& operator=(LazyType&&) noexcept = default;
    ~LazyType() = default;

    using T::T; // inherit constructors from T
    LazyType(const T& value) : T(value) {}
    LazyType(T&& value) noexcept : T(std::move(value)) {}

    // Construct from lazy expressions
    template<::lazex::traits::isAtom<T> U>
    requires (!::lazex::traits::isLazy<U, T>)
    LazyType(U&& value) : T(value.value()) {}

    template<::lazex::traits::isNode<T> NodeType>
    LazyType(const NodeType& node) {
        node.eval(*LAZEX_REF);
    }

    // Assignment operators
    template<::lazex::traits::isNode<T> NodeType>
    LAZEX_FORCE_INLINE
    LazyType& operator=(const NodeType& node){
        /*
        Do NOT do node.eval(value_) here, because if the node contains a reference to this LazyType,
        it will lead to invalid results, as the NodeEvaluator assumes `out` is separate memory.
        See eval_rule_impl for details. TODO: try to allow `out` to be one of the branches, without using more temporaries.
        */
        T::operator=(node.eval_worker());
        return *this;
    }

    template<::lazex::traits::isAtom<T> U>
    LAZEX_FORCE_INLINE
    LazyType& operator=(U&& other) requires (!::lazex::traits::isLazy<U, T>){
        T::operator=(other.value);
        return *this;
    }

    template<typename U>
    LAZEX_FORCE_INLINE
    LazyType& operator=(U&& other) requires (!::lazex::traits::isLazyExpr<U, T>){
        T::operator=(std::forward<U>(other));
        return *this;
    }

    // Compound assignment operators
    template<typename U>
    LAZEX_FORCE_INLINE
    LazyType& operator+=(U&& other){
        if constexpr (::lazex::traits::isNode<U, T>){
            *LAZEX_REF += other.eval_worker();
        } else if constexpr (::lazex::traits::isAtom<U, T>){
            *LAZEX_REF += get_value(other);
        } else {
            *LAZEX_REF += other;
        }
        return *this;
    }

    template<typename U>
    LAZEX_FORCE_INLINE
    LazyType& operator*=(U&& other){
        if constexpr (::lazex::traits::isNode<U, T>){
            *LAZEX_REF *= other.eval_worker();
        } else if constexpr (::lazex::traits::isAtom<U, T>){
            *LAZEX_REF *= get_value(other);
        } else {
            *LAZEX_REF *= other;
        }
        return *this;
    }

    template<typename U>
    LAZEX_FORCE_INLINE
    LazyType& operator-=(U&& other){
        if constexpr (::lazex::traits::isNode<U, T>){
            *LAZEX_REF -= other.eval_worker();
        } else if constexpr (::lazex::traits::isAtom<U, T>){
            *LAZEX_REF -= get_value(other);
        } else {
            *LAZEX_REF -= other;
        }
        return *this;
    }

    template<typename U>
    LAZEX_FORCE_INLINE
    LazyType& operator/=(U&& other){
        if constexpr (::lazex::traits::isNode<U, T>){
            *LAZEX_REF /= other.eval_worker();
        } else if constexpr (::lazex::traits::isAtom<U, T>){
            *LAZEX_REF /= get_value(other);
        } else {
            *LAZEX_REF /= other;
        }
        return *this;
    }
};

} // namespace lazex::detail


#undef LAZEX_REF

#endif // LAZEX_LAZYTYPE_HPP