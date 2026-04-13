#include <include/hook_module.hpp>
#include <SharedMemory.Common/shared_memory.hpp>
#include <windows.h>
#include <atomic>

namespace Hooking::Module {

    namespace {

        struct ConnectionState {
            std::atomic<bool>       connected{ false };
            HookVtable const*      vtable = nullptr;
        };

        ConnectionState g_conn;

    }

    HookVtable const* get_connection_vtable() noexcept {
        return g_conn.vtable;
    }

    bool Connect() {
        if (g_conn.connected.exchange(true, std::memory_order_acq_rel))
            return true;

        g_conn.vtable = SharedMemory::Resolve<HookVtable>("Hooking.Vtable");
        if (!g_conn.vtable || g_conn.vtable->version != vtable_version) {
            g_conn.vtable = nullptr;
            g_conn.connected.store(false, std::memory_order_release);
            return false;
        }

        return true;
    }

    void Disconnect() {
        if (!g_conn.connected.exchange(false, std::memory_order_acq_rel))
            return;
        g_conn.vtable = nullptr;
    }

    bool IsConnected() noexcept {
        return g_conn.connected.load(std::memory_order_acquire) && g_conn.vtable;
    }

    HookModule::HookModule(std::string_view name)
        : m_name(name) {
        if (!IsConnected() && !Connect()) return;

        m_id = g_conn.vtable->register_module(
            m_name.data(), static_cast<uint32_t>(m_name.size()));
    }

    HookModule::~HookModule() {
        if (m_id != invalid_id && IsConnected()) {
            g_conn.vtable->unregister_module(m_id);
        }
    }

    HookModule::HookModule(HookModule&& other) noexcept
        : m_id(std::exchange(other.m_id, invalid_id))
        , m_name(std::move(other.m_name)) {
    }

    HookModule& HookModule::operator=(HookModule&& other) noexcept {
        if (this != &other) {
            if (m_id != invalid_id && IsConnected()) {
                g_conn.vtable->unregister_module(m_id);
            }
            m_id   = std::exchange(other.m_id, invalid_id);
            m_name = std::move(other.m_name);
        }
        return *this;
    }

    bool HookModule::valid() const noexcept {
        return m_id != invalid_id && IsConnected();
    }

    uint32_t HookModule::id() const noexcept {
        return m_id;
    }

    std::string_view HookModule::name() const noexcept {
        return m_name;
    }

    bool HookModule::remove_hook(uint32_t hook_id) {
        if (!valid()) return false;
        return g_conn.vtable->remove_hook(m_id, hook_id);
    }

    bool HookModule::enable_hook(uint32_t hook_id) {
        if (!valid()) return false;
        return g_conn.vtable->enable_hook(m_id, hook_id);
    }

    bool HookModule::disable_hook(uint32_t hook_id) {
        if (!valid()) return false;
        return g_conn.vtable->disable_hook(m_id, hook_id);
    }

    void HookModule::enable_all() {
        if (!valid()) return;
        g_conn.vtable->enable_all(m_id);
    }

    void HookModule::disable_all() {
        if (!valid()) return;
        g_conn.vtable->disable_all(m_id);
    }

    HookStatus HookModule::get_hook_status(uint32_t hook_id) const {
        if (!valid()) return HookStatus::error;
        return g_conn.vtable->get_hook_status(m_id, hook_id);
    }

    uint32_t HookModule::get_hook_count() const {
        if (!valid()) return 0;
        return g_conn.vtable->get_hook_count(m_id);
    }

    bool HookModule::get_hook_info(uint32_t hook_id, HookInfo* out) const {
        if (!valid() || !out) return false;
        return g_conn.vtable->get_hook_info(m_id, hook_id, out);
    }

    uint32_t HookModule::register_render(fn_ui_render_callback callback) {
        if (!valid()) return invalid_id;
        return g_conn.vtable->register_ui_render(m_id, callback);
    }

    void HookModule::unregister_render(uint32_t callback_id) {
        if (!valid()) return;
        g_conn.vtable->unregister_ui_render(m_id, callback_id);
    }

    uint32_t HookModule::register_resize(fn_ui_resize_callback callback) {
        if (!valid()) return invalid_id;
        return g_conn.vtable->register_ui_resize(m_id, callback);
    }

    void HookModule::unregister_resize(uint32_t callback_id) {
        if (!valid()) return;
        g_conn.vtable->unregister_ui_resize(m_id, callback_id);
    }

    uint32_t HookModule::register_wndproc(fn_ui_wndproc_callback callback) {
        if (!valid()) return invalid_id;
        return g_conn.vtable->register_ui_wndproc(m_id, callback);
    }

    void HookModule::unregister_wndproc(uint32_t callback_id) {
        if (!valid()) return;
        g_conn.vtable->unregister_ui_wndproc(m_id, callback_id);
    }

    uint32_t get_module_count() {
        if (!IsConnected()) return 0;
        return g_conn.vtable->get_module_count();
    }

    bool get_module_info(uint32_t module_id, ModuleInfo* out) {
        if (!IsConnected() || !out) return false;
        return g_conn.vtable->get_module_info(module_id, out);
    }

    bool is_ui_ready() {
        if (!IsConnected()) return false;
        return g_conn.vtable->is_ui_ready();
    }

} // namespace Hooking::Module
