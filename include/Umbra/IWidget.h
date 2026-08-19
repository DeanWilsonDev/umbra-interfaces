#pragma once

#include "Umbra/LivenessGuard.h"
#include "Umbra/TextureHandle.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace Umbra {

// The strongly-typed diff between two matched widget states — one field per
// `IrisPropValue` variant member a Core primitive can actually carry (see the
// depending project's own prop-value type, e.g. Iris's `docs/iris_props_decision.md`).
// `int`/`float` are deliberately absent: no Core primitive currently needs a numeric
// prop diff — add a field here (and the corresponding prop-value variant member,
// wherever that lives) together, deliberately, if one ever does.
//
// A `std::nullopt` field means "unchanged, don't touch it" — only fields that actually
// differ between the old and new widget state are ever populated, so applying a diff
// never means re-setting everything from scratch.
struct IrisPropDiff {
    std::optional<std::string>            ClassName;
    std::optional<std::string>            Text;
    std::optional<std::string>            Src;
    std::optional<TextureHandle>          Handle;
    std::optional<bool>                   Checked;
    std::optional<std::function<void()>>  OnPress;
    std::optional<std::function<void()>>  OnRelease;
    std::optional<std::function<void()>>  OnHover;
    std::optional<std::function<void()>>  OnFocus;
    std::optional<std::function<void()>>  OnChange;
    std::optional<std::function<void(std::string)>> OnTextChange;
};

// The backend-agnostic contract a live widget must satisfy for a reconciler (e.g.
// Iris's Stage 3) to update it in place without knowing the concrete backend type
// underneath. A backend adapter (e.g. `iris-penumbra-backend`'s bridge from
// `Penumbra::Widgets::WidgetBase`) implements this to translate calls here into
// whatever that backend's real widget API actually looks like.
//
// The child-management methods mirror Penumbra's own `Box` (`AddChild`/`InsertChildAt`/
// `RemoveChild`/`MoveChild` — verified against the real, shipped widget, not just a
// requirements doc) precisely because a reconciler's documented matching rule ("same
// tag + key at the same position → update in place, recurse into children") needs a
// way to attach, detach, and reorder children at an arbitrary tree position — a plain
// `ApplyPropDiff` alone only covers updating a matched widget's own props, never its
// child structure. Every widget (even a leaf) implements these; a leaf's default
// behavior (see `IWidget::` — there is none, this is a pure interface) must report zero
// children and treat every mutating call as a no-op, exactly like Penumbra's own
// `WidgetBase::GetChildCount`/`GetChildAt` defaults.
class IWidget {
public:
    virtual void ApplyPropDiff(const IrisPropDiff& Diff) = 0;

    virtual std::size_t GetChildCount() const = 0;
    virtual IWidget*    GetChildAt(std::size_t Index) const = 0;

    // Inserts Child at Index, shifting existing children at/after Index one position
    // later. Index == GetChildCount() appends.
    virtual void InsertChildAt(std::size_t Index, std::unique_ptr<IWidget> Child) = 0;

    // Removes and returns the child at Index, shifting later children one position
    // earlier — the caller decides the removed widget's fate (drop it to unmount, or
    // reinsert it elsewhere, which is exactly how a keyed list-diff's "move" op is
    // expressed: RemoveChildAt then InsertChildAt at the new position).
    virtual std::unique_ptr<IWidget> RemoveChildAt(std::size_t Index) = 0;

    // Debug-only destruction-order guard (LivenessGuard.h) -- anything holding a raw,
    // non-owning IWidget* past what it can prove is this object's own lifetime (e.g.
    // Iris's SlotState::AttachedParent_) should keep a LivenessGuard::Watch from this
    // and call Watch::AssertAlive(...) immediately before dereferencing the pointer in
    // its own destructor, instead of risking a dangling-pointer crash inside whatever
    // concrete backend implements this interface.
    const LivenessGuard& Liveness() const { return Liveness_; }

    virtual ~IWidget() = default;

private:
    LivenessGuard Liveness_;
};

} // namespace Umbra
