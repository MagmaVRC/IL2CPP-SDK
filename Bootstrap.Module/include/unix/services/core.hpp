#pragma once

#include <unix/module.hpp>

namespace UNIx {

template <class T>
concept ConfigScalar =
    std::same_as<T, bool>     || std::same_as<T, int32_t>  || std::same_as<T, int64_t> ||
    std::same_as<T, uint32_t> || std::same_as<T, float>    || std::same_as<T, double>  ||
    std::same_as<T, std::string> || std::same_as<T, Color> ||
    std::same_as<T, Vec2>        || std::same_as<T, Vec3>  || std::same_as<T, Vec4>;

template <class T> concept ConfigArrayScalar = ConfigScalar<T> && !std::same_as<T, bool>;

/// <summary>Pack a key and its modifier mask into the uint32_t the hotkey rows carry.
/// Zero means "unbound", and a capture that reports zero was cancelled.</summary>
constexpr uint32_t PackCombo(Key k, KeyMods m) noexcept {
    return (uint32_t)(int32_t)k | ((uint32_t)m << 16);
}
constexpr Key     ComboKey (uint32_t combo) noexcept { return (Key)(int32_t)(combo & 0xFFFFu); }
constexpr KeyMods ComboMods(uint32_t combo) noexcept { return (KeyMods)(combo >> 16); }

/// <summary>This module's persistent settings. Keys live in the module's own namespace.</summary>
class Config {
public:
    /// <summary>Read one scalar. An absent key yields `def`.</summary>
    template <ConfigScalar T> [[nodiscard]] T Get(std::string_view key, T def = {}) const {
        if constexpr (std::same_as<T, bool>) {
            UNIX_USE(config, get_bool);
            return detail::g_d.config_get_bool(detail::g_self, Sv(key), def);
        } else if constexpr (std::same_as<T, int32_t>) {
            UNIX_USE(config, get_int);
            return detail::g_d.config_get_int(detail::g_self, Sv(key), def);
        } else if constexpr (std::same_as<T, int64_t>) {
            UNIX_USE(config, get_i64);
            return detail::g_d.config_get_i64(detail::g_self, Sv(key), def);
        } else if constexpr (std::same_as<T, uint32_t>) {
            UNIX_USE(config, get_u32);
            return detail::g_d.config_get_u32(detail::g_self, Sv(key), def);
        } else if constexpr (std::same_as<T, float>) {
            UNIX_USE(config, get_float);
            return detail::g_d.config_get_float(detail::g_self, Sv(key), def);
        } else if constexpr (std::same_as<T, double>) {
            UNIX_USE(config, get_double);
            return detail::g_d.config_get_double(detail::g_self, Sv(key), def);
        } else if constexpr (std::same_as<T, std::string>) {
            return TryGetString(key).value_or(std::move(def));
        } else if constexpr (std::same_as<T, Color>) {
            UNIX_USE(config, get_color);
            return detail::g_d.config_get_color(detail::g_self, Sv(key), def);
        } else if constexpr (std::same_as<T, Vec2>) {
            UNIX_USE(config, get_vec2);
            return detail::g_d.config_get_vec2(detail::g_self, Sv(key), def);
        } else if constexpr (std::same_as<T, Vec3>) {
            UNIX_USE(config, get_vec3);
            return detail::g_d.config_get_vec3(detail::g_self, Sv(key), def);
        } else {
            UNIX_USE(config, get_vec4);
            return detail::g_d.config_get_vec4(detail::g_self, Sv(key), def);
        }
    }

    /// <summary>Write one scalar. Persisted on the next Save().</summary>
    template <ConfigScalar T> void Set(std::string_view key, T const& v) {
        if constexpr (std::same_as<T, bool>) {
            UNIX_USE(config, set_bool);
            detail::g_d.config_set_bool(detail::g_self, Sv(key), v);
        } else if constexpr (std::same_as<T, int32_t>) {
            UNIX_USE(config, set_int);
            detail::g_d.config_set_int(detail::g_self, Sv(key), v);
        } else if constexpr (std::same_as<T, int64_t>) {
            UNIX_USE(config, set_i64);
            detail::g_d.config_set_i64(detail::g_self, Sv(key), v);
        } else if constexpr (std::same_as<T, uint32_t>) {
            UNIX_USE(config, set_u32);
            detail::g_d.config_set_u32(detail::g_self, Sv(key), v);
        } else if constexpr (std::same_as<T, float>) {
            UNIX_USE(config, set_float);
            detail::g_d.config_set_float(detail::g_self, Sv(key), v);
        } else if constexpr (std::same_as<T, double>) {
            UNIX_USE(config, set_double);
            detail::g_d.config_set_double(detail::g_self, Sv(key), v);
        } else if constexpr (std::same_as<T, std::string>) {
            UNIX_USE(config, set_string);
            detail::g_d.config_set_string(detail::g_self, Sv(key), Sv(v));
        } else if constexpr (std::same_as<T, Color>) {
            UNIX_USE(config, set_color);
            detail::g_d.config_set_color(detail::g_self, Sv(key), v);
        } else if constexpr (std::same_as<T, Vec2>) {
            UNIX_USE(config, set_vec2);
            detail::g_d.config_set_vec2(detail::g_self, Sv(key), v);
        } else if constexpr (std::same_as<T, Vec3>) {
            UNIX_USE(config, set_vec3);
            detail::g_d.config_set_vec3(detail::g_self, Sv(key), v);
        } else {
            UNIX_USE(config, set_vec4);
            detail::g_d.config_set_vec4(detail::g_self, Sv(key), v);
        }
    }

    /// <summary>Read the key into `value` now, and write it back on every Save().</summary>
    /// <param name="value">Storage owned by the object whose lifecycle callback is running;
    /// it must outlive the module. The two-argument form uses its contents as the fallback.</param>
    /// <param name="key">Config key, in this module's namespace.</param>
    template <ConfigScalar T> void Load(T* value, std::string_view key) {
        if (value) Load(value, key, *value);
    }
    template <ConfigScalar T> void Load(T* value, std::string_view key, T fallback) {
        if (!value) return;
        *value = Get<T>(key, fallback);
        detail::RegisterLoad(value, [value, k = std::string(key)] {
            ::UNIx::Host::Get().Config().Set<T>(k, *value);
        });
    }
    /// <summary>Drop a Load() registration before the storage goes away.</summary>
    template <class = void> void Unload(const void* value) { detail::UnregisterLoad(value); }

