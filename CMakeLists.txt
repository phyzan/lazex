cmake_minimum_required(VERSION 3.21)

project(lazex LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # build/compile_commands.json, picked up by clangd
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE Release)
    endif()
endif()

# ----------- Core (header-only) ---------------

add_library(lazex INTERFACE)
add_library(lazex::lazex ALIAS lazex)
target_include_directories(lazex INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(lazex INTERFACE cxx_std_20)

set(LAZEX_MPFR_RND "MPFR_RNDN" CACHE STRING
    "Rounding mode used by lazex/apps/lazex_mpreal.hpp. Empty: mpfr::mpreal::get_default_rnd() at runtime.")
if(LAZEX_MPFR_RND)
    target_compile_definitions(lazex INTERFACE LAZEX_MPFR_RND=${LAZEX_MPFR_RND})
endif()

# ----------- Optional MPFR support: lazex::mpfr ---------------
# mpreal.h is taken from the system, or downloaded if missing (on by default only
# when building lazex itself, so consumers are never fetched from behind their back).

option(LAZEX_ENABLE_MPREAL "Define lazex::mpfr (mpreal.h + MPFR/GMP)" ${PROJECT_IS_TOP_LEVEL})
option(LAZEX_FETCH_MPREAL "Download mpreal.h if it is not installed" ${PROJECT_IS_TOP_LEVEL})

if(LAZEX_ENABLE_MPREAL)
    find_path(MPREAL_INCLUDE_DIR mpreal.h)
    if(NOT MPREAL_INCLUDE_DIR AND LAZEX_FETCH_MPREAL)
        include(FetchContent)
        FetchContent_Declare(mpreal
            GIT_REPOSITORY https://github.com/advanpix/mpreal.git
            GIT_TAG        mpfrc++-3.7.2
            GIT_SHALLOW    TRUE)
        FetchContent_MakeAvailable(mpreal)
        set(MPREAL_INCLUDE_DIR ${mpreal_SOURCE_DIR})
    endif()
    if(NOT MPREAL_INCLUDE_DIR)
        message(FATAL_ERROR "mpreal.h not found: install it (e.g. libmpfrc++-dev), "
                            "set MPREAL_INCLUDE_DIR, or enable LAZEX_FETCH_MPREAL")
    endif()
    find_library(MPFR_LIBRARY mpfr REQUIRED)
    find_library(GMP_LIBRARY gmp REQUIRED)

    add_library(lazex_mpfr INTERFACE)
    add_library(lazex::mpfr ALIAS lazex_mpfr)
    target_include_directories(lazex_mpfr SYSTEM INTERFACE ${MPREAL_INCLUDE_DIR})
    target_link_libraries(lazex_mpfr INTERFACE lazex ${MPFR_LIBRARY} ${GMP_LIBRARY})
endif()

# ----------- Example (top-level builds only) ---------------

include(CMakeDependentOption)
cmake_dependent_option(LAZEX_BUILD_EXAMPLE "Build the example (uses mpfr::mpreal)"
    ${PROJECT_IS_TOP_LEVEL} "LAZEX_ENABLE_MPREAL" OFF)

if(LAZEX_BUILD_EXAMPLE)
    add_executable(example example.cpp)
    target_link_libraries(example PRIVATE lazex::mpfr)
endif()
