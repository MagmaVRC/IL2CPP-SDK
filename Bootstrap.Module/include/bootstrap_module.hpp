#pragma once

#include <Bootstrap.Common/include/bootstrap_shared.hpp>
#include <IL2CPP.Module/include/il2cpp_module.hpp>
#include <string_view>
#include <string>
#include <optional>
#include <tuple>
#include <vector>

#include "VRChat/VRChat.hpp"

namespace Bootstrap::Module {

    /// Low-level connection management. Do not mix with BootstrapModule RAII.
    [[nodiscard]] bool Connect();
    /// Low-level disconnection. Do not mix with BootstrapModule RAII.
    void Disconnect();
    [[nodiscard]] bool is_connected() noexcept;

    class BootstrapModule {
    public:
        BootstrapModule() = default;
        explicit BootstrapModule(std::string_view name);
        ~BootstrapModule();

        BootstrapModule(BootstrapModule const&) = delete;
        BootstrapModule& operator=(BootstrapModule const&) = delete;
        BootstrapModule(BootstrapModule&& other) noexcept;
        BootstrapModule& operator=(BootstrapModule&& other) noexcept;

        [[nodiscard]] bool valid() const noexcept;
        [[nodiscard]] uint32_t id() const noexcept;
        [[nodiscard]] std::string_view name() const noexcept;

    private:
        uint32_t m_id = invalid_id;
        std::string m_name;

        friend class UnityConsole;
        friend class UnityExplorer;
    };

    class UnityConsole {
    public:
        static UnityConsole& Get();

        void set_enable_colors(bool enable);
        void set_file_logging(bool enable, std::wstring_view path = L"");
        void set_enable_trace(bool enable);
        void set_enable_info_trace(bool enable);
        void set_deobfuscate_names(bool enable);

        void get_options(Bootstrap::UnityConsoleOptions* out);
        void set_options(Bootstrap::UnityConsoleOptions const* opts);

        uint32_t register_callback(uint32_t module_id, Bootstrap::fn_log_callback callback);
        void unregister_callback(uint32_t module_id, uint32_t callback_id);

        [[nodiscard]] bool valid() const noexcept;

    private:
        UnityConsole() = default;
    };

    class UnityExplorer {
    public:
        static UnityExplorer& Get();

        void show_game_object(void* gameObject);
        void show_component(void* component);

        uint32_t register_on_navigate(uint32_t module_id, Bootstrap::fn_explorer_navigate_callback callback);
        void unregister_on_navigate(uint32_t module_id, uint32_t callback_id);

        [[nodiscard]] bool valid() const noexcept;

    private:
        UnityExplorer() = default;
    };

    class Rendering {
    public:
        static Rendering& Get();

        uint32_t register_callback(uint32_t module_id, Bootstrap::fn_render_callback callback);
        void unregister_callback(uint32_t module_id, uint32_t callback_id);

        void* get_imgui_context();

        [[nodiscard]] bool valid() const noexcept;

    private:
        Rendering() = default;
    };

    class ModuleConfig {
    public:
        explicit ModuleConfig(uint32_t module_id);

        void set_int(std::string_view key, int32_t value);
        int32_t get_int(std::string_view key, int32_t default_val = 0);
        void set_float(std::string_view key, float value);
        float get_float(std::string_view key, float default_val = 0.f);
        void set_string(std::string_view key, std::string_view value);
        std::string get_string(std::string_view key, std::string_view default_val = "");
        void set_bool(std::string_view key, bool value);
        bool get_bool(std::string_view key, bool default_val = false);
        void save();
        bool has_key(std::string_view key);
        void remove_key(std::string_view key);

        void set_vec2(std::string_view key, float x, float y);
        std::pair<float, float> get_vec2(std::string_view key, float def_x = 0, float def_y = 0);
        void set_vec3(std::string_view key, float x, float y, float z);
        std::tuple<float, float, float> get_vec3(std::string_view key, float def_x = 0, float def_y = 0, float def_z = 0);
        void set_vec4(std::string_view key, float x, float y, float z, float w);
        std::tuple<float, float, float, float> get_vec4(std::string_view key, float def_x = 0, float def_y = 0, float def_z = 0, float def_w = 0);
        void set_color(std::string_view key, float r, float g, float b, float a);
        std::tuple<float, float, float, float> get_color(std::string_view key, float def_r = 0, float def_g = 0, float def_b = 0, float def_a = 1);
        void set_json(std::string_view key, std::string_view json);
        std::string get_json(std::string_view key);
        std::vector<std::string> get_keys();
        void clear();

