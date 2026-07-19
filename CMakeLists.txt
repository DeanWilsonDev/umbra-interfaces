cmake_minimum_required(VERSION 3.24)

project(UmbraInterfaces
    VERSION 0.1.0
    DESCRIPTION "Backend-agnostic interfaces shared between a widget-owning runtime and its rendering backends"
    LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Header-only, zero-dependency, on purpose: this package names no concrete
# runtime (Iris, or any future one) and no concrete backend (Penumbra, or
# any future Umbra Engine backend) anywhere in it. Anything that would give
# it an opinion about either side belongs in one of them, not here.
# ---------------------------------------------------------------------------
add_library(umbra_interfaces INTERFACE)
target_include_directories(umbra_interfaces INTERFACE include)
target_compile_features(umbra_interfaces INTERFACE cxx_std_20)
