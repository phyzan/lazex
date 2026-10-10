#ifndef LAZEX_MPREAL_HPP
#define LAZEX_MPREAL_HPP

/**
 * @file lazex_mpreal.hpp
 * @brief `lazex` library specialisations for `mpfr::mpreal` (arbitrary-precision arithmetic).
 *
 * This header wires the lazex expression-template library (`lazex.hpp`) to the MPFR C
 * library via the `mpfr::mpreal` C++ wrapper.  It provides:
 *
 * - `std::numeric_limits` specialisation for `LazyType<mpfr::mpreal>` via
 *   `LAZEX_DECLARE_NUMERIC_TYPE`.
 * - `LAZEX_SPECIALIZE_FUNCTIONS(mpfr::mpreal)` — type-specific `evaluate` overloads for
 *   `NEG`, `ABS`, and `SQRT` using the raw MPFR C API for maximum performance.
 * - `LAZEX_SPECIALIZE_OPERATIONS(mpfr::mpreal)` — type-specific `evaluate` overloads for
 *   `+`, `-`, `*`, `/`, `pow`, `max`, `min` covering all combinations of
 *   `mpfr::mpreal` with `double`, `int`, `float`, `long`, and `size_t`.
 * - Fused-arithmetic `LAZEX_OVERRIDE_OPER` specialisations that intercept structural
 *   patterns (e.g. `a + b*c`) and replace them with a single MPFR fused call
 *   (`mpfr_fma`, `mpfr_fms`, `mpfr_fmma`, `mpfr_fmms`) for higher accuracy and
 *   fewer rounding steps.
 * - `set_default_mpreal_prec(prec)` — change the default MPFR precision globally and
 *   resize all registered scratch buffers.
 * - `isfinite(LazyType<mpfr::mpreal>)` — overload matching `std::isfinite` for lazy
 *   mpreal variables.
 *
 * ### Fused operations (requires MPFR >= 4.0)
 * When `MPFR_VERSION >= MPFR_VERSION_NUM(4,0,0)`, the additional fused
 * multiply-multiply-add (`mpfr_fmma`) and subtract (`mpfr_fmms`) overrides are
 * compiled in.
 *
 * ### Precision management
 * `set_default_mpreal_prec` iterates
 * over the registered scratch buffers and calls `set_prec` on every scratch variable so that precision
 * changes are reflected throughout the evaluation pipeline.
 *
 * @note Include this header *instead of* `lazex.hpp` when working with
 *       `mpfr::mpreal`. It includes `lazex.hpp` internally.
 */

#include <mpreal.h>
#include "../lazex.hpp" // IWYU pragma: keep

/// Plug `LazyType<mpfr::mpreal>` into `std::numeric_limits` so that generic
/// numerical code (e.g. ODE solvers querying `epsilon`) works transparently.

#ifndef LAZEX_MPFR_RND
#define LAZEX_MPFR_RND mpfr::mpreal::get_default_rnd()
#endif

LAZEX_DECLARE_NUMERIC_TYPE(mpfr::mpreal);

template<>
inline constexpr size_t lazex::required_workers<mpfr::mpreal> = 1;