    /// <summary>Absent and empty are distinct: an absent key yields nullopt.</summary>
    template <class = void> [[nodiscard]] std::optional<std::string>
    TryGetString(std::string_view key) const {
        UNIX_USE(config, get_string);
        return detail::TryReadStr([&](char* b, uint32_t c) {
            return detail::g_d.config_get_string(detail::g_self, Sv(key), b, c);
        });
    }
    template <class = void> [[nodiscard]] bool HasKey(std::string_view key) const {
        UNIX_USE(config, has_key);
        return detail::g_d.config_has_key(detail::g_self, Sv(key));
    }
    template <class = void> void RemoveKey(std::string_view key) {
        UNIX_USE(config, remove_key);
        detail::g_d.config_remove_key(detail::g_self, Sv(key));
    }
    template <class = void> void Clear() {
        UNIX_USE(config, clear);
        detail::g_d.config_clear(detail::g_self);
    }
    template <class = void> [[nodiscard]] std::vector<std::string> GetKeys() const {
        UNIX_USE(config, get_keys);
        return detail::ReadList([&](char* b, uint32_t c) {
            return detail::g_d.config_get_keys(detail::g_self, b, c);
        });
    }
    /// <summary>Write every Load()-registered value back, then flush to disk.</summary>
    template <class = void> void Save() {
        UNIX_USE(config, save);
        detail::RunLoadWriteback();
        detail::g_d.config_save(detail::g_self);
    }

    template <ConfigArrayScalar T> [[nodiscard]] std::vector<T> GetArray(std::string_view key) const {
        if constexpr (std::same_as<T, std::string>) {
            UNIX_USE(config, get_list);
            return detail::ReadList([&](char* b, uint32_t c) {
                return detail::g_d.config_get_list(detail::g_self, Sv(key), b, c);
            });
        } else {
            UNIX_USE(config, get_blob);
            const std::vector<uint8_t> raw = detail::ReadBlob([&](void* b, uint32_t c) {
                return detail::g_d.config_get_blob(detail::g_self, Sv(key), b, c);
            });
            std::vector<T> out;
            if (raw.size() < sizeof(uint32_t)) return out;
            uint32_t n = 0;
            std::memcpy(&n, raw.data(), sizeof(n));
            const std::size_t avail = (raw.size() - sizeof(uint32_t)) / sizeof(T);
            if (n > avail) n = static_cast<uint32_t>(avail);
            out.resize(n);
            if (n) std::memcpy(out.data(), raw.data() + sizeof(uint32_t), n * sizeof(T));
            return out;
        }
    }
    template <ConfigArrayScalar T> void SetArray(std::string_view key, std::span<const T> v) {
        if constexpr (std::same_as<T, std::string>) {
            UNIX_USE(config, set_list);
            std::vector<unix_str> items;
            items.reserve(v.size());
            for (const std::string& s : v) items.push_back(Sv(s));
            detail::g_d.config_set_list(detail::g_self, Sv(key), items.data(),
                                        static_cast<uint32_t>(items.size()));
        } else {
            UNIX_USE(config, set_blob);
            std::vector<uint8_t> buf(sizeof(uint32_t) + v.size() * sizeof(T));
            const uint32_t n = static_cast<uint32_t>(v.size());
            std::memcpy(buf.data(), &n, sizeof(n));
            if (n) std::memcpy(buf.data() + sizeof(uint32_t), v.data(), v.size() * sizeof(T));
            detail::g_d.config_set_blob(detail::g_self, Sv(key), buf.data(),
                                        static_cast<uint32_t>(buf.size()));
        }
    }
    template <ConfigArrayScalar T> void SetArray(std::string_view key, const std::vector<T>& v) {
        SetArray<T>(key, std::span<const T>(v.data(), v.size()));
    }
    // std::span<const bool> is not constructible from std::vector<bool>, which is the shipped shape.
    template <class = void> void SetArray(std::string_view key, const std::vector<bool>& v) {
        UNIX_USE(config, set_blob);
        std::vector<uint8_t> buf(sizeof(uint32_t) + v.size());
        const uint32_t n = static_cast<uint32_t>(v.size());
        std::memcpy(buf.data(), &n, sizeof(n));
        for (std::size_t i = 0; i < v.size(); ++i) buf[sizeof(uint32_t) + i] = v[i] ? 1u : 0u;
        detail::g_d.config_set_blob(detail::g_self, Sv(key), buf.data(),
                                    static_cast<uint32_t>(buf.size()));
    }
    template <class = void> [[nodiscard]] std::vector<bool> GetBoolArray(std::string_view key) const {
        UNIX_USE(config, get_blob);
        const std::vector<uint8_t> raw = detail::ReadBlob([&](void* b, uint32_t c) {
            return detail::g_d.config_get_blob(detail::g_self, Sv(key), b, c);
        });
        std::vector<bool> out;
        if (raw.size() < sizeof(uint32_t)) return out;
        uint32_t n = 0;
        std::memcpy(&n, raw.data(), sizeof(n));
        const std::size_t avail = raw.size() - sizeof(uint32_t);
        if (n > avail) n = static_cast<uint32_t>(avail);
        out.resize(n);
        for (uint32_t i = 0; i < n; ++i) out[i] = raw[sizeof(uint32_t) + i] != 0;
        return out;
    }

    template <class = void> [[nodiscard]] std::vector<std::pair<std::string, std::string>>
    GetStringMap(std::string_view key) const {
        UNIX_USE(config, get_map);
        const std::vector<std::string> flat = detail::ReadList([&](char* b, uint32_t c) {
            return detail::g_d.config_get_map(detail::g_self, Sv(key), b, c);
        });
        std::vector<std::pair<std::string, std::string>> out;
        out.reserve(flat.size() / 2);
        for (std::size_t i = 0; i + 1 < flat.size(); i += 2) out.emplace_back(flat[i], flat[i + 1]);
        return out;
    }
    template <class = void>
    void SetStringMap(std::string_view key,
                      std::span<const std::pair<std::string, std::string>> entries) {
        UNIX_USE(config, set_map);
        std::vector<unix_str> flat;
        flat.reserve(entries.size() * 2);
        for (const auto& e : entries) { flat.push_back(Sv(e.first)); flat.push_back(Sv(e.second)); }
        detail::g_d.config_set_map(detail::g_self, Sv(key), flat.data(),
                                   static_cast<uint32_t>(flat.size()));
    }

