cmake_minimum_required(VERSION 3.14)

project(lazex)

add_library(lazex INTERFACE)

target_compile_features(lazex INTERFACE cxx_std_20)

target_include_directories(lazex
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(LAZEX_MPFR_RND "MPFR_RNDN" CACHE STRING
    "Rounding mode passed as LAZEX_MPFR_RND to consumers of the lazex target (see include/lazex/apps/lazex_mpreal.hpp). Leave empty to fall back to mpfr::mpreal::get_default_rnd() at runtime.")

if(LAZEX_MPFR_RND)
    target_compile_definitions(lazex
        INTERFACE
            LAZEX_MPFR_RND=${LAZEX_MPFR_RND}
    )
endif()

# ----------- Optional MPFR support (lazex/apps/lazex_mpreal.hpp) ---------------
# Kept separate from the core `lazex` target since most consumers never touch
# MPFR: link against `lazex::mpfr` instead of `lazex` to opt in to mpreal's
# include dir plus the mpfr/gmp system libraries. Gated by an explicit option
# rather than just checking whether external/mpreal exists: having the
# mpreal.h submodule checked out doesn't mean MPFR/GMP are actually installed
# and linkable, since the submodule may just be present from a blanket
# `git submodule update --init --recursive`.
option(LAZEX_ENABLE_MPREAL "Enable MPFR support: defines the lazex::mpfr target (mpreal.h include dir, mpfr/gmp links). Requires the mpreal submodule and system MPFR/GMP. Does not affect the headers themselves — include lazex/apps/lazex_mpreal.hpp explicitly to use it." OFF)

if(LAZEX_ENABLE_MPREAL)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/mpreal/mpreal.h)
        message(FATAL_ERROR "LAZEX_ENABLE_MPREAL requires the 'mpreal' submodule. Run: git submodule update --init --recursive")
    endif()

    add_library(mpreal INTERFACE)
    target_include_directories(mpreal
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/external/mpreal
    )

    add_library(lazex_mpfr INTERFACE)
    add_library(lazex::mpfr ALIAS lazex_mpfr)

    target_link_libraries(lazex_mpfr
        INTERFACE
            lazex
            mpreal
            mpfr
            gmp
    )
endif()

# ----------- Dev-only targets (example) ---------------
# Only built when lazex is configured directly (not when pulled in via
# add_subdirectory/FetchContent by another project), so
# consumers who only want the interface never build or even see this target.

if((DEFINED PROJECT_IS_TOP_LEVEL AND PROJECT_IS_TOP_LEVEL) OR
   (NOT DEFINED PROJECT_IS_TOP_LEVEL AND CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR))
    set(LAZEX_BUILD_EXAMPLE_DEFAULT ON)
else()
    set(LAZEX_BUILD_EXAMPLE_DEFAULT OFF)
endif()
option(LAZEX_BUILD_EXAMPLE "Build the lazex example executable" ${LAZEX_BUILD_EXAMPLE_DEFAULT})

if(LAZEX_BUILD_EXAMPLE)
    if(LAZEX_ENABLE_MPREAL)
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

        add_executable(example example.cpp)

        target_link_libraries(example
            PRIVATE
                lazex::mpfr
        )

        target_compile_options(example
            PRIVATE
                -O3
                -DNDEBUG
        )
    else()
        message(STATUS "Skipping the lazex example: it uses mpfr::mpreal, so it needs -DLAZEX_ENABLE_MPREAL=ON")
    endif()
endif()