        void set_double(std::string_view key, double value);
        double get_double(std::string_view key, double default_val = 0.0);
        void set_int64(std::string_view key, int64_t value);
        int64_t get_int64(std::string_view key, int64_t default_val = 0);
        void set_uint32(std::string_view key, uint32_t value);
        uint32_t get_uint32(std::string_view key, uint32_t default_val = 0);

        void set_int_array(std::string_view key, std::vector<int32_t> const& values);
        std::vector<int32_t> get_int_array(std::string_view key);
        void set_float_array(std::string_view key, std::vector<float> const& values);
        std::vector<float> get_float_array(std::string_view key);
        void set_string_array(std::string_view key, std::vector<std::string> const& values);
        std::vector<std::string> get_string_array(std::string_view key);
        void set_bool_array(std::string_view key, std::vector<bool> const& values);
        std::vector<bool> get_bool_array(std::string_view key);
        void set_double_array(std::string_view key, std::vector<double> const& values);
        std::vector<double> get_double_array(std::string_view key);

        void set_string_map(std::string_view key, std::vector<std::pair<std::string, std::string>> const& entries);
        std::vector<std::pair<std::string, std::string>> get_string_map(std::string_view key);

    private:
        uint32_t m_module_id;
    };

    class MessageBus {
    public:
        static MessageBus& Get();

        uint32_t subscribe(uint32_t module_id, std::string_view topic, Bootstrap::fn_message_callback cb);
        void unsubscribe(uint32_t module_id, uint32_t subscription_id);
        void publish(uint32_t module_id, std::string_view topic, const void* data, uint32_t data_len);
        void publish(uint32_t module_id, std::string_view topic, std::string_view message);

    private:
        MessageBus() = default;
    };

    class QuickMenu {
    public:
        static QuickMenu& Get();

        [[nodiscard]] bool valid() const noexcept;
        [[nodiscard]] bool is_ready();

        uint32_t create_page(uint32_t module_id, std::string_view name, std::string_view tooltip = "");
        uint32_t create_sub_page(uint32_t module_id, uint32_t parent_page_id, std::string_view name);
        void remove_page(uint32_t module_id, uint32_t page_id);

        uint32_t add_button(uint32_t module_id, uint32_t page_id, std::string_view text,
                            Bootstrap::fn_menu_button_callback callback);
        void remove_button(uint32_t module_id, uint32_t button_id);
        void set_button_text(uint32_t module_id, uint32_t button_id, std::string_view text);

        uint32_t add_toggle(uint32_t module_id, uint32_t page_id, std::string_view text,
                            bool default_state, Bootstrap::fn_menu_toggle_callback callback,
                            std::string_view config_key = "");
        void set_toggle_state(uint32_t module_id, uint32_t button_id, bool state);
        bool get_toggle_state(uint32_t module_id, uint32_t button_id);

        void set_button_icon(uint32_t module_id, uint32_t button_id, void* sprite_ptr);
        void set_button_icon(uint32_t module_id, uint32_t button_id, int32_t sprite_id);
        void set_button_color(uint32_t module_id, uint32_t button_id, float r, float g, float b, float a);

        void set_page_title(uint32_t module_id, uint32_t page_id, std::string_view title);
        void set_page_icon(uint32_t module_id, uint32_t page_id, void* sprite_ptr);
        void set_page_icon(uint32_t module_id, uint32_t page_id, int32_t sprite_id);
        void set_page_badge(uint32_t module_id, uint32_t page_id, bool visible,
                            std::string_view text = "", float r = 0, float g = 0, float b = 0, float a = 0);
        void set_page_badge(uint32_t module_id, uint32_t page_id, bool visible, std::string_view text,
                            std::nullptr_t no_color);

        void set_button_enabled(uint32_t module_id, uint32_t button_id, bool enabled);
        void set_button_visible(uint32_t module_id, uint32_t button_id, bool visible);
        uint32_t get_subpage_nav_button(uint32_t module_id, uint32_t sub_page_id);
        void set_subpage_nav_text(uint32_t module_id, uint32_t sub_page_id, std::string_view text);
        void set_subpage_nav_icon(uint32_t module_id, uint32_t sub_page_id, int32_t sprite_id);
        void set_image_sprite(void* image_component, int32_t sprite_id);

