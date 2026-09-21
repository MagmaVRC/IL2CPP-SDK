#pragma once

#include <unix/unix_module_abi.hpp>
#include <unix/unix_ctx.hpp>
#include <unix/unix_types.hpp>

#include <bitset>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <deque>
#include <format>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace UNIx {

// Services (unix/services/*.hpp)
class Config; class Menu; class MainMenu; class UserPage; class Players; class Renderer;
class Log; class FileSystem; class Bus; class Hotkeys; class Nameplates; class Tweens; class Ui;
class Deob; class Studio; class Clipboard; class Perf; class License; class WorldScripts;
class Console; class Explorer; class Photon; class Modules; class Telemetry; class Il2Cpp;
class Udon; class PlayerApi; class Net; class Notify;
// UI objects (unix/ui.hpp, except Hotkey and StudioPage which live in unix/services/core.hpp)
class UISurface; class UIPage; class UISubPage; class UISection; class UIMainMenuPage;
class UIFoldout; class UIButton; class UILabel;
class UIToggle; class UISlider; class UIEnumSelector; class UISeparator; class UICornerButton;
class UITextInput; class UIList;
class UIUserButton; class UIUserRow; class Nameplate; class Tween; class Hotkey; class StudioPage;

using Color = unix_color;   using Vec2 = unix_vec2;  using Vec3 = unix_vec3; using Vec4 = unix_vec4;
using Rect  = unix_rect;    using Icon = unix_icon;  using Status = unix_status;
using Frame = unix_frame;
using PlayerEvent = unix_player_ev;  using SceneEvent  = unix_scene_ev;
using WorldEvent  = unix_world_ev;   using PhotonEvent = unix_photon_ev;
using Message     = unix_message;    using InputEvent  = unix_input_ev;
using LogEvent    = unix_log_ev;     using UserEvent   = unix_user_ev;
using HookEvent   = unix_hook_ev;    using CommandEvent = unix_cmd_ev;
using WidgetEvent = unix_widget_ev;  using PageEvent   = unix_page_ev;
using TweenEvent  = unix_tween_ev;   using HotkeyEvent = unix_hotkey_ev;
using ExplorerEvent = unix_explorer_ev;
using ModuleInfo  = unix_module_info; using HotkeyInfo  = unix_hotkey_info;
using WorldScriptInfo = unix_ws_info; using WorldScriptEntry = unix_ws_entry;
using WorldScriptVar  = unix_ws_var;
using LicenseUser = unix_license_user; using LicenseProduct = unix_license_product;
using LicenseRedeem = unix_license_redeem;
using VrcPlayerOffsets = unix_vrc_player_offsets;
using PlayerOffsets    = unix_player_offsets;
using NameplateOffsets = unix_nameplate_offsets;

/// <summary>Which deob offset table `deob.offsets` should fill.</summary>
enum class OffsetTable : uint32_t {
    VrcPlayer = unix_offsets_vrc_player,
    Player    = unix_offsets_player,
    Nameplate = unix_offsets_nameplate,
};

/// <summary>A borrowed string view over a std::string_view. The one conversion, spelled once.</summary>
constexpr unix_str Sv(std::string_view s) noexcept { return { s.data(), (uint32_t)s.size() }; }

/// <summary>The other direction: a unix_str as a view, empty when the host sent nothing.</summary>
constexpr std::string_view View(unix_str s) noexcept {
    return s.data ? std::string_view{ s.data, s.size } : std::string_view{};
}

/// <summary>The instance id a world event carries -- `wrld_...:12345~region(eu)`, or
/// `local:...` for a local build. Unlike `id` this changes between two instances of one
/// world, so it is what a module keys its world state on.</summary>
/// <returns>Empty when the host predates the field; `id` is still filled in that case.</returns>
inline std::string_view LocationOf(const WorldEvent& e) noexcept {
    if (e.size < offsetof(unix_world_ev, location) + sizeof(unix_str)) return {};
    return View(e.location);
}

namespace detail {

void MissingOnce(uint64_t sym_id) noexcept;
void DeniedOnce (uint64_t sym_id) noexcept;

template <class R> struct MissingValue { static constexpr R value{}; };
template <> struct MissingValue<unix_status> { static constexpr unix_status value = unix_e_absent; };

template <class... A> struct RS { static constexpr bool value = false; };
template <class P, class C> struct RS<P, C> {
    static constexpr bool value = std::is_pointer_v<P> && std::is_same_v<C, uint32_t>;
};
template <class A0, class A1, class... Rest> struct RS<A0, A1, Rest...> {
    static constexpr bool value = RS<A1, Rest...>::value;
};
/// <summary>True when the last two parameters are the (pointer, uint32_t cap) required-size pair.</summary>
template <class... A> inline constexpr bool IsRequiredSize = RS<A...>::value;

template <class Tag, class F> struct MissingStub;
template <class Tag, class R, class... A>
struct MissingStub<Tag, R (UNIX_CC*)(A...)> {
    static R UNIX_CC fn(A...) {
        MissingOnce(Tag::id);
        if constexpr (std::is_same_v<R, uint32_t> && IsRequiredSize<A...>) return unix_absent;
        else if constexpr (!std::is_void_v<R>)                            return MissingValue<R>::value;
    }
};

template <class Tag, class F> struct DeniedStub;
template <class Tag, class R, class... A>
struct DeniedStub<Tag, R (UNIX_CC*)(A...)> {
    static R UNIX_CC fn(A...) {
        DeniedOnce(Tag::id);
        if constexpr (std::is_same_v<R, uint32_t> && IsRequiredSize<A...>) return unix_denied;
        else if constexpr (std::is_same_v<R, unix_status>)                return unix_e_denied;
        else if constexpr (!std::is_void_v<R>)                            return MissingValue<R>::value;
    }
};

#define UNIX_EXPORT(ns, n, r, a, ...) \
    struct Tag_##ns##_##n { static constexpr uint64_t id = ::UNIx::abi::fnv(#ns "." #n); };
#define UNIX_REMOVED(ns, n, r, a, ...)
#define UNIX_ALIAS(ns, n, r, a, ...)
#include <unix/unix_exports.inc>

}   // namespace detail

