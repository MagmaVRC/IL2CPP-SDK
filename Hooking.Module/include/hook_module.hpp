#pragma once
#include <Hooking.Common/hook_shared.hpp>
#include <string_view>
#include <string>
#include <type_traits>

namespace Hooking::Module {

    /// @internal
    extern HookVtable const* get_connection_vtable() noexcept;

    [[nodiscard]] bool Connect();
    void Disconnect();
    [[nodiscard]] bool IsConnected() noexcept;

    class HookModule {
    public:
        HookModule() = default;
        explicit HookModule(std::string_view name);
        ~HookModule();

        HookModule(HookModule const&) = delete;
        HookModule& operator=(HookModule const&) = delete;
        HookModule(HookModule&& other) noexcept;
        HookModule& operator=(HookModule&& other) noexcept;

        [[nodiscard]] bool valid() const noexcept;
        [[nodiscard]] uint32_t id() const noexcept;
        [[nodiscard]] std::string_view name() const noexcept;

        template<typename FnPtr>
        uint32_t create_detour(std::string_view hook_name,
                               FnPtr target,
                               FnPtr detour,
                               FnPtr* out_original);

        template<typename CallbackFn>
        uint32_t create_prefix(std::string_view hook_name,
                               void* target,
                               CallbackFn callback);

        template<typename CallbackFn>
        uint32_t create_suffix(std::string_view hook_name,
                               void* target,
                               CallbackFn callback);

        bool remove_hook(uint32_t hook_id);
        bool enable_hook(uint32_t hook_id);
        bool disable_hook(uint32_t hook_id);

        void enable_all();
        void disable_all();

        [[nodiscard]] HookStatus get_hook_status(uint32_t hook_id) const;
        [[nodiscard]] uint32_t   get_hook_count() const;
        [[nodiscard]] bool       get_hook_info(uint32_t hook_id, HookInfo* out) const;

        uint32_t register_render(fn_ui_render_callback callback);
        void unregister_render(uint32_t callback_id);

        uint32_t register_resize(fn_ui_resize_callback callback);
        void unregister_resize(uint32_t callback_id);

        uint32_t register_wndproc(fn_ui_wndproc_callback callback);
        void unregister_wndproc(uint32_t callback_id);

    private:
        uint32_t    m_id = invalid_id;
        std::string m_name;
    };

    [[nodiscard]] uint32_t get_module_count();
    [[nodiscard]] bool     get_module_info(uint32_t module_id, ModuleInfo* out);

    [[nodiscard]] bool is_ui_ready();

    template<typename FnPtr>
    uint32_t HookModule::create_detour(std::string_view hook_name,
                                        FnPtr target,
                                        FnPtr detour,
                                        FnPtr* out_original) {
        if (!valid() || !IsConnected()) return invalid_id;

        auto* vt = get_connection_vtable();
        if (!vt) return invalid_id;

        void* trampoline = nullptr;
        uint32_t hid = vt->create_detour(
            m_id,
            hook_name.data(), static_cast<uint32_t>(hook_name.size()),
            reinterpret_cast<void*>(target),
            reinterpret_cast<void*>(detour),
            &trampoline);

        if (hid != invalid_id && out_original)
            *out_original = reinterpret_cast<FnPtr>(trampoline);

        return hid;
    }

    template<typename CallbackFn>
    uint32_t HookModule::create_prefix(std::string_view hook_name,
                                         void* target,
                                         CallbackFn callback) {
        if (!valid() || !IsConnected()) return invalid_id;

        auto* vt = get_connection_vtable();
        if (!vt) return invalid_id;

        return vt->create_prefix(
            m_id,
            hook_name.data(), static_cast<uint32_t>(hook_name.size()),
            target,
            reinterpret_cast<void*>(callback));
    }

    template<typename CallbackFn>
    uint32_t HookModule::create_suffix(std::string_view hook_name,
                                         void* target,
                                         CallbackFn callback) {
        if (!valid() || !IsConnected()) return invalid_id;

        auto* vt = get_connection_vtable();
        if (!vt) return invalid_id;

        return vt->create_suffix(
            m_id,
            hook_name.data(), static_cast<uint32_t>(hook_name.size()),
            target,
            reinterpret_cast<void*>(callback));
    }

} // namespace Hooking::Module
