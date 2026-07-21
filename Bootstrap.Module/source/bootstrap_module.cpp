#include <bootstrap_module.hpp>
#include <bootstrap_internal.hpp>
#include <IL2CPP.Module/include/il2cpp_module.hpp>
#include <SharedMemory.Common/shared_memory.hpp>
#include <Windows.h>
#include <algorithm>
#include <atomic>
#include <charconv>
#include <unordered_map>
#include <cwchar>
#include <sstream>
#include <string>

namespace Bootstrap::Module {

    namespace {

        struct ConnectionState {
            std::atomic<bool> connected{ false };
            Bootstrap::BootstrapVtable const* vtable = nullptr;
        };

        ConnectionState g_conn;

    }

    Bootstrap::BootstrapVtable const* get_vtable() noexcept {
        return g_conn.vtable;
    }

    bool Connect() {
        if (g_conn.connected.exchange(true, std::memory_order_acq_rel))
            return true;

        g_conn.vtable = SharedMemory::Resolve<Bootstrap::BootstrapVtable>("Bootstrap.Vtable");
        if (!g_conn.vtable || g_conn.vtable->version != Bootstrap::vtable_version) {
            g_conn.vtable = nullptr;
            g_conn.connected.store(false, std::memory_order_release);
            return false;
        }

        (void)IL2CPP::Module::Connect();
        return true;
    }

    bool Disconnect() {
        if (!g_conn.connected.exchange(false, std::memory_order_acq_rel))
            return true;

        if (!IL2CPP::Module::Disconnect()) {
            g_conn.connected.store(true, std::memory_order_release);
            return false;
        }
        g_conn.vtable = nullptr;
        return true;
    }

    bool is_connected() noexcept {
        return g_conn.connected.load(std::memory_order_acquire) && g_conn.vtable;
    }

    BootstrapModule::BootstrapModule(std::string_view name) : m_name(name) {
        if (!is_connected() && !Connect()) return;

        m_id = g_conn.vtable->register_module(
            m_name.data(), static_cast<uint32_t>(m_name.size()), Bootstrap::sdk_version);
    }

    BootstrapModule::~BootstrapModule() {
        if (m_id != Bootstrap::invalid_id && is_connected()) {
            g_conn.vtable->unregister_module(m_id);
        }
    }

    BootstrapModule::BootstrapModule(BootstrapModule&& other) noexcept
        : m_id(std::exchange(other.m_id, Bootstrap::invalid_id))
        , m_name(std::move(other.m_name)) {
    }

    BootstrapModule& BootstrapModule::operator=(BootstrapModule&& other) noexcept {
        if (this != &other) {
            if (m_id != Bootstrap::invalid_id && is_connected()) {
                g_conn.vtable->unregister_module(m_id);
            }
            m_id = std::exchange(other.m_id, Bootstrap::invalid_id);
            m_name = std::move(other.m_name);
        }
        return *this;
    }

    bool BootstrapModule::valid() const noexcept {
        return m_id != Bootstrap::invalid_id && is_connected();
    }

    uint32_t BootstrapModule::id() const noexcept {
        return m_id;
    }

    std::string_view BootstrapModule::name() const noexcept {
        return m_name;
    }

    UnityConsole& UnityConsole::Get() {
        static UnityConsole instance;
        return instance;
    }

    bool UnityConsole::valid() const noexcept {
        return is_connected();
    }

    void UnityConsole::set_enable_colors(bool enable) {
        if (!valid()) return;
        Bootstrap::UnityConsoleOptions opts{};
        g_conn.vtable->get_console_options(&opts);
        opts.enable_colors = enable;
        g_conn.vtable->set_console_options(&opts);
    }

    void UnityConsole::set_file_logging(bool enable, std::wstring_view path) {
        if (!valid()) return;
        Bootstrap::UnityConsoleOptions opts{};
        g_conn.vtable->get_console_options(&opts);
        opts.enable_file_logging = enable;
        if (!path.empty()) {
            size_t copyLen = (std::min)(path.size(), static_cast<size_t>(259));
            wcsncpy_s(opts.log_file_path, 260, path.data(), copyLen);
        }
        g_conn.vtable->set_console_options(&opts);
    }

    void UnityConsole::set_enable_trace(bool enable) {
        if (!valid()) return;
        Bootstrap::UnityConsoleOptions opts{};
        g_conn.vtable->get_console_options(&opts);
        opts.enable_trace = enable;
        g_conn.vtable->set_console_options(&opts);
    }

    void UnityConsole::set_enable_info_trace(bool enable) {
        if (!valid()) return;
        Bootstrap::UnityConsoleOptions opts{};
        g_conn.vtable->get_console_options(&opts);
        opts.enable_info_trace = enable;
        g_conn.vtable->set_console_options(&opts);
    }

    void UnityConsole::set_deobfuscate_names(bool enable) {
        if (!valid()) return;
        Bootstrap::UnityConsoleOptions opts{};
        g_conn.vtable->get_console_options(&opts);
        opts.deobfuscate_names = enable;
        g_conn.vtable->set_console_options(&opts);
    }

    void UnityConsole::get_options(Bootstrap::UnityConsoleOptions* out) {
        if (!valid() || !out) return;
        g_conn.vtable->get_console_options(out);
    }

    void UnityConsole::set_options(Bootstrap::UnityConsoleOptions const* opts) {
        if (!valid() || !opts) return;
        g_conn.vtable->set_console_options(opts);
    }