struct Dispatch {
#define UNIX_EXPORT(ns, n, r, a, ...) \
    r (UNIX_CC* ns##_##n) a = &::UNIx::detail::MissingStub<detail::Tag_##ns##_##n, r (UNIX_CC*) a>::fn;
#define UNIX_REMOVED(ns, n, r, a, ...)
#define UNIX_ALIAS(ns, n, r, a, ...)
#include <unix/unix_exports.inc>
};

namespace detail {
inline Dispatch             g_d{};
inline unix_self            g_self{};
inline thread_local void*   g_has_out{};
}

inline void FillMissing(Dispatch& d) {
#define UNIX_EXPORT(ns, n, r, a, ...) \
    if (!d.ns##_##n) d.ns##_##n = &::UNIx::detail::MissingStub<detail::Tag_##ns##_##n, r (UNIX_CC*) a>::fn;
#define UNIX_REMOVED(ns, n, r, a, ...)
#define UNIX_ALIAS(ns, n, r, a, ...)
#include <unix/unix_exports.inc>
}

namespace detail {

/// <summary>One closure registration, addressed by the host as the callback's `ud`.
/// `ret` carries the closure for the two callback shapes whose return value the host reads.</summary>
struct Node {
    std::function<void(const void*)> fn;
    std::function<bool(const void*)> ret;
};

Node* AllocNodeRaw();
void  FreeNode(Node*) noexcept;
/// <summary>Bind a node's lifetime to the handle the host just minted. A null handle means
/// the registration failed, and the node is released immediately.</summary>
void  OwnNode(unix_handle h, Node* n);
/// <summary>core.cancel, plus the release of whatever node OwnNode tied to the handle.</summary>
void  Cancel(unix_handle h);

template <class F> [[nodiscard]] Node* AllocNode(F&& f) {
    Node* n = AllocNodeRaw();
    n->fn = std::forward<F>(f);
    return n;
}
template <class Ev, class F> [[nodiscard]] Node* NodeFor(F&& f) {
    return AllocNode([g = std::forward<F>(f)](const void* p) { g(*static_cast<const Ev*>(p)); });
}
template <class F> [[nodiscard]] Node* NodeForTask(F&& f) {
    return AllocNode([g = std::forward<F>(f)](const void*) { g(); });
}
template <class Ev, class F> [[nodiscard]] Node* NodeForVeto(F&& f) {
    Node* n = AllocNodeRaw();
    n->ret = [g = std::forward<F>(f)](const void* p) -> bool {
        return g(*static_cast<const Ev*>(p));
    };
    return n;
}

template <class Ev> void UNIX_CC Tramp(void* ud, const Ev* ev) {
    auto* n = static_cast<Node*>(ud);
    if (n && n->fn && ev) n->fn(ev);
}
void UNIX_CC TrampTask(void* ud);
bool UNIX_CC TrampPhoton(void* ud, const unix_photon_ev* ev);
bool UNIX_CC TrampAvail(void* ud);

/// <summary>Required-size string read. Tolerates both the length-excluding-NUL and the
/// capacity-including-NUL spellings of the protocol by always over-allocating by one.</summary>
template <class F> [[nodiscard]] std::optional<std::string> TryReadStr(F&& call) {
    const uint32_t need = call(nullptr, 0u);
    if (need == unix_absent || need == unix_denied) return std::nullopt;
    if (need == 0) return std::string{};
    std::string s(static_cast<std::size_t>(need) + 1u, '\0');
    const uint32_t got = call(s.data(), need + 1u);
    if (got == unix_absent || got == unix_denied || got > need + 1u) return std::nullopt;
    const std::size_t z = s.find('\0');
    if (z != std::string::npos) s.resize(z);
    return s;
}
template <class F> [[nodiscard]] std::string ReadStr(F&& call) {
    return TryReadStr(std::forward<F>(call)).value_or(std::string{});
}
/// <summary>Packed NUL-separated string list, terminated by one extra NUL.</summary>
template <class F> [[nodiscard]] std::vector<std::string> ReadList(F&& call) {
    std::vector<std::string> out;
    const uint32_t need = call(nullptr, 0u);
    if (need == unix_absent || need == unix_denied || need == 0) return out;
    std::vector<char> buf(static_cast<std::size_t>(need) + 2u, '\0');
    const uint32_t got = call(buf.data(), need + 2u);
    if (got == unix_absent || got == unix_denied) return out;
    const char* p = buf.data();
    const char* end = buf.data() + buf.size();
    while (p < end && *p) {
        const std::size_t len = std::strlen(p);
        out.emplace_back(p, len);
        p += len + 1;
    }
    return out;
}
template <class F> [[nodiscard]] std::vector<uint8_t> ReadBlob(F&& call) {
    std::vector<uint8_t> out;
    const uint32_t need = call(nullptr, 0u);
    if (need == unix_absent || need == unix_denied || need == 0) return out;
    out.resize(need);
    const uint32_t got = call(out.data(), need);
    if (got == unix_absent || got == unix_denied || got > need) { out.clear(); return out; }
    out.resize(got < need ? got : need);
    return out;
}
/// <summary>Typed-array read over the (T*, cap, stride) rows. Each element is stamped with
/// this build's sizeof before the call, per the growth rule.</summary>
template <class T, class F> [[nodiscard]] std::vector<T> ReadArray(F&& call) {
    std::vector<T> out;
    const uint32_t stride = static_cast<uint32_t>(sizeof(T));
    const uint32_t need = call(nullptr, 0u, stride);
    if (need == unix_absent || need == unix_denied || need == 0) return out;
    out.assign(need, T{});
    for (auto& e : out) e.size = stride;
    const uint32_t got = call(out.data(), need, stride);
    if (got == unix_absent || got == unix_denied) { out.clear(); return out; }
    if (got < need) out.resize(got);
    return out;
}

void RegisterLoad(const void* value, std::function<void()> writeback);
void UnregisterLoad(const void* value) noexcept;
void RunLoadWriteback();

}   // namespace detail

/// <summary>Move-only registration. Unregisters and clears its closure node on destruction.
/// A subscription over a menu, plate or tween handle must be destroyed on the Unity main
/// thread.</summary>
class Subscription {
public:
    Subscription() = default;
    /// <summary>Adopt a registration the host just minted.</summary>
    /// <param name="h">The host handle, or unix_null for a failed registration.</param>
    /// <param name="node">The closure node, or null for a registration whose `ud` is not one.</param>
    Subscription(unix_handle h, void* node) noexcept : m_handle(h), m_node(node) {}
    Subscription(Subscription&& o) noexcept : m_handle(o.m_handle), m_node(o.m_node) {
        o.m_handle = unix_null; o.m_node = nullptr;
    }
    Subscription& operator=(Subscription&& o) noexcept {
        if (this != &o) {
            Release();
            m_handle = o.m_handle; m_node = o.m_node;
            o.m_handle = unix_null; o.m_node = nullptr;
        }
        return *this;
    }
    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;
    ~Subscription() { Release(); }
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    void Release() noexcept {
        if (m_handle != unix_null) {
            detail::Cancel(m_handle);
            m_handle = unix_null;
        }
        if (m_node) {
            detail::FreeNode(static_cast<detail::Node*>(m_node));
            m_node = nullptr;
        }
    }
private:
    unix_handle m_handle{};
    void*       m_node{};
};

/// <summary>Releases the wrapped object on scope exit, through whichever of Remove(),
/// Destroy() or Cancel() it declares. The RAII variant of that explicit call. For a menu,
/// plate or tween wrapper the enclosing scope must exit on the Unity main thread.</summary>
template <class T> class Owned {
public:
    template <class... A>
        requires (!(sizeof...(A) == 1 && (std::same_as<std::remove_cvref_t<A>, Owned> && ...)))
    explicit Owned(A&&... a) : m_value(std::forward<A>(a)...) {}
    ~Owned() {
        if constexpr (requires { m_value.Remove(); })       m_value.Remove();
        else if constexpr (requires { m_value.Destroy(); }) m_value.Destroy();
        else                                                m_value.Cancel();
    }
    Owned(const Owned&) = delete;
    Owned& operator=(const Owned&) = delete;
    [[nodiscard]] T* operator->() noexcept { return &m_value; }
    [[nodiscard]] const T* operator->() const noexcept { return &m_value; }
    [[nodiscard]] T& operator*()  noexcept { return m_value; }
    [[nodiscard]] const T& operator*() const noexcept { return m_value; }
private:
    T m_value;
};

/// <summary>A player. A thin wrapper over the same raw pointer every consumer already stores.
/// Position, Distance, GetRank and GetRankColor live in unix/il2cpp.hpp, where the typed
/// layer is; include that header to call the last two.</summary>
class Player {
public:
    Player() = default;
    explicit Player(void* raw) noexcept : m_raw(raw) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_raw != nullptr; }
    [[nodiscard]] void* Raw() const noexcept { return m_raw; }
    /// <summary>False once the game destroyed the object behind this pointer.</summary>
    [[nodiscard]] bool IsAlive() const;
    [[nodiscard]] bool IsLocal() const;
    [[nodiscard]] std::string GetDisplayName() const;
    [[nodiscard]] std::string GetUserId() const;
    [[nodiscard]] PlayerRank  GetRank() const;
    [[nodiscard]] Color       GetRankColor() const;
    friend bool operator==(Player a, Player b) noexcept { return a.m_raw == b.m_raw; }
private:
    void* m_raw = nullptr;
};

/// <summary>Everything a module can reach, bound to this module's identity.</summary>
class Host {
public:
    /// <summary>The per-image Host. Never capture a Host& or a service reference.</summary>
    [[nodiscard]] static Host& Get() noexcept;

