cmake_minimum_required(VERSION 3.30)

project(UmbraInterfaces
    VERSION 0.1.0
    DESCRIPTION "Backend-agnostic interfaces shared between a widget-owning runtime and its rendering backends"
    LANGUAGES CXX)

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
   OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 18))
  message(FATAL_ERROR "umbra-interfaces needs GCC 14 or later or Clang 18 or later; found ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Header-only, zero-dependency, on purpose: this package names no concrete
# runtime (Iris, or any future one) and no concrete backend (Penumbra, or
# any future Umbra Engine backend) anywhere in it. Anything that would give
# it an opinion about either side belongs in one of them, not here.
# ---------------------------------------------------------------------------
add_library(umbra_interfaces INTERFACE)
target_include_directories(umbra_interfaces INTERFACE include)
target_compile_features(umbra_interfaces INTERFACE cxx_std_26)
