#ifndef LAZEX_MATH_HPP
#define LAZEX_MATH_HPP

#include <cmath>
#include <numbers>
#include <type_traits>

/**
 * @file math.hpp
 * @brief `LAZEX_USING_MATH` - makes unqualified math calls resolve correctly.
 */
#define LAZEX_USING_STD_MATH                            \
    using std::abs, std::pow, std::sqrt, std::exp,      \
          std::log, std::log10, std::sin, std::cos,     \
          std::tan, std::asin, std::acos, std::atan,    \
          std::sinh, std::cosh, std::tanh, std::erf

#define LAZEX_USING_MATH  \
    LAZEX_USING_STD_MATH; \
    using ::lazex::detail::constants

namespace lazex::detail{

// So that lazex's own generic code gets the same overload set it asks users to adopt,
// independently of which headers a translation unit happened to include first.
LAZEX_USING_STD_MATH;

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

} // namespace lazex::detail


namespace lazex{

LAZEX_USING_MATH;

} // namespace lazex


#endif // LAZEX_MATH_HPP