    uint32_t UnityConsole::register_callback(uint32_t module_id, Bootstrap::fn_log_callback callback) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return g_conn.vtable->register_log_callback(module_id, callback);
    }

    void UnityConsole::unregister_callback(uint32_t module_id, uint32_t callback_id) {
        if (!valid()) return;
        g_conn.vtable->unregister_log_callback(module_id, callback_id);
    }

    UnityExplorer& UnityExplorer::Get() {
        static UnityExplorer instance;
        return instance;
    }

    bool UnityExplorer::valid() const noexcept {
        return is_connected();
    }

    void UnityExplorer::show_game_object(void* gameObject) {
        if (!valid() || !gameObject) return;
        g_conn.vtable->show_gameobject_in_explorer(gameObject);
    }

    void UnityExplorer::show_component(void* component) {
        if (!valid() || !component) return;
        g_conn.vtable->show_component_in_explorer(component);
    }

    uint32_t UnityExplorer::register_on_navigate(uint32_t module_id, Bootstrap::fn_explorer_navigate_callback callback) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return g_conn.vtable->register_explorer_callback(module_id, callback);
    }

    void UnityExplorer::unregister_on_navigate(uint32_t module_id, uint32_t callback_id) {
        if (!valid()) return;
        g_conn.vtable->unregister_explorer_callback(module_id, callback_id);
    }

    // Rendering

    Rendering& Rendering::Get() {
        static Rendering instance;
        return instance;
    }

    bool Rendering::valid() const noexcept {
        return is_connected();
    }

    uint32_t Rendering::register_callback(uint32_t module_id, Bootstrap::fn_render_callback callback) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return g_conn.vtable->register_render_callback(module_id, callback);
    }

    void Rendering::unregister_callback(uint32_t module_id, uint32_t callback_id) {
        if (!valid()) return;
        g_conn.vtable->unregister_render_callback(module_id, callback_id);
    }

    void* Rendering::get_imgui_context() {
        if (!valid()) return nullptr;
        return g_conn.vtable->get_imgui_context();
    }

    // ModuleConfig

    ModuleConfig::ModuleConfig(uint32_t module_id) : m_module_id(module_id) {}

    void ModuleConfig::set_int(std::string_view key, int32_t value) {
        if (!is_connected()) return;
        g_conn.vtable->config_set_int(m_module_id, key.data(), static_cast<uint32_t>(key.size()), value);
    }

    int32_t ModuleConfig::get_int(std::string_view key, int32_t default_val) {
        if (!is_connected()) return default_val;
        return g_conn.vtable->config_get_int(m_module_id, key.data(), static_cast<uint32_t>(key.size()), default_val);
    }

    void ModuleConfig::set_float(std::string_view key, float value) {
        if (!is_connected()) return;
        g_conn.vtable->config_set_float(m_module_id, key.data(), static_cast<uint32_t>(key.size()), value);
    }

    float ModuleConfig::get_float(std::string_view key, float default_val) {
        if (!is_connected()) return default_val;
        return g_conn.vtable->config_get_float(m_module_id, key.data(), static_cast<uint32_t>(key.size()), default_val);
    }

    void ModuleConfig::set_string(std::string_view key, std::string_view value) {
        if (!is_connected()) return;
        g_conn.vtable->config_set_string(m_module_id, key.data(), static_cast<uint32_t>(key.size()),
            value.data(), static_cast<uint32_t>(value.size()));
    }

    std::string ModuleConfig::get_string(std::string_view key, std::string_view default_val) {
        if (!is_connected()) return std::string(default_val);
        char buf[4096];
        uint32_t len = g_conn.vtable->config_get_string(m_module_id, key.data(),
            static_cast<uint32_t>(key.size()), buf, sizeof(buf));
        if (len == 0) return std::string(default_val);
        return std::string(buf, len);
    }

    void ModuleConfig::set_bool(std::string_view key, bool value) {
        if (!is_connected()) return;
        g_conn.vtable->config_set_bool(m_module_id, key.data(), static_cast<uint32_t>(key.size()), value);
    }

    bool ModuleConfig::get_bool(std::string_view key, bool default_val) {
        if (!is_connected()) return default_val;
        return g_conn.vtable->config_get_bool(m_module_id, key.data(), static_cast<uint32_t>(key.size()), default_val);
    }

    void ModuleConfig::save() {
        if (!is_connected()) return;
        g_conn.vtable->config_save(m_module_id);
    }

    bool ModuleConfig::has_key(std::string_view key) {
        if (!is_connected()) return false;
        return g_conn.vtable->config_has_key(m_module_id, key.data(), static_cast<uint32_t>(key.size()));
    }

    void ModuleConfig::remove_key(std::string_view key) {
        if (!is_connected()) return;
        g_conn.vtable->config_remove_key(m_module_id, key.data(), static_cast<uint32_t>(key.size()));
    }

    // ModuleConfig (extended)

    void ModuleConfig::set_vec2(std::string_view key, float x, float y) {
        if (!is_connected()) return;
        g_conn.vtable->config_set_vec2(m_module_id, key.data(), static_cast<uint32_t>(key.size()), x, y);
    }

    std::pair<float, float> ModuleConfig::get_vec2(std::string_view key, float def_x, float def_y) {
        if (!is_connected()) return { def_x, def_y };
        float x, y;
        g_conn.vtable->config_get_vec2(m_module_id, key.data(), static_cast<uint32_t>(key.size()),
            &x, &y, def_x, def_y);
        return { x, y };
    }

    void ModuleConfig::set_vec3(std::string_view key, float x, float y, float z) {
        if (!is_connected()) return;
        g_conn.vtable->config_set_vec3(m_module_id, key.data(), static_cast<uint32_t>(key.size()), x, y, z);
    }

    std::tuple<float, float, float> ModuleConfig::get_vec3(std::string_view key, float def_x, float def_y, float def_z) {
        if (!is_connected()) return { def_x, def_y, def_z };
        float x, y, z;
        g_conn.vtable->config_get_vec3(m_module_id, key.data(), static_cast<uint32_t>(key.size()),
            &x, &y, &z, def_x, def_y, def_z);
        return { x, y, z };
    }

    void ModuleConfig::set_vec4(std::string_view key, float x, float y, float z, float w) {
        if (!is_connected()) return;
        g_conn.vtable->config_set_vec4(m_module_id, key.data(), static_cast<uint32_t>(key.size()), x, y, z, w);
    }

    std::tuple<float, float, float, float> ModuleConfig::get_vec4(std::string_view key,
        float def_x, float def_y,
        float def_z, float def_w) {
        if (!is_connected()) return { def_x, def_y, def_z, def_w };
        float x, y, z, w;
        g_conn.vtable->config_get_vec4(m_module_id, key.data(), static_cast<uint32_t>(key.size()),
            &x, &y, &z, &w, def_x, def_y, def_z, def_w);
        return { x, y, z, w };
    }

    void ModuleConfig::set_color(std::string_view key, float r, float g, float b, float a) {
        if (!is_connected()) return;
        g_conn.vtable->config_set_color(m_module_id, key.data(), static_cast<uint32_t>(key.size()), r, g, b, a);
    }

    std::tuple<float, float, float, float> ModuleConfig::get_color(std::string_view key,
        float def_r, float def_g,
        float def_b, float def_a) {
        if (!is_connected()) return { def_r, def_g, def_b, def_a };
        float r, g, b, a;
        g_conn.vtable->config_get_color(m_module_id, key.data(), static_cast<uint32_t>(key.size()),
            &r, &g, &b, &a, def_r, def_g, def_b, def_a);
        return { r, g, b, a };
    }

    void ModuleConfig::set_json(std::string_view key, std::string_view json) {
        if (!is_connected()) return;
        g_conn.vtable->config_set_json(m_module_id, key.data(), static_cast<uint32_t>(key.size()),
            json.data(), static_cast<uint32_t>(json.size()));
    }

    std::string ModuleConfig::get_json(std::string_view key) {
        if (!is_connected()) return {};
        char buf[4096];
        uint32_t len = g_conn.vtable->config_get_json(m_module_id, key.data(),
            static_cast<uint32_t>(key.size()), buf, sizeof(buf));
        if (len == 0) return {};
        return std::string(buf, len);
    }

    std::vector<std::string> ModuleConfig::get_keys() {
        if (!is_connected()) return {};
        char buf[8192];
        uint32_t len = g_conn.vtable->config_get_keys(m_module_id, buf, sizeof(buf));
        if (len == 0) return {};

        std::vector<std::string> keys;
        uint32_t start = 0;
        for (uint32_t i = 0; i < len; ++i) {
            if (buf[i] == '\0') {
                if (i > start) keys.emplace_back(buf + start, i - start);
                start = i + 1;
            }
        }
        if (start < len) keys.emplace_back(buf + start, len - start);
        return keys;
    }

    void ModuleConfig::clear() {
        if (!is_connected()) return;
        g_conn.vtable->config_clear(m_module_id);
    }

    // ModuleConfig (native type convenience)

    namespace {

        // Minimal JSON helpers for array serialization — no external dependency required.

        std::string escape_json_string(std::string_view s) {
            std::string out;
            out.reserve(s.size() + 2);
            out += '"';
            for (char c : s) {
                switch (c) {
                case '"':  out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\n': out += "\\n";  break;
                case '\r': out += "\\r";  break;
                case '\t': out += "\\t";  break;
                default:   out += c;      break;
                }
            }
            out += '"';
            return out;
        }

        std::string unescape_json_string(std::string_view s) {
            if (s.size() < 2 || s.front() != '"' || s.back() != '"') return std::string(s);
            s.remove_prefix(1);
            s.remove_suffix(1);
            std::string out;
            out.reserve(s.size());
            for (size_t i = 0; i < s.size(); ++i) {
                if (s[i] == '\\' && i + 1 < s.size()) {
                    switch (s[++i]) {
                    case '"':  out += '"';  break;
                    case '\\': out += '\\'; break;
                    case 'n':  out += '\n'; break;
                    case 'r':  out += '\r'; break;
                    case 't':  out += '\t'; break;
                    default:   out += '\\'; out += s[i]; break;
                    }
                }
                else {
                    out += s[i];
                }
            }
            return out;
        }

        // Split a JSON array body (content between [ and ]) into elements,
        // respecting quoted strings. Returns empty on malformed input.
        std::vector<std::string_view> split_json_array(std::string_view json) {
            std::vector<std::string_view> elems;
            // skip whitespace and find '['
            size_t pos = json.find('[');
            if (pos == std::string_view::npos) return elems;
            ++pos;

            while (pos < json.size()) {
                // skip whitespace
                while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' ||
                    json[pos] == '\n' || json[pos] == '\r')) ++pos;
                if (pos >= json.size() || json[pos] == ']') break;

                size_t start = pos;
                if (json[pos] == '"') {
                    // quoted string — find matching unescaped close quote
                    ++pos;
                    while (pos < json.size()) {
                        if (json[pos] == '\\') { pos += 2; continue; }
                        if (json[pos] == '"') { ++pos; break; }
                        ++pos;
                    }
                }
                else {
                    // number, bool, null — read until comma or ]
                    while (pos < json.size() && json[pos] != ',' && json[pos] != ']') ++pos;
                }
                // trim trailing whitespace from element
                size_t end = pos;
                while (end > start && (json[end - 1] == ' ' || json[end - 1] == '\t' ||
                    json[end - 1] == '\n' || json[end - 1] == '\r')) --end;
                if (end > start) elems.push_back(json.substr(start, end - start));

                // skip whitespace then comma
                while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' ||
                    json[pos] == '\n' || json[pos] == '\r')) ++pos;
                if (pos < json.size() && json[pos] == ',') ++pos;
            }
            return elems;
        }

    } // anon namespace

    void ModuleConfig::set_double(std::string_view key, double value) {
        char buf[64];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), value);
        if (ec == std::errc()) set_string(key, std::string_view(buf, ptr - buf));
    }

    double ModuleConfig::get_double(std::string_view key, double default_val) {
        auto s = get_string(key);
        if (s.empty()) return default_val;
        double val;
        auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), val);
        return ec == std::errc() ? val : default_val;
    }

    void ModuleConfig::set_int64(std::string_view key, int64_t value) {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), value);
        if (ec == std::errc()) set_string(key, std::string_view(buf, ptr - buf));
    }

    int64_t ModuleConfig::get_int64(std::string_view key, int64_t default_val) {
        auto s = get_string(key);
        if (s.empty()) return default_val;
        int64_t val;
        auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), val);
        return ec == std::errc() ? val : default_val;
    }

    void ModuleConfig::set_uint32(std::string_view key, uint32_t value) {
        set_int(key, static_cast<int32_t>(value));
    }

    uint32_t ModuleConfig::get_uint32(std::string_view key, uint32_t default_val) {
        return static_cast<uint32_t>(get_int(key, static_cast<int32_t>(default_val)));
    }

    void ModuleConfig::set_int_array(std::string_view key, std::vector<int32_t> const& values) {
        std::ostringstream os;
        os << '[';
        for (size_t i = 0; i < values.size(); ++i) {
            if (i) os << ',';
            os << values[i];
        }
        os << ']';
        set_json(key, os.str());
    }

    std::vector<int32_t> ModuleConfig::get_int_array(std::string_view key) {
        auto json = get_json(key);
        if (json.empty()) return {};
        auto elems = split_json_array(json);
        std::vector<int32_t> result;
        result.reserve(elems.size());
        for (auto e : elems) {
            int32_t val = 0;
            std::from_chars(e.data(), e.data() + e.size(), val);
            result.push_back(val);
        }
        return result;
    }

    void ModuleConfig::set_float_array(std::string_view key, std::vector<float> const& values) {
        std::ostringstream os;
        os << '[';
        for (size_t i = 0; i < values.size(); ++i) {
            if (i) os << ',';
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), values[i]);
            os.write(buf, ptr - buf);
        }
        os << ']';
        set_json(key, os.str());
    }

    std::vector<float> ModuleConfig::get_float_array(std::string_view key) {
        auto json = get_json(key);
        if (json.empty()) return {};
        auto elems = split_json_array(json);
        std::vector<float> result;
        result.reserve(elems.size());
        for (auto e : elems) {
            float val = 0.f;
            std::from_chars(e.data(), e.data() + e.size(), val);
            result.push_back(val);
        }
        return result;
    }

    void ModuleConfig::set_string_array(std::string_view key, std::vector<std::string> const& values) {
        std::string json = "[";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i) json += ',';
            json += escape_json_string(values[i]);
        }
        json += ']';
        set_json(key, json);
    }

    std::vector<std::string> ModuleConfig::get_string_array(std::string_view key) {
        auto json = get_json(key);
        if (json.empty()) return {};
        auto elems = split_json_array(json);
        std::vector<std::string> result;
        result.reserve(elems.size());
        for (auto e : elems) result.push_back(unescape_json_string(e));
        return result;
    }

    void ModuleConfig::set_bool_array(std::string_view key, std::vector<bool> const& values) {
        std::string json = "[";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i) json += ',';
            json += values[i] ? "true" : "false";
        }
        json += ']';
        set_json(key, json);
    }

    std::vector<bool> ModuleConfig::get_bool_array(std::string_view key) {
        auto json = get_json(key);
        if (json.empty()) return {};
        auto elems = split_json_array(json);
        std::vector<bool> result;
        result.reserve(elems.size());
        for (auto e : elems) result.push_back(e == "true");
        return result;
    }

    void ModuleConfig::set_double_array(std::string_view key, std::vector<double> const& values) {
        std::ostringstream os;
        os << '[';
        for (size_t i = 0; i < values.size(); ++i) {
            if (i) os << ',';
            char buf[64];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), values[i]);
            os.write(buf, ptr - buf);
        }
        os << ']';
        set_json(key, os.str());
    }

    std::vector<double> ModuleConfig::get_double_array(std::string_view key) {
        auto json = get_json(key);
        if (json.empty()) return {};
        auto elems = split_json_array(json);
        std::vector<double> result;
        result.reserve(elems.size());
        for (auto e : elems) {
            double val = 0.0;
            std::from_chars(e.data(), e.data() + e.size(), val);
            result.push_back(val);
        }
        return result;
    }

    void ModuleConfig::set_string_map(std::string_view key,
        std::vector<std::pair<std::string, std::string>> const& entries) {
        std::string json = "{";
        for (size_t i = 0; i < entries.size(); ++i) {
            if (i) json += ',';
            json += escape_json_string(entries[i].first);
            json += ':';
            json += escape_json_string(entries[i].second);
        }
        json += '}';
        set_json(key, json);
    }

    std::vector<std::pair<std::string, std::string>> ModuleConfig::get_string_map(std::string_view key) {
        auto json = get_json(key);
        if (json.empty()) return {};

        std::vector<std::pair<std::string, std::string>> result;
        // Minimal JSON object parser: {"key":"val","key2":"val2"}
        size_t pos = json.find('{');
        if (pos == std::string::npos) return result;
        ++pos;

        while (pos < json.size()) {
            // skip whitespace
            while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' ||
                json[pos] == '\r' || json[pos] == '\t')) ++pos;
            if (pos >= json.size() || json[pos] == '}') break;

            // read key (must be quoted string)
            if (json[pos] != '"') break;
            size_t key_start = pos;
            ++pos;
            while (pos < json.size()) {
                if (json[pos] == '\\') { pos += 2; continue; }
                if (json[pos] == '"') { ++pos; break; }
                ++pos;
            }
            auto k = unescape_json_string(std::string_view(json.data() + key_start, pos - key_start));

            // skip whitespace + colon
            while (pos < json.size() && json[pos] != ':') ++pos;
            if (pos < json.size()) ++pos;
            while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' ||
                json[pos] == '\r' || json[pos] == '\t')) ++pos;

            // read value (must be quoted string)
            if (pos >= json.size() || json[pos] != '"') break;
            size_t val_start = pos;
            ++pos;
            while (pos < json.size()) {
                if (json[pos] == '\\') { pos += 2; continue; }
                if (json[pos] == '"') { ++pos; break; }
                ++pos;
            }
            auto v = unescape_json_string(std::string_view(json.data() + val_start, pos - val_start));

            result.emplace_back(std::move(k), std::move(v));

            // skip whitespace + comma
            while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' ||
                json[pos] == '\r' || json[pos] == '\t')) ++pos;
            if (pos < json.size() && json[pos] == ',') ++pos;
        }
        return result;
    }

    // MessageBus

    MessageBus& MessageBus::Get() {
        static MessageBus instance;
        return instance;
    }

    uint32_t MessageBus::subscribe(uint32_t module_id, std::string_view topic, Bootstrap::fn_message_callback cb) {
        if (!is_connected() || !cb) return Bootstrap::invalid_id;
        return g_conn.vtable->subscribe_message(module_id, topic.data(), static_cast<uint32_t>(topic.size()), cb);
    }

    void MessageBus::unsubscribe(uint32_t module_id, uint32_t subscription_id) {
        if (!is_connected()) return;
        g_conn.vtable->unsubscribe_message(module_id, subscription_id);
    }

    void MessageBus::publish(uint32_t module_id, std::string_view topic, const void* data, uint32_t data_len) {
        if (!is_connected()) return;
        g_conn.vtable->publish_message(module_id, topic.data(), static_cast<uint32_t>(topic.size()),
            static_cast<uint8_t const*>(data), data_len);
    }

    void MessageBus::publish(uint32_t module_id, std::string_view topic, std::string_view message) {
        publish(module_id, topic, message.data(), static_cast<uint32_t>(message.size()));
    }

    // QuickMenu

    QuickMenu& QuickMenu::Get() {
        static QuickMenu instance;
        return instance;
    }

    bool QuickMenu::valid() const noexcept {
        return is_connected();
    }

    bool QuickMenu::is_ready() {
        if (!valid()) return false;
        return g_conn.vtable->qm_is_ready();
    }

    uint32_t QuickMenu::create_page(uint32_t module_id, std::string_view name, std::string_view tooltip) {
        if (!valid()) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_create_page(module_id,
            name.data(), static_cast<uint32_t>(name.size()),
            tooltip.data(), static_cast<uint32_t>(tooltip.size()));
    }

    uint32_t QuickMenu::create_sub_page(uint32_t module_id, uint32_t parent_page_id, std::string_view name) {
        if (!valid()) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_create_sub_page(module_id, parent_page_id,
            name.data(), static_cast<uint32_t>(name.size()));
    }

    void QuickMenu::remove_page(uint32_t module_id, uint32_t page_id) {
        if (!valid()) return;
        g_conn.vtable->qm_remove_page(module_id, page_id);
    }

    uint32_t QuickMenu::add_button(uint32_t module_id, uint32_t page_id, std::string_view text,
        Bootstrap::fn_menu_button_callback callback) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_add_button(module_id, page_id,
            text.data(), static_cast<uint32_t>(text.size()), callback);
    }

    void QuickMenu::remove_button(uint32_t module_id, uint32_t button_id) {
        if (!valid()) return;
        g_conn.vtable->qm_remove_button(module_id, button_id);
    }

    void QuickMenu::set_button_text(uint32_t module_id, uint32_t button_id, std::string_view text) {
        if (!valid()) return;
        g_conn.vtable->qm_set_button_text(module_id, button_id,
            text.data(), static_cast<uint32_t>(text.size()));
    }

    uint32_t QuickMenu::add_toggle(uint32_t module_id, uint32_t page_id, std::string_view text,
        bool default_state, Bootstrap::fn_menu_toggle_callback callback,
        std::string_view config_key) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_add_toggle(module_id, page_id,
            text.data(), static_cast<uint32_t>(text.size()),
            default_state, callback,
            config_key.data(), static_cast<uint32_t>(config_key.size()));
    }

    void QuickMenu::set_toggle_state(uint32_t module_id, uint32_t button_id, bool state) {
        if (!valid()) return;
        g_conn.vtable->qm_set_toggle_state(module_id, button_id, state);
    }

    bool QuickMenu::get_toggle_state(uint32_t module_id, uint32_t button_id) {
        if (!valid()) return false;
        return g_conn.vtable->qm_get_toggle_state(module_id, button_id);
    }

    void QuickMenu::set_button_icon(uint32_t module_id, uint32_t button_id, void* sprite_ptr) {
        if (!valid()) return;
        g_conn.vtable->qm_set_button_icon_ptr(module_id, button_id, sprite_ptr);
    }

    void QuickMenu::set_button_icon(uint32_t module_id, uint32_t button_id, int32_t sprite_id) {
        if (!valid()) return;
        g_conn.vtable->qm_set_button_icon_vrc(module_id, button_id, sprite_id);
    }

    void QuickMenu::set_button_color(uint32_t module_id, uint32_t button_id, float r, float g, float b, float a) {
        if (!valid()) return;
        g_conn.vtable->qm_set_button_color(module_id, button_id, r, g, b, a);
    }

    void QuickMenu::set_page_title(uint32_t module_id, uint32_t page_id, std::string_view title) {
        if (!valid()) return;
        g_conn.vtable->qm_set_page_title(module_id, page_id,
            title.data(), static_cast<uint32_t>(title.size()));
    }

    void QuickMenu::set_page_icon(uint32_t module_id, uint32_t page_id, void* sprite_ptr) {
        if (!valid()) return;
        g_conn.vtable->qm_set_page_icon_ptr(module_id, page_id, sprite_ptr);
    }

    void QuickMenu::set_page_icon(uint32_t module_id, uint32_t page_id, int32_t sprite_id) {
        if (!valid()) return;
        g_conn.vtable->qm_set_page_icon_vrc(module_id, page_id, sprite_id);
    }

    void QuickMenu::set_page_badge(uint32_t module_id, uint32_t page_id, bool visible,
        std::string_view text, float r, float g, float b, float a) {
        if (!valid()) return;
        float rgba[4] = { r, g, b, a };
        g_conn.vtable->qm_set_page_badge(module_id, page_id, visible,
            text.data(), static_cast<uint32_t>(text.size()), rgba);
    }

    void QuickMenu::set_page_badge(uint32_t module_id, uint32_t page_id, bool visible,
        std::string_view text, std::nullptr_t) {
        if (!valid()) return;
        g_conn.vtable->qm_set_page_badge(module_id, page_id, visible,
            text.data(), static_cast<uint32_t>(text.size()), nullptr);
    }

    // QuickMenu (state and navigation)

    void QuickMenu::set_button_enabled(uint32_t module_id, uint32_t button_id, bool enabled) {
        if (!valid()) return;
        g_conn.vtable->qm_set_button_enabled(module_id, button_id, enabled);
    }

    void QuickMenu::set_button_visible(uint32_t module_id, uint32_t button_id, bool visible) {
        if (!valid()) return;
        g_conn.vtable->qm_set_button_visible(module_id, button_id, visible);
    }

    uint32_t QuickMenu::get_subpage_nav_button(uint32_t module_id, uint32_t sub_page_id) {
        if (!valid()) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_get_subpage_nav_button(module_id, sub_page_id);
    }

    void QuickMenu::set_subpage_nav_text(uint32_t module_id, uint32_t sub_page_id, std::string_view text) {
        if (!valid()) return;
        g_conn.vtable->qm_set_subpage_nav_text(module_id, sub_page_id,
            text.data(), static_cast<uint32_t>(text.size()));
    }

    void QuickMenu::set_subpage_nav_icon(uint32_t module_id, uint32_t sub_page_id, int32_t sprite_id) {
        if (!valid()) return;
        g_conn.vtable->qm_set_subpage_nav_icon_vrc(module_id, sub_page_id, sprite_id);
    }

    void QuickMenu::set_image_sprite(void* image_component, int32_t sprite_id) {
        if (!valid() || !image_component) return;
        g_conn.vtable->qm_set_image_sprite(image_component, sprite_id);
    }

    // QuickMenu (foldouts, settings, sliders)

    uint32_t QuickMenu::add_foldout(uint32_t module_id, uint32_t page_id, std::string_view title,
        bool default_expanded, bool show_background, bool auto_separators) {
        if (!valid()) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_add_foldout(module_id, page_id,
            title.data(), static_cast<uint32_t>(title.size()), default_expanded, show_background, auto_separators);
    }

    void QuickMenu::set_foldout_expanded(uint32_t module_id, uint32_t foldout_id, bool expanded) {
        if (!valid()) return;
        g_conn.vtable->qm_set_foldout_expanded(module_id, foldout_id, expanded);
    }

    bool QuickMenu::get_foldout_expanded(uint32_t module_id, uint32_t foldout_id) {
        if (!valid()) return false;
        return g_conn.vtable->qm_get_foldout_expanded(module_id, foldout_id);
    }

    uint32_t QuickMenu::add_settings_toggle(uint32_t module_id, uint32_t foldout_id, std::string_view text,
        bool default_state, fn_menu_toggle_callback callback,
        std::string_view config_key, bool show_sub_element_indicator) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_add_settings_toggle(module_id, foldout_id,
            text.data(), static_cast<uint32_t>(text.size()),
            default_state, callback,
            config_key.data(), static_cast<uint32_t>(config_key.size()),
            show_sub_element_indicator);
    }

    uint32_t QuickMenu::add_enum_selector(uint32_t module_id, uint32_t foldout_id, std::string_view label,
        const char* const* options, uint32_t option_count,
        int32_t default_index, fn_menu_enum_callback callback,
        std::string_view config_key, bool show_sub_element_indicator) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_add_enum_selector(module_id, foldout_id,
            label.data(), static_cast<uint32_t>(label.size()),
            options, option_count, default_index, callback,
            config_key.data(), static_cast<uint32_t>(config_key.size()),
            show_sub_element_indicator);
    }

    void QuickMenu::set_enum_index(uint32_t module_id, uint32_t selector_id, int32_t index) {
        if (!valid()) return;
        g_conn.vtable->qm_set_enum_index(module_id, selector_id, index);
    }

    int32_t QuickMenu::get_enum_index(uint32_t module_id, uint32_t selector_id) {
        if (!valid()) return 0;
        return g_conn.vtable->qm_get_enum_index(module_id, selector_id);
    }

    uint32_t QuickMenu::add_slider(uint32_t module_id, uint32_t foldout_id, std::string_view label,
        float min_val, float max_val, float default_val,
        fn_menu_slider_callback callback,
        std::string_view config_key, std::string_view format_str,
        bool show_sub_element_indicator, float power) {
        if (!valid() || !callback) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_add_slider(module_id, foldout_id,
            label.data(), static_cast<uint32_t>(label.size()),
            min_val, max_val, default_val, callback,
            config_key.data(), static_cast<uint32_t>(config_key.size()),
            format_str.data(), static_cast<uint32_t>(format_str.size()),
            show_sub_element_indicator, power);
    }

    void QuickMenu::set_slider_value(uint32_t module_id, uint32_t slider_id, float value) {
        if (!valid()) return;
        g_conn.vtable->qm_set_slider_value(module_id, slider_id, value);
    }

    float QuickMenu::get_slider_value(uint32_t module_id, uint32_t slider_id) {
        if (!valid()) return 0.f;
        return g_conn.vtable->qm_get_slider_value(module_id, slider_id);
    }

    uint32_t QuickMenu::add_separator(uint32_t module_id, uint32_t foldout_id) {
        if (!valid()) return Bootstrap::invalid_id;
        return g_conn.vtable->qm_add_separator(module_id, foldout_id);
    }

    void QuickMenu::navigate_to(uint32_t page_id) {
        if (!valid()) return;
        g_conn.vtable->qm_navigate_to(page_id);
    }

    void QuickMenu::navigate_back() {
        if (!valid()) return;
        g_conn.vtable->qm_navigate_back();
    }

    // Ergonomic QuickMenu layer

    namespace {
        std::unordered_map<uint32_t, std::function<void()>>        g_btnFns;
        std::unordered_map<uint32_t, std::function<void(bool)>>    g_toggleFns;
        std::unordered_map<uint32_t, std::function<void(float)>>   g_sliderFns;
        std::unordered_map<uint32_t, std::function<void(int32_t)>> g_enumFns;

        void __cdecl btn_tramp(uint32_t id) {
            auto it = g_btnFns.find(id);
            if (it != g_btnFns.end() && it->second) it->second();
        }
        void __cdecl toggle_tramp(uint32_t id, bool on) {
            auto it = g_toggleFns.find(id);
            if (it != g_toggleFns.end() && it->second) it->second(on);
        }
        void __cdecl slider_tramp(uint32_t id, float v) {
            auto it = g_sliderFns.find(id);
            if (it != g_sliderFns.end() && it->second) it->second(v);
        }
        void __cdecl enum_tramp(uint32_t id, int32_t i) {
            auto it = g_enumFns.find(id);
            if (it != g_enumFns.end() && it->second) it->second(i);
        }

        std::vector<std::function<void()>> g_onReady;
        bool g_onReadyFired = false;
        bool g_onReadyRegistered = false;

        void __cdecl onready_tramp() {
            g_onReadyFired = true;
            auto copy = g_onReady;
            for (auto& f : copy) if (f) f();
        }
    }

    // Button

    Button& Button::text(std::string_view t)         { QuickMenu::Get().set_button_text(module_id, id, t); return *this; }
    Button& Button::icon(int32_t sprite_id)          { QuickMenu::Get().set_button_icon(module_id, id, sprite_id); return *this; }
    Button& Button::icon(void* sprite_ptr)           { QuickMenu::Get().set_button_icon(module_id, id, sprite_ptr); return *this; }
    Button& Button::color(float r, float g, float b, float a) { QuickMenu::Get().set_button_color(module_id, id, r, g, b, a); return *this; }
    Button& Button::enabled(bool e)                  { QuickMenu::Get().set_button_enabled(module_id, id, e); return *this; }
    Button& Button::visible(bool v)                  { QuickMenu::Get().set_button_visible(module_id, id, v); return *this; }
    void    Button::remove()                         { QuickMenu::Get().remove_button(module_id, id); g_btnFns.erase(id); }

    // Toggle

    bool    Toggle::state() const                    { return QuickMenu::Get().get_toggle_state(module_id, id); }
    Toggle& Toggle::set(bool state)                  { QuickMenu::Get().set_toggle_state(module_id, id, state); return *this; }

    // Slider

    float   Slider::value() const                    { return QuickMenu::Get().get_slider_value(module_id, id); }
    Slider& Slider::set(float value)                 { QuickMenu::Get().set_slider_value(module_id, id, value); return *this; }

    // EnumSelector

    int32_t       EnumSelector::index() const        { return QuickMenu::Get().get_enum_index(module_id, id); }
    EnumSelector& EnumSelector::set(int32_t index)   { QuickMenu::Get().set_enum_index(module_id, id, index); return *this; }

    // Foldout

    Toggle Foldout::toggle(std::string_view text, bool default_state,
                           std::function<void(bool)> on_change, std::string_view config_key, bool sub_indicator) {
        uint32_t tid = QuickMenu::Get().add_settings_toggle(module_id, id, text, default_state,
                                                            &toggle_tramp, config_key, sub_indicator);
        if (tid != invalid_id && on_change) g_toggleFns[tid] = std::move(on_change);
        return Toggle{ module_id, tid };
    }

    Slider Foldout::slider(std::string_view label, float min_val, float max_val, float default_val,
                           std::function<void(float)> on_change, std::string_view config_key,
                           std::string_view format_str, bool sub_indicator, float power) {
        uint32_t sid = QuickMenu::Get().add_slider(module_id, id, label, min_val, max_val, default_val,
                                                   &slider_tramp, config_key, format_str, sub_indicator, power);
        if (sid != invalid_id && on_change) g_sliderFns[sid] = std::move(on_change);
        return Slider{ module_id, sid };
    }

    EnumSelector Foldout::enum_selector(std::string_view label, const char* const* options, uint32_t option_count,
                                        int32_t default_index, std::function<void(int32_t)> on_change,
                                        std::string_view config_key, bool sub_indicator) {
        uint32_t eid = QuickMenu::Get().add_enum_selector(module_id, id, label, options, option_count,
                                                          default_index, &enum_tramp, config_key, sub_indicator);
        if (eid != invalid_id && on_change) g_enumFns[eid] = std::move(on_change);
        return EnumSelector{ module_id, eid };
    }

    void     Foldout::separator()                    { QuickMenu::Get().add_separator(module_id, id); }
    Foldout& Foldout::set_expanded(bool expanded)    { QuickMenu::Get().set_foldout_expanded(module_id, id, expanded); return *this; }
    bool     Foldout::expanded() const               { return QuickMenu::Get().get_foldout_expanded(module_id, id); }

    // PageRef

    Button PageRef::button(std::string_view text, std::function<void()> on_click) {
        uint32_t bid = QuickMenu::Get().add_button(module_id, id, text, &btn_tramp);
        if (bid != invalid_id && on_click) g_btnFns[bid] = std::move(on_click);
        return Button{ module_id, bid };
    }

    Toggle PageRef::toggle(std::string_view text, bool default_state,
                           std::function<void(bool)> on_change, std::string_view config_key) {
        uint32_t tid = QuickMenu::Get().add_toggle(module_id, id, text, default_state, &toggle_tramp, config_key);
        if (tid != invalid_id && on_change) g_toggleFns[tid] = std::move(on_change);
        return Toggle{ module_id, tid };
    }

    Foldout PageRef::foldout(std::string_view title, bool default_expanded,
                             bool show_background, bool auto_separators) {
        uint32_t fid = QuickMenu::Get().add_foldout(module_id, id, title, default_expanded, show_background, auto_separators);
        return Foldout{ module_id, fid };
    }

    // SubPage

    SubPage& SubPage::nav_text(std::string_view text) { QuickMenu::Get().set_subpage_nav_text(module_id, id, text); return *this; }
    SubPage& SubPage::nav_icon(int32_t sprite_id)     { QuickMenu::Get().set_subpage_nav_icon(module_id, id, sprite_id); return *this; }
    Button   SubPage::nav_button() const              { return Button{ module_id, QuickMenu::Get().get_subpage_nav_button(module_id, id) }; }

    // Page

    SubPage Page::sub_page(std::string_view name) {
        return SubPage{ { module_id, QuickMenu::Get().create_sub_page(module_id, id, name) } };
    }
    Page&  Page::icon(int32_t sprite_id)             { QuickMenu::Get().set_page_icon(module_id, id, sprite_id); return *this; }
    Page&  Page::icon(void* sprite_ptr)              { QuickMenu::Get().set_page_icon(module_id, id, sprite_ptr); return *this; }
    Page&  Page::title(std::string_view title)       { QuickMenu::Get().set_page_title(module_id, id, title); return *this; }
    Page&  Page::badge(bool visible, std::string_view text) { QuickMenu::Get().set_page_badge(module_id, id, visible, text); return *this; }
    void   Page::navigate_to()                       { QuickMenu::Get().navigate_to(id); }
    void   Page::remove()                            { QuickMenu::Get().remove_page(module_id, id); }

    // Menu

    bool Menu::is_ready() const { return QuickMenu::Get().is_ready(); }

    void Menu::on_ready(std::function<void()> build) {
        if (!build) return;
        if (g_onReadyFired) { build(); return; }
        g_onReady.push_back(std::move(build));
        if (!g_onReadyRegistered && is_connected() && g_conn.vtable->register_menu_event) {
            g_onReadyRegistered = true;
            g_conn.vtable->register_menu_event(m_module_id, MenuEvent::QuickMenuSetup, &onready_tramp);
        }
    }

    Page Menu::page(std::string_view name, std::string_view tooltip) {
        return Page{ { m_module_id, QuickMenu::Get().create_page(m_module_id, name, tooltip) } };
    }

    void Menu::navigate_back() { QuickMenu::Get().navigate_back(); }

    // PlayerEvents

    PlayerEvents& PlayerEvents::Get() {
        static PlayerEvents instance;
        return instance;
    }

    uint32_t PlayerEvents::register_event(uint32_t module_id, Bootstrap::PlayerEvent event,
        fn_player_simple_callback callback) {
        if (!is_connected() || !callback) return Bootstrap::invalid_id;
        return g_conn.vtable->register_player_event(module_id, event, callback);
    }

    void PlayerEvents::unregister_event(uint32_t module_id, uint32_t callback_id) {
        if (!is_connected()) return;
        g_conn.vtable->unregister_player_event(module_id, callback_id);
    }

    void PlayerEvents::invoke(void* player, Bootstrap::PlayerEvent event) {
        if (!is_connected()) return;
        g_conn.vtable->invoke_player_event(player, event);
    }

    void* PlayerEvents::get_local_player() {
        if (!is_connected()) return nullptr;
        return g_conn.vtable->get_local_player();
    }

    void* PlayerEvents::get_local_player_api() {
        if (!is_connected()) return nullptr;
        return g_conn.vtable->get_local_player_api();
    }

    void* PlayerEvents::get_local_vrc_player() {
        if (!is_connected()) return nullptr;
        return g_conn.vtable->get_local_vrc_player();
    }

    Bootstrap::PlayerRank PlayerEvents::get_player_rank(void* player) {
        if (!is_connected() || !player) return Bootstrap::PlayerRank::Visitor;
        return static_cast<Bootstrap::PlayerRank>(
            g_conn.vtable->get_player_rank(player));
    }

    Bootstrap::Color PlayerEvents::get_rank_color(Bootstrap::PlayerRank rank) {
        Bootstrap::Color c{ 1.f, 1.f, 1.f, 1.f };
        if (!is_connected()) return c;
        g_conn.vtable->get_rank_color(static_cast<uint8_t>(rank), &c.r, &c.g, &c.b, &c.a);
        return c;
    }

    // TweenService

    TweenService& TweenService::Get() {
        static TweenService instance;
        return instance;
    }

    uint32_t TweenService::anchored_position(uint32_t module_id, void* rect_transform,
        float from_x, float from_y, float to_x, float to_y,
        float duration_ms, int32_t ease_type) {
        if (!is_connected() || !rect_transform) return Bootstrap::invalid_id;
        return g_conn.vtable->tween_anchored_position(module_id, rect_transform,
            from_x, from_y, to_x, to_y, duration_ms, ease_type);
    }

    uint32_t TweenService::local_position(uint32_t module_id, void* transform,
        float from_x, float from_y, float from_z,
        float to_x, float to_y, float to_z,
        float duration_ms, int32_t ease_type) {
        if (!is_connected() || !transform) return Bootstrap::invalid_id;
        return g_conn.vtable->tween_local_position(module_id, transform,
            from_x, from_y, from_z, to_x, to_y, to_z, duration_ms, ease_type);
    }

    uint32_t TweenService::local_scale(uint32_t module_id, void* transform,
        float from_x, float from_y, float from_z,
        float to_x, float to_y, float to_z,
        float duration_ms, int32_t ease_type) {
        if (!is_connected() || !transform) return Bootstrap::invalid_id;
        return g_conn.vtable->tween_local_scale(module_id, transform,
            from_x, from_y, from_z, to_x, to_y, to_z, duration_ms, ease_type);
    }

    void TweenService::cancel(uint32_t tween_id) {
        if (!is_connected()) return;
        g_conn.vtable->tween_cancel(tween_id);
    }

    void TweenService::cancel_all(uint32_t module_id) {
        if (!is_connected()) return;
        g_conn.vtable->tween_cancel_all(module_id);
    }

    uint32_t TweenService::tween_float(uint32_t module_id, float from, float to,
        float duration_ms, int32_t ease_type,
        fn_tween_completion_callback on_complete) {
        if (!is_connected()) return Bootstrap::invalid_id;
        return g_conn.vtable->tween_float(module_id, from, to, duration_ms, ease_type, on_complete);
    }

    void TweenService::cancel_all_for_target(void* target_ptr) {
        if (!is_connected() || !target_ptr) return;
        g_conn.vtable->tween_cancel_all_for_target(target_ptr);
    }

    uint32_t TweenService::anchored_position_ex(uint32_t module_id, void* rect_transform,
        float from_x, float from_y, float to_x, float to_y,
        float duration_ms, int32_t ease_type,
        fn_tween_completion_callback on_complete) {
        if (!is_connected() || !rect_transform) return Bootstrap::invalid_id;
        return g_conn.vtable->tween_anchored_position_ex(module_id, rect_transform,
            from_x, from_y, to_x, to_y, duration_ms, ease_type, on_complete);
    }

    uint32_t TweenService::local_position_ex(uint32_t module_id, void* transform,
        float from_x, float from_y, float from_z,
        float to_x, float to_y, float to_z,
        float duration_ms, int32_t ease_type,
        fn_tween_completion_callback on_complete) {
        if (!is_connected() || !transform) return Bootstrap::invalid_id;
        return g_conn.vtable->tween_local_position_ex(module_id, transform,
            from_x, from_y, from_z, to_x, to_y, to_z, duration_ms, ease_type, on_complete);
    }

    uint32_t TweenService::local_scale_ex(uint32_t module_id, void* transform,
        float from_x, float from_y, float from_z,
        float to_x, float to_y, float to_z,
        float duration_ms, int32_t ease_type,
        fn_tween_completion_callback on_complete) {
        if (!is_connected() || !transform) return Bootstrap::invalid_id;
        return g_conn.vtable->tween_local_scale_ex(module_id, transform,
            from_x, from_y, from_z, to_x, to_y, to_z, duration_ms, ease_type, on_complete);
    }

    // NameplateService

    NameplateService& NameplateService::Get() {
        static NameplateService instance;
        return instance;
    }

    std::string NameplateService::create_plate(uint32_t module_id, void* player,
        float pos_x, float pos_y, float pos_z,
        std::string_view label, std::string_view tag) {
        if (!is_connected() || !player) return {};
        char buf[16]{};
        uint32_t written = g_conn.vtable->np_create_plate(module_id, player,
            pos_x, pos_y, pos_z,
            label.data(), static_cast<uint32_t>(label.size()),
            tag.data(), static_cast<uint32_t>(tag.size()),
            buf, sizeof(buf));
        if (written == 0) return {};
        return std::string(buf, written);
    }

    void NameplateService::destroy_plate(uint32_t module_id, void* player, std::string_view plate_id) {
        if (!is_connected() || !player) return;
        g_conn.vtable->np_destroy_plate(module_id, player,
            plate_id.data(), static_cast<uint32_t>(plate_id.size()));
    }

    void NameplateService::destroy_plates_by_tag(uint32_t module_id, void* player, std::string_view tag_prefix) {
        if (!is_connected() || !player) return;
        g_conn.vtable->np_destroy_plates_by_tag(module_id, player,
            tag_prefix.data(), static_cast<uint32_t>(tag_prefix.size()));
    }

    void NameplateService::set_plate_text(uint32_t module_id, void* player,
        std::string_view plate_id, std::string_view text) {
        if (!is_connected() || !player) return;
        g_conn.vtable->np_set_plate_text(module_id, player,
            plate_id.data(), static_cast<uint32_t>(plate_id.size()),
            text.data(), static_cast<uint32_t>(text.size()));
    }

    void NameplateService::set_plate_text_color(uint32_t module_id, void* player,
        std::string_view plate_id,
        float r, float g, float b, float a) {
        if (!is_connected() || !player) return;
        g_conn.vtable->np_set_plate_text_color(module_id, player,
            plate_id.data(), static_cast<uint32_t>(plate_id.size()), r, g, b, a);
    }

    void NameplateService::set_plate_icon_color(uint32_t module_id, void* player,
        std::string_view plate_id,
        float r, float g, float b, float a) {
        if (!is_connected() || !player) return;
        g_conn.vtable->np_set_plate_icon_color(module_id, player,
            plate_id.data(), static_cast<uint32_t>(plate_id.size()), r, g, b, a);
    }

    void NameplateService::set_plate_position(uint32_t module_id, void* player,
        std::string_view plate_id,
        float x, float y, float z) {
        if (!is_connected() || !player) return;
        g_conn.vtable->np_set_plate_position(module_id, player,
            plate_id.data(), static_cast<uint32_t>(plate_id.size()), x, y, z);
    }

    void NameplateService::set_plate_background_enabled(uint32_t module_id, void* player,
        std::string_view plate_id, bool enabled) {
        if (!is_connected() || !player) return;
        g_conn.vtable->np_set_plate_background_enabled(module_id, player,
            plate_id.data(), static_cast<uint32_t>(plate_id.size()), enabled);
    }

    // ClientUsage

    ClientUsage& ClientUsage::Get() {
        static ClientUsage instance;
        return instance;
    }

    void ClientUsage::register_client(uint32_t module_id, std::string_view client_name, std::string_view api_key) {
        if (!is_connected()) return;
        g_conn.vtable->cu_register_client(module_id,
            client_name.data(), static_cast<uint32_t>(client_name.size()),
            api_key.data(), static_cast<uint32_t>(api_key.size()));
    }

    bool ClientUsage::is_client_registered(std::string_view client_name) {
        if (!is_connected()) return false;
        return g_conn.vtable->cu_is_client_registered(
            client_name.data(), static_cast<uint32_t>(client_name.size()));
    }

    // Performance

    Performance& Performance::Get() {
        static Performance instance;
        return instance;
    }

    int32_t Performance::get_int(int32_t setting_id) {
        if (!is_connected()) return 0;
        return g_conn.vtable->perf_get_int(setting_id);
    }

    void Performance::set_int(int32_t setting_id, int32_t value) {
        if (!is_connected()) return;
        g_conn.vtable->perf_set_int(setting_id, value);
    }

    float Performance::get_float(int32_t setting_id) {
        if (!is_connected()) return 0.f;
        return g_conn.vtable->perf_get_float(setting_id);
    }

    void Performance::set_float(int32_t setting_id, float value) {
        if (!is_connected()) return;
        g_conn.vtable->perf_set_float(setting_id, value);
    }

    void Performance::force_gc(int32_t generation) {
        if (!is_connected()) return;
        g_conn.vtable->perf_force_gc(generation);
    }

    void Performance::full_cleanup() {
        if (!is_connected()) return;
        g_conn.vtable->perf_full_cleanup();
    }

    // KeyAuth

    KeyAuth& KeyAuth::Get() {
        static KeyAuth instance;
        return instance;
    }

    bool KeyAuth::is_ready() {
        if (!is_connected()) return false;
        return g_conn.vtable->ka_is_ready();
    }

    bool KeyAuth::get_user(Bootstrap::KeyAuthUserInfo* out) {
        if (!is_connected() || !out) return false;
        return g_conn.vtable->ka_get_user(out);
    }

    bool KeyAuth::has_product(std::string_view product_id, Bootstrap::KeyAuthProductResult* out) {
        if (!is_connected() || !out) return false;
        return g_conn.vtable->ka_has_product(product_id.data(), static_cast<uint32_t>(product_id.size()), out);
    }

    bool KeyAuth::redeem_license(std::string_view key, Bootstrap::KeyAuthRedeemResult* out) {
        if (!is_connected() || !out) return false;
        return g_conn.vtable->ka_redeem_license(key.data(), static_cast<uint32_t>(key.size()), out);
    }

    uint32_t KeyAuth::get_licenses(Bootstrap::KeyAuthProductResult* out_buf, uint32_t buf_count) {
        if (!is_connected() || !out_buf || buf_count == 0) return 0;
        return g_conn.vtable->ka_get_licenses(out_buf, buf_count);
    }

    std::optional<Bootstrap::KeyAuthUserInfo> KeyAuth::get_user() {
        Bootstrap::KeyAuthUserInfo info{};
        if (!get_user(&info)) return std::nullopt;
        return info;
    }

    std::optional<Bootstrap::KeyAuthProductResult> KeyAuth::check_product(std::string_view product_id) {
        Bootstrap::KeyAuthProductResult result{};
        if (!has_product(product_id, &result)) return std::nullopt;
        return result;
    }

    // FileSystem

    FileSystem& FileSystem::Get() {
        static FileSystem instance;
        return instance;
    }

    bool FileSystem::write_file(uint32_t module_id, std::string_view path, const void* data, uint32_t len) {
        if (!is_connected()) return false;
        return g_conn.vtable->fs_write_file(module_id,
            path.data(), static_cast<uint32_t>(path.size()),
            static_cast<uint8_t const*>(data), len);
    }

    bool FileSystem::write_file(uint32_t module_id, std::string_view path, std::string_view data) {
        return write_file(module_id, path, data.data(), static_cast<uint32_t>(data.size()));
    }

    std::string FileSystem::read_file(uint32_t module_id, std::string_view path) {
        if (!is_connected()) return {};
        // First get size to allocate appropriately
        uint64_t sz = g_conn.vtable->fs_file_size(module_id,
            path.data(), static_cast<uint32_t>(path.size()));
        if (sz == 0) {
            // Might still be a valid empty file or not exist — try a small read
            char buf[4096];
            uint32_t len = g_conn.vtable->fs_read_file(module_id,
                path.data(), static_cast<uint32_t>(path.size()), buf, sizeof(buf));
            if (len == 0) return {};
            return std::string(buf, len);
        }
        // Cap at reasonable size for a single read
        uint32_t read_size = static_cast<uint32_t>((std::min)(sz, static_cast<uint64_t>(16u * 1024u * 1024u)));
        std::string result(read_size, '\0');
        uint32_t len = g_conn.vtable->fs_read_file(module_id,
            path.data(), static_cast<uint32_t>(path.size()),
            result.data(), read_size);
        result.resize(len);
        return result;
    }

    bool FileSystem::file_exists(uint32_t module_id, std::string_view path) {
        if (!is_connected()) return false;
        return g_conn.vtable->fs_file_exists(module_id,
            path.data(), static_cast<uint32_t>(path.size()));
    }

    bool FileSystem::delete_file(uint32_t module_id, std::string_view path) {
        if (!is_connected()) return false;
        return g_conn.vtable->fs_delete_file(module_id,
            path.data(), static_cast<uint32_t>(path.size()));
    }

    bool FileSystem::create_directory(uint32_t module_id, std::string_view path) {
        if (!is_connected()) return false;
        return g_conn.vtable->fs_create_dir(module_id,
            path.data(), static_cast<uint32_t>(path.size()));
    }

    bool FileSystem::delete_directory(uint32_t module_id, std::string_view path) {
        if (!is_connected()) return false;
        return g_conn.vtable->fs_delete_dir(module_id,
            path.data(), static_cast<uint32_t>(path.size()));
    }

    std::vector<std::string> FileSystem::list_directory(uint32_t module_id, std::string_view path) {
        if (!is_connected()) return {};
        char buf[8192];
        uint32_t len = g_conn.vtable->fs_list_dir(module_id,
            path.data(), static_cast<uint32_t>(path.size()), buf, sizeof(buf));
        if (len == 0) return {};

        // Entries are null-delimited
        std::vector<std::string> entries;
        uint32_t start = 0;
        for (uint32_t i = 0; i < len; ++i) {
            if (buf[i] == '\0') {
                if (i > start) entries.emplace_back(buf + start, i - start);
                start = i + 1;
            }
        }
        if (start < len) entries.emplace_back(buf + start, len - start);
        return entries;
    }

    uint64_t FileSystem::file_size(uint32_t module_id, std::string_view path) {
        if (!is_connected()) return 0;
        return g_conn.vtable->fs_file_size(module_id,
            path.data(), static_cast<uint32_t>(path.size()));
    }

    bool FileSystem::append_file(uint32_t module_id, std::string_view path, const void* data, uint32_t len) {
        if (!is_connected()) return false;
        return g_conn.vtable->fs_append_file(module_id,
            path.data(), static_cast<uint32_t>(path.size()),
            static_cast<uint8_t const*>(data), len);
    }

    bool FileSystem::append_file(uint32_t module_id, std::string_view path, std::string_view data) {
        return append_file(module_id, path, data.data(), static_cast<uint32_t>(data.size()));
    }

    // Clipboard

    Clipboard& Clipboard::Get() {
        static Clipboard instance;
        return instance;
    }

    bool Clipboard::set(std::string_view text) {
        if (!is_connected()) return false;
        return g_conn.vtable->clipboard_set(text.data(), static_cast<uint32_t>(text.size()));
    }

    std::string Clipboard::get() {
        if (!is_connected()) return {};
        char buf[4096];
        uint32_t len = g_conn.vtable->clipboard_get(buf, sizeof(buf));
        if (len == 0) return {};
        return std::string(buf, len);
    }

}
