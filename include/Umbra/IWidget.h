#pragma once

#include "Umbra/TextureHandle.h"

#include <functional>
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
};

// The backend-agnostic contract a live widget must satisfy for a reconciler (e.g.
// Iris's Stage 3) to update it in place without knowing the concrete backend type
// underneath. A backend adapter (e.g. `iris-penumbra-backend`'s bridge from
// `Penumbra::Widgets::WidgetBase`) implements this to translate `ApplyPropDiff` calls
// into whatever that backend's real widget API actually looks like.
class IWidget {
public:
    virtual void ApplyPropDiff(const IrisPropDiff& Diff) = 0;
    virtual ~IWidget() = default;
};

} // namespace Umbra