    // Templates so the body instantiates at the call site, where the service class is
    // complete: the service headers are included after this declaration, not before it.
    template <class = void> [[nodiscard]] ::UNIx::Config&        Config();
    template <class = void> [[nodiscard]] ::UNIx::Menu&          Menu();
    template <class = void> [[nodiscard]] ::UNIx::MainMenu&      MainMenu();
    template <class = void> [[nodiscard]] ::UNIx::Players&       Players();
    template <class = void> [[nodiscard]] ::UNIx::Renderer&      Renderer();
    template <class = void> [[nodiscard]] ::UNIx::Log&           Log();
    template <class = void> [[nodiscard]] ::UNIx::FileSystem&    Fs();
    template <class = void> [[nodiscard]] ::UNIx::Bus&           Bus();
    template <class = void> [[nodiscard]] ::UNIx::Hotkeys&       Hotkeys();
    template <class = void> [[nodiscard]] ::UNIx::Nameplates&    Nameplates();
    template <class = void> [[nodiscard]] ::UNIx::Tweens&        Tweens();
    template <class = void> [[nodiscard]] ::UNIx::Ui&            Ui();
    template <class = void> [[nodiscard]] ::UNIx::Deob&          Deob();
    template <class = void> [[nodiscard]] ::UNIx::Studio&        Studio();
    template <class = void> [[nodiscard]] ::UNIx::Clipboard&     Clipboard();
    template <class = void> [[nodiscard]] ::UNIx::Perf&          Perf();
    template <class = void> [[nodiscard]] ::UNIx::License&       License();
    template <class = void> [[nodiscard]] ::UNIx::WorldScripts&  WorldScripts();
    template <class = void> [[nodiscard]] ::UNIx::Console&       Console();
    template <class = void> [[nodiscard]] ::UNIx::Explorer&      Explorer();
    template <class = void> [[nodiscard]] ::UNIx::Photon&        Photon();
    template <class = void> [[nodiscard]] ::UNIx::Modules&       Modules();
    template <class = void> [[nodiscard]] ::UNIx::Telemetry&     Telemetry();
    template <class = void> [[nodiscard]] ::UNIx::Il2Cpp&        Il2Cpp();
    template <class = void> [[nodiscard]] ::UNIx::Udon&          Udon();
    template <class = void> [[nodiscard]] ::UNIx::PlayerApi&     PlayerApi();
    template <class = void> [[nodiscard]] ::UNIx::Net&           Net();
    template <class = void> [[nodiscard]] ::UNIx::Notify&        Notify();