    // Named scalar surface: inline forwarders over the templates above.
    template <class = void> void SetBool  (std::string_view k, bool v)     { Set<bool>(k, v); }
    template <class = void> [[nodiscard]] bool GetBool(std::string_view k, bool def = false) const { return Get<bool>(k, def); }
    template <class = void> void SetInt   (std::string_view k, int32_t v)  { Set<int32_t>(k, v); }
    template <class = void> [[nodiscard]] int32_t GetInt(std::string_view k, int32_t def = 0) const { return Get<int32_t>(k, def); }
    template <class = void> void SetI64   (std::string_view k, int64_t v)  { Set<int64_t>(k, v); }
    template <class = void> [[nodiscard]] int64_t GetI64(std::string_view k, int64_t def = 0) const { return Get<int64_t>(k, def); }
    template <class = void> void SetU32   (std::string_view k, uint32_t v) { Set<uint32_t>(k, v); }
    template <class = void> [[nodiscard]] uint32_t GetU32(std::string_view k, uint32_t def = 0) const { return Get<uint32_t>(k, def); }
    template <class = void> void SetFloat (std::string_view k, float v)    { Set<float>(k, v); }
    template <class = void> [[nodiscard]] float GetFloat(std::string_view k, float def = 0.f) const { return Get<float>(k, def); }
    template <class = void> void SetDouble(std::string_view k, double v)   { Set<double>(k, v); }
    template <class = void> [[nodiscard]] double GetDouble(std::string_view k, double def = 0.0) const { return Get<double>(k, def); }
    template <class = void> void SetString(std::string_view k, std::string_view v) {
        UNIX_USE(config, set_string);
        detail::g_d.config_set_string(detail::g_self, Sv(k), Sv(v));
    }
    template <class = void> [[nodiscard]] std::string GetString(std::string_view k,
                                                                std::string_view def = "") const {
        return TryGetString(k).value_or(std::string(def));
    }
    template <class = void> void SetVec2(std::string_view k, Vec2 v) { Set<Vec2>(k, v); }
    template <class = void> [[nodiscard]] Vec2 GetVec2(std::string_view k, Vec2 def = {}) const { return Get<Vec2>(k, def); }
    template <class = void> void SetVec3(std::string_view k, Vec3 v) { Set<Vec3>(k, v); }
    template <class = void> [[nodiscard]] Vec3 GetVec3(std::string_view k, Vec3 def = {}) const { return Get<Vec3>(k, def); }
    template <class = void> void SetVec4(std::string_view k, Vec4 v) { Set<Vec4>(k, v); }
    template <class = void> [[nodiscard]] Vec4 GetVec4(std::string_view k, Vec4 def = {}) const { return Get<Vec4>(k, def); }
    template <class = void> void SetColor(std::string_view k, Color v) { Set<Color>(k, v); }
    template <class = void> [[nodiscard]] Color GetColor(std::string_view k,
                                                         Color def = { 0.f, 0.f, 0.f, 1.f }) const {
        return Get<Color>(k, def);
    }
    template <class = void> void SetStringArray(std::string_view k, const std::vector<std::string>& v) {
        SetArray<std::string>(k, std::span<const std::string>(v.data(), v.size()));
    }
    template <class = void> [[nodiscard]] std::vector<std::string> GetStringArray(std::string_view k) const {
        return GetArray<std::string>(k);
    }
};

/// <summary>The client log. Every write also reaches the console fan-out.</summary>
class Log {
public:
    template <class = void> void Write(LogLevel level, std::string_view text) {
        UNIX_USE(log, write);
        detail::g_d.log_write(detail::g_self, static_cast<uint32_t>(level), Sv(text));
    }
    template <class... A> void Info (std::format_string<A...> f, A&&... a) {
        Write(LogLevel::Log, std::format(f, std::forward<A>(a)...));
    }
    template <class... A> void Debug(std::format_string<A...> f, A&&... a) {
        Write(LogLevel::Log, std::format(f, std::forward<A>(a)...));
    }
    template <class... A> void Warn (std::format_string<A...> f, A&&... a) {
        Write(LogLevel::Warning, std::format(f, std::forward<A>(a)...));
    }
    template <class... A> void Error(std::format_string<A...> f, A&&... a) {
        Write(LogLevel::Error, std::format(f, std::forward<A>(a)...));
    }
    /// <summary>Every line the client logs, including Unity's own callback.</summary>
    template <class = void> [[nodiscard]] Subscription OnMessage(std::function<void(const LogEvent&)> fn) {
        UNIX_USE(log, on_message);
        detail::Node* n = detail::NodeFor<unix_log_ev>(std::move(fn));
        const unix_handle h = detail::g_d.log_on_message(detail::g_self, &detail::Tramp<unix_log_ev>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
    /// <summary>The unix_log_flag mask currently in force.</summary>
    template <class = void> [[nodiscard]] uint32_t GetFlags() const {
        UNIX_USE(log, options);
        unix_log_options o{};
        o.size = sizeof(o);
        if (detail::g_d.log_options(detail::g_self, &o) != unix_ok) return 0;
        return o.values;
    }
    /// <summary>Change only the flags named in `mask`.</summary>
    template <class = void> Status SetFlags(uint32_t mask, uint32_t values) {
        UNIX_USE(log, set_options);
        unix_log_options o{};
        o.size = sizeof(o);
        o.mask = mask;
        o.values = values;
        return detail::g_d.log_set_options(detail::g_self, &o);
    }
    template <class = void> [[nodiscard]] std::string GetFilePath() const {
        UNIX_USE(log, file_path);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.log_file_path(detail::g_self, b, c);
        });
    }
    template <class = void> Status SetFilePath(std::string_view path) {
        UNIX_USE(log, set_file_path);
        return detail::g_d.log_set_file_path(detail::g_self, Sv(path));
    }
};

/// <summary>This module's sandboxed file store. Every path is relative to GetRoot().</summary>
class FileSystem {
public:
    template <class = void> [[nodiscard]] std::vector<uint8_t> Read(std::string_view path) const {
        UNIX_USE(fs, read);
        return detail::ReadBlob([&](void* b, uint32_t c) {
            return detail::g_d.fs_read(detail::g_self, Sv(path), b, c);
        });
    }
    template <class = void> [[nodiscard]] std::string ReadText(std::string_view path) const {
        const std::vector<uint8_t> b = Read(path);
        return std::string(reinterpret_cast<const char*>(b.data()), b.size());
    }
    template <class = void> Status Write(std::string_view path, std::span<const uint8_t> data) {
        UNIX_USE(fs, write);
        return detail::g_d.fs_write(detail::g_self, Sv(path), data.data(),
                                    static_cast<uint32_t>(data.size()));
    }
    template <class = void> Status Write(std::string_view path, std::string_view text) {
        UNIX_USE(fs, write);
        return detail::g_d.fs_write(detail::g_self, Sv(path), text.data(),
                                    static_cast<uint32_t>(text.size()));
    }
    template <class = void> Status Append(std::string_view path, std::span<const uint8_t> data) {
        UNIX_USE(fs, append);
        return detail::g_d.fs_append(detail::g_self, Sv(path), data.data(),
                                     static_cast<uint32_t>(data.size()));
    }
    template <class = void> Status Append(std::string_view path, std::string_view text) {
        UNIX_USE(fs, append);
        return detail::g_d.fs_append(detail::g_self, Sv(path), text.data(),
                                     static_cast<uint32_t>(text.size()));
    }
    template <class = void> [[nodiscard]] std::vector<std::string> ListDir(std::string_view path) const {
        UNIX_USE(fs, list_dir);
        return detail::ReadList([&](char* b, uint32_t c) {
            return detail::g_d.fs_list_dir(detail::g_self, Sv(path), b, c);
        });
    }
    template <class = void> [[nodiscard]] bool IsFile(std::string_view path) const {
        UNIX_USE(fs, is_file);
        return detail::g_d.fs_is_file(detail::g_self, Sv(path));
    }
    template <class = void> [[nodiscard]] bool IsDir(std::string_view path) const {
        UNIX_USE(fs, is_dir);
        return detail::g_d.fs_is_dir(detail::g_self, Sv(path));
    }
    template <class = void> Status DeleteFile(std::string_view path) {
        UNIX_USE(fs, delete_file);
        return detail::g_d.fs_delete_file(detail::g_self, Sv(path));
    }
    template <class = void> Status DeleteDir(std::string_view path) {
        UNIX_USE(fs, delete_dir);
        return detail::g_d.fs_delete_dir(detail::g_self, Sv(path));
    }
    template <class = void> Status CreateDir(std::string_view path) {
        UNIX_USE(fs, create_dir);
        return detail::g_d.fs_create_dir(detail::g_self, Sv(path));
    }
    /// <summary>Size in bytes, or nullopt when the file does not exist.</summary>
    template <class = void> [[nodiscard]] std::optional<uint64_t> GetSize(std::string_view path) const {
        UNIX_USE(fs, size);
        uint64_t n = 0;
        if (detail::g_d.fs_size(detail::g_self, Sv(path), &n) != unix_ok) return std::nullopt;
        return n;
    }
    /// <summary>This module's sandbox root, as an absolute path.</summary>
    template <class = void> [[nodiscard]] std::string GetRoot() const {
        UNIX_USE(fs, root);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.fs_root(detail::g_self, b, c);
        });
    }
};

