#pragma once
#include <cstdint>
#include <string_view>

namespace Hooking {

    enum class HookType : uint8_t {
        detour = 0,
        prefix = 1,
        suffix = 2
    };

    enum class HookStatus : uint8_t {
        disabled = 0,
        enabled  = 1,
        error    = 2,
        removed  = 3,
    };

    constexpr uint32_t invalid_id = ~0u;

    struct HookId {
        uint32_t module;
        uint32_t hook;

        [[nodiscard]] constexpr bool operator==(HookId const&) const noexcept = default;
    };

    constexpr HookId invalid_hook_id{ invalid_id, invalid_id };

    struct HookInfo {
        void*        target;
        void*        detour;
        void*        trampoline;
        char const*  name;
        HookId       id;
        uint32_t     name_length;
        HookType     type;
        HookStatus   status;
        uint8_t      _pad[2] = {};
    };

    struct ModuleInfo {
        char const*  name;
        uint32_t     id;
        uint32_t     name_length;
        uint32_t     hook_count;
        uint32_t     _pad = {};
    };

    struct UiRenderContext {
        void*    swap_chain;
        void*    device;
        void*    context;
        void*    window;
        uint32_t width;
        uint32_t height;
    };

    using fn_register_module   = uint32_t(__cdecl*)(char const* name, uint32_t name_len);
    using fn_unregister_module = void(__cdecl*)(uint32_t module_id);

    using fn_create_detour = uint32_t(__cdecl*)(uint32_t module_id,
                                                char const* name, uint32_t name_len,
                                                void* target,
                                                void* detour,
                                                void** out_trampoline);

    using fn_create_prefix = uint32_t(__cdecl*)(uint32_t module_id,
                                                char const* name, uint32_t name_len,
                                                void* target,
                                                void* callback);

    using fn_create_suffix = uint32_t(__cdecl*)(uint32_t module_id,
                                                char const* name, uint32_t name_len,
                                                void* target,
                                                void* callback);

    using fn_remove_hook  = bool(__cdecl*)(uint32_t module_id, uint32_t hook_id);
    using fn_enable_hook  = bool(__cdecl*)(uint32_t module_id, uint32_t hook_id);
    using fn_disable_hook = bool(__cdecl*)(uint32_t module_id, uint32_t hook_id);

    using fn_get_hook_status  = HookStatus(__cdecl*)(uint32_t module_id, uint32_t hook_id);
    using fn_get_hook_count   = uint32_t(__cdecl*)(uint32_t module_id);
    using fn_get_hook_info    = bool(__cdecl*)(uint32_t module_id, uint32_t hook_id, HookInfo* out);
    using fn_get_module_count = uint32_t(__cdecl*)();
    using fn_get_module_info  = bool(__cdecl*)(uint32_t module_id, ModuleInfo* out);

    using fn_enable_all  = void(__cdecl*)(uint32_t module_id);
    using fn_disable_all = void(__cdecl*)(uint32_t module_id);

    using fn_ui_render_callback = void(__cdecl*)(UiRenderContext const* ctx);
    using fn_ui_resize_callback = void(__cdecl*)(uint32_t new_width, uint32_t new_height);
    using fn_ui_wndproc_callback = bool(__cdecl*)(void* hwnd, uint32_t msg, uintptr_t wparam, intptr_t lparam);

    using fn_register_ui_render    = uint32_t(__cdecl*)(uint32_t module_id, fn_ui_render_callback callback);
    using fn_unregister_ui_render  = void(__cdecl*)(uint32_t module_id, uint32_t callback_id);
    using fn_register_ui_resize    = uint32_t(__cdecl*)(uint32_t module_id, fn_ui_resize_callback callback);
    using fn_unregister_ui_resize  = void(__cdecl*)(uint32_t module_id, uint32_t callback_id);
    using fn_register_ui_wndproc   = uint32_t(__cdecl*)(uint32_t module_id, fn_ui_wndproc_callback callback);
    using fn_unregister_ui_wndproc = void(__cdecl*)(uint32_t module_id, uint32_t callback_id);
    using fn_is_ui_ready           = bool(__cdecl*)();

    struct HookVtable {
        fn_register_module    register_module;
        fn_unregister_module  unregister_module;

        fn_create_detour      create_detour;
        fn_create_prefix      create_prefix;
        fn_create_suffix      create_suffix;
        fn_remove_hook        remove_hook;
        fn_enable_hook        enable_hook;
        fn_disable_hook       disable_hook;

        fn_get_hook_status    get_hook_status;
        fn_get_hook_count     get_hook_count;
        fn_get_hook_info      get_hook_info;
        fn_get_module_count   get_module_count;
        fn_get_module_info    get_module_info;

        fn_enable_all         enable_all;
        fn_disable_all        disable_all;

        fn_register_ui_render     register_ui_render;
        fn_unregister_ui_render   unregister_ui_render;
        fn_register_ui_resize     register_ui_resize;
        fn_unregister_ui_resize   unregister_ui_resize;
        fn_register_ui_wndproc    register_ui_wndproc;
        fn_unregister_ui_wndproc  unregister_ui_wndproc;
        fn_is_ui_ready            is_ui_ready;
        uint32_t                  version;
        uint32_t                  _pad = {};
    };

    constexpr uint32_t vtable_version = 1;

} // namespace Hooking
