#pragma once

#include <cstdio>
#include <cstdlib>
#include <memory>

namespace Umbra {

// A destruction-order guard: an owned object embeds one as a member (cost: one
// shared_ptr<char>). Anything holding a raw, non-owning pointer into that object keeps a
// LivenessGuard::Watch instead of trusting the pointer blindly, and calls
// Watch::AssertAlive(...) immediately before dereferencing it in a destructor (the moment
// C++'s automatic reverse-declaration-order teardown is most likely to have gotten the
// real dependency graph wrong).
//
// This is not a substitute for getting destruction order right -- it's a way to fail
// loudly, with an attributable message, the instant an ordering mistake is made, instead
// of a dynamic_cast-on-garbage crash (or worse, silent corruption) several frames removed
// from the actual mistake, only diagnosable after the fact via a debugger backtrace or
// ASan. Every real crash this was written against (see pharos-proto's own
// docs/next_steps.md) was exactly this shape: a raw Umbra::IWidget*/Umbra::
// IWidgetLifecycle* held past the pointee's own destruction, only found afterward via
// lldb -- SlotState::~SlotState() -> PenumbraWidget::RemoveChildAt's dangling
// dynamic_cast, and UmbraLifecycleBridge::OnTick/OnUnmount's dangling Inner_.
//
// Debug builds only, by design (NDEBUG-gated to a real no-op below): a development-time
// correctness aid, not a Release-mode safety net. A Release build should never pay for
// it, and a real destruction-order bug should be caught long before a Release build
// ships.
#ifdef NDEBUG

class LivenessGuard {
public:
    class Watch {
    public:
        explicit Watch(const LivenessGuard&) {}
        Watch() = default;
        void Reset(const LivenessGuard&) {}
        void Reset() {}
        bool IsAlive() const { return true; }
        void AssertAlive(const char*) const {}
    };
};

#else

class LivenessGuard {
public:
    LivenessGuard() : Token_(std::make_shared<char>()) {}

    // Not copyable/movable -- a copy would share a token that outlives the original
    // (wrong: each owner needs its own liveness), and a move would leave the moved-from
    // object's own token pointing at nothing meaningful. Owners embed this by value and
    // never need to copy/move it independently of the whole containing object anyway.
    LivenessGuard(const LivenessGuard&) = delete;
    LivenessGuard& operator=(const LivenessGuard&) = delete;
    LivenessGuard(LivenessGuard&&) = delete;
    LivenessGuard& operator=(LivenessGuard&&) = delete;

    class Watch {
    public:
        explicit Watch(const LivenessGuard& Guard) : Token_(Guard.Token_) {}
        Watch() = default; // Unwatched -- IsAlive()/AssertAlive() are no-ops until Reset().

        void Reset(const LivenessGuard& Guard) { Token_ = Guard.Token_; }
        void Reset() { Token_.reset(); }

        bool IsAlive() const { return !Token_.expired(); }

        // Call immediately before dereferencing the watched raw pointer, especially from
        // a destructor -- aborts with a clear, attributable message instead of
        // proceeding into a dangling dereference. `What` should name both the watcher
        // and the watched relationship, e.g. "SlotState::AttachedParent_".
        void AssertAlive(const char* What) const {
            if (Token_.expired()) {
                std::fprintf(stderr,
                    "Umbra::LivenessGuard: %s was destroyed before its watcher -- fix "
                    "destruction order (see umbra-interfaces/include/Umbra/LivenessGuard.h)\n",
                    What);
                std::abort();
            }
        }

    private:
        std::weak_ptr<char> Token_;
    };

private:
    friend class Watch;
    std::shared_ptr<char> Token_;
};

#endif

} // namespace Umbra
