cmake_minimum_required(VERSION 3.24)
project(lazex LANGUAGES CXX)

add_library(lazex INTERFACE)
add_library(lazex::lazex ALIAS lazex)
target_include_directories(lazex INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(lazex INTERFACE cxx_std_20)

set(LAZEX_MPFR_RND "MPFR_RNDN" CACHE STRING
    "Rounding mode used by lazex/apps/lazex_mpreal.hpp. Empty: mpfr::mpreal::get_default_rnd() at runtime.")
if(LAZEX_MPFR_RND)
    target_compile_definitions(lazex INTERFACE LAZEX_MPFR_RND=${LAZEX_MPFR_RND})
endif()

# Building lazex itself: a target compiling each header on its own (not built by default)
# gives compile_commands.json an entry for every header.
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    file(GLOB_RECURSE LAZEX_HEADERS CONFIGURE_DEPENDS include/*.hpp)
    target_sources(lazex INTERFACE FILE_SET HEADERS BASE_DIRS include FILES ${LAZEX_HEADERS})
    set_target_properties(lazex PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON)
endif()