        uint32_t add_foldout(uint32_t module_id, uint32_t page_id, std::string_view title,
                             bool default_expanded = true, bool show_background = false,
                             bool auto_separators = false);
        void set_foldout_expanded(uint32_t module_id, uint32_t foldout_id, bool expanded);
        bool get_foldout_expanded(uint32_t module_id, uint32_t foldout_id);

        uint32_t add_settings_toggle(uint32_t module_id, uint32_t foldout_id, std::string_view text,
                                     bool default_state, Bootstrap::fn_menu_toggle_callback callback,
                                     std::string_view config_key = "", bool show_sub_element_indicator = false);
        uint32_t add_enum_selector(uint32_t module_id, uint32_t foldout_id, std::string_view label,
                                   const char* const* options, uint32_t option_count,
                                   int32_t default_index, Bootstrap::fn_menu_enum_callback callback,
                                   std::string_view config_key = "", bool show_sub_element_indicator = false);
        void set_enum_index(uint32_t module_id, uint32_t selector_id, int32_t index);
        int32_t get_enum_index(uint32_t module_id, uint32_t selector_id);

        uint32_t add_slider(uint32_t module_id, uint32_t foldout_id, std::string_view label,
                            float min_val, float max_val, float default_val,
                            Bootstrap::fn_menu_slider_callback callback,
                            std::string_view config_key = "", std::string_view format_str = "",
                            bool show_sub_element_indicator = false, float power = 1.0f);
        void set_slider_value(uint32_t module_id, uint32_t slider_id, float value);
        float get_slider_value(uint32_t module_id, uint32_t slider_id);

        uint32_t add_separator(uint32_t module_id, uint32_t foldout_id);

        void navigate_to(uint32_t page_id);
        void navigate_back();

    private:
        QuickMenu() = default;
    };

    class PlayerEvents {
    public:
        static PlayerEvents& Get();

        uint32_t register_event(uint32_t module_id, Bootstrap::PlayerEvent event,
                                Bootstrap::fn_player_simple_callback callback);
        void unregister_event(uint32_t module_id, uint32_t callback_id);
        void invoke(void* player, Bootstrap::PlayerEvent event);

        void* get_local_player();
        void* get_local_player_api();
        void* get_local_vrc_player();

        Bootstrap::PlayerRank get_player_rank(void* player);
        Bootstrap::Color get_rank_color(Bootstrap::PlayerRank rank);

    private:
        PlayerEvents() = default;
    };

    class TweenService {
    public:
        static TweenService& Get();

        uint32_t anchored_position(uint32_t module_id, void* rect_transform,
                                   float from_x, float from_y, float to_x, float to_y,
                                   float duration_ms, int32_t ease_type = 0);
        uint32_t local_position(uint32_t module_id, void* transform,
                                float from_x, float from_y, float from_z,
                                float to_x, float to_y, float to_z,
                                float duration_ms, int32_t ease_type = 0);
        uint32_t local_scale(uint32_t module_id, void* transform,
                             float from_x, float from_y, float from_z,
                             float to_x, float to_y, float to_z,
                             float duration_ms, int32_t ease_type = 0);
        void cancel(uint32_t tween_id);
        void cancel_all(uint32_t module_id);

        uint32_t tween_float(uint32_t module_id, float from, float to,
                             float duration_ms, int32_t ease_type = 0,
                             fn_tween_completion_callback on_complete = nullptr);
        void cancel_all_for_target(void* target_ptr);

        uint32_t anchored_position_ex(uint32_t module_id, void* rect_transform,
                                       float from_x, float from_y, float to_x, float to_y,
                                       float duration_ms, int32_t ease_type = 0,
                                       fn_tween_completion_callback on_complete = nullptr);
        uint32_t local_position_ex(uint32_t module_id, void* transform,
                                    float from_x, float from_y, float from_z,
                                    float to_x, float to_y, float to_z,
                                    float duration_ms, int32_t ease_type = 0,
                                    fn_tween_completion_callback on_complete = nullptr);
        uint32_t local_scale_ex(uint32_t module_id, void* transform,
                                 float from_x, float from_y, float from_z,
                                 float to_x, float to_y, float to_z,
                                 float duration_ms, int32_t ease_type = 0,
                                 fn_tween_completion_callback on_complete = nullptr);

