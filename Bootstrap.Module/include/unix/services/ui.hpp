#pragma once
#ifndef UNIX_USE
#  error "<unix/services/ui.hpp> is included by <unix/module.hpp>; include that instead"
#endif

#include <cstddef>

// Ui::Adopt drives the module's own ImGui copy. Declaring the three entry points here keeps
// <imgui.h> out of the SDK; include <imgui.h> first and these are skipped entirely. Only a
// module that calls Adopt instantiates them, so a module without ImGui still links.
#ifndef IMGUI_VERSION
struct ImGuiContext;
typedef void* (*ImGuiMemAllocFunc)(size_t sz, void* user_data);
typedef void  (*ImGuiMemFreeFunc)(void* ptr, void* user_data);
namespace ImGui {
const char* GetVersion();
void SetCurrentContext(ImGuiContext* ctx);
void SetAllocatorFunctions(ImGuiMemAllocFunc alloc_func, ImGuiMemFreeFunc free_func,
                           void* user_data);
}   // namespace ImGui
#endif

namespace UNIx {

namespace detail {
inline constexpr uint32_t kAllPlayerPhases = (1u << 5) - 1u;
constexpr uint32_t PhaseBit(unix_player_phase p) noexcept {
    return 1u << static_cast<uint32_t>(p);
}
constexpr uint32_t PhaseBit(MenuPhase p) noexcept { return 1u << static_cast<uint32_t>(p); }
}   // namespace detail

/// <summary>Owned copy of the user the menu has selected; the callback form gets a borrowed
/// UserEvent instead.</summary>
struct SelectedUser {
    void* player = nullptr;
    void* api_user = nullptr;
    std::string user_id, display_name;
    uint32_t kind = 0;
};

/// <summary>Owned copy of everything the host knows about the selected user. `valid` is false
/// when no user page is open.</summary>
struct UserInfo {
    std::string user_id, display_name, avatar_id, platform, status;
    std::string avatar_name, avatar_release;
    UserPageKind kind = UserPageKind::None;
    PlayerRank rank = PlayerRank::Visitor;
    /// <summary>The Photon actor number; -1 outside the instance.</summary>
    int32_t player_id = -1;
    /// <summary>Time the user has been in the instance, 0 when unknown.</summary>
    uint64_t joined_ms = 0;
    bool is_local = false, in_instance = false, is_friend = false, on_mobile = false,
         age_verified = false, valid = false;
};

/// <summary>The 14-slot user page.</summary>
class UserPage {
public:
    /// <summary>A mini icon button in the page header, beside VRChat's own.</summary>
    /// <param name="tooltip">Hover text; the button shows only its icon.</param>
    template <class = void> UIUserButton AddHeaderButton(std::string_view tooltip, Sprite icon,
                                                         std::function<void(const UserEvent&)> onClick,
                                                         UserPageAudience audience = UserPageAudience::All);
    template <class = void> [[nodiscard]] UserInfo GetInfo() const;
    /// <summary>A built-in pill in the info strip under the profile header, above every action
    /// row. Refreshed on selection change and at most twice a second while the page is open.</summary>
    /// <param name="line">Strip line, 0 or 1.</param>
    template <class = void> UIUserPill AddInfoPill(UserPill kind, int line = 0,
                                                   UserPageAudience audience = UserPageAudience::All);
    /// <summary>A pill whose text the module supplies; returning nothing hides it for that pass.</summary>
    /// <param name="line">Strip line, 0 or 1.</param>
    template <class = void> UIUserPill AddInfoPill(
        std::function<std::optional<PillText>(const UserEvent&)> provide, int line = 0,
        UserPageAudience audience = UserPageAudience::All);
    /// <summary>Re-sample the open page, re-ask every SetVisibleWhen predicate and re-fire
    /// OnSelect. unix_e_absent when no user page is open.</summary>
    template <class = void> Status Refresh();
    /// <summary>A pooled row. Fill it with UIUserRow::AddButton.</summary>
    template <class = void> UIUserRow AddRow(UserPageAudience audience = UserPageAudience::All);
    template <class = void> UIUserButton AddButton(std::string_view text, Sprite icon,
                                                   std::function<void(const UserEvent&)> onClick,
                                                   UserPageAudience audience = UserPageAudience::All);
    template <class = void> UISubPage AddSubPage(std::string_view name);
    template <class = void> [[nodiscard]] Subscription OnSelect(
        std::function<void(const UserEvent&)> fn);
    template <class = void> [[nodiscard]] SelectedUser GetSelected() const;
    /// <summary>The open user page's GameObject, or null when none is open.</summary>
    template <class = void> [[nodiscard]] void* GetPageObject() const;
    /// <summary>The container of the open user page's action rows, or null.</summary>
    template <class = void> [[nodiscard]] void* GetActionsObject() const;
};

/// <summary>One entry of the QuickMenu's page stack. `page` is set only for this module's
/// pages; another module's page has an empty title and neither flag.</summary>
struct PageCrumb {
    UISurface   page{};
    std::string title;
    bool mine = false, native = false, top = false;
};

/// <summary>The QuickMenu itself. A page is created by constructing a UIPage.</summary>
class Menu {
public:
    /// <summary>Run build once the QuickMenu is live and sprites are cached, or immediately if
    /// that phase already fired. Subscribed off the Unity main thread, a phase that already
    /// fired runs on the next main-thread pump instead.</summary>
    template <class = void> void OnReady(std::function<void()> build);
    /// <summary>Run build at one menu lifecycle phase.</summary>
    template <class = void> void OnPhase(MenuPhase phase, std::function<void()> build);
    template <class = void> [[nodiscard]] bool IsReady() const;
    template <class = void> [[nodiscard]] ::UNIx::UserPage& UserPage();
    /// <returns>True when a page was popped; false on a top-level page or a native page.</returns>
    template <class = void> bool NavigateBack();
    /// <summary>Return to the Launchpad and clear the page stack.</summary>
    template <class = void> Status NavigateRoot();
    /// <summary>The QuickMenu's page stack, root first.</summary>
    template <class = void> [[nodiscard]] std::vector<PageCrumb> PageStack() const;
    /// <summary>A plain-text report of the menu's page state, for diagnostics.</summary>
    template <class = void> [[nodiscard]] std::string DumpPages() const;
    /// <summary>True while the QuickMenu is open.</summary>
    template <class = void> [[nodiscard]] bool IsOpen() const;
    /// <summary>Be told when the QuickMenu or the MainMenu opens or closes.</summary>
    template <class = void> [[nodiscard]] Subscription OnOpen(std::function<void(MenuKind, bool open)> fn);
    /// <summary>Be told when one of VRChat's own pages shows or hides.</summary>
    template <class = void> [[nodiscard]] Subscription OnNativePage(
        std::function<void(const NativePageEvent&)> fn);
    /// <summary>Open one of VRChat's own QuickMenu pages by GameObject name, such as
    /// Menu_QM_GeneralSettings. unix_e_absent for an unknown name.</summary>
    template <class = void> Status OpenNativePage(std::string_view name);
    /// <summary>The GameObject name of the page on top of the QuickMenu's stack.</summary>
    template <class = void> [[nodiscard]] std::string GetCurrentPageName() const;
    /// <summary>A foldout parented under any GameObject, such as one of VRChat's own pages.</summary>
    template <class = void> UIFoldout AddFoldoutIn(void* gameObject, std::string_view title,
                                                   bool expanded = true, bool background = true,
                                                   bool autoSeparators = true);
    /// <summary>A mini button pinned into the corner of any GameObject.</summary>
    template <class = void> UICornerButton AddCornerButtonOn(void* gameObject, Sprite icon,
                                                             std::function<void()> onClick,
                                                             bool topLeft = false);
    /// <summary>Set the sprite on any UnityEngine.UI.Image.</summary>
    template <class = void> Status SetImageSprite(void* image, Sprite icon);
    template <class = void> Status SetImageSprite(void* image, void* sprite);
    /// <summary>Turn the menu's hover effects on or off. Process-wide, not per module.</summary>
    template <class = void> Status SetHoverEffects(bool on);
};

/// <summary>VRChat's MainMenu. A page is a tab holding a side list; controls go on its
/// sections.</summary>
class MainMenu {
public:
    /// <summary>Run build once VRChat's MainMenu is ready, or immediately if it already is.
    /// Use this rather than Menu::OnReady, which fires too early to build one of these.</summary>
    template <class = void> void OnReady(std::function<void()> build);
    template <class = void> [[nodiscard]] bool IsReady() const;
    /// <param name="name">Tab label, and the panel's title.</param>
    template <class = void> [[nodiscard]] UIMainMenuPage CreatePage(std::string_view name,
                                                                    std::string_view tooltip = "");
    /// <summary>A section in VRChat's own Settings list rather than a page of your own.</summary>
    template <class = void> [[nodiscard]] UISection AddSettingsSection(std::string_view name);
    /// <summary>Re-acquire a page this module already made. Never creates.</summary>
    template <class = void> [[nodiscard]] UIMainMenuPage FindPage(std::string_view name);
    /// <summary>The MainMenu's root GameObject, or null before it is ready.</summary>
    template <class = void> [[nodiscard]] void* GetRootObject() const;
    /// <summary>Pop the MainMenu's top page, as its Back button does.</summary>
    template <class = void> void NavigateBack();
};

/// <summary>The players in the instance, and the local one.</summary>
class Players {
public:
    template <class = void> [[nodiscard]] Player GetLocal() const;
    template <class = void> [[nodiscard]] void* GetLocalApiUser() const;
    template <class = void> [[nodiscard]] void* GetLocalVrcPlayer() const;
    template <class = void> [[nodiscard]] std::vector<Player> GetAll() const;
    template <class = void> [[nodiscard]] Subscription OnJoin(std::function<void(Player)> fn);
    template <class = void> [[nodiscard]] Subscription OnLeave(std::function<void(Player)> fn);
    template <class = void> [[nodiscard]] Subscription OnAwake(std::function<void(Player)> fn);
    template <class = void> [[nodiscard]] Subscription OnEvent(
        std::function<void(const PlayerEvent&)> fn);
    template <class = void> [[nodiscard]] Color GetRankColor(PlayerRank rank) const;
    /// <summary>The rank of a VRChat APIUser.</summary>
    template <class = void> [[nodiscard]] PlayerRank GetRankOf(void* apiUser) const;
    template <class = void> Status InjectEvent(Player p, unix_player_phase phase);
};

/// <summary>The present path.</summary>
class Renderer {
public:
    /// <summary>Inside the host's ImGui frame. Do not call NewFrame or Render.</summary>
    template <class = void> [[nodiscard]] Subscription OnFrame(
        std::function<void(const Frame&)> fn);
    /// <summary>After every OnFrame, before Present, outside any host frame.</summary>
    template <class = void> [[nodiscard]] Subscription OnPresent(
        std::function<void(const Frame&)> fn);
};

/// <summary>Custom nameplate rows.</summary>
class Nameplates {
public:
    /// <param name="tag">Caller-chosen id; DestroyByTag matches on its prefix.</param>
    template <class = void> Nameplate Create(Player p, Vec3 offset, std::string_view text,
                                             std::string_view tag);
    /// <summary>Destroy every plate this module made for the player whose tag starts with the
    /// prefix.</summary>
    template <class = void> void DestroyByTag(Player p, std::string_view tagPrefix);
    /// <summary>Process-global, not per-module. Needs unix_cap_host_settings.</summary>
    template <class = void> Status SetCullDistance(float metres);
    /// <summary>Process-global, not per-module. Needs unix_cap_host_settings.</summary>
    template <class = void> Status SetRedundantSuppression(bool on);
};

/// <summary>Everything one tween needs. Components of From and To beyond the property's arity
/// are ignored.</summary>
struct TweenDesc {
    /// <summary>A GameObject or Component; null for TweenProperty::Float.</summary>
    void* Target = nullptr;
    TweenProperty Property = TweenProperty::LocalPosition;
    /// <summary>Start value; empty starts from the current value.</summary>
    std::optional<Vec4> From{};
    Vec4 To{};
    /// <summary>For `TweenProperty::Float`: the host writes the value here every frame on the
    /// Unity main thread. The float must outlive the tween, or the tween must be cancelled
    /// first; after `Tween::Cancel` returns the host never touches it again.</summary>
    float* Bound = nullptr;
    uint32_t DurationMs = 250, DelayMs = 0;
    Ease Curve = Ease::EaseInOut;
    /// <summary>Back overshoot, elastic amplitude and period, spring damping and frequency, or
    /// bezier x1, y1, x2, y2.</summary>
    std::optional<std::array<float, 4>> CurveParams{};
    /// <summary>Play count; -1 repeats until cancelled.</summary>
    int32_t Loops = 1;
    bool Yoyo = false, Unscaled = true;
    /// <summary>Called once when the tween ends: `true` when it finished, `false` when the host
    /// cut it short. Not called after `Tween::Cancel`.</summary>
    std::function<void(bool completed)> OnDone{};
};

/// <summary>One step of a sequence: a tween, or a wait when WaitMs is set and the tween has
/// neither Target nor Bound.</summary>
struct TweenStep {
    TweenDesc Tween{};
    uint32_t WaitMs = 0;
    /// <summary>Run this tween together with the step before it.</summary>
    bool WithPrevious = false;
};

/// <summary>Host-driven interpolations.</summary>
class Tweens {
public:
    /// <summary>Start one tween.</summary>
    template <class = void> Tween Play(const TweenDesc& desc);
    /// <summary>Start steps one after another, as one tween handle. Each step's OnDone may fire
    /// once per loop.</summary>
    /// <param name="loops">Sequence play count; -1 repeats until cancelled.</param>
    template <class = void> Tween PlaySequence(std::span<const TweenStep> steps, int32_t loops = 1,
                                               std::function<void(bool completed)> onDone = {});
    template <class = void> [[deprecated("use Tweens::Play with TweenProperty::AnchoredPosition")]]
    Tween AnchoredPos(void* rectTransform, Vec2 from, Vec2 to,
                      uint32_t ms, Ease ease = Ease::Linear,
                      std::function<void(bool completed)> onDone = {});
    template <class = void> [[deprecated("use Tweens::Play with TweenProperty::LocalPosition")]]
    Tween LocalPos(void* transform, Vec3 from, Vec3 to, uint32_t ms,
                   Ease ease = Ease::Linear,
                   std::function<void(bool completed)> onDone = {});
    template <class = void> [[deprecated("use Tweens::Play with TweenProperty::LocalScale")]]
    Tween LocalScale(void* transform, Vec3 from, Vec3 to, uint32_t ms,
                     Ease ease = Ease::Linear,
                     std::function<void(bool completed)> onDone = {});
    template <class = void> [[deprecated("use Tweens::Play with TweenProperty::Float and TweenDesc::Bound")]]
    Tween Value(float from, float to, uint32_t ms,
                Ease ease = Ease::Linear,
                std::function<void(bool completed)> onDone = {});
    template <class = void> void CancelAll();
    template <class = void> void CancelTarget(void* target);
};

/// <summary>The host's ImGui context, the input claim and the loader's resources.</summary>
class Ui {
public:
    /// <summary>Adopt the host's ImGui context and allocators for this image. Call once at the
    /// top of OnRender; false means the module's ImGui build does not match the host's — do not
    /// draw.</summary>
    template <class = void> [[nodiscard]] bool Adopt(const Frame& f);
    template <class = void> [[nodiscard]] void* GetImGuiContext() const;
    /// <summary>Claim a screen rect for this frame so input over it routes here. Cleared every
    /// frame.</summary>
    template <class = void> void ClaimRegion(Rect r);
    template <class = void> [[nodiscard]] bool Contains(Vec2 point) const;
    /// <summary>Hold the cursor unlocked while want is true. One claim per module; released on
    /// unload.</summary>
    template <class = void> void ClaimCursor(bool want);
    /// <summary>A loader-shipped resource by name. Empty if absent or denied.</summary>
    template <class = void> [[nodiscard]] std::vector<uint8_t> GetLoaderResource(
        std::string_view name) const;
    /// <summary>Open VRChat's world keyboard. Empty when OnSubmit is empty or the keyboard is
    /// not ready.</summary>
    template <class = void> KeyboardRequest OpenKeyboard(const KeyboardDesc& desc);
    /// <summary>True once the world keyboard can be opened.</summary>
    template <class = void> [[nodiscard]] bool IsKeyboardReady() const;
};

// ---------------------------------------------------------------------------------------
// Bodies.
// ---------------------------------------------------------------------------------------

namespace detail {

inline Subscription Subscribe(unix_handle h, Node* n) {
    if (!h) { OwnNode(unix_null, n); return {}; }
    return Subscription{ h, n };
}

inline Subscription SubscribePlayer(uint32_t phaseMask, std::function<void(const PlayerEvent&)> fn) {
    Node* n = AllocNode([f = std::move(fn)](const void* p) {
        f(*static_cast<const PlayerEvent*>(p));
    });
    return Subscribe(g_d.players_on_event(g_self, phaseMask, &TrampPlayer, n), n);
}

}   // namespace detail

template <class>
UIUserRow UserPage::AddRow(UserPageAudience audience) {
    UNIX_USE(users, add_row);
    return UIUserRow{ detail::g_d.users_add_row(detail::g_self,
                                                static_cast<uint32_t>(audience)) };
}

template <class>
UIUserButton UserPage::AddButton(std::string_view text, Sprite icon,
                                 std::function<void(const UserEvent&)> onClick,
                                 UserPageAudience audience) {
    UNIX_USE(users, add_button);
    detail::Node* n = nullptr;
    if (onClick) {
        n = detail::AllocNode([f = std::move(onClick)](const void* p) {
            f(*static_cast<const UserEvent*>(p));
        });
    }
    unix_button_desc d{};
    d.size     = sizeof(d);
    d.text     = Sv(text);
    d.icon     = detail::MakeIcon(icon);
    d.on_click = n ? reinterpret_cast<unix_cb_widget>(&detail::TrampUser) : nullptr;
    d.ud       = n;
    const unix_handle h = detail::g_d.users_add_button(detail::g_self, &d,
                                                       static_cast<uint32_t>(audience));
    detail::OwnNode(h, n);
    return UIUserButton{ h };
}

template <class>
UISubPage UserPage::AddSubPage(std::string_view name) {
    UNIX_USE(users, add_sub_page);
    return UISubPage{ detail::g_d.users_add_sub_page(detail::g_self, Sv(name)) };
}

template <class>
Subscription UserPage::OnSelect(std::function<void(const UserEvent&)> fn) {
    UNIX_USE(users, on_select);
    detail::Node* n = detail::AllocNode([f = std::move(fn)](const void* p) {
        f(*static_cast<const UserEvent*>(p));
    });
    return detail::Subscribe(detail::g_d.users_on_select(detail::g_self, &detail::TrampUser, n), n);
}

template <class>
SelectedUser UserPage::GetSelected() const {
    UNIX_USE(users, selected);
    UNIX_USE(users, selected_id);
    UNIX_USE(users, selected_name);
    UNIX_USE(users, selected_kind);
    SelectedUser u{};
    u.api_user = nullptr;
    u.player   = detail::g_d.users_selected(detail::g_self, &u.api_user);
    u.user_id  = detail::ReadStr([](char* b, uint32_t c) {
        return detail::g_d.users_selected_id(detail::g_self, b, c);
    });
    u.display_name = detail::ReadStr([](char* b, uint32_t c) {
        return detail::g_d.users_selected_name(detail::g_self, b, c);
    });
    u.kind = detail::g_d.users_selected_kind(detail::g_self);
    return u;
}

template <class>
UIUserButton UserPage::AddHeaderButton(std::string_view tooltip, Sprite icon,
                                       std::function<void(const UserEvent&)> onClick,
                                       UserPageAudience audience) {
    UNIX_USE(users, add_header_button);
    if (!onClick) return UIUserButton{};
    detail::Node* n = detail::AllocNode([f = std::move(onClick)](const void* p) {
        f(*static_cast<const UserEvent*>(p));
    });
    unix_button_desc d{};
    d.size     = sizeof(d);
    d.text     = Sv(tooltip);
    d.tooltip  = Sv(tooltip);
    d.icon     = detail::MakeIcon(icon);
    d.on_click = reinterpret_cast<unix_cb_widget>(&detail::TrampUser);
    d.ud       = n;
    const unix_handle h = detail::g_d.users_add_header_button(detail::g_self, &d,
                                                              static_cast<uint32_t>(audience));
    detail::OwnNode(h, n);
    return UIUserButton{ h };
}

template <class>
UserInfo UserPage::GetInfo() const {
    UNIX_USE(users, selected_info);
    unix_user_info raw{};
    raw.size = sizeof(raw);
    UserInfo out{};
    if (detail::g_d.users_selected_info(detail::g_self, &raw) != unix_ok) return out;
    out.user_id      = raw.user_id;
    out.display_name = raw.display_name;
    out.avatar_id    = raw.avatar_id;
    out.platform     = raw.platform;
    out.status       = raw.status;
    out.kind         = static_cast<UserPageKind>(raw.kind);
    out.rank         = static_cast<PlayerRank>(raw.rank);
    out.player_id    = raw.player_id;
    out.is_local     = raw.is_local;
    out.in_instance  = raw.in_instance;
    out.is_friend    = raw.is_friend;
    out.on_mobile    = raw.on_mobile;
    out.age_verified = raw.age_verified;
    if (raw.size >= offsetof(unix_user_info, avatar_name) + sizeof(raw.avatar_name))
        out.avatar_name = raw.avatar_name;
    if (raw.size >= offsetof(unix_user_info, avatar_release) + sizeof(raw.avatar_release))
        out.avatar_release = raw.avatar_release;
    if (raw.size >= offsetof(unix_user_info, joined_ms) + sizeof(raw.joined_ms))
        out.joined_ms = raw.joined_ms;
    out.valid        = true;
    return out;
}

template <class>
UIUserPill UserPage::AddInfoPill(UserPill kind, int line, UserPageAudience audience) {
    UNIX_USE(users, add_info_pill);
    unix_user_pill_desc d{};
    d.size     = sizeof(d);
    d.kind     = static_cast<uint32_t>(kind);
    d.line     = line > 0 ? 1u : 0u;
    d.audience = static_cast<uint32_t>(audience);
    return UIUserPill{ detail::g_d.users_add_info_pill(detail::g_self, &d) };
}

template <class>
UIUserPill UserPage::AddInfoPill(std::function<std::optional<PillText>(const UserEvent&)> provide,
                                 int line, UserPageAudience audience) {
    UNIX_USE(users, add_info_pill);
    if (!provide) return UIUserPill{};
    detail::Node* n = detail::AllocNodeRaw();
    n->ret = [f = std::move(provide)](const void* p) -> bool {
        const auto* call = static_cast<const detail::UserPillCall*>(p);
        std::optional<PillText> r = f(*static_cast<const UserEvent*>(call->ev));
        if (!r || r->text.empty()) return false;
        detail::CopyFixed(call->out->text, r->text);
        detail::CopyFixed(call->out->copy, r->copy);
        detail::CopyFixed(call->out->tooltip, r->tooltip);
        if (r->color) {
            call->out->color  = *r->color;
            call->out->tinted = true;
        }
        return true;
    };
    unix_user_pill_desc d{};
    d.size     = sizeof(d);
    d.kind     = unix_pill_custom;
    d.line     = line > 0 ? 1u : 0u;
    d.audience = static_cast<uint32_t>(audience);
    d.provide  = &detail::TrampUserPill;
    d.ud       = n;
    const unix_handle h = detail::g_d.users_add_info_pill(detail::g_self, &d);
    detail::OwnNode(h, n);
    return UIUserPill{ h };
}

template <class>
Status UserPage::Refresh() {
    UNIX_USE(users, refresh);
    return detail::g_d.users_refresh(detail::g_self);
}

template <class>
void* UserPage::GetPageObject() const {
    UNIX_USE(users, page_object);
    return detail::g_d.users_page_object(detail::g_self);
}

template <class>
void* UserPage::GetActionsObject() const {
    UNIX_USE(users, actions_object);
    return detail::g_d.users_actions_object(detail::g_self);
}

template <class>
void Menu::OnReady(std::function<void()> build) {
    OnPhase(MenuPhase::QuickMenuPostSetup, std::move(build));
}

template <class>
void Menu::OnPhase(MenuPhase phase, std::function<void()> build) {
    UNIX_USE(menu, on_event);
    if (!build) return;
    detail::Node* n = detail::AllocNode([f = std::move(build)](const void*) { f(); });
    const unix_handle h = detail::g_d.menu_on_event(detail::g_self, detail::PhaseBit(phase),
                                                    &detail::TrampMenu, n);
    detail::OwnNode(h, n);
}

template <class>
bool Menu::IsReady() const {
    UNIX_USE(menu, is_ready);
    return detail::g_d.menu_is_ready(detail::g_self);
}

template <class>
::UNIx::UserPage& Menu::UserPage() {
    static ::UNIx::UserPage page{};
    return page;
}

template <class>
bool Menu::NavigateBack() {
    UNIX_USE(menu, navigate_back);
    return detail::g_d.menu_navigate_back(detail::g_self) == unix_ok;
}

template <class>
Status Menu::NavigateRoot() {
    UNIX_USE(menu, navigate_root);
    return detail::g_d.menu_navigate_root(detail::g_self);
}

template <class>
std::vector<PageCrumb> Menu::PageStack() const {
    UNIX_USE(menu, page_stack);
    const uint32_t need = detail::g_d.menu_page_stack(detail::g_self, nullptr, 0);
    if (need == 0 || need >= 0xFFFFFF00u) return {};
    std::vector<unix_page_crumb> raw(need);
    for (auto& c : raw) c.size = static_cast<uint32_t>(sizeof(unix_page_crumb));
    const uint32_t got = detail::g_d.menu_page_stack(detail::g_self, raw.data(), need);
    if (got == 0 || got >= 0xFFFFFF00u) return {};
    const uint32_t n = got < need ? got : need;
    std::vector<PageCrumb> out;
    out.reserve(n);
    for (uint32_t i = 0; i < n; ++i) {
        const unix_page_crumb& c = raw[i];
        std::size_t len = 0;
        while (len < sizeof(c.title) && c.title[len]) ++len;
        PageCrumb p{};
        p.page   = UISurface{ c.page };
        p.title.assign(c.title, len);
        p.mine   = (c.flags & unix_crumb_mine) != 0;
        p.native = (c.flags & unix_crumb_native) != 0;
        p.top    = (c.flags & unix_crumb_top) != 0;
        out.push_back(std::move(p));
    }
    return out;
}

template <class>
std::string Menu::DumpPages() const {
    UNIX_USE(menu, dump_pages);
    return detail::ReadStr([](char* b, uint32_t c) {
        return detail::g_d.menu_dump_pages(detail::g_self, b, c);
    });
}

template <class>
bool Menu::IsOpen() const {
    UNIX_USE(menu, is_open);
    return detail::g_d.menu_is_open(detail::g_self);
}

template <class>
Subscription Menu::OnOpen(std::function<void(MenuKind, bool)> fn) {
    UNIX_USE(menu, on_open);
    if (!fn) return {};
    detail::Node* n = detail::NodeFor<unix_menu_open_ev>(
        [f = std::move(fn)](const unix_menu_open_ev& e) { f(static_cast<MenuKind>(e.menu), e.open); });
    const unix_handle h = detail::g_d.menu_on_open(detail::g_self, &detail::Tramp<unix_menu_open_ev>, n);
    if (h == unix_null) { detail::FreeNode(n); return {}; }
    return Subscription{ h, n };
}

template <class>
Subscription Menu::OnNativePage(std::function<void(const NativePageEvent&)> fn) {
    UNIX_USE(menu, on_native_page);
    if (!fn) return {};
    detail::Node* n = detail::NodeFor<unix_native_page_ev>(std::move(fn));
    const unix_handle h = detail::g_d.menu_on_native_page(detail::g_self,
                                                          &detail::Tramp<unix_native_page_ev>, n);
    if (h == unix_null) { detail::FreeNode(n); return {}; }
    return Subscription{ h, n };
}

template <class>
Status Menu::OpenNativePage(std::string_view name) {
    UNIX_USE(menu, open_native_page);
    return detail::g_d.menu_open_native_page(detail::g_self, Sv(name));
}

template <class>
std::string Menu::GetCurrentPageName() const {
    UNIX_USE(menu, current_page);
    return detail::ReadStr([](char* b, uint32_t c) {
        return detail::g_d.menu_current_page(detail::g_self, b, c);
    });
}

template <class>
UIFoldout Menu::AddFoldoutIn(void* gameObject, std::string_view title, bool expanded,
                             bool background, bool autoSeparators) {
    UNIX_USE(menu, add_foldout_in);
    unix_foldout_desc d{};
    d.size            = sizeof(d);
    d.title           = Sv(title);
    d.expanded        = expanded;
    d.background      = background;
    d.auto_separators = autoSeparators;
    return UIFoldout{ detail::g_d.menu_add_foldout_in(detail::g_self, gameObject, &d) };
}

template <class>
UICornerButton Menu::AddCornerButtonOn(void* gameObject, Sprite icon,
                                       std::function<void()> onClick, bool topLeft) {
    UNIX_USE(menu, add_corner_button_on);
    if (!onClick) return UICornerButton{};
    detail::Node* n = detail::AllocNode([f = std::move(onClick)](const void*) { f(); });
    const unix_handle h = detail::g_d.menu_add_corner_button_on(
        detail::g_self, gameObject, detail::MakeIcon(icon), topLeft, &detail::TrampWidget, n);
    detail::OwnNode(h, n);
    return UICornerButton{ h };
}

template <class>
Status Menu::SetImageSprite(void* image, Sprite icon) {
    UNIX_USE(menu, set_image_sprite);
    return detail::g_d.menu_set_image_sprite(detail::g_self, image, detail::MakeIcon(icon));
}

template <class>
Status Menu::SetImageSprite(void* image, void* sprite) {
    UNIX_USE(menu, set_image_sprite);
    return detail::g_d.menu_set_image_sprite(detail::g_self, image, detail::MakeIcon(sprite));
}

template <class>
Status Menu::SetHoverEffects(bool on) {
    UNIX_USE(menu, set_hover_fx);
    return detail::g_d.menu_set_hover_fx(detail::g_self, on);
}

template <class>
void MainMenu::OnReady(std::function<void()> build) {
    Host::Get().Menu().OnPhase(MenuPhase::MainMenuSetup, std::move(build));
}

template <class>
bool MainMenu::IsReady() const {
    UNIX_USE(mainmenu, is_ready);
    return detail::g_d.mainmenu_is_ready(detail::g_self);
}

template <class>
UIMainMenuPage MainMenu::CreatePage(std::string_view name, std::string_view tooltip) {
    UNIX_USE(mainmenu, create_page);
    return UIMainMenuPage{ detail::g_d.mainmenu_create_page(detail::g_self, Sv(name),
                                                            Sv(tooltip)) };
}

template <class>
UISection MainMenu::AddSettingsSection(std::string_view name) {
    UNIX_USE(mainmenu, settings_section);
    return UISection{ detail::g_d.mainmenu_settings_section(detail::g_self, Sv(name)) };
}

template <class>
UIMainMenuPage MainMenu::FindPage(std::string_view name) {
    UNIX_USE(mainmenu, find_page);
    return UIMainMenuPage{ detail::g_d.mainmenu_find_page(detail::g_self, Sv(name)) };
}

template <class>
void* MainMenu::GetRootObject() const {
    UNIX_USE(mainmenu, root_object);
    return detail::g_d.mainmenu_root_object(detail::g_self);
}

template <class>
void MainMenu::NavigateBack() {
    UNIX_USE(mainmenu, navigate_back);
    detail::g_d.mainmenu_navigate_back(detail::g_self);
}

template <class>
Player Players::GetLocal() const {
    UNIX_USE(players, local);
    return Player{ detail::g_d.players_local(detail::g_self) };
}

template <class>
void* Players::GetLocalApiUser() const {
    UNIX_USE(players, local_api);
    return detail::g_d.players_local_api(detail::g_self);
}

template <class>
void* Players::GetLocalVrcPlayer() const {
    UNIX_USE(players, local_vrc);
    return detail::g_d.players_local_vrc(detail::g_self);
}

template <class>
std::vector<Player> Players::GetAll() const {
    UNIX_USE(players, all);
    const uint32_t need = detail::g_d.players_all(detail::g_self, nullptr, 0);
    if (need == 0 || need >= 0xFFFFFF00u) return {};
    std::vector<void*> raw(need);
    const uint32_t got = detail::g_d.players_all(detail::g_self, raw.data(), need);
    if (got == 0 || got > need) return {};
    std::vector<Player> out;
    out.reserve(got);
    for (uint32_t i = 0; i < got; ++i) out.emplace_back(raw[i]);
    return out;
}

template <class>
Subscription Players::OnJoin(std::function<void(Player)> fn) {
    UNIX_USE(players, on_event);
    return detail::SubscribePlayer(detail::PhaseBit(unix_player_joined),
                                   [f = std::move(fn)](const PlayerEvent& e) {
                                       f(Player{ e.player });
                                   });
}

template <class>
Subscription Players::OnLeave(std::function<void(Player)> fn) {
    UNIX_USE(players, on_event);
    return detail::SubscribePlayer(detail::PhaseBit(unix_player_left),
                                   [f = std::move(fn)](const PlayerEvent& e) {
                                       f(Player{ e.player });
                                   });
}

template <class>
Subscription Players::OnAwake(std::function<void(Player)> fn) {
    UNIX_USE(players, on_event);
    return detail::SubscribePlayer(detail::PhaseBit(unix_player_awake),
                                   [f = std::move(fn)](const PlayerEvent& e) {
                                       f(Player{ e.player });
                                   });
}

template <class>
Subscription Players::OnEvent(std::function<void(const PlayerEvent&)> fn) {
    UNIX_USE(players, on_event);
    return detail::SubscribePlayer(detail::kAllPlayerPhases, std::move(fn));
}

template <class>
Color Players::GetRankColor(PlayerRank rank) const {
    UNIX_USE(players, rank_color);
    return detail::g_d.players_rank_color(detail::g_self, static_cast<uint32_t>(rank));
}

template <class>
PlayerRank Players::GetRankOf(void* apiUser) const {
    UNIX_USE(players, rank);
    return static_cast<PlayerRank>(detail::g_d.players_rank(detail::g_self, apiUser));
}

template <class>
Status Players::InjectEvent(Player p, unix_player_phase phase) {
    UNIX_USE(players, inject_event);
    return detail::g_d.players_inject_event(detail::g_self, p.Raw(),
                                            static_cast<uint32_t>(phase));
}

template <class>
Subscription Renderer::OnFrame(std::function<void(const Frame&)> fn) {
    UNIX_USE(render, on_frame);
    detail::Node* n = detail::AllocNode([f = std::move(fn)](const void* p) {
        f(*static_cast<const Frame*>(p));
    });
    return detail::Subscribe(detail::g_d.render_on_frame(detail::g_self, &detail::TrampRender, n),
                             n);
}

template <class>
Subscription Renderer::OnPresent(std::function<void(const Frame&)> fn) {
    UNIX_USE(render, on_present);
    detail::Node* n = detail::AllocNode([f = std::move(fn)](const void* p) {
        f(*static_cast<const Frame*>(p));
    });
    return detail::Subscribe(detail::g_d.render_on_present(detail::g_self, &detail::TrampRender, n),
                             n);
}

template <class>
Nameplate Nameplates::Create(Player p, Vec3 offset, std::string_view text, std::string_view tag) {
    UNIX_USE(plates, create);
    return Nameplate{ detail::g_d.plates_create(detail::g_self, p.Raw(), offset, Sv(text),
                                                Sv(tag)) };
}

template <class>
void Nameplates::DestroyByTag(Player p, std::string_view tagPrefix) {
    UNIX_USE(plates, destroy_by_tag);
    detail::g_d.plates_destroy_by_tag(detail::g_self, p.Raw(), Sv(tagPrefix));
}

template <class>
Status Nameplates::SetCullDistance(float metres) {
    UNIX_USE(plates, set_cull_distance);
    return detail::g_d.plates_set_cull_distance(detail::g_self, metres);
}

template <class>
Status Nameplates::SetRedundantSuppression(bool on) {
    UNIX_USE(plates, set_redundant_suppression);
    return detail::g_d.plates_set_redundant_suppression(detail::g_self, on);
}

namespace detail {

constexpr uint32_t TweenArity(TweenProperty p) noexcept {
    switch (p) {
        case TweenProperty::AnchoredPosition:
        case TweenProperty::SizeDelta:        return 2;
        case TweenProperty::LocalPosition:
        case TweenProperty::LocalScale:
        case TweenProperty::LocalEulerAngles: return 3;
        case TweenProperty::Color:            return 4;
        default:                              return 1;
    }
}

inline Node* TweenDoneNode(std::function<void(bool)> fn) {
    if (!fn) return nullptr;
    return AllocNode([f = std::move(fn)](const void* p) {
        f(static_cast<const unix_tween_ev*>(p)->completed != 0);
    });
}

inline unix_tween_desc MakeTweenDesc(const TweenDesc& t) {
    unix_tween_desc d{};
    d.size        = sizeof(d);
    d.target      = t.Target;
    d.property    = static_cast<unix_tween_prop>(t.Property);
    const uint32_t arity = TweenArity(t.Property);
    if (t.From) {
        d.from       = *t.From;
        d.from_count = arity;
    }
    d.to          = t.To;
    d.to_count    = arity;
    d.bound       = t.Bound;
    d.duration_ms = static_cast<float>(t.DurationMs);
    d.delay_ms    = static_cast<float>(t.DelayMs);
    d.ease        = static_cast<uint32_t>(t.Curve);
    if (t.CurveParams) {
        for (std::size_t i = 0; i < 4; ++i) d.ease_params[i] = (*t.CurveParams)[i];
        d.ease_has_params = true;
    }
    d.loops    = t.Loops;
    d.yoyo     = t.Yoyo;
    d.unscaled = t.Unscaled;
    return d;
}

}   // namespace detail

template <class>
Tween Tweens::Play(const TweenDesc& desc) {
    UNIX_USE(tween, play);
    unix_tween_desc d = detail::MakeTweenDesc(desc);
    detail::Node* n = detail::TweenDoneNode(desc.OnDone);
    d.on_done = n ? &detail::TrampTweenFinal : nullptr;
    d.ud      = n;
    const unix_handle h = detail::g_d.tween_play(detail::g_self, &d);
    detail::OwnNode(h, n);
    return Tween{ h };
}

template <class>
Tween Tweens::PlaySequence(std::span<const TweenStep> steps, int32_t loops,
                           std::function<void(bool)> onDone) {
    UNIX_USE(tween, play_sequence);
    std::vector<unix_tween_step> raw(steps.size());
    std::vector<detail::Node*> nodes;
    for (std::size_t i = 0; i < steps.size(); ++i) {
        const TweenStep& s = steps[i];
        unix_tween_step& r = raw[i];
        r.size          = sizeof(r);
        r.with_previous = s.WithPrevious;
        r.wait_ms       = static_cast<float>(s.WaitMs);
        r.tween         = detail::MakeTweenDesc(s.Tween);
        const bool wait = s.WaitMs != 0 && !s.Tween.Target && !s.Tween.Bound;
        r.kind          = wait ? unix_step_wait : unix_step_tween;
        if (!wait) {
            if (detail::Node* n = detail::TweenDoneNode(s.Tween.OnDone)) {
                r.tween.on_done = &detail::TrampTween;
                r.tween.ud      = n;
                nodes.push_back(n);
            }
        }
    }
    detail::Node* seq = nullptr;
    if (onDone || !nodes.empty()) {
        seq = onDone ? detail::TweenDoneNode(std::move(onDone)) : detail::AllocNodeRaw();
        nodes.push_back(seq);
    }
    const unix_handle h = detail::g_d.tween_play_sequence(
        detail::g_self, raw.empty() ? nullptr : raw.data(), static_cast<uint32_t>(raw.size()),
        static_cast<uint32_t>(sizeof(unix_tween_step)), loops,
        seq ? &detail::TrampTweenFinal : nullptr, seq);
    for (detail::Node* n : nodes) detail::OwnNode(h, n);
    return Tween{ h };
}

template <class>
Tween Tweens::AnchoredPos(void* rectTransform, Vec2 from, Vec2 to, uint32_t ms, Ease ease,
                          std::function<void(bool)> onDone) {
    UNIX_USE(tween, anchored_pos);
    detail::Node* n = detail::TweenDoneNode(std::move(onDone));
    const unix_handle h = detail::g_d.tween_anchored_pos(
        detail::g_self, rectTransform, from, to, ms, static_cast<uint32_t>(ease),
        n ? &detail::TrampTweenFinal : nullptr, n);
    detail::OwnNode(h, n);
    return Tween{ h };
}

template <class>
Tween Tweens::LocalPos(void* transform, Vec3 from, Vec3 to, uint32_t ms, Ease ease,
                       std::function<void(bool)> onDone) {
    UNIX_USE(tween, local_pos);
    detail::Node* n = detail::TweenDoneNode(std::move(onDone));
    const unix_handle h = detail::g_d.tween_local_pos(
        detail::g_self, transform, from, to, ms, static_cast<uint32_t>(ease),
        n ? &detail::TrampTweenFinal : nullptr, n);
    detail::OwnNode(h, n);
    return Tween{ h };
}

template <class>
Tween Tweens::LocalScale(void* transform, Vec3 from, Vec3 to, uint32_t ms, Ease ease,
                         std::function<void(bool)> onDone) {
    UNIX_USE(tween, local_scale);
    detail::Node* n = detail::TweenDoneNode(std::move(onDone));
    const unix_handle h = detail::g_d.tween_local_scale(
        detail::g_self, transform, from, to, ms, static_cast<uint32_t>(ease),
        n ? &detail::TrampTweenFinal : nullptr, n);
    detail::OwnNode(h, n);
    return Tween{ h };
}

template <class>
Tween Tweens::Value(float from, float to, uint32_t ms, Ease ease,
                    std::function<void(bool)> onDone) {
    UNIX_USE(tween, value);
    detail::Node* n = detail::TweenDoneNode(std::move(onDone));
    const unix_handle h = detail::g_d.tween_value(detail::g_self, from, to, ms,
                                                  static_cast<uint32_t>(ease),
                                                  n ? &detail::TrampTweenFinal : nullptr, n);
    detail::OwnNode(h, n);
    return Tween{ h };
}

template <class>
void Tweens::CancelAll() {
    UNIX_USE(tween, cancel_all);
    detail::g_d.tween_cancel_all(detail::g_self);
}

template <class>
void Tweens::CancelTarget(void* target) {
    UNIX_USE(tween, cancel_target);
    detail::g_d.tween_cancel_target(detail::g_self, target);
}

template <class>
bool Ui::Adopt(const Frame& f) {
    if (!f.imgui || !f.imgui_version) return false;
#ifdef IMGUI_USE_WCHAR32
    constexpr std::string_view layout = "+w32";
#else
    constexpr std::string_view layout = "";
#endif
    const std::string_view host{ f.imgui_version };
    const std::string_view mine{ ::ImGui::GetVersion() };
    if (host.size() != mine.size() + layout.size() || !host.starts_with(mine) || !host.ends_with(layout))
        return false;
    if (f.imgui_alloc && f.imgui_free)
        ::ImGui::SetAllocatorFunctions(f.imgui_alloc, f.imgui_free, f.imgui_alloc_ud);
    ::ImGui::SetCurrentContext(static_cast<ImGuiContext*>(f.imgui));
    return true;
}

template <class>
void* Ui::GetImGuiContext() const {
    UNIX_USE(ui, imgui_context);
    return detail::g_d.ui_imgui_context(detail::g_self);
}

template <class>
void Ui::ClaimRegion(Rect r) {
    UNIX_USE(input, claim_region);
    detail::g_d.input_claim_region(detail::g_self, r);
}

template <class>
bool Ui::Contains(Vec2 point) const {
    UNIX_USE(input, contains);
    return detail::g_d.input_contains(detail::g_self, point);
}

template <class>
void Ui::ClaimCursor(bool want) {
    UNIX_USE(cursor, claim);
    detail::g_d.cursor_claim(detail::g_self, want);
}

template <class>
std::vector<uint8_t> Ui::GetLoaderResource(std::string_view name) const {
    UNIX_USE(loader, resource);
    return detail::UiSizedBlob([name](void* b, uint32_t c) {
        return detail::g_d.loader_resource(detail::g_self, Sv(name), b, c);
    });
}

template <class>
KeyboardRequest Ui::OpenKeyboard(const KeyboardDesc& desc) {
    UNIX_USE(input, open_keyboard);
    if (!desc.OnSubmit) return KeyboardRequest{};
    detail::Node* n = detail::AllocNode([f = desc.OnSubmit](const void* p) {
        f(View(static_cast<const unix_keyboard_ev*>(p)->text));
    });
    unix_keyboard_desc d{};
    d.size        = sizeof(d);
    d.title       = Sv(desc.Title);
    d.initial     = Sv(desc.Initial);
    d.placeholder = Sv(desc.Placeholder);
    d.limit       = desc.Limit;
    d.multiline   = desc.Multiline;
    d.numeric     = desc.Numeric;
    d.on_submit   = &detail::TrampKeyboardFinal;
    d.ud          = n;
    const unix_handle h = detail::g_d.input_open_keyboard(detail::g_self, &d);
    detail::OwnNode(h, n);
    return KeyboardRequest{ h };
}

template <class>
bool Ui::IsKeyboardReady() const {
    UNIX_USE(input, keyboard_ready);
    return detail::g_d.input_keyboard_ready(detail::g_self);
}

// ---- Player ----
// GetRank and GetRankColor are defined in <unix/il2cpp.hpp>: no row turns a Player into the
// APIUser `players.rank` requires, so they go through the typed layer, like Position.

// ponytail: liveness is membership in players.all, an O(n) scan per call. players.* carries no
// liveness probe; add one and this becomes a single call.
template <class>
bool Player::IsAlive() const {
    UNIX_USE(players, all);
    if (!m_raw) return false;
    const uint32_t need = detail::g_d.players_all(detail::g_self, nullptr, 0);
    if (need == 0 || need >= 0xFFFFFF00u) return false;
    std::vector<void*> raw(need);
    const uint32_t got = detail::g_d.players_all(detail::g_self, raw.data(), need);
    for (uint32_t i = 0; i < got && i < need; ++i)
        if (raw[i] == m_raw) return true;
    return false;
}

template <class>
bool Player::IsLocal() const {
    UNIX_USE(players, is_local);
    return m_raw && detail::g_d.players_is_local(detail::g_self, m_raw);
}

template <class>
std::string Player::GetDisplayName() const {
    UNIX_USE(players, display_name);
    if (!m_raw) return {};
    void* p = m_raw;
    return detail::ReadStr([p](char* b, uint32_t c) {
        return detail::g_d.players_display_name(detail::g_self, p, b, c);
    });
}

template <class>
std::string Player::GetUserId() const {
    UNIX_USE(players, user_id);
    if (!m_raw) return {};
    void* p = m_raw;
    return detail::ReadStr([p](char* b, uint32_t c) {
        return detail::g_d.players_user_id(detail::g_self, p, b, c);
    });
}

template <class>
void* Player::GetApi() const {
    UNIX_USE(player, from_player);
    return m_raw ? detail::g_d.player_from_player(detail::g_self, m_raw) : nullptr;
}

}   // namespace UNIx
