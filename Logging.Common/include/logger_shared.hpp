#pragma once
#include <cstdint>

namespace Logger {

    enum class Level : uint8_t {
        trace = 0,
        debug = 1,
        info = 2,
        warn = 3,
        error = 4,
        fatal = 5,
        off = 6
    };

    enum class LevelMask : uint8_t {
        none = 0,
        trace = 1 << 0,
        debug = 1 << 1,
        info = 1 << 2,
        warn = 1 << 3,
        error = 1 << 4,
        fatal = 1 << 5,
        all = 0x3F
    };

    [[nodiscard]] constexpr LevelMask operator|(LevelMask a, LevelMask b) noexcept {
        return static_cast<LevelMask>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    [[nodiscard]] constexpr LevelMask operator&(LevelMask a, LevelMask b) noexcept {
        return static_cast<LevelMask>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }

    [[nodiscard]] constexpr LevelMask operator~(LevelMask a) noexcept {
        return static_cast<LevelMask>(~static_cast<uint8_t>(a) & 0x3F);
    }

    [[nodiscard]] constexpr LevelMask to_mask(Level lvl) noexcept {
        return static_cast<LevelMask>(1 << static_cast<uint8_t>(lvl));
    }

    enum class FilterMode : uint8_t {
        disabled = 0,
        whitelist = 1,
        blacklist = 2
    };

    enum class Color : uint8_t {
        black = 0,
        dark_blue = 1,
        dark_green = 2,
        dark_cyan = 3,
        dark_red = 4,
        dark_magenta = 5,
        dark_yellow = 6,
        gray = 7,
        dark_gray = 8,
        blue = 9,
        green = 10,
        cyan = 11,
        red = 12,
        magenta = 13,
        yellow = 14,
        white = 15,
        default_color = 255
    };

    struct ModuleId {
        uint32_t module;
        uint32_t submodule;

        [[nodiscard]] constexpr bool operator==(ModuleId const&) const noexcept = default;
    };

    constexpr uint32_t invalid_id = ~0u;
    constexpr ModuleId invalid_module_id{ invalid_id, invalid_id };

    struct LogEntry {
        char const* message;
        char const* file;
        char const* function;
        ModuleId    id;
        uint32_t    message_length;
        uint32_t    line;
        Level       lvl;
        uint8_t     _pad[7] = {};
    };

    struct ModuleInfo {
        char const* name;
        uint32_t    id;
        uint32_t    name_length;
        LevelMask   mask;
        Color       fg;
        Color       bg;
        uint8_t     _pad[5] = {};
    };

    struct SubmoduleInfo {
        char const* name;
        uint32_t    module_id;
        uint32_t    submodule_id;
        uint32_t    name_length;
        LevelMask   mask;
        Color       fg;
        Color       bg;
        uint8_t     _pad = {};
    };

    using fn_log = void(__cdecl*)(LogEntry const*);
    using fn_register_module = uint32_t(__cdecl*)(char const* name, uint32_t name_len);
    using fn_register_submod = uint32_t(__cdecl*)(uint32_t module_id, char const* name, uint32_t name_len);
    using fn_unregister_mod = void(__cdecl*)(uint32_t module_id);
    using fn_unregister_sub = void(__cdecl*)(uint32_t module_id, uint32_t submodule_id);
    using fn_set_module_mask = void(__cdecl*)(uint32_t module_id, LevelMask mask);
    using fn_set_submod_mask = void(__cdecl*)(uint32_t module_id, uint32_t submodule_id, LevelMask mask);
    using fn_set_module_color = void(__cdecl*)(uint32_t module_id, Color fg, Color bg);
    using fn_set_submod_color = void(__cdecl*)(uint32_t module_id, uint32_t submodule_id, Color fg, Color bg);
    using fn_is_enabled = bool(__cdecl*)(Level lvl, ModuleId id);
    using fn_flush = void(__cdecl*)();

    struct LoggerVtable {
        fn_log                log;
        fn_register_module    register_module;
        fn_register_submod    register_submodule;
        fn_unregister_mod     unregister_module;
        fn_unregister_sub     unregister_submodule;
        fn_set_module_mask    set_module_mask;
        fn_set_submod_mask    set_submodule_mask;
        fn_set_module_color   set_module_color;
        fn_set_submod_color   set_submodule_color;
        fn_is_enabled         is_enabled;
        fn_flush              flush;
        uint32_t              version;
        uint32_t              _pad = {};
    };

    constexpr uint32_t vtable_version = 1;

} // namespace Logger