    private:
        TweenService() = default;
    };

    class NameplateService {
    public:
        static NameplateService& Get();

        std::string create_plate(uint32_t module_id, void* player,
                                 float pos_x, float pos_y, float pos_z,
                                 std::string_view label, std::string_view tag = "");
        void destroy_plate(uint32_t module_id, void* player, std::string_view plate_id);
        void destroy_plates_by_tag(uint32_t module_id, void* player, std::string_view tag_prefix);
        void set_plate_text(uint32_t module_id, void* player, std::string_view plate_id, std::string_view text);
        void set_plate_text_color(uint32_t module_id, void* player, std::string_view plate_id,
                                  float r, float g, float b, float a);
        void set_plate_icon_color(uint32_t module_id, void* player, std::string_view plate_id,
                                  float r, float g, float b, float a);
        void set_plate_position(uint32_t module_id, void* player, std::string_view plate_id,
                                float x, float y, float z);

    private:
        NameplateService() = default;
    };

    class ClientUsage {
    public:
        static ClientUsage& Get();

        void register_client(uint32_t module_id, std::string_view client_name, std::string_view api_key);
        bool is_client_registered(std::string_view client_name);

    private:
        ClientUsage() = default;
    };

    class Performance {
    public:
        static Performance& Get();

        int32_t get_int(int32_t setting_id);
        void set_int(int32_t setting_id, int32_t value);
        float get_float(int32_t setting_id);
        void set_float(int32_t setting_id, float value);

        void force_gc(int32_t generation = 2);
        void full_cleanup();

    private:
        Performance() = default;
    };

    class KeyAuth {
    public:
        static KeyAuth& Get();

        [[nodiscard]] bool is_ready();
        bool get_user(Bootstrap::KeyAuthUserInfo* out);
        bool has_product(std::string_view product_id, Bootstrap::KeyAuthProductResult* out);
        bool redeem_license(std::string_view key, Bootstrap::KeyAuthRedeemResult* out);
        uint32_t get_licenses(Bootstrap::KeyAuthProductResult* out_buf, uint32_t buf_count);

        std::optional<Bootstrap::KeyAuthUserInfo> get_user();
        std::optional<Bootstrap::KeyAuthProductResult> check_product(std::string_view product_id);

    private:
        KeyAuth() = default;
    };

    class WebSocket {
    public:
        static WebSocket& Get();

        uint32_t connect(uint32_t module_id, std::string_view url, std::string_view protocols = "");
        bool send(uint32_t module_id, uint32_t handle, const void* data, uint32_t len, bool binary = false);
        bool send(uint32_t module_id, uint32_t handle, std::string_view text);
        void close(uint32_t module_id, uint32_t handle, uint16_t code = 1000, std::string_view reason = "");
        bool is_connected(uint32_t handle);
        void set_callbacks(uint32_t module_id, uint32_t handle,
            fn_ws_open_callback on_open, fn_ws_message_callback on_message,
            fn_ws_close_callback on_close, fn_ws_error_callback on_error);

    private:
        WebSocket() = default;
    };

    class FileSystem {
    public:
        static FileSystem& Get();

        bool write_file(uint32_t module_id, std::string_view path, const void* data, uint32_t len);
        bool write_file(uint32_t module_id, std::string_view path, std::string_view data);
        std::string read_file(uint32_t module_id, std::string_view path);
        bool file_exists(uint32_t module_id, std::string_view path);
        bool delete_file(uint32_t module_id, std::string_view path);
        bool create_directory(uint32_t module_id, std::string_view path);
        bool delete_directory(uint32_t module_id, std::string_view path);
        std::vector<std::string> list_directory(uint32_t module_id, std::string_view path);
        uint64_t file_size(uint32_t module_id, std::string_view path);
        bool append_file(uint32_t module_id, std::string_view path, const void* data, uint32_t len);
        bool append_file(uint32_t module_id, std::string_view path, std::string_view data);

    private:
        FileSystem() = default;
    };

    class Clipboard {
    public:
        static Clipboard& Get();

        bool set(std::string_view text);
        std::string get();

    private:
        Clipboard() = default;
    };

} // namespace Bootstrap::Module
