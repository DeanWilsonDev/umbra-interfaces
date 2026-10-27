#pragma once

#include "Umbra/LivenessGuard.h"
#include "Umbra/TextureHandle.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Umbra {

struct ChaosPropDiff {
    std::optional<std::string>            ClassName;
    std::optional<std::string>            Text;
    std::optional<std::string>            Src;
    std::optional<std::string>            Icon;
    std::optional<TextureHandle>          Handle;
    std::optional<bool>                   Checked;
    std::optional<std::function<void()>>  OnPress;
    std::optional<std::function<void()>>  OnRelease;
    std::optional<std::function<void()>>  OnHover;
    std::optional<std::function<void()>>  OnFocus;
    std::optional<std::function<void()>>  OnChange;
    std::optional<std::function<void(std::string)>> OnTextChange;

    // Drag and drop. An element with a `dragPayload` string prop (in `Strings`) is
    // draggable: once a press passes the backend's threshold it gets OnDragStart, then
    // OnDrag with the pointer's window position in logical pixels as the drag starts and
    // on every move, then OnDragEnd on release, dropped or not. An element with any of
    // the last three is a drop target: OnDragEnter and OnDragLeave as a drag's pointer
    // crosses it, OnDrop with the payload on release over it.
    std::optional<std::function<void()>>             OnDragStart;
    std::optional<std::function<void(float, float)>> OnDrag;
    std::optional<std::function<void()>>             OnDragEnd;
    std::optional<std::function<void()>>             OnDragEnter;
    std::optional<std::function<void()>>             OnDragLeave;
    std::optional<std::function<void(std::string)>>  OnDrop;

    // A key no focused field used, when this is the topmost element, in paint order, with
    // an OnKeyDown: a menu over a modal gets it before the modal does. The key is named as
    // the web's `KeyboardEvent.key` names it ("Escape", "Enter", "Tab", "ArrowDown", "Home",
    // "Backspace"). A backend sends only the keys it knows.
    std::optional<std::function<void(std::string)>>  OnKeyDown;

    // The release of a key OnKeyDown would have delivered, named the same way and routed
    // the same way: to the topmost element with an OnKeyUp at the time of the release,
    // not to whichever element took the press. An element that tracks held keys treats
    // a release it never sees as released when it stops being topmost.
    std::optional<std::function<void(std::string)>>  OnKeyUp;

    // The pointer leaving an element it was over; the counterpart of OnHover.
    std::optional<std::function<void()>>             OnHoverEnd;

    // A `<select>`'s chosen option. Its options travel in StringLists["options"] and
    // the current one in Strings["selected"].
    std::optional<std::function<void(std::string)>>  OnSelect;

    // A `<checkbox>`'s new state after the user toggles it. Checked carries the
    // state the program set.
    std::optional<std::function<void(bool)>>         OnToggle;

    // Every number-valued prop (int or float) whose value changed or was added, by
    // name, ints widened to float. Lets a backend follow numeric props it reads by
    // name rather than through a field of their own -- a stylesheet's `attr()`
    // bindings, say -- without this struct growing a field per prop.
    std::unordered_map<std::string, float> Numbers;

    // The same for string props (every one, including those with a field above), and
    // for the two list kinds: a chart's `labels`, a series' `values`.
    std::unordered_map<std::string, std::string>              Strings;
    std::unordered_map<std::string, std::vector<float>>       NumberLists;
    std::unordered_map<std::string, std::vector<std::string>> StringLists;
};

// The backend-agnostic contract a live widget must satisfy for a reconciler (e.g.
// Chaos's Stage 3) to update it in place without knowing the concrete backend type
// underneath. A backend adapter (e.g. `penumbra-ui-backend`'s bridge from
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
    virtual void ApplyPropDiff(const ChaosPropDiff& Diff) = 0;

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
    // Chaos's SlotState::AttachedParent_) should keep a LivenessGuard::Watch from this
    // and call Watch::AssertAlive(...) immediately before dereferencing the pointer in
    // its own destructor, instead of risking a dangling-pointer crash inside whatever
    // concrete backend implements this interface.
    const LivenessGuard& Liveness() const { return Liveness_; }

    virtual ~IWidget() = default;

private:
    LivenessGuard Liveness_;
};

} // namespace Umbra