/// <summary>In-process module-to-module messaging. No network is involved.</summary>
class Bus {
public:
    /// <summary>Subscribe to a topic. A trailing "*" matches a prefix.</summary>
    template <class = void> [[nodiscard]] Subscription Subscribe(std::string_view topic,
                                                                 std::function<void(const Message&)> fn) {
        UNIX_USE(bus, subscribe);
        detail::Node* n = detail::NodeFor<unix_message>(std::move(fn));
        const unix_handle h = detail::g_d.bus_subscribe(detail::g_self, Sv(topic),
                                                        &detail::Tramp<unix_message>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
    template <class = void> Status Publish(std::string_view topic, std::span<const uint8_t> payload) {
        UNIX_USE(bus, publish);
        return detail::g_d.bus_publish(detail::g_self, Sv(topic), payload.data(),
                                       static_cast<uint32_t>(payload.size()));
    }
    template <class = void> Status Publish(std::string_view topic, std::string_view text) {
        UNIX_USE(bus, publish);
        return detail::g_d.bus_publish(detail::g_self, Sv(topic), text.data(),
                                       static_cast<uint32_t>(text.size()));
    }
    template <class = void> [[nodiscard]] std::vector<std::string> GetTopics() const {
        UNIX_USE(bus, topics);
        return detail::ReadList([&](char* b, uint32_t c) {
            return detail::g_d.bus_topics(detail::g_self, b, c);
        });
    }
};

class Clipboard {
public:
    template <class = void> [[nodiscard]] std::string Get() const {
        UNIX_USE(clipboard, get);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.clipboard_get(detail::g_self, b, c);
        });
    }
    template <class = void> Status Set(std::string_view text) {
        UNIX_USE(clipboard, set);
        return detail::g_d.clipboard_set(detail::g_self, Sv(text));
    }
};

/// <summary>Quality settings and the two one-shot cleanups. Unity main thread only.</summary>
class Perf {
public:
    template <class = void> [[nodiscard]] int32_t GetInt(PerfSetting s) const {
        UNIX_USE(perf, get_int);
        return detail::g_d.perf_get_int(detail::g_self, static_cast<uint32_t>(s));
    }
    template <class = void> Status SetInt(PerfSetting s, int32_t v) {
        UNIX_USE(perf, set_int);
        return detail::g_d.perf_set_int(detail::g_self, static_cast<uint32_t>(s), v);
    }
    template <class = void> [[nodiscard]] float GetFloat(PerfSetting s) const {
        UNIX_USE(perf, get_float);
        return detail::g_d.perf_get_float(detail::g_self, static_cast<uint32_t>(s));
    }
    template <class = void> Status SetFloat(PerfSetting s, float v) {
        UNIX_USE(perf, set_float);
        return detail::g_d.perf_set_float(detail::g_self, static_cast<uint32_t>(s), v);
    }
    /// <param name="generation">Managed heap generation, or -1 for all.</param>
    template <class = void> Status ForceGc(int32_t generation = -1) {
        UNIX_USE(perf, force_gc);
        return detail::g_d.perf_force_gc(detail::g_self, generation);
    }
    template <class = void> Status FullCleanup() {
        UNIX_USE(perf, full_cleanup);
        return detail::g_d.perf_full_cleanup(detail::g_self);
    }
    template <class = void> [[nodiscard]] int32_t GetCameraRenderingPath(void* camera) const {
        UNIX_USE(perf, camera_rendering_path);
        return detail::g_d.perf_camera_rendering_path(detail::g_self, camera);
    }
    template <class = void> Status SetCameraRenderingPath(void* camera, int32_t path) {
        UNIX_USE(perf, set_camera_rendering_path);
        return detail::g_d.perf_set_camera_rendering_path(detail::g_self, camera, path);
    }
    /// <summary>One-shot and process-global. There is no inverse.</summary>
    template <class = void> Status ForceForward() {
        UNIX_USE(perf, force_forward);
        return detail::g_d.perf_force_forward(detail::g_self);
    }
};

