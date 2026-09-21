#pragma once
#include <stdint.h>
#include <stddef.h>       // size_t: unix_imgui_alloc, abi::fnv_n
#include <concepts>       // std::convertible_to: the UnixTagged concept
#include <type_traits>

#define UNIX_ABI          1u
#define UNIX_VER(ma, mi, pa) ((uint32_t)((ma) & 0xFFu) << 16 | \
                              (uint32_t)((mi) & 0xFFu) <<  8 | (uint32_t)((pa) & 0xFFu))
#define UNIX_SDK_VERSION  UNIX_VER(7, 4, 0)
#define UNIX_CC           __cdecl
#define UNIX_BIND_REQ_V1  32u     // the FROZEN 7.0 stride floor, never sizeof (§3.2)

// Two levels, or __COUNTER__ never expands. Both sides need them: UNIX_BIND_EXPORT in the
// host and UNIX_REQUIRE / UNIX_SUPPRESS in a module TU that includes nothing of Bootstrap's.
#define UNIX_CAT2(a, b) a##b
#define UNIX_CAT(a, b)  UNIX_CAT2(a, b)

extern "C" {

typedef uint64_t unix_self;     // (index << 32) | tag — the hidden ABI argument 0, never authored
typedef uint32_t unix_handle;   // opaque; host-minted, monotonic per service, never reused (§3.8)

typedef enum : uint32_t {
    unix_absent = 0xFFFFFFFFu, unix_denied = 0xFFFFFFFEu,   // >= 0xFFFFFF00 is reserved
} unix_size_sentinel;
enum : uint32_t { unix_null = 0 };   // the null handle; never issued

typedef enum : int32_t {
    unix_ok = 0, unix_e_absent = -1, unix_e_denied = -2, unix_e_arg = -3,
    unix_e_state = -4, unix_e_thread = -5, unix_e_removed = -6, unix_e_busy = -7,
} unix_status;

// Value aggregates: kAbiTag, no `size`, frozen forever. Only the TAG is folded into a symbol
// id — never sizeof, or a struct could not grow (§3.3, §3.11). These never grow regardless.
struct unix_str  { const char* data; uint32_t size;
                   static constexpr uint32_t kAbiTag = 0x73747200u; };
struct unix_vec2 { float x, y;       static constexpr uint32_t kAbiTag = 0x76326600u; };
struct unix_vec3 { float x, y, z;    static constexpr uint32_t kAbiTag = 0x76336600u; };
struct unix_vec4 { float x, y, z, w; static constexpr uint32_t kAbiTag = 0x76346600u; };
struct unix_color{ float r, g, b, a; static constexpr uint32_t kAbiTag = 0x636f6c00u; };
struct unix_rect { float x, y, w, h; static constexpr uint32_t kAbiTag = 0x72637400u; };

typedef enum : uint32_t {
    unix_icon_none = 0,        // clear the icon
    unix_icon_bundle,          // a `Sprite` enum value, in vrc_id
    unix_icon_sprite_ptr,      // a live sprite object, in sprite
    unix_icon_vrc_id           // a raw VRChat sprite id, in vrc_id
} unix_icon_kind;

struct unix_icon { unix_icon_kind kind; void* sprite; int32_t vrc_id;
                   static constexpr uint32_t kAbiTag = 0x69636f00u; };

typedef enum : uint64_t {
    unix_cap_none = 0,
    unix_cap_config = 1ull<<0,  unix_cap_fs = 1ull<<1,        unix_cap_menu = 1ull<<2,
    unix_cap_players = 1ull<<3, unix_cap_hooks = 1ull<<4,
    unix_cap_memory = 1ull<<5,  // reserved at 7.0: declarable, but no row carries it (§6.7)
    unix_cap_net = 1ull<<6,     // reserved at 7.0 when every net.* row was a tombstone; live at
                                // 7.1, where it gates VRC.SDKBase.Networking (§6.7)
    unix_cap_clipboard = 1ull<<7, unix_cap_license = 1ull<<8,
    unix_cap_perf = 1ull<<9,    unix_cap_worldscripts = 1ull<<10,
    unix_cap_photon_veto = 1ull<<11, unix_cap_player_inject = 1ull<<12,
    unix_cap_telemetry = 1ull<<13,   unix_cap_studio = 1ull<<14, unix_cap_explorer = 1ull<<15,
    unix_cap_bus = 1ull<<16,         unix_cap_hotkey = 1ull<<17, unix_cap_introspect = 1ull<<18,
    unix_cap_resources = 1ull<<19,   unix_cap_host_settings = 1ull<<20,
    // Reading and driving world scripts: entry points, heap variables, local and networked
    // events, and the RPC path. Separate from unix_cap_worldscripts, which is the host's own
    // curated world-script bindings, not raw Udon.
    unix_cap_udon = 1ull<<21,
    // Drawing on VRChat's own HUD notification widgets. Separate from unix_cap_menu, which
    // is the module's own menu surface rather than the client's notification stack.
    unix_cap_notify = 1ull<<22,
} unix_cap;                                        // 23 of 64 spent at 7.4

// Every bit this ABI version defines. Widened with each new bit; §7.2 rung 3 masks the
// grant with it, because a host holding only the enumerators cannot compute the complement.
enum : uint64_t { unix_cap_all = (1ull << 23) - 1 };

typedef enum : uint32_t { unix_host_native = 0, unix_host_vm, unix_host_static } unix_host_kind;

typedef enum : uint32_t {
    unix_thread_unknown = 0, unix_thread_unity_main, unix_thread_render,
    unix_thread_photon, unix_thread_loader
} unix_thread;                                     // what core.thread returns (§6.2)

typedef enum : uint32_t {
    unix_phase_pre_il2cpp = 0, unix_phase_post_il2cpp, unix_phase_menu_ready
} unix_module_phase;

typedef enum : uint32_t {
    unix_menu_qm_pre_setup = 0, unix_menu_qm_setup, unix_menu_qm_post_setup,
    unix_menu_pre_ready, unix_menu_ready, unix_menu_late_ready,
    unix_menu_il2cpp_ready, unix_menu_world_loaded, unix_menu_players_ready,
    unix_menu_renderer_ready, unix_menu_mm_setup
} unix_menu_phase;

typedef enum : uint32_t {
    unix_player_awake = 0, unix_player_joined, unix_player_joined_complete,
    unix_player_left, unix_player_update
} unix_player_phase;

typedef enum : uint32_t {
    unix_h_page = 1, unix_h_ctl, unix_h_plate, unix_h_tween, unix_h_sub,
    unix_h_hotkey, unix_h_studio_page, unix_h_command, unix_h_hook,
    unix_h_am_page, unix_h_am_root
} unix_handle_kind;   // NOT encoded in the handle: the host keeps {handle -> owner, kind} (§3.8)

// Append-only, like note kinds. A module treats an unrecognised kind as a change.
typedef enum : uint32_t { unix_wk_change = 0, unix_wk_restore = 1 } unix_widget_kind;

typedef enum : uint32_t {
    unix_note_deprecated = 1, unix_note_experimental, unix_note_removed,
    unix_note_unknown, unix_note_host_too_old, unix_note_signature_mismatch,
    unix_note_denied, unix_note_undeclared_cap, unix_note_unused_cap,
    unix_note_unknown_cap, unix_note_slot_budget,
    unix_note_unavailable_summary,   // N rows unbound; `have` is the count (§4.3)
    unix_note_bad_request,           // a structural refusal inside bind (§5.3)
} unix_note_kind;

struct unix_bind_note {
    uint32_t       size;
    unix_note_kind kind;
    unix_str       symbol, replacement, message, code;
    uint32_t       since, removal;  // removal, or the version it was removed in
    uint32_t       have, want;      // host_sdk and the module's sdk_built
    uint32_t       req_index;       // the request this answers; 0xFFFFFFFF = module-level
    static constexpr uint32_t kAbiTag = 0x6e6f7400u;
};

typedef void (UNIX_CC* unix_note_sink)(void* ud, const unix_bind_note*);

typedef enum : uint32_t {
    unix_req_optional = 0,     unix_req_required = 1u<<0,
    unix_req_silent   = 1u<<1, unix_req_by_name  = 1u<<2,
    unix_req_report   = 1u<<3,
} unix_bind_flag;

struct unix_bind_req {
    uint64_t id;        // name mixed with the signature tag
    uint64_t name_id;   // fnv1a64(name) alone; no plaintext name crosses (§3.2, R14)
    uint32_t flags; uint32_t _pad; void** out;
    static constexpr uint32_t kAbiTag = 0x72657100u;
};
static_assert(sizeof(unix_bind_req) >= UNIX_BIND_REQ_V1,
    "unix_bind_req may only grow by appending; UNIX_BIND_REQ_V1 is the frozen 7.0 floor");

/// <summary>Resolve a batch of symbols for one module. The only pointer that crosses.</summary>
/// <param name="stride">sizeof(unix_bind_req) as the caller built it; below UNIX_BIND_REQ_V1
/// is refused, at or above it is accepted and read to min(stride, sizeof own).</param>
/// <returns>Count successfully bound; unbound requests leave *out untouched.</returns>
typedef uint32_t (UNIX_CC* unix_fn_bind)(unix_self, unix_bind_req*, uint32_t count,
                                         uint32_t stride, unix_note_sink, void* notes_ud);

struct unix_context {
    uint32_t       size, abi, host_sdk;
    unix_host_kind kind;
    unix_self      self;
    unix_fn_bind   bind;
    const void    *hooking, *logging, *il2cpp, *unity;
};

/* ---- event payloads ---- */

// Every aggregate below leads with `size` and carries kAbiTag. `tcode` static_asserts on a
// public aggregate without one, so adding a struct here without a tag names the type (§3.3).

// The allocator pair is typed, not void*: a function pointer does not implicitly convert to
// void* in C++.
typedef void* (UNIX_CC* unix_imgui_alloc)(size_t sz, void* ud);
typedef void  (UNIX_CC* unix_imgui_free) (void* p,   void* ud);

struct unix_frame {
    uint32_t size, width, height; uint64_t frame_index;
    void *device, *context, *swapchain, *hwnd, *imgui, *rtv;
    unix_imgui_alloc imgui_alloc; unix_imgui_free imgui_free; void* imgui_alloc_ud;
    const char* imgui_version;
    float dt;
    static constexpr uint32_t kAbiTag = 0x66726d00u;
};
struct unix_player_ev  { uint32_t size; unix_player_phase phase; void *player, *api_user;
                         static constexpr uint32_t kAbiTag = 0x706c7900u; };
struct unix_scene_ev   { uint32_t size; int32_t handle, mode; uint32_t loaded;
                         static constexpr uint32_t kAbiTag = 0x73636e00u; };
typedef enum : uint32_t { unix_world_entered = 0, unix_world_left } unix_world_phase;
// `location` is the whole instance id -- `wrld_...:12345~region(eu)`, or `local:...` for a
// local build -- so a module can tell a new instance of the same world from a re-entry, and
// see the transitions `id` alone cannot show. Appended and size-guarded, so kAbiTag does not
// move (§6.3). `id` stays the bare world id and is empty outside a published world.
struct unix_world_ev   { uint32_t size; unix_world_phase phase; int32_t handle; unix_str id, name;
                         unix_str location;
                         static constexpr uint32_t kAbiTag = 0x776c6400u; };
// The Photon payload is a managed object, not a byte buffer: consumers call GetCode /
// GetSender / GetCustomData / GetParameter / SetSender on IL2CPP::VRChat::EventData.
// `custom_data` is GetCustomData() hoisted so a filter need not cross into il2cpp.
struct unix_photon_ev  { uint32_t size; uint8_t code; int32_t sender;
                         void *event_data, *custom_data;
                         static constexpr uint32_t kAbiTag = 0x70686f00u; };
struct unix_widget_ev  { uint32_t size; unix_handle ctl; unix_widget_kind kind;
                         bool b; float f; int32_t i; unix_str s;
                         static constexpr uint32_t kAbiTag = 0x77676500u; };
struct unix_message    { uint32_t size; unix_str sender, topic; const uint8_t* data; uint32_t len;
                         static constexpr uint32_t kAbiTag = 0x6d736700u; };
struct unix_log_ev     { uint32_t size; unix_str msg, stack; uint32_t level;
                         static constexpr uint32_t kAbiTag = 0x6c6f6700u; };
struct unix_explorer_ev{ uint32_t size; void *gameobject, *component;
                         static constexpr uint32_t kAbiTag = 0x78706c00u; };
struct unix_user_ev    { uint32_t size; void *player, *api_user;
                         unix_str user_id, display_name; uint32_t kind;
                         static constexpr uint32_t kAbiTag = 0x75737200u; };
struct unix_tween_ev   { uint32_t size; unix_handle tween; uint32_t completed;
                         static constexpr uint32_t kAbiTag = 0x74776e00u; };
struct unix_hotkey_ev  { uint32_t size; unix_handle binding; uint32_t combo;  // 0 == cancelled
                         static constexpr uint32_t kAbiTag = 0x686b7900u; };
struct unix_cmd_ev     { uint32_t size; unix_str line; const unix_str* argv; uint32_t argc;
                         static constexpr uint32_t kAbiTag = 0x636d6400u; };
struct unix_hook_ev    { uint32_t size; void* method; void** args; uint32_t argc;
                         static constexpr uint32_t kAbiTag = 0x686b6f00u; };
struct unix_input_ev   { uint32_t size; void* hwnd; uint32_t msg; uint64_t wparam; int64_t lparam;
                         static constexpr uint32_t kAbiTag = 0x696e7000u; };
// Page lifecycle, delivered to a UNIx::UIPage subclass (§6.4). `dt` is meaningful for Tick only.
typedef enum : uint32_t {
    unix_page_built = 0, unix_page_shown, unix_page_hidden, unix_page_tick, unix_page_scene_loaded
} unix_page_event;
struct unix_page_ev    { uint32_t size; unix_handle page; unix_page_event kind; float dt;
                         static constexpr uint32_t kAbiTag = 0x70657600u; };

/* ---- callback shapes: one per payload, never reused across payloads ---- */

typedef void (UNIX_CC* unix_cb_task)    (void* ud);
typedef void (UNIX_CC* unix_cb_render)  (void* ud, const unix_frame*);
typedef void (UNIX_CC* unix_cb_player)  (void* ud, const unix_player_ev*);
typedef void (UNIX_CC* unix_cb_widget)  (void* ud, const unix_widget_ev*);
typedef void (UNIX_CC* unix_cb_log)     (void* ud, const unix_log_ev*);
typedef void (UNIX_CC* unix_cb_explorer)(void* ud, const unix_explorer_ev*);
typedef void (UNIX_CC* unix_cb_message) (void* ud, const unix_message*);
typedef void (UNIX_CC* unix_cb_menu)    (void* ud, unix_menu_phase);
typedef void (UNIX_CC* unix_cb_user)    (void* ud, const unix_user_ev*);
typedef void (UNIX_CC* unix_cb_tween)   (void* ud, const unix_tween_ev*);
typedef void (UNIX_CC* unix_cb_hotkey)  (void* ud, const unix_hotkey_ev*);
typedef void (UNIX_CC* unix_cb_command) (void* ud, const unix_cmd_ev*);
typedef void (UNIX_CC* unix_cb_hook)    (void* ud, const unix_hook_ev*);
typedef void (UNIX_CC* unix_cb_page)    (void* ud, const unix_page_ev*);
typedef bool (UNIX_CC* unix_cb_photon)  (void* ud, const unix_photon_ev*);
typedef bool (UNIX_CC* unix_cb_avail)   (void* ud);   // Studio PageDesc::available
typedef float(UNIX_CC* unix_cb_radial)  (void* ud);
typedef void (UNIX_CC* unix_cb_radial_set)(void* ud, float v);

/* ---- descriptor structs: every one leads with `size` (§3.11) ---- */

struct unix_button_desc { uint32_t size; unix_str text, tooltip; unix_icon icon; unix_color color;
                          unix_cb_widget on_click; void* ud;
                          static constexpr uint32_t kAbiTag = 0x62746e00u; };
// `bound` is the raw module member the host re-reads at 4 Hz (§6.3). Null for the plain forms.
// `module_owns_write`: ToggleDesc::Apply owns the transition and the write to *bound, so the
// host fires on_change and does NOT write *bound; the poll restores the control if Apply
// refused. Appended and size-guarded, so kAbiTag does not move (§6.3).
struct unix_toggle_desc { uint32_t size; unix_str label, tooltip, config_key;
                          bool def, sub_indicator; unix_cb_widget on_change; void* ud;
                          bool* bound;
                          bool module_owns_write;
                          static constexpr uint32_t kAbiTag = 0x74676c00u; };
struct unix_slider_desc { uint32_t size; unix_str label, tooltip, format, config_key;
                          float value, min, max, power, display_scale; bool sub_indicator;
                          unix_cb_widget on_change; void* ud;
                          float* bound;
                          static constexpr uint32_t kAbiTag = 0x736c6400u; };
struct unix_enum_desc   { uint32_t size; unix_str label, config_key;
                          const unix_str* options; uint32_t count; int32_t def;
                          bool sub_indicator; unix_cb_widget on_change; void* ud;
                          int32_t* bound;
                          static constexpr uint32_t kAbiTag = 0x656e6d00u; };
struct unix_foldout_desc{ uint32_t size; unix_str title; bool expanded, background, auto_separators;
                          static constexpr uint32_t kAbiTag = 0x666c6400u; };
// A text field row, cloned from VRChat's own input field. There is no managed listener to
// attach from a module, so the host polls the field and reports a settled value through
// `on_change` with the text in unix_widget_ev::s. That string is the host's and is valid only
// for the duration of the call.
struct unix_text_desc   { uint32_t size; unix_str label, tooltip, value, config_key;
                          unix_cb_widget on_change; void* ud;
                          static constexpr uint32_t kAbiTag = 0x74786400u; };
// Studio's PageDesc: the icon is a UIKit glyph string, the body draws outside any frame the
// host owns, and `available` gates the page in the sidebar.
struct unix_page_desc   { uint32_t size; unix_str id, title, icon_glyph; uint32_t group;
                          unix_cb_task on_draw; unix_cb_avail available; void* ud;
                          static constexpr uint32_t kAbiTag = 0x70676500u; };
struct unix_cmd_desc    { uint32_t size; unix_str name, help; unix_cb_command on_run; void* ud;
                          static constexpr uint32_t kAbiTag = 0x63736400u; };
// One item on the Action Menu wheel. The wheel rebuilds a page from its build callback on
// every open, so a descriptor is submitted per build rather than retained.
// `state` makes the item a toggle: the menu polls it for the tick, there is no on/off to set.
// `page` makes it open another Action Menu page instead of firing `on_click`.
// `icon` names a field of VRChat's own menu icon table (home, options, tools, back, ...);
// `texture` is a UnityEngine.Texture2D and wins when both are given.
struct unix_am_desc     { uint32_t size; unix_str label, icon; void* texture;
                          unix_handle page; unix_cb_widget on_click; unix_cb_avail state;
                          void* ud; bool back;
                          static constexpr uint32_t kAbiTag = 0x616d6900u; };
// `signature` and `is_static` are what HookPersistence::ResolveAndRegister needs to fill a
// PendingHook; without them the class/method pair is ambiguous. Inline char arrays, not
// unix_str: explorer.list_hooks writes this row back into caller storage (§3.6).
struct unix_hook_desc   { uint32_t size; char class_name[128], method_name[128], signature[192];
                          uint8_t argc; bool is_static; uint32_t hook_type;
                          unix_cb_hook on_hit; void* ud;
                          static constexpr uint32_t kAbiTag = 0x686f6b00u; };
// One word of flags, matching the host's atomic mask (§5.6). `mask` says which bits this call
// applies; `values` carries them. The log file path is log.file_path / log.set_file_path.
struct unix_log_options { uint32_t size, mask, values;
                          static constexpr uint32_t kAbiTag = 0x6c6f7000u; };
typedef enum : uint32_t {
    unix_log_colors = 1u<<0, unix_log_trace = 1u<<1, unix_log_info_trace = 1u<<2,
    unix_log_deobfuscate = 1u<<3, unix_log_to_file = 1u<<4, unix_log_to_console = 1u<<5,
    // VRChat's own Application.Log stream. Off by default: the game is chatty enough to
    // bury Bootstrap's own output. Honoured per line, so flipping it takes effect at once.
    unix_log_unity = 1u<<6,
} unix_log_flag;

// Host-filled records. Inline char arrays, never unix_str: no pointer the host writes into a
// caller's struct outlives the call (§3.6).
struct unix_module_info    { uint32_t size; char name[64], version[32], author[64];
                             uint32_t kind; bool enabled;
                             static constexpr uint32_t kAbiTag = 0x6d6f6900u; };
struct unix_hotkey_info    { uint32_t size; char id[64], display_name[96];
                             uint32_t key, mods, default_key, default_mods; bool enabled;
                             static constexpr uint32_t kAbiTag = 0x686b6900u; };
struct unix_ws_info        { uint32_t size; char id[64], name[96], page_host[64], page_name[64];
                             uint32_t entry_count, var_count; bool any_world; char version[32];
                             static constexpr uint32_t kAbiTag = 0x77736900u; };
struct unix_ws_entry       { uint32_t size; char id[64], label[96]; uint32_t kind; bool state;
                             static constexpr uint32_t kAbiTag = 0x77736500u; };
struct unix_ws_var         { uint32_t size; char id[64], label[96], s[128];
                             uint32_t type; float f;
                             bool has_range; float min_value, max_value;
                             static constexpr uint32_t kAbiTag = 0x77737600u; };
struct unix_license_user   { uint32_t size; char name[64], email[128]; uint64_t expires;
                             static constexpr uint32_t kAbiTag = 0x6c757300u; };
struct unix_license_product{ uint32_t size; char id[64], name[96]; uint64_t expires;
                             static constexpr uint32_t kAbiTag = 0x6c707200u; };
struct unix_license_redeem { uint32_t size; uint32_t status; char message[192];
                             static constexpr uint32_t kAbiTag = 0x6c726400u; };

// deob.offsets payloads (§6.6). Offsets only: vrc_types.hpp's structs carry live IL2CPP class,
// singleton and method pointers this row must not hand out, and have no leading `size`.
typedef enum : uint32_t {
    unix_offsets_vrc_player = 0, unix_offsets_player = 1, unix_offsets_nameplate = 2
} unix_offset_table;
struct unix_vrc_player_offsets { uint32_t size;
                                 int32_t vrc_player_api, player, player_nameplate, pose_recorder,
                                         serializer, sync_physics, api_avatar, api_avatar_fallback,
                                         gameobject_nameplate, gameobject_avatar, animator;
                                 static constexpr uint32_t kAbiTag = 0x6f766100u; };
struct unix_player_offsets     { uint32_t size;
                                 int32_t vrc_player_api, vrc_player, api_user, uspeaker;
                                 int32_t user_model, user_model_api_user;
                                 static constexpr uint32_t kAbiTag = 0x706f6600u; };
struct unix_nameplate_offsets  { uint32_t size;
                                 int32_t gameobject_contents, gameobject_sub_text,
                                         gameobject_text_container, gameobject_main_container,
                                         gameobject_icon, gameobject_quick_stats,
                                         gameobject_platform, gameobject_interaction_status,
                                         gameobject_pronouns, gameobject_player_status_icons,
                                         gameobject_user_muted, gameobject_user_volume,
                                         gameobject_listen_blocked, gameobject_earmuffs_icon,
                                         gameobject_focus_view_icon, gameobject_dev_banner,
                                         gameobject_dev_icon, gameobject_group_info,
                                         gameobject_avatar_progress, gameobject_friend_icon;
                                 static constexpr uint32_t kAbiTag = 0x6e706f00u; };

/* ---- player (VRCPlayerApi) and net (VRC.SDKBase.Networking) ---- */

// Every flag one VRCPlayerApi answers, in one call: asking eleven times costs eleven managed
// invokes for state that is read together.
struct unix_player_info { uint32_t size; int32_t player_id;
                          char display_name[128];
                          bool is_local, is_master, is_instance_owner, is_moderator, is_super,
                               is_suspended, is_valid, in_vr, grounded, vrc_plus;
                          static constexpr uint32_t kAbiTag = 0x70696e00u; };

struct unix_locomotion   { uint32_t size;
                           float gravity, run_speed, walk_speed, strafe_speed, jump_impulse;
                           bool immobilized;   // set only; VRChat exposes no getter
                           static constexpr uint32_t kAbiTag = 0x6c6f6300u; };

struct unix_voice        { uint32_t size;
                           float gain, distance_near, distance_far, volumetric_radius;
                           bool lowpass;
                           static constexpr uint32_t kAbiTag = 0x766f6300u; };

// Write-only: VRChat exposes setters for avatar audio and no getters, so a read would be a lie.
struct unix_avatar_audio { uint32_t size;
                           float gain, near_radius, far_radius, volumetric_radius;
                           bool force_spatial, custom_curve;
                           static constexpr uint32_t kAbiTag = 0x61766100u; };

struct unix_avatar_scale { uint32_t size;
                           float eye_height, eye_height_min, eye_height_max;
                           bool manual_scaling_allowed;
                           static constexpr uint32_t kAbiTag = 0x61767300u; };

struct unix_storage_usage{ uint32_t size;
                           int32_t player_data_used, player_data_limit,
                                   player_object_used, player_object_limit;
                           static constexpr uint32_t kAbiTag = 0x73747500u; };

// player.set_nameplate_visible and set_nameplate_color both restore VRChat's own value rather
// than take one, so the "off" case is a value, not a null pointer.
typedef enum : int32_t {
    unix_plate_restore = -1, unix_plate_hide = 0, unix_plate_show = 1
} unix_plate_visibility;

/* ---- udon ---- */

// VRChat's NetworkEventTarget, in its order. Owner is 1 and Others is 2: the two are easy to
// swap, and swapping them silently sends to the wrong audience.
typedef enum : uint32_t {
    unix_udon_all = 0, unix_udon_owner, unix_udon_others, unix_udon_self
} unix_udon_target;

// VRChat's RPC Destination, which is a wider audience list than NetworkEventTarget and is the
// only path that reaches ONE named player.
typedef enum : uint32_t {
    unix_rpc_all = 0, unix_rpc_others, unix_rpc_owner, unix_rpc_master,
    unix_rpc_all_buffered, unix_rpc_others_buffered, unix_rpc_local,
    unix_rpc_all_buffer_one, unix_rpc_others_buffer_one, unix_rpc_target_player
} unix_rpc_target;

typedef enum : uint32_t {
    unix_udon_v_none = 0, unix_udon_v_bool, unix_udon_v_int, unix_udon_v_uint,
    unix_udon_v_float, unix_udon_v_string, unix_udon_v_object
} unix_udon_value_kind;

// One Udon value, in text plus a decoded scalar. `text` is inline, never a pointer: the host
// fills this record into caller storage (§3.6). Writing it, `text` is the literal and
// `type_name` optionally forces the type it is parsed as.
struct unix_udon_value { uint32_t size; unix_udon_value_kind kind;
                         bool b; int64_t i; double d; void* obj;
                         char text[192], type_name[128];
                         static constexpr uint32_t kAbiTag = 0x75647600u; };

// One named argument for udon.run_event_params, which VRChat binds to the program's own
// parameter symbols for the duration of the call.
struct unix_udon_arg   { uint32_t size; char name[96]; unix_udon_value value;
                         static constexpr uint32_t kAbiTag = 0x75646100u; };

struct unix_udon_entry { uint32_t size; char name[128]; uint32_t address;
                         bool exported, network_callable;
                         int32_t max_events_per_second; uint32_t param_count;
                         static constexpr uint32_t kAbiTag = 0x75646500u; };

struct unix_udon_param { uint32_t size; char name[96], type_name[128], symbol[128];
                         static constexpr uint32_t kAbiTag = 0x75647000u; };

struct unix_udon_var   { uint32_t size; char name[128], type_name[128], text[192];
                         uint32_t address; unix_udon_value_kind kind;
                         bool synced; char interpolation[16];
                         static constexpr uint32_t kAbiTag = 0x75647200u; };

struct unix_udon_info  { uint32_t size; char script_name[128], object_name[128], object_path[256];
                         uint32_t entry_count, symbol_count, heap_capacity, sync_method;
                         bool ready, has_error, initialized, interactive,
                              event_processing_disabled, networking_supported;
                         static constexpr uint32_t kAbiTag = 0x75646900u; };

typedef enum : uint32_t { unix_udon_input_button = 0, unix_udon_input_axis } unix_udon_input_kind;

struct unix_udon_input { uint32_t size; unix_udon_input_kind kind;
                         bool boolean_value; float float_value; uint32_t hand;
                         static constexpr uint32_t kAbiTag = 0x75646e00u; };

// Lifecycle: a behaviour finished loading, or asked for serialization.
typedef enum : uint32_t { unix_udon_init = 0, unix_udon_serialize } unix_udon_life;
struct unix_udon_ev    { uint32_t size; unix_udon_life kind; void *behaviour, *program;
                         static constexpr uint32_t kAbiTag = 0x75646c00u; };

// One networked Udon event crossing this client, before it is delivered or sent. Returning
// false from the callback drops it: nothing runs locally, nothing goes out.
typedef enum : uint32_t { unix_udon_inbound = 0, unix_udon_outbound } unix_udon_direction;
struct unix_udon_net_ev { uint32_t size; unix_udon_direction direction;
                          void *behaviour, *sender;              // sender: VRCPlayerApi, inbound only
                          unix_str event, script;
                          const unix_udon_value* params; uint32_t param_count;
                          uint64_t serial; double time;
                          static constexpr uint32_t kAbiTag = 0x75646e76u; };

typedef void (UNIX_CC* unix_cb_udon)     (void* ud, const unix_udon_ev*);
typedef bool (UNIX_CC* unix_cb_udon_net) (void* ud, const unix_udon_net_ev*);

// VRChat's HUD notification widgets, oldest to newest. unix_notify_auto picks the best one the
// running client has: the Voyager toast when that HUD is up, else the carousel toast.
typedef enum : uint32_t {
    unix_notify_auto = 0,
    unix_notify_card,        // legacy NotificationHud card: icon with a ring timer, two lines
    unix_notify_pill,        // SpecialNotification: centre pill, one line, no countdown
    unix_notify_banner,      // SpecialNotification with image_url: full image panel
    unix_notify_toast,       // carousel toast: one line, icon, queued three at a time
    unix_notify_event,       // carousel toast with title, body and a badge
    unix_notify_voyager,     // Voyager small: stacks, several on screen at once
    unix_notify_voyager_big, // Voyager large: ring timer, title and body, strictly sequential
    unix_notify_announcement,// a menu page in VRChat's own styling: heading, body and buttons,
                             // up until it is answered rather than for a duration
} unix_notify_style;

typedef enum : uint32_t {
    unix_notify_centre = 0, unix_notify_left, unix_notify_right
} unix_notify_placement;

typedef enum : uint32_t {
    unix_notify_low = 0, unix_notify_normal, unix_notify_high
} unix_notify_priority;

// Every widget is a single shared object, so several modules notifying at once are scheduled
// rather than allowed to overwrite one another. These say what to do when a style is busy.
enum : uint32_t {
    unix_notify_f_none       = 0,
    unix_notify_f_replace    = 1u<<0,   // drop this module's queued rows on the same channel
    unix_notify_f_now        = 1u<<1,   // show at once, overwriting whatever is up
    unix_notify_f_skip_busy  = 1u<<2,   // drop this one rather than queue it
    unix_notify_f_coalesce   = 1u<<3,   // fold into a queued row with the same channel and text
};

// Everything every widget can show. A field a style has no slot for is ignored rather than
// refused, so one description can be retargeted at another style without editing it.
// An announcement's buttons report which one was pressed, by its ordinal among the button
// rows. A notification taken down without an answer reports unix_notify_no_answer instead.
typedef void (UNIX_CC* unix_cb_notify_action)(void* ud, uint32_t button);
enum : uint32_t {
    unix_notify_no_answer = 0xFFFFFFFFu,
    // A toggle reports while the page stays up, so it is not the answer: the handler runs and
    // the announcement goes on waiting for a button.
    unix_notify_toggled   = 0x80000000u,
};

// An announcement is described as rows, top to bottom, so a module lays the page out rather
// than filling fixed slots. Unknown kinds are skipped, which is what lets rows be added.
typedef enum : uint32_t {
    unix_ann_heading = 0,   // large title line
    unix_ann_text,          // a paragraph
    unix_ann_pill,          // the small tag above the heading, tinted
    unix_ann_image,         // sprite in `icon`, `height` pixels tall
    unix_ann_button,        // `text`, optional `icon`; tint marks it the primary one
    unix_ann_separator,
    unix_ann_toggle,        // `text` with a switch; its state is reported as a press
    unix_ann_footer,        // small print under the buttons
} unix_ann_row_kind;

struct unix_ann_row { uint32_t size; unix_ann_row_kind kind;
                      unix_str text; unix_icon icon; unix_color tint;
                      float height; bool on;
                      static constexpr uint32_t kAbiTag = 0x616e7277u; };

struct unix_notification { uint32_t size; unix_notify_style style;
                           unix_str title, text, badge_text, image_url;
                           unix_str action_text, dismiss_text;   // the two-button shorthand
                           const unix_ann_row* rows; uint32_t row_count;  // or the full layout
                           unix_str header;              // the window's own title, "News"
                           unix_cb_notify_action on_action; void* action_ud;
                           bool closable;                // show the corner close button
                           unix_icon icon, badge_icon;
                           unix_color tint;              // alpha 0 keeps the widget's own colour
                           unix_color text_color;        // the lines' colour; alpha 0 keeps VRChat's
                           float text_scale;             // multiplies the lines' font size; <= 0 keeps it
                           float seconds;                // <= 0 takes the style's default
                           unix_notify_placement placement;
                           unix_notify_priority priority;
                           uint32_t channel;             // the module's own grouping tag, 0 for none
                           uint32_t flags;               // unix_notify_f_*
                           bool sound;
                           static constexpr uint32_t kAbiTag = 0x6e746679u; };

struct unix_module_desc {
    uint32_t size; void* user;
    bool (UNIX_CC* on_load)(void*, const unix_context*);
    void (UNIX_CC* on_unload)(void*);
    void (UNIX_CC* on_enabled)(void*, bool);
    void (UNIX_CC* on_update)(void*, float);
    void (UNIX_CC* on_late_update)(void*);
    void (UNIX_CC* on_fixed_update)(void*);
    void (UNIX_CC* on_post_late_update)(void*);
    void (UNIX_CC* on_render)(void*, const unix_frame*);
    void (UNIX_CC* on_present)(void*, const unix_frame*);
    void (UNIX_CC* on_scene)(void*, const unix_scene_ev*);
    void (UNIX_CC* on_player)(void*, const unix_player_ev*);
    void (UNIX_CC* on_update_player)(void*, const unix_player_ev*);
    void (UNIX_CC* on_world)(void*, const unix_world_ev*);
    void (UNIX_CC* on_menu)(void*, unix_menu_phase);
    bool (UNIX_CC* on_photon)(void*, const unix_photon_ev*);
    void (UNIX_CC* on_message)(void*, const unix_message*);
    bool (UNIX_CC* on_wndproc)(void*, const unix_input_ev*);   /* true swallows the message */
    void (UNIX_CC* on_resize)(void*, uint32_t width, uint32_t height);
};

// No capability field: what a module reaches for is in its .uxuse section, written by the
// linker from the call sites themselves, so a hand-kept mask could only disagree with it.
struct unix_manifest {
    uint32_t size, abi, sdk_built, sdk_min_host;
    const char *name, *version, *author, *url;
    uint32_t phase;
    const char* const* provides;  uint32_t provides_count;
    const char* const* requires_; uint32_t requires_count;
    const char* const* opt_in;    uint32_t opt_in_count;
};

// One entry of the ".uxuse" section: a symbol the image calls, written at link time. A reader
// walks the section's raw bytes and stops at a zero `size` (file padding). `sym` is an index
// into the SDK the module was built against and means nothing to a newer table; `id` and
// `name_id` are the stable identity, so a row that has since been deprecated or removed still
// resolves — which is how a host reports obsolete usage without running the module.
struct unix_use_rec { uint32_t size; uint32_t sym; uint64_t id; uint64_t name_id; };

typedef const unix_module_desc* (UNIX_CC* unix_fn_init)(const unix_context*);

} // extern "C"

