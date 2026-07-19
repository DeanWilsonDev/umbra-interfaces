#pragma once

namespace Umbra {

// Per-frame timing handed to lifecycle hooks. A plain struct (not a float parameter)
// so more fields (e.g. total elapsed time) can be added later without changing every
// OnTick override.
struct TickInfo {
    float DeltaSeconds = 0.0f;
};

// Shared lifecycle contract between a widget-owning runtime (Iris, for now) and a
// backend's own frame loop (Penumbra's Application today; a future Umbra Engine
// backend later). The backend calls these hooks; it never calls into the runtime
// directly, and this header names nothing runtime- or backend-specific — that's the
// whole point of it living here rather than in either project.
class IWidgetLifecycle {
public:
    virtual void OnMount() {}
    virtual void OnUnmount() {}
    virtual void OnTick(const TickInfo& Info) {}
    virtual ~IWidgetLifecycle() = default;
};

} // namespace Umbra