    template <class = void> [[nodiscard]] std::string    GetName() const;
    template <class = void> [[nodiscard]] unix_host_kind GetKind() const;
    /// <summary>Resolve one symbol by name, for scripting bridges only. Runs the same
    /// adjudicator as any bind: capability gating, notes, and null on anything but a clean row.</summary>
    /// <param name="symbol">Fully qualified row name, e.g. "menu.add_button".</param>
    /// <returns>True when this host bound the symbol for this module.</returns>
    template <class = void> [[nodiscard]] bool Has(std::string_view symbol) const;
    /// <summary>Run fn on the Unity main thread at the next Update.</summary>
    template <class = void> void RunOnMain(std::function<void()> fn);
    /// <summary>False once the host has begun revoking this module.</summary>
    template <class = void> [[nodiscard]] bool IsLive() const noexcept;
    /// <summary>Acquire another module's service. Null when absent; ask again later.</summary>
    /// <param name="name">Service name the provider published.</param>
    /// <param name="major">Must match the provider's major exactly.</param>
    /// <param name="minor">Minimum minor required; the provider's must be at least this.</param>
    template <class T> [[nodiscard]] const T* Require(std::string_view name,
                                                      uint16_t major, uint16_t minor = 0);
    /// <summary>Publish a service struct for other modules to acquire.</summary>
    /// <param name="svc">Pointer into this module's image; it must outlive every consumer.</param>
    template <class = void> Status Provide(std::string_view name, uint16_t major, uint16_t minor,
                                           const void* svc, uint32_t svc_size);
};

inline Host& Host::Get() noexcept { static Host h; return h; }

/// <summary>Protected service accessors, so a Module or UIPage subclass never writes Host::Get().
/// Static and stateless: they add no bytes to a subclass and cannot be captured by mistake.</summary>
class Services {
protected:
#define UNIX_SERVICE_ACCESSOR(CLASS, ACCESSOR)                                          \
    template <class = void> [[nodiscard]] static ::UNIx::CLASS& ACCESSOR() {            \
        return Host::Get().ACCESSOR();                                                  \
    }
    UNIX_SERVICE_ACCESSOR(Config,       Config)
    UNIX_SERVICE_ACCESSOR(Menu,         Menu)
    UNIX_SERVICE_ACCESSOR(MainMenu,     MainMenu)
    UNIX_SERVICE_ACCESSOR(Players,      Players)
    UNIX_SERVICE_ACCESSOR(Renderer,     Renderer)
    UNIX_SERVICE_ACCESSOR(Log,          Log)
    UNIX_SERVICE_ACCESSOR(FileSystem,   Fs)
    UNIX_SERVICE_ACCESSOR(Bus,          Bus)
    UNIX_SERVICE_ACCESSOR(Hotkeys,      Hotkeys)
    UNIX_SERVICE_ACCESSOR(Nameplates,   Nameplates)
    UNIX_SERVICE_ACCESSOR(Tweens,       Tweens)
    UNIX_SERVICE_ACCESSOR(Ui,           Ui)
    UNIX_SERVICE_ACCESSOR(Deob,         Deob)
    UNIX_SERVICE_ACCESSOR(Studio,       Studio)
    UNIX_SERVICE_ACCESSOR(Clipboard,    Clipboard)
    UNIX_SERVICE_ACCESSOR(Perf,         Perf)
    UNIX_SERVICE_ACCESSOR(License,      License)
    UNIX_SERVICE_ACCESSOR(WorldScripts, WorldScripts)
    UNIX_SERVICE_ACCESSOR(Console,      Console)
    UNIX_SERVICE_ACCESSOR(Explorer,     Explorer)
    UNIX_SERVICE_ACCESSOR(Photon,       Photon)
    UNIX_SERVICE_ACCESSOR(Modules,      Modules)
    UNIX_SERVICE_ACCESSOR(Telemetry,    Telemetry)
    UNIX_SERVICE_ACCESSOR(Il2Cpp,       Il2Cpp)
#undef UNIX_SERVICE_ACCESSOR
};

/// <summary>Optional base class. Only the hooks you override are subscribed. Every override
/// must be public: the desc filler is a non-member and is access-checked against the subclass.</summary>
class Module : protected Services {
public:
    virtual ~Module() = default;
    virtual bool OnLoad() { return true; }
    virtual void OnUnload() {}
    virtual void OnEnabled(bool) {}
    virtual void OnUpdate(float /*dt*/) {}
    virtual void OnLateUpdate() {}
    virtual void OnFixedUpdate() {}
    virtual void OnPostLateUpdate() {}
    /// <summary>Present thread, inside the host's ImGui frame. Do not call NewFrame or Render.</summary>
    virtual void OnRender(const Frame&) {}
    /// <summary>Present thread, outside any host frame. Own your context; call Ui().Adopt first.</summary>
    virtual void OnPresent(const Frame&) {}
    virtual void OnScene(const SceneEvent&) {}
    virtual void OnMenu(MenuPhase) {}
    /// <summary>Every player phase, including JoinedComplete. Overriding any of the four
    /// player hooks subscribes the single on_player slot.</summary>
    virtual void OnPlayer(const PlayerEvent&) {}
    virtual void OnPlayerAwake (Player) {}
    virtual void OnPlayerJoined(Player) {}
    virtual void OnPlayerLeft  (Player) {}
    virtual void OnUpdatePlayer(Player) {}
    virtual void OnWorld(const WorldEvent&) {}
    virtual void OnMessage(const Message&) {}
    /// <summary>Window message pump. Return true to swallow the message.</summary>
    virtual bool OnInput(const InputEvent&) { return false; }
    virtual void OnResize(uint32_t /*width*/, uint32_t /*height*/) {}
    /// <summary>Photon network thread. Returning false suppresses the event game-wide.</summary>
    virtual bool OnPhoton(const PhotonEvent&) { return true; }

