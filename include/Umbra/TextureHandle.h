#pragma once

namespace Umbra {

// Opaque, runtime-owned reference to a decoded backend texture. A texture handle is a
// rendering-backend concept, not specific to any one project depending on this
// interfaces package — swapping handles during a reconciler's diffing pass (e.g. Iris's
// Stage 3, docs/iris_stage3_decision_doc.md §5's `<Image>` update path) is meant to be a
// pointer assignment, zero disk I/O, unlike a `src`-path's synchronous re-decode.
//
// This type carries no data or behavior of its own here — it's a shared vocabulary
// type. Populating and loading through it is entirely the depending project's own
// runtime's job (e.g. a future `iris::LoadTextures()`).
class TextureHandle {
public:
    TextureHandle() = default;
};

} // namespace Umbra
