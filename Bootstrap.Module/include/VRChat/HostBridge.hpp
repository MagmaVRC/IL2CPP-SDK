#pragma once

#include <unix/unix_module_abi.hpp>

#include <cstdint>
#include <optional>
#include <string>

#if defined(UNIX_IN_HOST) && UNIX_IN_HOST

namespace UNIxBootstrap::Api::Typed {

    void*       PlayersLocal() noexcept;
    void*       PlayersLocalApi() noexcept;
    void*       PlayersLocalVrc() noexcept;
    uint32_t    PlayersRank(void* apiUser) noexcept;
    unix_color  PlayersRankColor(uint32_t rank) noexcept;
    unix_handle PlayersOnEvent(uint32_t phase, unix_cb_player cb, void* ud) noexcept;
    unix_status CoreCancel(unix_handle handle) noexcept;
    uint32_t    DeobOffsets(uint32_t table, void* out, uint32_t cap) noexcept;
    std::string VrcPlayerString(void* vrcPlayer, uint32_t which);

} // namespace UNIxBootstrap::Api::Typed

namespace IL2CPP::VRChat::Bridge {

    [[nodiscard]] inline void* LocalPlayer() noexcept {
        return ::UNIxBootstrap::Api::Typed::PlayersLocal();
    }
    [[nodiscard]] inline void* LocalPlayerApi() noexcept {
        return ::UNIxBootstrap::Api::Typed::PlayersLocalApi();
    }
    [[nodiscard]] inline void* LocalVrcPlayer() noexcept {
        return ::UNIxBootstrap::Api::Typed::PlayersLocalVrc();
    }
    [[nodiscard]] inline uint32_t Rank(void* apiUser) noexcept {
        return ::UNIxBootstrap::Api::Typed::PlayersRank(apiUser);
    }
    [[nodiscard]] inline unix_color RankColor(uint32_t rank) noexcept {
        return ::UNIxBootstrap::Api::Typed::PlayersRankColor(rank);
    }
    [[nodiscard]] inline unix_handle OnPlayerEvent(uint32_t phase, unix_cb_player cb, void* ud) noexcept {
        return ::UNIxBootstrap::Api::Typed::PlayersOnEvent(phase, cb, ud);
    }
    inline void Cancel(unix_handle handle) noexcept {
        ::UNIxBootstrap::Api::Typed::CoreCancel(handle);
    }
    [[nodiscard]] inline uint32_t OffsetsRaw(uint32_t table, void* out, uint32_t cap) noexcept {
        return ::UNIxBootstrap::Api::Typed::DeobOffsets(table, out, cap);
    }
    [[nodiscard]] inline std::string VrcPlayerString(void* vrcPlayer, uint32_t which) {
        return ::UNIxBootstrap::Api::Typed::VrcPlayerString(vrcPlayer, which);
    }

} // namespace IL2CPP::VRChat::Bridge

#else

#include <unix/module.hpp>

namespace IL2CPP::VRChat::Bridge {

    [[nodiscard]] inline void* LocalPlayer() noexcept {
        return ::UNIx::detail::g_d.players_local(::UNIx::detail::g_self);
    }
    [[nodiscard]] inline void* LocalPlayerApi() noexcept {
        return ::UNIx::detail::g_d.players_local_api(::UNIx::detail::g_self);
    }
    [[nodiscard]] inline void* LocalVrcPlayer() noexcept {
        return ::UNIx::detail::g_d.players_local_vrc(::UNIx::detail::g_self);
    }
    [[nodiscard]] inline uint32_t Rank(void* apiUser) noexcept {
        return ::UNIx::detail::g_d.players_rank(::UNIx::detail::g_self, apiUser);
    }
    [[nodiscard]] inline unix_color RankColor(uint32_t rank) noexcept {
        return ::UNIx::detail::g_d.players_rank_color(::UNIx::detail::g_self, rank);
    }
    [[nodiscard]] inline unix_handle OnPlayerEvent(uint32_t phase, unix_cb_player cb, void* ud) noexcept {
        return ::UNIx::detail::g_d.players_on_event(::UNIx::detail::g_self, phase, cb, ud);
    }
    inline void Cancel(unix_handle handle) noexcept {
        ::UNIx::detail::g_d.core_cancel(::UNIx::detail::g_self, handle);
    }
    [[nodiscard]] inline uint32_t OffsetsRaw(uint32_t table, void* out, uint32_t cap) noexcept {
        return ::UNIx::detail::g_d.deob_offsets(::UNIx::detail::g_self, table, out, cap);
    }
    [[nodiscard]] inline std::string VrcPlayerString(void*, uint32_t) {
        return {};
    }

} // namespace IL2CPP::VRChat::Bridge

#endif

namespace IL2CPP::VRChat::Bridge {

    enum : uint32_t { kStrUsername = 0, kStrDisplayName = 1, kStrUserId = 2, kStrPronouns = 3 };

    template <class T>
    [[nodiscard]] inline std::optional<T> Offsets(uint32_t table) noexcept {
        T out{};
        out.size = static_cast<uint32_t>(sizeof(T));
        const uint32_t n = OffsetsRaw(table, &out, static_cast<uint32_t>(sizeof(T)));
        if (n == 0u || n >= 0xFFFFFF00u) return std::nullopt;
        return out;
    }

    /// <summary>Whether a pointer can actually be read. A pointer can be non-null, in
    /// user-mode range and still land in reserved, uncommitted memory, which reads as a
    /// plausible address right up until the dereference faults.</summary>
    [[nodiscard]] inline bool Probe(const void* p) noexcept {
        __try {
            const volatile auto probe = *static_cast<const volatile uintptr_t*>(p);
            (void)probe;
            return true;
        } __except (1) {
            return false;
        }
    }

    /// <summary>Whether `self` still looks like a live IL2CPP object rather than a stale
    /// handle. Every object begins with its class pointer, so an object whose class does
    /// not read back is one that has been collected or was never an object at all.</summary>
    [[nodiscard]] inline bool LiveObject(void* self) noexcept {
        const auto p = reinterpret_cast<uintptr_t>(self);
        if (p < 0x10000ull || p >= 0x7FFFFFFFFFFFull || (p & 7ull) != 0ull) return false;
        if (!Probe(self)) return false;

        const auto klass = *reinterpret_cast<void* volatile*>(self);
        const auto k = reinterpret_cast<uintptr_t>(klass);
        if (k < 0x10000ull || k >= 0x7FFFFFFFFFFFull || (k & 7ull) != 0ull) return false;
        return Probe(klass);
    }

    /// <summary>Read a pointer-sized member at a deobfuscated offset.</summary>
    /// <returns>Null when the object is not one any more, rather than faulting on it.</returns>
    [[nodiscard]] inline void* MemberAt(void* self, int32_t offset) noexcept {
        // A null check alone is what let a stale reference through: the crash that prompted
        // this read a field off 0x200000008, which is non-null and reserved.
        if (!self || offset <= 0 || !LiveObject(self)) return nullptr;

        void* value = nullptr;
        __try {
            value = *reinterpret_cast<void**>(
                reinterpret_cast<uintptr_t>(self) + static_cast<uintptr_t>(offset));
        } __except (1) {
            return nullptr;
        }
        return value;
    }

} // namespace IL2CPP::VRChat::Bridge
