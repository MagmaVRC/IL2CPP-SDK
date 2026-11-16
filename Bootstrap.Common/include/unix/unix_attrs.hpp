#pragma once
#include <cstddef>
#include <cstdint>
#include "unix_module_abi.hpp"

namespace UNIx {

enum class Thread    : uint8_t { Any, UnityMain, Render, Photon, Loader };
enum class Stability : uint8_t { Stable, Experimental, Internal };

// Member order is a language rule: every .inc row writes its designators as a subsequence
// of this declaration order, and an out-of-order row is a hard MSVC error.
struct Attr {
    uint32_t    since        = 0;         // UNIX_VER(major, minor, patch)
    uint32_t    deprecated   = 0;         // 0 = live
    uint32_t    removal      = 0;         // planned; forces a date on every deprecation
    uint32_t    removed      = 0;         // 0 = present
    const char* replacement  = nullptr;
    const char* code         = nullptr;   // "UNIX0060", stable, unique across the table
    const char* message      = nullptr;
    const char* url          = nullptr;
    uint32_t    legacy_below = 0;         // hand fn_legacy to modules declaring sdk < this
    uint64_t    caps         = 0;         // what calling this row reaches; names the feature
                                          // a .uxuse record resolves to. Not a gate.
    Thread      thread       = Thread::Any;
    Stability   stability    = Stability::Stable;
};

/// <summary>A half-written attribute row is a compile error, not a runtime surprise.</summary>
/// <param name="e">The export table; each row exposes an `attr` member of type Attr.</param>
/// <returns>False if any row is deprecated without a removal date, code or replacement;
/// removed without a code or replacement; Experimental without a code; carries a removal
/// that does not follow its deprecation; stamps since/deprecated/removed past this
/// SDK version; or is still present at or past its removal version. `removal` is otherwise
/// unbounded: it is a future date.</returns>
template <class Row, std::size_t N>
consteval bool UnixAttrsAreWellFormed(const Row (&e)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        const Attr& a = e[i].attr;
        if (a.deprecated && !(a.removal && a.code && a.replacement)) return false;
        if (a.removed    && !(a.code && a.replacement))              return false;
        if (a.stability == Stability::Experimental && !a.code)       return false;
        if (a.removal && !(a.removal > a.deprecated))                return false;
        if (a.removal && !a.removed && a.removal <= UNIX_SDK_VERSION) return false;
        if (a.since > UNIX_SDK_VERSION || a.deprecated > UNIX_SDK_VERSION
                                      || a.removed > UNIX_SDK_VERSION) return false;
    }
    return true;
}

} // namespace UNIx
