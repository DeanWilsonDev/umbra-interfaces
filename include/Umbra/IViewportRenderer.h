#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace Umbra {

// One rendered frame of a viewport: tightly packed RGBA8 pixels, top row first. The
// pointer is owned by the renderer and stays valid until its next Render call; a
// backend copies the pixels into its own texture before returning to the renderer.
struct ViewportFrame {
    const std::uint8_t* Pixels = nullptr;
    std::uint32_t       Width  = 0;
    std::uint32_t       Height = 0;
};

// The modifier keys held during a pointer or key event, as the backend saw them.
struct ViewportModifiers {
    bool Shift    = false;
    bool Control  = false;
    bool Alt      = false;
    bool Shortcut = false; // Command on macOS, Control elsewhere
};

enum class ViewportPointerButton : std::uint8_t {
    Left   = 0,
    Middle = 1,
    Right  = 2,
};

// The contract a host implements so that a UI runtime's `<viewport renderer="Name">`
// element can show a scene the host renders itself: a game engine's level view, a
// model preview, a paint canvas. The runtime names the renderer by the string a host
// registered it under; the backend owning the element calls Render each frame with the
// element's size in device pixels, uploads the pixels it gets back, and forwards the
// pointer and key input that lands on the element. Nothing here names a backend, a
// runtime or a graphics API: the host draws however it likes and hands back pixels.
//
// Coordinates are in viewport pixels, origin top-left. Key names follow the web's
// `KeyboardEvent.key` ("w", "Escape", "ArrowLeft", "Delete", "F2"). Key returns true when
// the renderer used the key, so the backend can keep it from reaching other handlers.
//
// What a backend guarantees:
// - PointerMoved is delivered before any PointerButton or Wheel of the same frame, and on
//   every frame the pointer is over the element or a button it pressed there is still
//   held, so the position the renderer last saw is current when a button event arrives.
// - A button pressed over the element is released to the same renderer, wherever the
//   pointer is by then.
// - KeyName names the key itself, not the character it would type, so a press and its
//   release carry the same name whatever modifiers changed in between; the modifiers
//   held at each event come in Modifiers.
// - Key is delivered only while the pointer is over the element or one of its buttons is
//   held. A release that happens elsewhere is not delivered, so a renderer that tracks
//   held keys drops them on HoverChanged(false) once no button is held, the way Lantern's
//   level view does.
class IViewportRenderer {
public:
    virtual std::optional<ViewportFrame> Render(std::uint32_t WidthPixels, std::uint32_t HeightPixels) = 0;

    virtual void PointerMoved(float X, float Y) {}
    virtual void PointerButton(ViewportPointerButton Button, bool Down, const ViewportModifiers& Modifiers) {}
    virtual void Wheel(float Notches, float NotchesX) {}
    virtual bool Key(std::string_view KeyName, bool Down, const ViewportModifiers& Modifiers) { return false; }
    virtual void HoverChanged(bool PointerIsOver) {}
    virtual void Tick(float DeltaSeconds) {}

    virtual ~IViewportRenderer() = default;
};

} // namespace Umbra