class License {
public:
    /// <summary>False until the licensing handshake has completed; ask again later.</summary>
    template <class = void> [[nodiscard]] bool IsReady() const {
        UNIX_USE(license, is_ready);
        return detail::g_d.license_is_ready(detail::g_self);
    }
    template <class = void> [[nodiscard]] std::optional<LicenseUser> GetUser() const {
        UNIX_USE(license, user);
        LicenseUser u{};
        u.size = sizeof(u);
        if (detail::g_d.license_user(detail::g_self, &u) != unix_ok) return std::nullopt;
        return u;
    }
    /// <summary>The product record when this user holds it, nullopt otherwise.</summary>
    template <class = void> [[nodiscard]] std::optional<LicenseProduct>
    GetProduct(std::string_view productId) const {
        UNIX_USE(license, has_product);
        LicenseProduct p{};
        p.size = sizeof(p);
        if (detail::g_d.license_has_product(detail::g_self, Sv(productId), &p) != unix_ok)
            return std::nullopt;
        return p;
    }
    template <class = void> [[nodiscard]] bool HasProduct(std::string_view productId) const {
        return GetProduct(productId).has_value();
    }
    /// <summary>Redeem a key. The result carries the server's status and message.</summary>
    template <class = void> [[nodiscard]] std::optional<LicenseRedeem> Redeem(std::string_view key) {
        UNIX_USE(license, redeem);
        LicenseRedeem r{};
        r.size = sizeof(r);
        if (detail::g_d.license_redeem(detail::g_self, Sv(key), &r) != unix_ok) return std::nullopt;
        return r;
    }
    template <class = void> [[nodiscard]] std::vector<LicenseProduct> GetLicenses() const {
        UNIX_USE(license, licenses);
        return detail::ReadArray<LicenseProduct>([&](LicenseProduct* b, uint32_t cap, uint32_t stride) {
            return detail::g_d.license_licenses(detail::g_self, b, cap, stride);
        });
    }
};

/// <summary>One rebindable hotkey this module owns. The binding outlives any OnPress.</summary>
class Hotkey {
public:
    Hotkey() = default;
    explicit Hotkey(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(Hotkey a, Hotkey b) noexcept { return a.m_handle == b.m_handle; }

    template <class = void> [[nodiscard]] Subscription OnPress(std::function<void()> fn) {
        UNIX_USE(hotkey, on_press);
        detail::Node* n = detail::NodeForTask(std::move(fn));
        const unix_handle h = detail::g_d.hotkey_on_press(detail::g_self, m_handle,
                                                          &detail::Tramp<unix_hotkey_ev>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
    /// <summary>Level state at the last main-thread sample. Safe from any thread.</summary>
    template <class = void> [[nodiscard]] bool IsDown() const {
        UNIX_USE(hotkey, down);
        return detail::g_d.hotkey_down(detail::g_self, m_handle);
    }
    /// <summary>True only on the frame the key went down.</summary>
    template <class = void> [[nodiscard]] bool WasPressed() const {
        UNIX_USE(hotkey, pressed);
        return detail::g_d.hotkey_pressed(detail::g_self, m_handle);
    }
    template <class = void> Status SetBinding(Key key, KeyMods mods = KeyMods::None) {
        UNIX_USE(hotkey, set);
        return detail::g_d.hotkey_set(detail::g_self, m_handle,
                                      static_cast<uint32_t>(static_cast<int32_t>(key)),
                                      static_cast<uint32_t>(mods));
    }
    /// <summary>Restore this binding's registered default.</summary>
    template <class = void> Status Reset() {
        UNIX_USE(hotkey, reset);
        return detail::g_d.hotkey_reset(detail::g_self, m_handle);
    }
    template <class = void> Status SetEnabled(bool on) {
        UNIX_USE(hotkey, set_enabled);
        return detail::g_d.hotkey_set_enabled(detail::g_self, m_handle, on);
    }
    template <class = void> [[nodiscard]] bool IsEnabled() const {
        UNIX_USE(hotkey, enabled);
        return detail::g_d.hotkey_enabled(detail::g_self, m_handle);
    }
    /// <summary>Start capturing a new combination for this binding.</summary>
    /// <param name="done">Fires with the pressed combination, or (Key::None, KeyMods::None)
    /// if the capture was cancelled.</param>
    template <class = void> [[nodiscard]] Subscription BeginCapture(std::function<void(Key, KeyMods)> done) {
        UNIX_USE(hotkey, capture_begin);
        detail::Node* n = detail::NodeFor<unix_hotkey_ev>(
            [f = std::move(done)](const unix_hotkey_ev& e) {
                f(ComboKey(e.combo), ComboMods(e.combo));
            });
        const unix_handle h = detail::g_d.hotkey_capture_begin(detail::g_self, m_handle,
                                                               &detail::Tramp<unix_hotkey_ev>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
    template <class = void> void Remove() {
        if (m_handle == unix_null) return;
        detail::Cancel(m_handle);
        m_handle = unix_null;
    }
private:
    unix_handle m_handle{};
};

class Hotkeys {
public:
    /// <summary>Declare a rebindable hotkey. The host persists the binding in this module's
    /// namespace and scopes the name to this module.</summary>
    /// <param name="id">Stable key, unique within this module.</param>
    /// <param name="displayName">Label the rebind UI shows.</param>
    template <class = void> Hotkey Register(std::string_view id, std::string_view displayName,
                                            Key def, KeyMods mods = KeyMods::None) {
        UNIX_USE(hotkey, register);
        return Hotkey{ detail::g_d.hotkey_register(detail::g_self, Sv(id), Sv(displayName),
                                                   static_cast<uint32_t>(static_cast<int32_t>(def)),
                                                   static_cast<uint32_t>(mods)) };
    }
    template <class = void> Status CancelCapture() {
        UNIX_USE(hotkey, capture_cancel);
        return detail::g_d.hotkey_capture_cancel(detail::g_self);
    }
    template <class = void> [[nodiscard]] bool IsCapturing() const {
        UNIX_USE(hotkey, is_capturing);
        return detail::g_d.hotkey_is_capturing(detail::g_self);
    }
    /// <summary>Human-readable binding: "F5", "Ctrl+Shift+W", "Unbound".</summary>
    template <class = void> [[nodiscard]] std::string GetKeyName(Key key,
                                                                 KeyMods mods = KeyMods::None) const {
        UNIX_USE(hotkey, key_name);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.hotkey_key_name(detail::g_self,
                                               static_cast<uint32_t>(static_cast<int32_t>(key)),
                                               static_cast<uint32_t>(mods), b, c);
        });
    }
    template <class = void> [[nodiscard]] std::vector<HotkeyInfo> GetBindings() const {
        UNIX_USE(hotkey, bindings);
        return detail::ReadArray<HotkeyInfo>([&](HotkeyInfo* b, uint32_t cap, uint32_t stride) {
            return detail::g_d.hotkey_bindings(detail::g_self, b, cap, stride);
        });
    }
};

/// <summary>Deobfuscated names and the VRChat field-offset tables.</summary>
class Deob {
public:
    /// <summary>Resolve one obfuscated name. Empty when no map is loaded. Unity main thread.</summary>
    template <class = void> [[nodiscard]] std::string Resolve(std::string_view name) const {
        UNIX_USE(deob, resolve);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.deob_resolve(detail::g_self, Sv(name), b, c);
        });
    }
    /// <summary>Resolve a dotted path, member by member. Unity main thread.</summary>
    template <class = void> [[nodiscard]] std::string ResolvePath(std::string_view path) const {
        UNIX_USE(deob, resolve_path);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.deob_resolve_path(detail::g_self, Sv(path), b, c);
        });
    }
    template <class = void> [[nodiscard]] std::string GetCurrentWorldId() const {
        UNIX_USE(deob, current_world);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.deob_current_world(detail::g_self, b, c);
        });
    }
    template <class = void> [[nodiscard]] std::optional<VrcPlayerOffsets> GetVrcPlayerOffsets() const {
        return ReadOffsets<VrcPlayerOffsets>(OffsetTable::VrcPlayer);
    }
    template <class = void> [[nodiscard]] std::optional<PlayerOffsets> GetPlayerOffsets() const {
        return ReadOffsets<PlayerOffsets>(OffsetTable::Player);
    }
    template <class = void> [[nodiscard]] std::optional<NameplateOffsets> GetNameplateOffsets() const {
        return ReadOffsets<NameplateOffsets>(OffsetTable::Nameplate);
    }