namespace UNIx::abi {

/// <summary>FNV-1a over a NUL-terminated literal. constexpr, not consteval: Api::Initialize
/// and the by-name resolve path both need it at runtime (§5.1, §8.6).</summary>
constexpr uint64_t fnv(const char* s, uint64_t h = 0xcbf29ce484222325ull) {
    return *s ? fnv(s + 1, (h ^ (uint64_t)(uint8_t)*s) * 0x100000001b3ull) : h;
}
/// <summary>FNV-1a over a counted string. A unix_str is not NUL-guaranteed.</summary>
constexpr uint64_t fnv_n(const char* s, size_t n) {
    uint64_t h = 0xcbf29ce484222325ull;
    for (size_t i = 0; i < n; ++i) h = (h ^ (uint64_t)(uint8_t)s[i]) * 0x100000001b3ull;
    return h;
}
constexpr uint64_t mix(uint64_t h, uint64_t v) {
    for (int i = 0; i < 8; ++i) h = (h ^ (uint8_t)(v >> (i * 8))) * 0x100000001b3ull;
    return h;
}

template <class T> concept UnixTagged = requires { { T::kAbiTag } -> std::convertible_to<uint32_t>; };

template <class T> consteval uint64_t tcode();

/// <summary>Folds a function type's return, parameters and arity, so two callback shapes
/// never share a code. UNIX_CC is __cdecl, which MSVC ignores on x64, so R(A...) matches
/// R(__cdecl)(A...); this ABI is x64-only and on x86 it would not.</summary>
template <class F> struct fcode { static consteval uint64_t v() { return 'F'; } };
template <class R, class... A> struct fcode<R(A...)> {
    static consteval uint64_t v() {
        uint64_t h = mix('F', tcode<R>());
        ((h = mix(h, tcode<A>())), ...);
        return mix(h, sizeof...(A));
    }
};
template <class R, class... A> struct fcode<R(A...) noexcept> : fcode<R(A...)> {};

/// <summary>Type code folded into a symbol id. Distinguishes width, signedness and pointee.</summary>
template <class T> consteval uint64_t tcode() {
    static_assert(!std::is_reference_v<T> && !std::is_array_v<T> && !std::is_member_pointer_v<T>
                  && !std::is_union_v<std::remove_cv_t<T>>,
                  "unix ABI signatures take scalars, pointers and tagged aggregates only");
    using U = std::remove_cv_t<T>;
    if constexpr (std::is_void_v<U>)                 return 'v';
    else if constexpr (std::is_same_v<U, bool>)      return 'b';
    else if constexpr (std::is_function_v<U>)        return fcode<U>::v();
    else if constexpr (std::is_pointer_v<U>)         return mix('p', tcode<std::remove_pointer_t<U>>());
    else if constexpr (std::is_floating_point_v<U>)  return 'f' | ((uint64_t)sizeof(U) << 8);
    else if constexpr (std::is_enum_v<U>)            return 'e' | ((uint64_t)sizeof(U) << 8);
    else if constexpr (std::is_class_v<U>) {
        static_assert(UnixTagged<U>,
                      "public ABI aggregate needs `static constexpr uint32_t kAbiTag`");
        return mix('S', (uint64_t)U::kAbiTag << 32);   // tag only: sizeof would forbid growth
    }
    else                                             return 'i' | ((uint64_t)sizeof(U) << 8)
                                                                | ((uint64_t)std::is_signed_v<U> << 24)
                                                                | ((uint64_t)std::is_same_v<U, char> << 25);
}

template <class...> inline constexpr bool unix_always_false = false;

template <class F> struct sigof {
    static_assert(unix_always_false<F>,
        "UNIX_BIND_EXPORT target is not a plain " "__cdecl" " function pointer");
};
template <class R, class... A> struct sigof<R (UNIX_CC*)(A...)> {
    static consteval uint64_t hash(uint64_t h) {
        h = mix(h, tcode<R>());
        ((h = mix(h, tcode<A>())), ...);
        return mix(h, sizeof...(A));
    }
};
template <class R, class... A> struct sigof<R (UNIX_CC*)(A...) noexcept> : sigof<R (UNIX_CC*)(A...)> {};

/// <summary>Symbol id: the name mixed with a signature tag derived from the function type.</summary>
template <class F> consteval uint64_t sym_id(const char* name) { return sigof<F>::hash(fnv(name)); }

static_assert(tcode<unix_cb_widget>() != tcode<unix_cb_render>(),
              "callback shapes collapsed to one code — fcode is not being reached");

// kAbiTag ALONE is an aggregate's ABI identity (§3.3), so two aggregates sharing one make two
// different rows hash the same, silently. §5.2's five asserts cover ids, names, aliases, codes
// and attributes but not tags, and tcode's own assert only catches a MISSING tag. The
// aggregates are not .inc rows, so the list is hand-kept — one line per struct declared above.
inline constexpr uint32_t kUnixAbiTags[] = {
    unix_str::kAbiTag, unix_vec2::kAbiTag, unix_vec3::kAbiTag, unix_vec4::kAbiTag,
    unix_color::kAbiTag, unix_rect::kAbiTag, unix_icon::kAbiTag,
    unix_bind_note::kAbiTag, unix_bind_req::kAbiTag,
    unix_frame::kAbiTag, unix_player_ev::kAbiTag, unix_scene_ev::kAbiTag,
    unix_world_ev::kAbiTag, unix_photon_ev::kAbiTag, unix_widget_ev::kAbiTag,
    unix_message::kAbiTag, unix_log_ev::kAbiTag, unix_explorer_ev::kAbiTag,
    unix_user_ev::kAbiTag, unix_tween_ev::kAbiTag, unix_hotkey_ev::kAbiTag,
    unix_cmd_ev::kAbiTag, unix_hook_ev::kAbiTag, unix_input_ev::kAbiTag,
    unix_page_ev::kAbiTag,
    unix_button_desc::kAbiTag, unix_toggle_desc::kAbiTag, unix_slider_desc::kAbiTag,
    unix_enum_desc::kAbiTag, unix_foldout_desc::kAbiTag, unix_text_desc::kAbiTag,
    unix_page_desc::kAbiTag,
    unix_cmd_desc::kAbiTag, unix_hook_desc::kAbiTag, unix_log_options::kAbiTag,
    unix_module_info::kAbiTag, unix_hotkey_info::kAbiTag,
    unix_ws_info::kAbiTag, unix_ws_entry::kAbiTag, unix_ws_var::kAbiTag,
    unix_license_user::kAbiTag, unix_license_product::kAbiTag, unix_license_redeem::kAbiTag,
    unix_vrc_player_offsets::kAbiTag, unix_player_offsets::kAbiTag,
    unix_nameplate_offsets::kAbiTag,
};
consteval bool UnixTagsAreUnique() {
    for (size_t i = 1; i < sizeof(kUnixAbiTags) / sizeof(*kUnixAbiTags); ++i)
        for (size_t j = 0; j < i; ++j)
            if (kUnixAbiTags[i] == kUnixAbiTags[j]) return false;
    return true;
}
static_assert(UnixTagsAreUnique(),
              "two ABI aggregates share a kAbiTag — every row naming either one hashes the same");

} // namespace UNIx::abi