    /// <summary>Fans one on_player event out to OnPlayer plus the matching typed hook.</summary>
    void DispatchPlayer(const PlayerEvent& e);
};

inline void Module::DispatchPlayer(const PlayerEvent& e) {
    OnPlayer(e);
    switch (e.phase) {
    case unix_player_awake:  OnPlayerAwake (Player{ e.player }); break;
    case unix_player_joined: OnPlayerJoined(Player{ e.player }); break;
    case unix_player_left:   OnPlayerLeft  (Player{ e.player }); break;
    default: break;
    }
}

namespace detail {

/// <summary>The only bind entry. Writes g_ctx, then g_self, then g_bound, then g_d last.</summary>
/// <param name="self">A statically linked shim's own identity; 0 takes ctx->self.</param>
/// <returns>False when a UNIX_REQUIRE'd symbol is unbound, or when nothing bound at all.</returns>
bool BindAll(const unix_context* ctx, unix_self self = 0);

enum SymIndex : uint32_t {
#define UNIX_EXPORT(ns, n, r, a, ...) kSym_##ns##_##n,
#define UNIX_REMOVED(ns, n, r, a, ...)
#define UNIX_ALIAS(ns, n, r, a, ...)
#include <unix/unix_exports.inc>
    kSymCount
};

constinit inline std::bitset<kSymCount> g_bound;
constinit inline std::bitset<kSymCount> g_required;
constinit inline std::bitset<kSymCount> g_used_syms;
constinit inline std::bitset<kSymCount> g_silent;

/// <summary>Every row's identity, by build-time index. The id folds the signature, so it
/// survives a row being renamed or retired: a host resolves a record the module's own SDK
/// had as live against whatever that row is today.</summary>
inline constexpr struct { uint64_t id, name_id; } kSymIds[kSymCount] = {
#define UNIX_EXPORT(ns, n, r, a, ...) \
    { ::UNIx::abi::sym_id<r (UNIX_CC*) a>(#ns "." #n), ::UNIx::abi::fnv(#ns "." #n) },
#define UNIX_REMOVED(ns, n, r, a, ...)
#define UNIX_ALIAS(ns, n, r, a, ...)
#include <unix/unix_exports.inc>
};

// One record per symbol this image calls, emitted into .uxuse at link time: the linker keeps
// exactly the instantiations UNIX_USE kept, so the section IS the used set, readable from the
// file without running a byte of the module. Ordered arbitrarily; a reader iterates until a
// zero size, which is where the section's file padding begins.
#pragma section(".uxuse", read)
template <uint32_t S>
__declspec(allocate(".uxuse")) inline constexpr unix_use_rec used_rec_v = {
    (uint32_t)sizeof(unix_use_rec), S, kSymIds[S].id, kSymIds[S].name_id
};

// Volatile so the store survives optimisation: taking a record's address in dead code leaves
// no relocation, and /OPT:REF then drops the record with it. Used's constructor is emitted
// either way — it is a dynamic initialiser — so this costs one store per used symbol at
// startup and nothing per call.
inline const unix_use_rec* volatile g_use_anchor = nullptr;

template <uint32_t S> struct Require { Require() noexcept { g_required.set(S); } };
template <uint32_t S> struct Used {
    Used() noexcept { g_used_syms.set(S); g_use_anchor = &used_rec_v<S>; }
};
template <uint32_t S> struct Silence { Silence() noexcept { g_silent.set(S); } };

template <uint32_t S> inline const Used<S> used_v{};

inline void* const kDeniedStubs[kSymCount] = {
#define UNIX_EXPORT(ns, n, r, a, ...) \
    reinterpret_cast<void*>(&DeniedStub<Tag_##ns##_##n, r (UNIX_CC*) a>::fn),
#define UNIX_REMOVED(ns, n, r, a, ...)
#define UNIX_ALIAS(ns, n, r, a, ...)
#include <unix/unix_exports.inc>
};

/// <summary>Fills a desc from a UNIx::Module subclass, subscribing only the overridden hooks.
/// Detection is by member-pointer TYPE identity, which is exact at compile time.</summary>
template <class T>
const unix_module_desc* DescFor(T* self) {
    static unix_module_desc d{};
    d.size = sizeof(d);
    d.user = self;
#define UNIX_HOOK(SLOT, SIG, ASSIGN) if constexpr (!std::is_same_v<decltype(&T::SLOT), SIG>) { ASSIGN }
    UNIX_HOOK(OnLoad, bool (Module::*)(),
              d.on_load = [](void* u, const unix_context*) -> bool {
                  return static_cast<T*>(u)->OnLoad(); };)
    UNIX_HOOK(OnUnload, void (Module::*)(),
              d.on_unload = [](void* u) { static_cast<T*>(u)->OnUnload(); };)
    UNIX_HOOK(OnEnabled, void (Module::*)(bool),
              d.on_enabled = [](void* u, bool on) { static_cast<T*>(u)->OnEnabled(on); };)
    UNIX_HOOK(OnUpdate, void (Module::*)(float),
              d.on_update = [](void* u, float dt) { static_cast<T*>(u)->OnUpdate(dt); };)
    UNIX_HOOK(OnLateUpdate, void (Module::*)(),
              d.on_late_update = [](void* u) { static_cast<T*>(u)->OnLateUpdate(); };)
    UNIX_HOOK(OnFixedUpdate, void (Module::*)(),
              d.on_fixed_update = [](void* u) { static_cast<T*>(u)->OnFixedUpdate(); };)
    UNIX_HOOK(OnPostLateUpdate, void (Module::*)(),
              d.on_post_late_update = [](void* u) { static_cast<T*>(u)->OnPostLateUpdate(); };)
    UNIX_HOOK(OnRender, void (Module::*)(const Frame&),
              d.on_render = [](void* u, const unix_frame* f) { static_cast<T*>(u)->OnRender(*f); };)
    UNIX_HOOK(OnPresent, void (Module::*)(const Frame&),
              d.on_present = [](void* u, const unix_frame* f) { static_cast<T*>(u)->OnPresent(*f); };)
    UNIX_HOOK(OnScene, void (Module::*)(const SceneEvent&),
              d.on_scene = [](void* u, const unix_scene_ev* e) { static_cast<T*>(u)->OnScene(*e); };)
    UNIX_HOOK(OnUpdatePlayer, void (Module::*)(Player),
              d.on_update_player = [](void* u, const unix_player_ev* e) {
                  static_cast<T*>(u)->OnUpdatePlayer(Player{ e->player }); };)
    UNIX_HOOK(OnWorld, void (Module::*)(const WorldEvent&),
              d.on_world = [](void* u, const unix_world_ev* e) { static_cast<T*>(u)->OnWorld(*e); };)
    UNIX_HOOK(OnMessage, void (Module::*)(const Message&),
              d.on_message = [](void* u, const unix_message* m) { static_cast<T*>(u)->OnMessage(*m); };)
    UNIX_HOOK(OnInput, bool (Module::*)(const InputEvent&),
              d.on_wndproc = [](void* u, const unix_input_ev* e) -> bool {
                  return static_cast<T*>(u)->OnInput(*e); };)
    UNIX_HOOK(OnResize, void (Module::*)(uint32_t, uint32_t),
              d.on_resize = [](void* u, uint32_t w, uint32_t h) {
                  static_cast<T*>(u)->OnResize(w, h); };)
    UNIX_HOOK(OnPhoton, bool (Module::*)(const PhotonEvent&),
              d.on_photon = [](void* u, const unix_photon_ev* e) -> bool {
                  return static_cast<T*>(u)->OnPhoton(*e); };)
    // One slot, four virtuals: subscribe if any of them moved, and let Module fan out.
    if constexpr (!std::is_same_v<decltype(&T::OnPlayer),       void (Module::*)(const PlayerEvent&)> ||
                  !std::is_same_v<decltype(&T::OnPlayerAwake),  void (Module::*)(Player)>             ||
                  !std::is_same_v<decltype(&T::OnPlayerJoined), void (Module::*)(Player)>             ||
                  !std::is_same_v<decltype(&T::OnPlayerLeft),   void (Module::*)(Player)>)
        d.on_player = [](void* u, const unix_player_ev* e) {
            static_cast<T*>(u)->DispatchPlayer(*e); };
    // Written out because it is the one slot whose SDK type differs from its ABI type.
    UNIX_HOOK(OnMenu, void (Module::*)(MenuPhase),
              d.on_menu = [](void* u, unix_menu_phase p) {
                  static_cast<T*>(u)->OnMenu(static_cast<MenuPhase>(p)); };)
#undef UNIX_HOOK
    return &d;
}

}   // namespace detail
}   // namespace UNIx

/// <summary>Compile-time feature test. A typo is a compile error, not a silent false.</summary>
#define UNIX_HAS(ns, n) (::UNIx::detail::g_bound.test(::UNIx::detail::kSym_##ns##_##n))

/// <summary>Record that this TU calls a symbol, so its bind request carries unix_req_report.
/// The first line of every inline service wrapper body, beside the dispatch it guards.</summary>
#define UNIX_USE(ns, n) ((void)&::UNIx::detail::used_v<::UNIx::detail::kSym_##ns##_##n>)

/// <summary>Declare a symbol load-critical: the module refuses to load without it.
/// Namespace scope, in the module's own TU.</summary>
#define UNIX_REQUIRE(ns, n)                                                              \
    namespace { const ::UNIx::detail::Require<::UNIx::detail::kSym_##ns##_##n>            \
                UNIX_CAT(ux_req_, __COUNTER__); }

/// <summary>Suppress this symbol's bind note on both channels. Namespace scope.</summary>
#define UNIX_SUPPRESS(ns, n)                                                             \
    namespace { const ::UNIx::detail::Silence<::UNIx::detail::kSym_##ns##_##n>            \
                UNIX_CAT(ux_sil_, __COUNTER__); }

/// <summary>Define one Host service accessor over the per-image singleton. Every service is
/// stateless, so the singleton is a zero-initialised empty object with no dynamic init.</summary>
#define UNIX_DEFINE_SERVICE(CLASS, ACCESSOR)                                             \
    template <class> inline ::UNIx::CLASS& ::UNIx::Host::ACCESSOR() {                    \
        static ::UNIx::CLASS s; return s;                                                \
    }

// Last, and outside the namespace they reopen — but AFTER the four macros above, because
// every wrapper body in them opens with UNIX_USE.
#include <unix/ui.hpp>
#include <unix/services/core.hpp>
#include <unix/services/ui.hpp>
#include <unix/services/udon.hpp>
#include <unix/services/notify.hpp>
#include <unix/services/player.hpp>
#if __has_include(<unix/services/photon.hpp>)
#  include <unix/services/photon.hpp>
#endif
#if __has_include(<unix/services/telemetry.hpp>)
#  include <unix/services/telemetry.hpp>
#endif
#include <unix/compat/bootstrap_module_names.hpp>

// The 23 service singletons, defined once, after every service header. A body is instantiated
// only when its accessor is called, so a service whose header this build does not ship costs
// nothing until something asks for it.
UNIX_DEFINE_SERVICE(Config,       Config)
UNIX_DEFINE_SERVICE(Menu,         Menu)
UNIX_DEFINE_SERVICE(MainMenu,     MainMenu)
UNIX_DEFINE_SERVICE(Players,      Players)
UNIX_DEFINE_SERVICE(Renderer,     Renderer)
UNIX_DEFINE_SERVICE(Log,          Log)
UNIX_DEFINE_SERVICE(FileSystem,   Fs)
UNIX_DEFINE_SERVICE(Bus,          Bus)
UNIX_DEFINE_SERVICE(Hotkeys,      Hotkeys)
UNIX_DEFINE_SERVICE(Nameplates,   Nameplates)
UNIX_DEFINE_SERVICE(Tweens,       Tweens)
UNIX_DEFINE_SERVICE(Ui,           Ui)
UNIX_DEFINE_SERVICE(Deob,         Deob)
UNIX_DEFINE_SERVICE(Studio,       Studio)
UNIX_DEFINE_SERVICE(Clipboard,    Clipboard)
UNIX_DEFINE_SERVICE(Perf,         Perf)
UNIX_DEFINE_SERVICE(License,      License)
UNIX_DEFINE_SERVICE(WorldScripts, WorldScripts)
UNIX_DEFINE_SERVICE(Console,      Console)
UNIX_DEFINE_SERVICE(Explorer,     Explorer)
UNIX_DEFINE_SERVICE(Photon,       Photon)
UNIX_DEFINE_SERVICE(Modules,      Modules)
UNIX_DEFINE_SERVICE(Telemetry,    Telemetry)
UNIX_DEFINE_SERVICE(Il2Cpp,       Il2Cpp)
UNIX_DEFINE_SERVICE(Udon,         Udon)
UNIX_DEFINE_SERVICE(PlayerApi,    PlayerApi)
UNIX_DEFINE_SERVICE(Net,          Net)
UNIX_DEFINE_SERVICE(Notify,       Notify)

template <> struct std::hash<UNIx::Player> {
    std::size_t operator()(UNIx::Player p) const noexcept { return std::hash<void*>{}(p.Raw()); }
};

// The module's own SDK MAJOR, minor and patch zeroed: a 7.3 module runs on any 7.x host and
// degrades per symbol.
#ifndef UNIX_MIN_HOST
#  define UNIX_MIN_HOST (UNIX_SDK_VERSION & 0x00FF0000u)
#endif

#define UNIX_MANIFEST(...)                                                    \
    extern "C" __declspec(dllexport) const unix_manifest UNIxModuleDecl = {   \
        .size         = (uint32_t)sizeof(unix_manifest),                      \
        .abi          = UNIX_ABI,                                             \
        .sdk_built    = UNIX_SDK_VERSION,                                     \
        .sdk_min_host = UNIX_MIN_HOST,                                        \
        __VA_ARGS__ }

/// Fills a desc from a type deriving from UNIx::Module. Optional sugar over UNIX_INIT.
#define UNIX_MODULE(TYPE, ...)                                                \
    UNIX_MANIFEST(__VA_ARGS__);                                               \
    static TYPE g_uxModule;                                                   \
    extern "C" __declspec(dllexport)                                          \
    const unix_module_desc* UNIX_CC UNIxModuleInit(const unix_context* ctx) { \
        if (!ctx || ctx->abi != UNIX_ABI || !::UNIx::detail::BindAll(ctx))     \
            return nullptr;                                                   \
        return ::UNIx::detail::DescFor(&g_uxModule);                          \
    }

/// Raw form for consumers with their own module system: fill a desc yourself.
#define UNIX_INIT(FN)                                                         \
    extern "C" __declspec(dllexport)                                          \
    const unix_module_desc* UNIX_CC UNIxModuleInit(const unix_context* ctx) { \
        if (!ctx || ctx->abi != UNIX_ABI || !::UNIx::detail::BindAll(ctx))     \
            return nullptr;                                                   \
        return FN(ctx);                                                       \
    }

/// In-image form for a statically linked shim: UNIX_MODULE with both dllexports dropped and
/// every emitted name parameterised on TYPE. BindAll keeps the identity IssueStaticInstance minted.
#define UNIX_STATIC_MODULE(TYPE, ...)                                              \
    extern "C" const unix_manifest UNIxModuleDecl_##TYPE = {                       \
        .size = (uint32_t)sizeof(unix_manifest), .abi = UNIX_ABI,                  \
        .sdk_built = UNIX_SDK_VERSION, .sdk_min_host = UNIX_MIN_HOST,              \
        __VA_ARGS__ };                                                             \
    static TYPE g_uxModule_##TYPE;                                                 \
    const unix_module_desc* UNIX_CC UNIxModuleInit_##TYPE(const unix_context* ctx) {\
        if (!ctx || ctx->abi != UNIX_ABI ||                                        \
            !::UNIx::detail::BindAll(ctx, ::UNIx::detail::g_self))                 \
            return nullptr;                                                        \
        return ::UNIx::detail::DescFor(&g_uxModule_##TYPE);                        \
    }

/// <summary>Declare a constant-initialised string table for a manifest list field.
/// Namespace scope, above the UNIX_MODULE that names it.</summary>
#define UNIX_STRINGS(NAME, ...) inline constexpr const char* const NAME[] = { __VA_ARGS__ }

// unix_manifest spells the list pointer `requires_` but its count `requires_count`, so the
// count designator is looked up rather than pasted from the field name.
#define UNIX_LIST_COUNT_provides  provides_count
#define UNIX_LIST_COUNT_requires_ requires_count
#define UNIX_LIST_COUNT_opt_in    opt_in_count

/// <summary>Expand a UNIX_STRINGS table as a manifest list field and its count, in
/// unix_manifest declaration order.</summary>
#define UNIX_LIST(FIELD, NAME)                                                \
    .FIELD = NAME,                                                            \
    .UNIX_CAT(UNIX_LIST_COUNT_, FIELD) = (uint32_t)(sizeof(NAME) / sizeof(*(NAME)))