private:
    template <class T> [[nodiscard]] std::optional<T> ReadOffsets(OffsetTable table) const {
        UNIX_USE(deob, offsets);
        T out{};
        out.size = sizeof(T);
        const uint32_t got = detail::g_d.deob_offsets(detail::g_self, static_cast<uint32_t>(table),
                                                      &out, static_cast<uint32_t>(sizeof(T)));
        if (got == 0 || got == unix_absent || got == unix_denied ||
            got > static_cast<uint32_t>(sizeof(T)))
            return std::nullopt;
        return out;
    }
};

/// <summary>The client console: commands, and the shared output fan-out.</summary>
class Console {
public:
    /// <summary>Register a console command. The subscription unregisters it.</summary>
    /// <param name="name">Command word, without a prefix.</param>
    /// <param name="help">One-line help shown by the console's own help command.</param>
    template <class = void> [[nodiscard]] Subscription
    RegisterCommand(std::string_view name, std::string_view help,
                    std::function<void(const CommandEvent&)> onRun) {
        UNIX_USE(console, register_command);
        detail::Node* n = detail::NodeFor<unix_cmd_ev>(std::move(onRun));
        unix_cmd_desc d{};
        d.size = sizeof(d);
        d.name = Sv(name);
        d.help = Sv(help);
        d.on_run = &detail::Tramp<unix_cmd_ev>;
        d.ud = n;
        const unix_handle h = detail::g_d.console_register_command(detail::g_self, &d);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
    template <class = void> Status Write(std::string_view text) {
        UNIX_USE(console, write);
        return detail::g_d.console_write(detail::g_self, Sv(text));
    }
};

/// <summary>One page in the Studio sidebar.</summary>
class StudioPage {
public:
    StudioPage() = default;
    explicit StudioPage(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(StudioPage a, StudioPage b) noexcept { return a.m_handle == b.m_handle; }

    template <class = void> Status Show() {
        UNIX_USE(studio, show_page);
        return detail::g_d.studio_show_page(detail::g_self, m_handle);
    }
    template <class = void> Status Toggle() {
        UNIX_USE(studio, toggle_page);
        return detail::g_d.studio_toggle_page(detail::g_self, m_handle);
    }
    template <class = void> void Remove() {
        if (m_handle == unix_null) return;
        UNIX_USE(studio, unregister_page);
        detail::g_d.studio_unregister_page(detail::g_self, m_handle);
        m_handle = unix_null;
    }
private:
    unix_handle m_handle{};
};

class Studio {
public:
    /// <summary>Register a sidebar page. `draw` renders the body only; `available` gates it.</summary>
    /// <param name="id">Stable page id, unique within this module.</param>
    /// <param name="iconGlyph">UIKit glyph string shown in the sidebar.</param>
    template <class = void> StudioPage RegisterPage(std::string_view id, std::string_view title,
                                                    std::string_view iconGlyph, PageGroup group,
                                                    std::function<void()> draw,
                                                    std::function<bool()> available = {}) {
        UNIX_USE(studio, register_page);
        detail::Node* n = detail::NodeForTask(std::move(draw));
        unix_page_desc d{};
        d.size = sizeof(d);
        d.id = Sv(id);
        d.title = Sv(title);
        d.icon_glyph = Sv(iconGlyph);
        d.group = static_cast<uint32_t>(static_cast<int32_t>(group));
        d.on_draw = &detail::TrampTask;
        d.ud = n;
        if (available) {
            n->ret = [f = std::move(available)](const void*) -> bool { return f(); };
            d.available = &detail::TrampAvail;
        }
        const unix_handle h = detail::g_d.studio_register_page(detail::g_self, &d);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return StudioPage{ h };
    }
    /// <summary>Close the Studio window.</summary>
    template <class = void> Status Hide() {
        UNIX_USE(studio, hide);
        return detail::g_d.studio_hide(detail::g_self);
    }
};

/// <summary>The world scripts bound in the current world. Unity main thread only.</summary>
class WorldScripts {
public:
    template <class = void> [[nodiscard]] uint32_t GetCount() const {
        UNIX_USE(worldscripts, count);
        return detail::g_d.worldscripts_count(detail::g_self);
    }
    /// <summary>Bumped whenever the bound set, its entries or its variables change.</summary>
    template <class = void> [[nodiscard]] uint32_t GetRevision() const {
        UNIX_USE(worldscripts, revision);
        return detail::g_d.worldscripts_revision(detail::g_self);
    }
    template <class = void> [[nodiscard]] std::string GetStatus() const {
        UNIX_USE(worldscripts, status);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.worldscripts_status(detail::g_self, b, c);
        });
    }
    template <class = void> Status Rebind() {
        UNIX_USE(worldscripts, rebind);
        return detail::g_d.worldscripts_rebind(detail::g_self);
    }
    template <class = void> [[nodiscard]] std::optional<WorldScriptInfo> GetInfo(uint32_t index) const {
        UNIX_USE(worldscripts, info);
        WorldScriptInfo i{};
        i.size = sizeof(i);
        if (detail::g_d.worldscripts_info(detail::g_self, index, &i) != unix_ok) return std::nullopt;
        return i;
    }
    template <class = void> [[nodiscard]] std::vector<WorldScriptEntry> GetEntries(uint32_t index) const {
        UNIX_USE(worldscripts, entries);
        return detail::ReadArray<WorldScriptEntry>(
            [&](WorldScriptEntry* b, uint32_t cap, uint32_t stride) {
                return detail::g_d.worldscripts_entries(detail::g_self, index, b, cap, stride);
            });
    }
    template <class = void> [[nodiscard]] std::vector<WorldScriptVar> GetVars(uint32_t index) const {
        UNIX_USE(worldscripts, vars);
        return detail::ReadArray<WorldScriptVar>(
            [&](WorldScriptVar* b, uint32_t cap, uint32_t stride) {
                return detail::g_d.worldscripts_vars(detail::g_self, index, b, cap, stride);
            });
    }
    template <class = void> Status Invoke(uint32_t index, std::string_view entryId) {
        UNIX_USE(worldscripts, invoke);
        return detail::g_d.worldscripts_invoke(detail::g_self, index, Sv(entryId));
    }
    template <class = void> Status SetToggle(uint32_t index, std::string_view entryId, bool state) {
        UNIX_USE(worldscripts, set_toggle);
        return detail::g_d.worldscripts_set_toggle(detail::g_self, index, Sv(entryId), state);
    }
    template <class = void> Status SetVar(uint32_t index, std::string_view varId, float value) {
        UNIX_USE(worldscripts, set_number);
        return detail::g_d.worldscripts_set_number(detail::g_self, index, Sv(varId), value);
    }
    template <class = void> Status SetVar(uint32_t index, std::string_view varId, std::string_view value) {
        UNIX_USE(worldscripts, set_string);
        return detail::g_d.worldscripts_set_string(detail::g_self, index, Sv(varId), Sv(value));
    }
    /// <summary>Fires when GetRevision() moves, so a client need not poll it per frame.</summary>
    template <class = void> [[nodiscard]] Subscription OnChanged(std::function<void()> fn) {
        UNIX_USE(worldscripts, on_changed);
        detail::Node* n = detail::NodeForTask(std::move(fn));
        const unix_handle h = detail::g_d.worldscripts_on_changed(detail::g_self,
                                                                  &detail::TrampTask, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
};

/// <summary>The runtime inspector, and the IL2CPP method hooks it owns.</summary>
class Explorer {
public:
    template <class = void> Status ShowGameObject(void* gameObject) {
        UNIX_USE(explorer, show_gameobject);
        return detail::g_d.explorer_show_gameobject(detail::g_self, gameObject);
    }
    template <class = void> Status ShowComponent(void* component) {
        UNIX_USE(explorer, show_component);
        return detail::g_d.explorer_show_component(detail::g_self, component);
    }
    /// <summary>Fires whenever the inspector navigates to a new object.</summary>
    template <class = void> [[nodiscard]] Subscription
    OnNavigate(std::function<void(const ExplorerEvent&)> fn) {
        UNIX_USE(explorer, on_navigate);
        detail::Node* n = detail::NodeFor<unix_explorer_ev>(std::move(fn));
        const unix_handle h = detail::g_d.explorer_on_navigate(detail::g_self,
                                                               &detail::Tramp<unix_explorer_ev>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
    /// <summary>Hook one IL2CPP method by name. The subscription removes the hook.</summary>
    /// <param name="signature">Full method signature; empty when the name and arity are unique.</param>
    /// <param name="argc">Parameter count, which disambiguates overloads.</param>
    template <class = void> [[nodiscard]] Subscription
    HookMethod(std::string_view className, std::string_view methodName, std::string_view signature,
               uint8_t argc, bool isStatic, std::function<void(const HookEvent&)> onHit,
               uint32_t hookType = 0) {
        UNIX_USE(explorer, hook_method);
        detail::Node* n = detail::NodeFor<unix_hook_ev>(std::move(onHit));
        unix_hook_desc d{};
        d.size = sizeof(d);
        d.class_name = Sv(className);
        d.method_name = Sv(methodName);
        d.signature = Sv(signature);
        d.argc = argc;
        d.is_static = isStatic;
        d.hook_type = hookType;
        d.on_hit = &detail::Tramp<unix_hook_ev>;
        d.ud = n;
        const unix_handle h = detail::g_d.explorer_hook_method(detail::g_self, &d);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
    template <class = void> [[nodiscard]] std::vector<unix_hook_desc> GetHooks() const {
        UNIX_USE(explorer, list_hooks);
        return detail::ReadArray<unix_hook_desc>([&](unix_hook_desc* b, uint32_t cap, uint32_t stride) {
            return detail::g_d.explorer_list_hooks(detail::g_self, b, cap, stride);
        });
    }
    /// <summary>Dump the live IL2CPP runtime to a file under this module's sandbox.</summary>
    template <class = void> Status DumpRuntime(std::string_view path, uint32_t flags = 0) {
        UNIX_USE(explorer, dump_runtime);
        return detail::g_d.explorer_dump_runtime(detail::g_self, Sv(path), flags);
    }
};

/// <summary>What else is loaded. Needs unix_cap_introspect.</summary>
class Modules {
public:
    template <class = void> [[nodiscard]] std::vector<std::string> GetNames() const {
        UNIX_USE(modules, list);
        return detail::ReadList([&](char* b, uint32_t c) {
            return detail::g_d.modules_list(detail::g_self, b, c);
        });
    }
    template <class = void> [[nodiscard]] std::optional<ModuleInfo> GetInfo(std::string_view name) const {
        UNIX_USE(modules, info);
        ModuleInfo i{};
        i.size = sizeof(i);
        if (detail::g_d.modules_info(detail::g_self, Sv(name), &i) != unix_ok) return std::nullopt;
        return i;
    }
    /// <summary>Be told when any module is enabled or disabled. The name is the host's and
    /// lives only for the call.</summary>
    template <class = void> [[nodiscard]] Subscription
    OnModuleToggled(std::function<void(std::string_view name, bool enabled)> fn) {
        UNIX_USE(modules, on_changed);
        if (!fn) return {};
        detail::Node* n = detail::NodeFor<unix_module_ev>([f = std::move(fn)](const unix_module_ev& e) {
            if (e.kind == unix_mod_enabled) f(View(e.name), e.enabled);
        });
        const unix_handle h = detail::g_d.modules_on_changed(detail::g_self, 1u << unix_mod_enabled,
                                                             &detail::Tramp<unix_module_ev>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
    /// <summary>Be told when one of this module's own settings changes. The key is the host's
    /// and lives only for the call.</summary>
    template <class = void> [[nodiscard]] Subscription
    OnSettingChanged(std::function<void(std::string_view key)> fn) {
        UNIX_USE(modules, on_changed);
        if (!fn) return {};
        detail::Node* n = detail::NodeFor<unix_module_ev>([f = std::move(fn)](const unix_module_ev& e) {
            if (e.kind == unix_mod_setting) f(View(e.key));
        });
        const unix_handle h = detail::g_d.modules_on_changed(detail::g_self, 1u << unix_mod_setting,
                                                             &detail::Tramp<unix_module_ev>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
};

/// <summary>The application: VR mode, window focus and display.</summary>
class App {
public:
    /// <summary>VR mode, focus and display in one call. Zeroed when the host cannot say.</summary>
    template <class = void> [[nodiscard]] AppState GetState() const {
        UNIX_USE(core, app_state);
        AppState s{};
        s.size = sizeof(s);
        if (detail::g_d.core_app_state(detail::g_self, &s) != unix_ok) {
            s = AppState{};
            s.size = sizeof(s);
        }
        return s;
    }
    template <class = void> [[nodiscard]] bool IsVr() const { return GetState().in_vr; }
    template <class = void> [[nodiscard]] bool IsFocused() const { return GetState().focused; }
    /// <summary>Be told when the client enters or leaves VR.</summary>
    template <class = void> [[nodiscard]] Subscription OnVrMode(std::function<void(bool vr)> fn) {
        const bool any = static_cast<bool>(fn);
        return Subscribe(unix_app_vr_mode, [f = std::move(fn)](const unix_app_ev& e) { f(e.on); }, any);
    }
    /// <summary>Be told when the game window gains or loses focus.</summary>
    template <class = void> [[nodiscard]] Subscription OnFocus(std::function<void(bool focused)> fn) {
        const bool any = static_cast<bool>(fn);
        return Subscribe(unix_app_focus, [f = std::move(fn)](const unix_app_ev& e) { f(e.on); }, any);
    }
    /// <summary>Be told when the display size or refresh rate changes.</summary>
    template <class = void> [[nodiscard]] Subscription OnDisplay(std::function<void(const AppEvent&)> fn) {
        const bool any = static_cast<bool>(fn);
        return Subscribe(unix_app_display, [f = std::move(fn)](const unix_app_ev& e) { f(e); }, any);
    }
    /// <summary>Switch the client into or out of VR. Experimental: opt in with UNIX0050 in the
    /// manifest. unix_e_busy while a switch is already running.</summary>
    template <class = void> Status RequestVr(bool vr) {
        UNIX_USE(core, request_vr);
        return detail::g_d.core_request_vr(detail::g_self, vr);
    }
private:
    template <class F> [[nodiscard]] Subscription Subscribe(unix_app_kind kind, F&& fn, bool any) {
        UNIX_USE(core, on_app_event);
        if (!any) return {};
        detail::Node* n = detail::NodeFor<unix_app_ev>([kind, g = std::forward<F>(fn)](const unix_app_ev& e) {
            if (e.kind == kind) g(e);
        });
        const unix_handle h = detail::g_d.core_on_app_event(detail::g_self, 1u << kind,
                                                            &detail::Tramp<unix_app_ev>, n);
        if (h == unix_null) { detail::FreeNode(n); return {}; }
        return Subscription{ h, n };
    }
};

class Telemetry {
public:
    /// <summary>Register this module as a usage client so the loader counts it.</summary>
    /// <param name="name">Client name shown in the usage report.</param>
    /// <param name="usageKey">The key the client was issued.</param>
    template <class = void> Status RegisterClient(std::string_view name, std::string_view usageKey) {
        UNIX_USE(usage, register_client);
        return detail::g_d.usage_register_client(detail::g_self, Sv(name), Sv(usageKey));
    }

    /// <summary>Whether a usage key is already registered for this process.</summary>
    template <class = void> [[nodiscard]] bool IsRegistered(std::string_view usageKey) const {
        UNIX_USE(usage, is_registered);
        return detail::g_d.usage_is_registered(detail::g_self, Sv(usageKey));
    }
};

template <class> inline std::string Host::GetName() const {
    UNIX_USE(core, module_name);
    return detail::ReadStr([&](char* b, uint32_t c) {
        return detail::g_d.core_module_name(detail::g_self, b, c);
    });
}

template <class> inline unix_host_kind Host::GetKind() const {
    UNIX_USE(core, module_kind);
    return static_cast<unix_host_kind>(detail::g_d.core_module_kind(detail::g_self));
}

template <class> inline ThreadKind Host::GetThread() const {
    UNIX_USE(core, thread);
    return static_cast<ThreadKind>(detail::g_d.core_thread(detail::g_self));
}

template <class> inline bool Host::Has(std::string_view symbol) const {
    if (!detail::g_ctx || !detail::g_ctx->bind) return false;
    // Namespace- and function-scope statics, never automatics: the VM path range-checks both.
    static thread_local unix_bind_req req{};
    detail::g_has_out = nullptr;
    req.id = 0;
    req.name_id = abi::fnv_n(symbol.data(), symbol.size());
    req.flags = unix_req_by_name | unix_req_silent;
    req._pad = 0;
    req.out = &detail::g_has_out;
    detail::g_ctx->bind(detail::g_self, &req, 1u,
                        static_cast<uint32_t>(sizeof(unix_bind_req)), nullptr, nullptr);
    return detail::g_has_out != nullptr;
}

template <class> inline void Host::RunOnMain(std::function<void()> fn) {
    UNIX_USE(core, run_on_main);
    detail::Node* n = detail::NodeForTask(std::move(fn));
    if (detail::g_d.core_run_on_main(detail::g_self, &detail::TrampTask, n) != unix_ok)
        detail::FreeNode(n);
}

template <class> inline bool Host::IsLive() const noexcept {
    UNIX_USE(core, live);
    return detail::g_d.core_live(detail::g_self);
}

template <class T> inline const T* Host::Require(std::string_view name,
                                                 uint16_t major, uint16_t minor) {
    UNIX_USE(core, require);
    return static_cast<const T*>(detail::g_d.core_require(detail::g_self, Sv(name), major, minor));
}

template <class> inline Status Host::Provide(std::string_view name, uint16_t major, uint16_t minor,
                                             const void* svc, uint32_t svc_size) {
    UNIX_USE(core, provide);
    return detail::g_d.core_provide(detail::g_self, Sv(name), major, minor, svc, svc_size);
}

}   // namespace UNIx
