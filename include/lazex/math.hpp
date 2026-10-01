#ifndef LAZEX_MATH_HPP
#define LAZEX_MATH_HPP

#include <cmath>


/**
 * @file math.hpp
 * @brief `LAZEX_USING_MATH` - makes unqualified math calls resolve correctly.
 */
#define LAZEX_USING_MATH                                                       \
    using std::abs, std::pow, std::sqrt, std::exp, std::log,                   \
          std::sin, std::cos, std::tan,                                        \
          std::asin, std::acos, std::atan,                                     \
          std::sinh, std::cosh, std::tanh,                                     \
          std::erf


namespace lazex::detail{

// So that lazex's own generic code gets the same overload set it asks users to adopt,
// independently of which headers a translation unit happened to include first.
LAZEX_USING_MATH;

} // namespace lazex::detail


namespace lazex{

LAZEX_USING_MATH;

} // namespace lazex


#endif // LAZEX_MATH_HPP
