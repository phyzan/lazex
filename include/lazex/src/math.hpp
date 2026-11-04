#ifndef LAZEX_MATH_HPP
#define LAZEX_MATH_HPP

#include <cmath>
#include <numbers>
#include <type_traits>

/**
 * @file math.hpp
 * @brief `LAZEX_USING_MATH` - makes unqualified math calls resolve correctly.
 */
#define LAZEX_USING_MATH                                                \
    using ::lazex::abs, ::lazex::pow, ::lazex::sqrt, ::lazex::exp,      \
          ::lazex::log, ::lazex::log10, ::lazex::sin, ::lazex::cos,     \
          ::lazex::tan, ::lazex::cot, ::lazex::sec, ::lazex::csc,       \
          ::lazex::asin, ::lazex::acos, ::lazex::atan, ::lazex::acot,   \
          ::lazex::asec, ::lazex::acsc, ::lazex::atan2, ::lazex::sinh,  \
          ::lazex::cosh, ::lazex::tanh, ::lazex::erf;

namespace lazex::detail{

// The overloads for plain values. The lazy ones join them in this namespace, so that each lazex
// function is a single overload set, for lazex's own generic code and for LAZEX_USING_MATH alike.
using std::abs, std::pow, std::sqrt, std::exp,
      std::log, std::log10, std::sin, std::cos,
      std::tan, std::asin, std::acos, std::atan,
      std::atan2, std::sinh, std::cosh, std::tanh,
      std::erf;

template<typename T>
struct constants {
    static T pi() {
        if constexpr (std::is_floating_point_v<T>) {
            return std::numbers::pi_v<T>;
        } else {
            using std::acos;
            return acos(T(-1));
        }
    }

    static T two_over_sqrtpi() {
        if constexpr (std::is_floating_point_v<T>) {
            return T(2) * std::numbers::inv_sqrtpi_v<T>;
        } else {
            using std::sqrt;
            return T(2) / sqrt(pi());
        }
    }
};

// The mpfr::mpreal specialization is in apps/lazex_mpreal.hpp, with lazex's other mpreal
// support, so that lazex itself does not depend on MPFR.


/**
 * @brief `f^P`, with the integer exponent fixed at compile time, for a plain value.
 *
 * A product of `|P|` factors, reciprocated when `P < 0`. `power<1>(f)` is `f` itself.
 * A lazy expression takes the `IntegralPower` overload in unary_ops/unary_ops.hpp instead.
 */
template<int P, typename F>
requires (!requires { typename F::lazy_value_type; })
inline decltype(auto) power(const F& f){
    static_assert(P != 0, "Integer power must be non-zero");
    if constexpr (P < 0) {
        return 1 / power<-P>(f);
    } else if constexpr (P == 1) {
        return (f);
    } else {
        return power<P-1>(f) * f;
    }
}

} // namespace lazex::detail


namespace lazex{

using detail::constants;

} // namespace lazex


#endif // LAZEX_MATH_HPP