namespace lazex::detail {


/**
 * @brief Unary function specialisations for `mpfr::mpreal`.
 *
 * Overrides `CustomUnaryEvaluator<mpfr::mpreal>::evaluate` for `NEG`, `ABS`, and `SQRT`
 * using the corresponding raw MPFR C library functions, which avoid any overhead from
 * the `mpfr::mpreal` operator overloads and respect the global rounding mode.
 */
LAZEX_SPECIALIZE_FUNCTIONS(mpfr::mpreal){

    using T = mpfr::mpreal;
    using Base = UnaryEvaluator<CustomUnaryEvaluator<T>, T>;
    using Base::eval_rule;
    using Base::evaluate;

    // neg
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::NEG, T &out, Pool<T> /**/, const T &a){
        mpfr_neg(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // abs
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::ABS, T &out, Pool<T> /**/, const T &a){
        mpfr_abs(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // sqrt
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::SQRT, T &out, Pool<T> /**/, const T &a){
        mpfr_sqrt(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // exp
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::EXP, T &out, Pool<T> /**/, const T &a){
        mpfr_exp(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // log
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::LOG, T &out, Pool<T> /**/, const T &a){
        mpfr_log(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // sin
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::SIN, T &out, Pool<T> /**/, const T &a){
        mpfr_sin(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // cos
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::COS, T &out, Pool<T> /**/, const T &a){
        mpfr_cos(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // tan
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::TAN, T &out, Pool<T> /**/, const T &a){
        mpfr_tan(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // cot
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::COT, T &out, Pool<T> /**/, const T &a){
        mpfr_cot(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // sec
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::SEC, T &out, Pool<T> /**/, const T &a){
        mpfr_sec(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // csc
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::CSC, T &out, Pool<T> /**/, const T &a){
        mpfr_csc(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // asin
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::ASIN, T &out, Pool<T> /**/, const T &a){
        mpfr_asin(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // acos
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::ACOS, T &out, Pool<T> /**/, const T &a){
        mpfr_acos(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // atan
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::ATAN, T &out, Pool<T> /**/, const T &a){
        mpfr_atan(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // sinh
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::SINH, T &out, Pool<T> /**/, const T &a){
        mpfr_sinh(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // cosh
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::COSH, T &out, Pool<T> /**/, const T &a){
        mpfr_cosh(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // tanh
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::TANH, T &out, Pool<T> /**/, const T &a){
        mpfr_tanh(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // erf
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::ERF, T &out, Pool<T> /**/, const T &a){
        mpfr_erf(out.mpfr_ptr(), a.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

};



/**
 * @brief Binary operation specialisations for `mpfr::mpreal`.
 *
 * Overrides `CustomBinaryEvaluator<mpfr::mpreal>::evaluate` for all combinations of
 * `mpfr::mpreal` with `double`, `int`, `float`, `long`, and `size_t` for the
 * operations `+`, `-`, `*`, `/`, `pow`, `max`, and `min`.
 *
 * Each overload uses the most efficient MPFR C function for the given operand types
 * (e.g. `mpfr_add_d` when one operand is `double`, `mpfr_add_si` for `int`/`long`).
 *
 * Additionally, structural `LAZEX_OVERRIDE_OPER` specialisations replace common sub-expression
 * patterns with MPFR fused operations:
 *
 * | Pattern              | MPFR function  | Condition          |
 * |----------------------|----------------|--------------------|
 * | `a + b*c`            | `mpfr_fma`     | always             |
 * | `a*b + c`            | `mpfr_fma`     | always             |
 * | `a*b - c`            | `mpfr_fms`     | always             |
 * | `c - a*b`            | `mpfr_fms+neg` | always             |
 * | `a*b + c*d`          | `mpfr_fmma`    | MPFR >= 4.0        |
 * | `a*b - c*d`          | `mpfr_fmms`    | MPFR >= 4.0        |
 * | `a + (b + c)`        | `mpfr_sum`     | always (3-sum)     |
 */
LAZEX_SPECIALIZE_OPERATIONS(mpfr::mpreal){

    using T = mpfr::mpreal;
    using Base = BinaryEvaluator<CustomBinaryEvaluator<T>, T>;
    using Base::evaluate;
    using Base::eval_rule;

    // mpreal with mpreal
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const T &a, const T &b){
        mpfr_add(out.mpfr_ptr(), a.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with double
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const T &a, const double &b){
        mpfr_add_d(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const double &a, const T &b){
        mpfr_add_d(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }

    // mpreal with int
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const T &a, const int &b){
        mpfr_add_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const int &a, const T &b){
        mpfr_add_si(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }

    // mpreal with float
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const T &a, const float &b){
        mpfr_add_d(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const float &a, const T &b){
        mpfr_add_d(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }

    // mpreal with long
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const T &a, const long &b){
        mpfr_add_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const long &a, const T &b){
        mpfr_add_si(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }

    // mpreal with size_t
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const T &a, const size_t &b){
        mpfr_add_ui(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::PLUS, T &out, Pool<T> /**/, const size_t &a, const T &b){
        mpfr_add_ui(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }






    // mpreal with mpreal
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const T &a, const T &b){
        mpfr_sub(out.mpfr_ptr(), a.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with double
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const T &a, const double &b){
        mpfr_sub_d(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const double &a, const T &b){
        mpfr_d_sub(out.mpfr_ptr(), a, b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with int
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const T &a, const int &b){
        mpfr_sub_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const int &a, const T &b){
        mpfr_si_sub(out.mpfr_ptr(), a, b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with float
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const T &a, const float &b){
        mpfr_sub_d(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const float &a, const T &b){
        mpfr_d_sub(out.mpfr_ptr(), a, b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with long
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const T &a, const long &b){
        mpfr_sub_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const long &a, const T &b){
        mpfr_si_sub(out.mpfr_ptr(), a, b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with size_t
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const T &a, const size_t &b){
        mpfr_sub_ui(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MINUS, T &out, Pool<T> /**/, const size_t &a, const T &b){
        mpfr_ui_sub(out.mpfr_ptr(), a, b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }







    // mpreal with mpreal
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const T &a, const T &b){
        mpfr_mul(out.mpfr_ptr(), a.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with double
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const T &a, const double &b){
        mpfr_mul_d(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const double &a, const T &b){
        mpfr_mul_d(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }

    // mpreal with int
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const T &a, const int &b){
        mpfr_mul_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const int &a, const T &b){
        mpfr_mul_si(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }

    // mpreal with float
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const T &a, const float &b){
        mpfr_mul_d(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const float &a, const T &b){
        mpfr_mul_d(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }

    // mpreal with long
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const T &a, const long &b){
        mpfr_mul_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const long &a, const T &b){
        mpfr_mul_si(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }

    // mpreal with size_t
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const T &a, const size_t &b){
        mpfr_mul_ui(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MUL, T &out, Pool<T> /**/, const size_t &a, const T &b){
        mpfr_mul_ui(out.mpfr_ptr(), b.mpfr_srcptr(), a, LAZEX_MPFR_RND);
    }








    // mpreal with mpreal
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> /**/, const T &a, const T &b){
        mpfr_div(out.mpfr_ptr(), a.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with double
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> /**/, const T &a, const double &b){
        mpfr_div_d(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> /**/, const double &a, const T &b){
        mpfr_d_div(out.mpfr_ptr(), a, b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with int
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> /**/, const T &a, const int &b){
        mpfr_div_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> workers, const int &a, const T &b){
        T& worker = workers.consume();
        mpfr_set_si(worker.mpfr_ptr(), a, LAZEX_MPFR_RND);
        mpfr_div(out.mpfr_ptr(), worker.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with float
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> /**/, const T &a, const float &b){
        mpfr_div_d(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> /**/, const float &a, const T &b){
        mpfr_d_div(out.mpfr_ptr(), a, b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with long
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> /**/, const T &a, const long &b){
        mpfr_div_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> workers, const long &a, const T &b){
        T& worker = workers.consume();
        mpfr_set_si(worker.mpfr_ptr(), a, LAZEX_MPFR_RND);
        mpfr_div(out.mpfr_ptr(), worker.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with size_t
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> /**/, const T &a, const size_t &b){
        mpfr_div_ui(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::DIV, T &out, Pool<T> workers, const size_t &a, const T &b){
        T& worker = workers.consume();
        mpfr_set_ui(worker.mpfr_ptr(), a, LAZEX_MPFR_RND);
        mpfr_div(out.mpfr_ptr(), worker.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }






    // mpreal with mpreal
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> /**/, const T &a, const T &b){
        mpfr_pow(out.mpfr_ptr(), a.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with double
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> workers, const T &a, const double &b){
        T& worker = workers.consume();
        mpfr_set_d(worker.mpfr_ptr(), b, LAZEX_MPFR_RND);
        mpfr_pow(out.mpfr_ptr(), a.mpfr_srcptr(), worker.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> workers, const double &a, const T &b){
        T& worker = workers.consume();
        mpfr_set_d(worker.mpfr_ptr(), a, LAZEX_MPFR_RND);
        mpfr_pow(out.mpfr_ptr(), worker.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with int
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> /**/, const T &a, const int &b){
        mpfr_pow_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> workers, const int &a, const T &b){
        T& worker = workers.consume();
        mpfr_set_si(worker.mpfr_ptr(), a, LAZEX_MPFR_RND);
        mpfr_pow(out.mpfr_ptr(), worker.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with float
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> workers, const T &a, const float &b){
        T& worker = workers.consume();
        mpfr_set_d(worker.mpfr_ptr(), static_cast<double>(b), LAZEX_MPFR_RND);
        mpfr_pow(out.mpfr_ptr(), a.mpfr_srcptr(), worker.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> workers, const float &a, const T &b){
        T& worker = workers.consume();
        mpfr_set_d(worker.mpfr_ptr(), static_cast<double>(a), LAZEX_MPFR_RND);
        mpfr_pow(out.mpfr_ptr(), worker.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with long
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> /**/, const T &a, const long &b){
        mpfr_pow_si(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> workers, const long &a, const T &b){
        T& worker = workers.consume();
        mpfr_set_si(worker.mpfr_ptr(), a, LAZEX_MPFR_RND);
        mpfr_pow(out.mpfr_ptr(), worker.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // mpreal with size_t
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> /**/, const T &a, const size_t &b){
        mpfr_pow_ui(out.mpfr_ptr(), a.mpfr_srcptr(), b, LAZEX_MPFR_RND);
    }

    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::POW, T &out, Pool<T> workers, const size_t &a, const T &b){
        T& worker = workers.consume();
        mpfr_set_ui(worker.mpfr_ptr(), a, LAZEX_MPFR_RND);
        mpfr_pow(out.mpfr_ptr(), worker.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // min
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MIN, T &out, Pool<T> /**/, const T &a, const T &b){
        mpfr_min(out.mpfr_ptr(), a.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

    // max
    LAZEX_FORCE_INLINE static void evaluate(lazex::tags::MAX, T &out, Pool<T> /**/, const T &a, const T &b){
        mpfr_max(out.mpfr_ptr(), a.mpfr_srcptr(), b.mpfr_srcptr(), LAZEX_MPFR_RND);
    }




    using Mul_T_T = lazex::patterns::Multiplication<T, T>;
    using Add_T_T = lazex::patterns::Addition<T, T>;

    /// 3-operand sum: `a + (b + c)` via `mpfr_sum` for improved accuracy.
    LAZEX_OVERRIDE_OPER(T, a, b, T, lazex::tags::PLUS, Add_T_T){
        // b is an Add expression with lhs and rhs
        T& a_mut = const_cast<T&>(get_value(a));
        T& b_lhs_mut = const_cast<T&>(get_value(b.get<0>()));
        T& b_rhs_mut = const_cast<T&>(get_value(b.get<1>()));
        mpfr_ptr tmp[3] = {a_mut.mpfr_ptr(), b_lhs_mut.mpfr_ptr(), b_rhs_mut.mpfr_ptr()};
        mpfr_sum(out.mpfr_ptr(), tmp, 3, LAZEX_MPFR_RND);
    }

    /// Fused multiply-add: `a + b*c` via `mpfr_fma`.
    LAZEX_OVERRIDE_OPER(T, a, b, T, lazex::tags::PLUS, Mul_T_T){
        // b is a Mul expression with lhs and rhs
        mpfr_fma(out.mpfr_ptr(),
                 get_value(b.get<0>()).mpfr_srcptr(),
                 get_value(b.get<1>()).mpfr_srcptr(),
                 get_value(a).mpfr_srcptr(),
                 LAZEX_MPFR_RND);
    }

    /// Fused multiply-add (commuted): `a*b + c` via `mpfr_fma`.
    LAZEX_OVERRIDE_OPER(T, a, b, Mul_T_T, lazex::tags::PLUS, T){
        mpfr_fma(out.mpfr_ptr(),
                 get_value(a.get<0>()).mpfr_srcptr(),
                 get_value(a.get<1>()).mpfr_srcptr(),
                 get_value(b).mpfr_srcptr(),
                 LAZEX_MPFR_RND);
    }

    /// Fused multiply-subtract: `a*b - c` via `mpfr_fms`.
    LAZEX_OVERRIDE_OPER(T, a, b, Mul_T_T, lazex::tags::MINUS, T){
        mpfr_fms(out.mpfr_ptr(),
                 get_value(a.get<0>()).mpfr_srcptr(),
                 get_value(a.get<1>()).mpfr_srcptr(),
                 get_value(b).mpfr_srcptr(),
                 LAZEX_MPFR_RND);
    }

    /// Negated fused multiply-subtract: `c - a*b` via `mpfr_fms` followed by negation.
    LAZEX_OVERRIDE_OPER(T, a, b, T, lazex::tags::MINUS, Mul_T_T){
        mpfr_fms(out.mpfr_ptr(),
                 get_value(b.get<0>()).mpfr_srcptr(),
                 get_value(b.get<1>()).mpfr_srcptr(),
                 get_value(a).mpfr_srcptr(),
                 LAZEX_MPFR_RND);
        mpfr_neg(out.mpfr_ptr(), out.mpfr_srcptr(), LAZEX_MPFR_RND);
    }

#if MPFR_VERSION >= MPFR_VERSION_NUM(4, 0, 0)
    /// Fused multiply-multiply-add: `a*b + c*d` via `mpfr_fmma` (MPFR >= 4.0).
    LAZEX_OVERRIDE_OPER(T, a, b, Mul_T_T, lazex::tags::PLUS, Mul_T_T){
        mpfr_fmma(out.mpfr_ptr(),
                  get_value(a.get<0>()).mpfr_srcptr(),
                  get_value(a.get<1>()).mpfr_srcptr(),
                  get_value(b.get<0>()).mpfr_srcptr(),
                  get_value(b.get<1>()).mpfr_srcptr(),
                  LAZEX_MPFR_RND);
    }

    /// Fused multiply-multiply-subtract: `a*b - c*d` via `mpfr_fmms` (MPFR >= 4.0).
    LAZEX_OVERRIDE_OPER(T, a, b, Mul_T_T, lazex::tags::MINUS, Mul_T_T){
        mpfr_fmms(out.mpfr_ptr(),
                  get_value(a.get<0>()).mpfr_srcptr(),
                  get_value(a.get<1>()).mpfr_srcptr(),
                  get_value(b.get<0>()).mpfr_srcptr(),
                  get_value(b.get<1>()).mpfr_srcptr(),
                  LAZEX_MPFR_RND);
    }
#endif // MPFR_VERSION >= 4.0

};


inline bool isfinite(const LazyType<mpfr::mpreal>& x){
    return mpfr::isfinite(get_value(x));
}

template<>
struct WorkerReset<mpfr::mpreal>{
    static void apply(mpfr::mpreal& worker){
        worker.set_prec(mpfr::mpreal::get_default_prec());
    }
};


}; // namespace lazex::detail


namespace lazex {


using lazex::detail::isfinite;

/**
 * @brief Set the global default MPFR precision and resize all scratch buffers.
 * @param prec  The new MPFR precision in bits (e.g. 256 for quad-like precision).
 */
inline void set_default_mpreal_prec(mpfr_prec_t prec){
    mpfr::mpreal::set_default_prec(prec);
    update_workers<mpfr::mpreal>();
}

} // namespace lazex


#endif // LAZEX_MPREAL_HPP