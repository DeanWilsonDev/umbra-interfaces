# umbra-interfaces

Backend-agnostic interfaces shared between a widget-owning reactive runtime and its
rendering backends — currently used by [Chaos](https://github.com/DeanWilsonDev/chaos) (the
runtime) and, via [penumbra-ui-backend](https://github.com/DeanWilsonDev/penumbra-ui-backend),
[Penumbra](https://github.com/DeanWilsonDev/penumbra-proto) (a rendering backend).

## Why this exists

`IWidget`/`ChaosPropDiff` and `IWidgetLifecycle`/`TickInfo` are the contract a reconciler
(Chaos's Stage 3, `docs/iris_stage3_decision_doc.md` in the `chaos` repo) needs to update a
live widget in place without knowing which concrete backend built it, and the contract a
backend's own frame loop needs to drive that reconciler's per-frame work — without either
side depending on the other's headers directly.

This package has **zero dependencies** — not on Chaos, not on Penumbra, not on anything.
It names no concrete runtime and no concrete backend anywhere in it. A runtime depends on
it to know what shape a widget must satisfy; a backend (via its own bridge/adapter code —
e.g. `penumbra-ui-backend`) depends on it to know what shape to implement.

## What's here

- `include/Umbra/IWidget.h` — `IWidget::ApplyPropDiff(const ChaosPropDiff&)`, the
  reconciler-facing update contract, and `ChaosPropDiff` itself: one `std::optional` field
  per prop-value kind a Core primitive can carry.
- `include/Umbra/IWidgetLifecycle.h` — `IWidgetLifecycle` (`OnMount`/`OnUnmount`/`OnTick`)
  and `TickInfo`, the per-frame hook contract between a backend's frame loop and anything
  that needs to run logic alongside it.
- `include/Umbra/TextureHandle.h` — an opaque texture-handle vocabulary type, referenced
  by `ChaosPropDiff::Handle`.

## Requirements

- GCC 14 or later, Clang 19 or later, or a recent AppleClang
- On Windows, clang-cl from Clang 19 or later; MSVC's cl.exe isn't supported
- CMake 3.30 or later
- C++26 — the `umbra_interfaces` target requires `cxx_std_26` of everything that links it

## Build

Header-only; nothing to build standalone. Consuming projects add this repo as a git
submodule and `add_subdirectory` it, linking against the `umbra_interfaces` INTERFACE
target:

```cmake
add_subdirectory(vendor/umbra-interfaces)
target_link_libraries(your_target PUBLIC umbra_interfaces)
```
