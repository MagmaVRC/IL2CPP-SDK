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

/// <summary>The 14-slot user page.</summary>
class UserPage {
public:
    /// <summary>A pooled row. Fill it with UIUserRow::AddButton.</summary>
    template <class = void> UIUserRow AddRow(UserPageAudience audience = UserPageAudience::All);
    template <class = void> UIUserButton AddButton(std::string_view text, Sprite icon,
                                                   std::function<void(const UserEvent&)> onClick,
                                                   UserPageAudience audience = UserPageAudience::All);
    template <class = void> UISubPage AddSubPage(std::string_view name);
    template <class = void> [[nodiscard]] Subscription OnSelect(
        std::function<void(const UserEvent&)> fn);
    template <class = void> [[nodiscard]] SelectedUser GetSelected() const;
};

/// <summary>The QuickMenu itself. A page is created by constructing a UIPage.</summary>
class Menu {
public:
    /// <summary>Run build once the QuickMenu is live and sprites are cached, or immediately if
    /// that phase already fired.</summary>
    template <class = void> void OnReady(std::function<void()> build);
    /// <summary>Run build at one menu lifecycle phase.</summary>
    template <class = void> void OnPhase(MenuPhase phase, std::function<void()> build);
    template <class = void> [[nodiscard]] bool IsReady() const;
    template <class = void> [[nodiscard]] ::UNIx::UserPage& UserPage();
    template <class = void> void NavigateBack();
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

/// <summary>Host-driven interpolations.</summary>
class Tweens {
public:
    template <class = void> Tween AnchoredPos(void* rectTransform, Vec2 from, Vec2 to,
                                              uint32_t ms, Ease ease = Ease::Linear,
                                              std::function<void(bool completed)> onDone = {});
    template <class = void> Tween LocalPos(void* transform, Vec3 from, Vec3 to, uint32_t ms,
                                           Ease ease = Ease::Linear,
                                           std::function<void(bool completed)> onDone = {});
    template <class = void> Tween LocalScale(void* transform, Vec3 from, Vec3 to, uint32_t ms,
                                             Ease ease = Ease::Linear,
                                             std::function<void(bool completed)> onDone = {});
    template <class = void> Tween Value(float from, float to, uint32_t ms,
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
void Menu::NavigateBack() {
    UNIX_USE(menu, navigate_back);
    detail::g_d.menu_navigate_back(detail::g_self);
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

template <class>
Tween Tweens::AnchoredPos(void* rectTransform, Vec2 from, Vec2 to, uint32_t ms, Ease ease,
                          std::function<void(bool)> onDone) {
    UNIX_USE(tween, anchored_pos);
    detail::Node* n = nullptr;
    if (onDone) {
        n = detail::AllocNode([f = std::move(onDone)](const void* p) {
            f(static_cast<const unix_tween_ev*>(p)->completed != 0);
        });
    }
    const unix_handle h = detail::g_d.tween_anchored_pos(
        detail::g_self, rectTransform, from, to, ms, static_cast<uint32_t>(ease),
        n ? &detail::TrampTween : nullptr, n);
    detail::OwnNode(h, n);
    return Tween{ h };
}

template <class>
Tween Tweens::LocalPos(void* transform, Vec3 from, Vec3 to, uint32_t ms, Ease ease,
                       std::function<void(bool)> onDone) {
    UNIX_USE(tween, local_pos);
    detail::Node* n = nullptr;
    if (onDone) {
        n = detail::AllocNode([f = std::move(onDone)](const void* p) {
            f(static_cast<const unix_tween_ev*>(p)->completed != 0);
        });
    }
    const unix_handle h = detail::g_d.tween_local_pos(
        detail::g_self, transform, from, to, ms, static_cast<uint32_t>(ease),
        n ? &detail::TrampTween : nullptr, n);
    detail::OwnNode(h, n);
    return Tween{ h };
}

template <class>
Tween Tweens::LocalScale(void* transform, Vec3 from, Vec3 to, uint32_t ms, Ease ease,
                         std::function<void(bool)> onDone) {
    UNIX_USE(tween, local_scale);
    detail::Node* n = nullptr;
    if (onDone) {
        n = detail::AllocNode([f = std::move(onDone)](const void* p) {
            f(static_cast<const unix_tween_ev*>(p)->completed != 0);
        });
    }
    const unix_handle h = detail::g_d.tween_local_scale(
        detail::g_self, transform, from, to, ms, static_cast<uint32_t>(ease),
        n ? &detail::TrampTween : nullptr, n);
    detail::OwnNode(h, n);
    return Tween{ h };
}

template <class>
Tween Tweens::Value(float from, float to, uint32_t ms, Ease ease,
                    std::function<void(bool)> onDone) {
    UNIX_USE(tween, value);
    detail::Node* n = nullptr;
    if (onDone) {
        n = detail::AllocNode([f = std::move(onDone)](const void* p) {
            f(static_cast<const unix_tween_ev*>(p)->completed != 0);
        });
    }
    const unix_handle h = detail::g_d.tween_value(detail::g_self, from, to, ms,
                                                  static_cast<uint32_t>(ease),
                                                  n ? &detail::TrampTween : nullptr, n);
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
    if (std::string_view{ f.imgui_version } != std::string_view{ ::ImGui::GetVersion() })
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

// ---- Player ----
// GetRank and GetRankColor are defined in <unix/il2cpp.hpp>: no row turns a Player into the
// APIUser `players.rank` requires, so they go through the typed layer, like Position.

// ponytail: liveness is membership in players.all, an O(n) scan per call. players.* carries no
// liveness probe; add one and this becomes a single call.
inline bool Player::IsAlive() const {
    if (!m_raw) return false;
    const uint32_t need = detail::g_d.players_all(detail::g_self, nullptr, 0);
    if (need == 0 || need >= 0xFFFFFF00u) return false;
    std::vector<void*> raw(need);
    const uint32_t got = detail::g_d.players_all(detail::g_self, raw.data(), need);
    for (uint32_t i = 0; i < got && i < need; ++i)
        if (raw[i] == m_raw) return true;
    return false;
}

inline bool Player::IsLocal() const {
    return m_raw && detail::g_d.players_is_local(detail::g_self, m_raw);
}

inline std::string Player::GetDisplayName() const {
    if (!m_raw) return {};
    void* p = m_raw;
    return detail::ReadStr([p](char* b, uint32_t c) {
        return detail::g_d.players_display_name(detail::g_self, p, b, c);
    });
}

inline std::string Player::GetUserId() const {
    if (!m_raw) return {};
    void* p = m_raw;
    return detail::ReadStr([p](char* b, uint32_t c) {
        return detail::g_d.players_user_id(detail::g_self, p, b, c);
    });
}

}   // namespace UNIx
