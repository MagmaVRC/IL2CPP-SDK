#pragma once
#ifndef UNIX_USE
#  error "<unix/ui.hpp> is included by <unix/module.hpp>; include that instead"
#endif

namespace UNIx {

/// <summary>Options for a slider. Value is ignored by the bound overloads: there the pointer
/// is authoritative.</summary>
struct SliderDesc {
    float Min = 0.f, Max = 1.f, Value = 0.f;
    std::string_view Format{};
    std::string_view ConfigKey{};
    float Power = 1.f;
    bool  SubIndicator = false;
    std::function<void(float)> OnChange{};
};

/// <summary>Options for a toggle.</summary>
struct ToggleDesc {
    std::string_view ConfigKey{};
    bool  SubIndicator = false;
    std::function<void(bool)> OnChange{};
    /// <summary>Owns performing the transition and writing the bound value; may refuse.
    /// If it leaves the value unchanged the control snaps back on the next host poll.</summary>
    std::function<void(bool)> Apply{};
};

/// <summary>Options for an enum selector. Value is ignored by the bound overloads.</summary>
struct EnumDesc {
    std::span<const char* const> Options{};
    int32_t Value = 0;
    std::string_view ConfigKey{};
    bool  SubIndicator = false;
    std::function<void(int32_t)> OnChange{};
};

namespace detail {

constexpr unix_icon MakeIcon(Sprite s) noexcept {
    return s == Sprite::None ? unix_icon{ unix_icon_none, nullptr, 0 }
                             : unix_icon{ unix_icon_bundle, nullptr, static_cast<int32_t>(s) };
}
constexpr unix_icon MakeIcon(void* sprite) noexcept {
    return sprite ? unix_icon{ unix_icon_sprite_ptr, sprite, 0 }
                  : unix_icon{ unix_icon_none, nullptr, 0 };
}

// One trampoline per callback shape; the closure node's address is the ud (SDK spec 6.4).
inline void UNIX_CC TrampWidget(void* ud, const unix_widget_ev* e) {
    auto* n = static_cast<Node*>(ud);
    if (n && n->fn && e && e->kind != unix_wk_restore) n->fn(e);
}
inline void UNIX_CC TrampUser(void* ud, const unix_user_ev* e) {
    auto* n = static_cast<Node*>(ud);
    if (n && n->fn && e) n->fn(e);
}
inline void UNIX_CC TrampPlayer(void* ud, const unix_player_ev* e) {
    auto* n = static_cast<Node*>(ud);
    if (n && n->fn && e) n->fn(e);
}
inline void UNIX_CC TrampRender(void* ud, const unix_frame* f) {
    auto* n = static_cast<Node*>(ud);
    if (n && n->fn && f) n->fn(f);
}
inline void UNIX_CC TrampTween(void* ud, const unix_tween_ev* e) {
    auto* n = static_cast<Node*>(ud);
    if (n && n->fn && e) n->fn(e);
}
inline void UNIX_CC TrampMenu(void* ud, unix_menu_phase p) {
    auto* n = static_cast<Node*>(ud);
    if (n && n->fn) n->fn(&p);
}

template <class Call>
std::vector<uint8_t> UiSizedBlob(Call&& call) {
    const uint32_t need = call(nullptr, 0u);
    if (need == 0 || need >= 0xFFFFFF00u) return {};
    std::vector<uint8_t> out(need);
    const uint32_t got = call(out.data(), need);
    if (got == 0 || got > need) return {};
    out.resize(got);
    return out;
}

}   // namespace detail

/// <summary>Options for a text field row.</summary>
struct TextInputDesc {
    std::string_view Value{};
    std::string_view Tooltip{};
    std::string_view ConfigKey{};
    /// <summary>Fired once the field settles on a new value. The string is the host's and
    /// lives only for the call.</summary>
    std::function<void(std::string_view)> OnChange{};
};

/// <summary>Content surface of a page or sub-page. Every control the host offers can go
/// straight onto one: a foldout groups them, it is not what makes them possible.</summary>
class UISurface {
public:
    UISurface() = default;
    explicit UISurface(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(const UISurface& a, const UISurface& b) noexcept {
        return a.m_handle == b.m_handle;
    }

    /// <summary>A tile on this surface.</summary>
    /// <param name="onClick">Fired on the Unity main thread. Empty for a decorative tile.</param>
    template <class = void> UIButton AddButton(std::string_view text,
                                               std::function<void()> onClick = {});
    /// <summary>A toggle whose state the host owns.</summary>
    template <class = void> UIToggle AddToggle(std::string_view text, bool def,
                                               const ToggleDesc& desc = {});
    /// <summary>Two-way bind against an existing member. The pointer belongs to the Unity main
    /// thread. AddToggle(label, nullptr) is ambiguous: pass a real bool* or a real bool.</summary>
    template <class = void> UIToggle AddToggle(std::string_view text, bool* value,
                                               const ToggleDesc& desc = {});
    template <class = void> UIFoldout AddFoldout(std::string_view title, bool expanded = true,
                                                 bool background = true,
                                                 bool autoSeparators = false);

    // The settings rows, placed straight on the surface rather than inside a foldout. They
    // land in the page's own column beside the foldouts, not in the tile grid, so a page can
    // be one list of settings without a section header it does not want.
    template <class = void> UISlider AddSlider(std::string_view label, const SliderDesc& desc);
    template <class = void> UISlider AddSlider(std::string_view label, float* value,
                                               const SliderDesc& desc);
    template <class = void> UISlider AddSlider(std::string_view label, float* value,
                                               float min, float max);
    template <class = void> UIEnumSelector AddEnum(std::string_view label, const EnumDesc& desc);
    template <class = void> UIEnumSelector AddEnum(std::string_view label, int32_t* value,
                                                   const EnumDesc& desc);
    template <class = void> UIEnumSelector AddEnum(std::string_view label, int32_t* value,
                                                   std::span<const char* const> options);
    template <class = void> UILabel     AddLabel(std::string_view text);
    template <class = void> UISeparator AddSeparator();
    template <class = void> UITextInput AddTextInput(std::string_view label,
                                                     const TextInputDesc& desc = {});
    /// <summary>A run of rows replaced as a block, for content not known when the page is
    /// built.</summary>
    /// <param name="onPress">Receives the index of the row pressed.</param>
    template <class = void> UIList AddList(std::function<void(uint32_t)> onPress = {});
protected:
    unix_handle m_handle{};
};

/// <summary>Foldout content. The full control set lives here, matching the host.</summary>
class UIFoldout {
public:
    UIFoldout() = default;
    explicit UIFoldout(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UIFoldout a, UIFoldout b) noexcept { return a.m_handle == b.m_handle; }

    template <class = void> UIToggle AddToggle(std::string_view text, bool def,
                                               const ToggleDesc& desc = {});
    template <class = void> UIToggle AddToggle(std::string_view text, bool* value,
                                               const ToggleDesc& desc = {});
    template <class = void> UISlider AddSlider(std::string_view label, const SliderDesc& desc);
    template <class = void> UISlider AddSlider(std::string_view label, float* value,
                                               const SliderDesc& desc);
    /// <summary>The common case: bind a float with a range and host defaults for the rest.</summary>
    template <class = void> UISlider AddSlider(std::string_view label, float* value,
                                               float min, float max);
    template <class = void> UIEnumSelector AddEnum(std::string_view label, const EnumDesc& desc);
    template <class = void> UIEnumSelector AddEnum(std::string_view label, int32_t* value,
                                                   const EnumDesc& desc);
    /// <summary>The common case: bind an index against a fixed option list.</summary>
    template <class = void> UIEnumSelector AddEnum(std::string_view label, int32_t* value,
                                                   std::span<const char* const> options);
    template <class = void> UILabel     AddLabel(std::string_view text);
    template <class = void> UISeparator AddSeparator();
    template <class = void> UITextInput AddTextInput(std::string_view label,
                                                     const TextInputDesc& desc = {});
    template <class = void> UIList AddList(std::function<void(uint32_t)> onPress = {});
    template <class = void> void SetExpanded(bool expanded);
    template <class = void> [[nodiscard]] bool IsExpanded() const;
    /// <summary>Be told when the foldout expands or collapses, including when the user works
    /// VRChat's own header toggle rather than SetExpanded.</summary>
    template <class = void> void OnChange(std::function<void(bool)> fn);
    template <class = void> UIFoldout& SetEnabled(bool on);
    template <class = void> UIFoldout& SetVisible(bool on);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A page reached from a nav row on its parent page.</summary>
class UISubPage : public UISurface {
public:
    using UISurface::UISurface;
    template <class = void> UISubPage& SetNavText(std::string_view text);
    template <class = void> UISubPage& SetNavIcon(Sprite icon);
    template <class = void> UISubPage& SetNavAlert(bool on);
    template <class = void> [[nodiscard]] UIButton GetNavButton() const;
    /// <summary>Pin a small button into this sub-page's nav row corner; the row still opens
    /// the page.</summary>
    template <class = void> UICornerButton AddCornerButton(Sprite icon,
                                                           std::function<void()> onClick);
    template <class = void> void Remove();
};

/// <summary>A QuickMenu page, and the one SDK object with virtuals. Constructing it creates the
/// host page, or re-acquires the one this module already made under that name; destroying it
/// cancels the event registration and leaves the page alone.</summary>
class UIPage : public UISurface, protected Services {
public:
    UIPage() = default;
    /// <param name="name">Page name. Constructing twice with the same name re-acquires.</param>
    /// <param name="tooltip">Hover text for the page's nav tile.</param>
    template <class = void> explicit UIPage(std::string_view name,
                                            std::string_view tooltip = "");
    /// <summary>Adopt a page this module already owns. Registers no page events.</summary>
    explicit UIPage(unix_handle existing) noexcept : UISurface(existing) {}
    UIPage(const UIPage&) = delete;
    UIPage& operator=(const UIPage&) = delete;
    virtual ~UIPage();
    /// <summary>Look one up by name. Never creates.</summary>
    template <class = void> [[nodiscard]] static UIPage Find(std::string_view name);

    template <class = void> UISubPage AddSubPage(std::string_view name);
    template <class = void> UIPage& SetIcon(Sprite icon);
    template <class = void> UIPage& SetIcon(void* sprite);
    template <class = void> UIPage& SetTitle(std::string_view title);
    /// <summary>Badge with the theme's default colour.</summary>
    template <class = void> UIPage& SetBadge(bool visible, std::string_view text = "");
    template <class = void> UIPage& SetBadge(bool visible, std::string_view text, Color bg);
    template <class = void> void NavigateTo();
    /// <summary>True while the menu has this page open.</summary>
    template <class = void> [[nodiscard]] bool IsVisible() const;
    /// <summary>Show or hide this page's tab. The page stays, and NavigateTo and a nav row
    /// on another page still reach it.</summary>
    template <class = void> UIPage& SetTabVisible(bool visible);
    /// <summary>Destroy the host page. Unity main thread only.</summary>
    template <class = void> void Remove();

protected:
    /// <summary>Build the page's contents. Delivered on the first main-thread pump after
    /// construction, never from the constructor.</summary>
    virtual void OnBuild() {}
    virtual void OnShow() {}
    virtual void OnHide() {}
    /// <summary>Per frame, only while this is the page the menu has open.</summary>
    virtual void OnTick(float /*dt*/) {}
    virtual void OnSceneLoaded() {}

private:
    static void UNIX_CC OnPageEvent(void* ud, const unix_page_ev* e);
    void DispatchPageEvent(const unix_page_ev& e);
    unix_handle m_events{};
};

/// <summary>One row of a MainMenu page's side list, and the column its controls go on.
/// VRChat shows exactly one section at a time and drives the switch itself.</summary>
class UISection : public UISurface {
public:
    using UISurface::UISurface;
    /// <summary>Show this section, as clicking its row does.</summary>
    template <class = void> void Select();
    /// <summary>Relabel the row, and the title its column takes when picked.</summary>
    template <class = void> UISection& SetName(std::string_view name);
    /// <summary>Destroy the section and its row. Unity main thread only.</summary>
    template <class = void> void Remove();
};

/// <summary>A tab on VRChat's MainMenu, holding a side list of sections. Controls go on a
/// section, never on the page.</summary>
class UIMainMenuPage {
public:
    UIMainMenuPage() = default;
    explicit UIMainMenuPage(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    /// <param name="name">Row label, and the column's title once picked.</param>
    template <class = void> UISection AddSection(std::string_view name);
    /// <summary>Open this page's tab.</summary>
    template <class = void> void Open();
    /// <summary>The title over the side list.</summary>
    template <class = void> UIMainMenuPage& SetTitle(std::string_view title);
    /// <summary>Destroy the page, its tab and its sections. Unity main thread only.</summary>
    template <class = void> void Remove();

private:
    unix_handle m_handle{};
};

/// <summary>A tile on a page or sub-page.</summary>
class UIButton {
public:
    UIButton() = default;
    explicit UIButton(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UIButton a, UIButton b) noexcept { return a.m_handle == b.m_handle; }

    template <class = void> UIButton& SetText(std::string_view text);
    template <class = void> UIButton& SetIcon(Sprite icon);
    template <class = void> UIButton& SetIcon(void* sprite);
    template <class = void> UIButton& SetColor(Color c);
    template <class = void> UIButton& SetEnabled(bool on);
    template <class = void> UIButton& SetVisible(bool on);
    template <class = void> [[nodiscard]] void* GetObject() const;
    /// <summary>Pin a mini button into this tile's corner. The tile still does its own thing.</summary>
    template <class = void> UICornerButton AddCornerButton(Sprite icon,
                                                           std::function<void()> onClick);
    /// <summary>The corner form that opens a page instead of firing a callback.</summary>
    template <class = void> UICornerButton AddCornerPage(Sprite icon, UIPage& target);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A settings toggle. Toggle ids share the host's button id space, so the four
/// button setters accept one.</summary>
class UIToggle {
public:
    UIToggle() = default;
    explicit UIToggle(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UIToggle a, UIToggle b) noexcept { return a.m_handle == b.m_handle; }

    template <class = void> [[nodiscard]] bool GetState() const;
    template <class = void> void SetState(bool on);
    /// <summary>Drive the control without re-firing its own change callback.</summary>
    template <class = void> void SetStateSilent(bool on);
    template <class = void> UIToggle& SetIcon(Sprite icon);
    template <class = void> UIToggle& SetIcon(void* sprite);
    template <class = void> UIToggle& SetEnabled(bool on);
    template <class = void> UIToggle& SetVisible(bool on);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A float slider inside a foldout.</summary>
class UISlider {
public:
    UISlider() = default;
    explicit UISlider(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UISlider a, UISlider b) noexcept { return a.m_handle == b.m_handle; }

    template <class = void> [[nodiscard]] float GetValue() const;
    template <class = void> void SetValue(float v);
    /// <summary>Drive the control without re-firing its own change callback.</summary>
    template <class = void> void SetValueSilent(float v);
    template <class = void> UISlider& SetEnabled(bool on);
    template <class = void> UISlider& SetVisible(bool on);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>An index selector over a fixed option list, inside a foldout.</summary>
class UIEnumSelector {
public:
    UIEnumSelector() = default;
    explicit UIEnumSelector(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UIEnumSelector a, UIEnumSelector b) noexcept {
        return a.m_handle == b.m_handle;
    }

    template <class = void> [[nodiscard]] int32_t GetIndex() const;
    template <class = void> void SetIndex(int32_t i);
    /// <summary>Drive the control without re-firing its own change callback.</summary>
    template <class = void> void SetIndexSilent(int32_t i);
    template <class = void> UIEnumSelector& SetEnabled(bool on);
    template <class = void> UIEnumSelector& SetVisible(bool on);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A text row inside a foldout.</summary>
class UILabel {
public:
    UILabel() = default;
    explicit UILabel(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UILabel a, UILabel b) noexcept { return a.m_handle == b.m_handle; }

    template <class = void> UILabel& SetText(std::string_view text);
    template <class = void> UILabel& SetEnabled(bool on);
    template <class = void> UILabel& SetVisible(bool on);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A divider inside a foldout.</summary>
class UISeparator {
public:
    UISeparator() = default;
    explicit UISeparator(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UISeparator a, UISeparator b) noexcept {
        return a.m_handle == b.m_handle;
    }

    template <class = void> UISeparator& SetVisible(bool on);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A text field row, cloned from VRChat's own input field. There is no managed
/// listener to attach, so the host polls the field and reports a settled value.</summary>
class UITextInput {
public:
    UITextInput() = default;
    explicit UITextInput(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UITextInput a, UITextInput b) noexcept {
        return a.m_handle == b.m_handle;
    }

    template <class = void> [[nodiscard]] std::string GetText() const;
    template <class = void> UITextInput& SetText(std::string_view text);
    template <class = void> UITextInput& SetEnabled(bool on);
    template <class = void> UITextInput& SetVisible(bool on);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A run of rows that is replaced as a block. The rows keep the place the list was
/// declared at, so rebuilding one does not drop it below everything added since.</summary>
class UIList {
public:
    UIList() = default;
    explicit UIList(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UIList a, UIList b) noexcept { return a.m_handle == b.m_handle; }

    template <class = void> void SetItems(std::span<const std::string_view> items);
    template <class = void> void SetItems(std::span<const char* const> items);
    template <class = void> void Clear();
    template <class = void> [[nodiscard]] uint32_t Count() const;
    /// <summary>One row, so it can be restyled after the list was filled.</summary>
    template <class = void> [[nodiscard]] UIButton Row(uint32_t index) const;
    template <class = void> UIList& SetEnabled(bool on);
    template <class = void> UIList& SetVisible(bool on);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A mini button pinned into a tile's or a nav row's corner.</summary>
class UICornerButton {
public:
    UICornerButton() = default;
    explicit UICornerButton(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UICornerButton a, UICornerButton b) noexcept {
        return a.m_handle == b.m_handle;
    }

    /// <summary>Tint the corner to show state. A per-change push, not a bound pointer.</summary>
    template <class = void> UICornerButton& SetColor(Color c);
    template <class = void> UICornerButton& SetIcon(Sprite icon);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A button on the user page.</summary>
class UIUserButton {
public:
    UIUserButton() = default;
    explicit UIUserButton(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UIUserButton a, UIUserButton b) noexcept {
        return a.m_handle == b.m_handle;
    }

    template <class = void> UIUserButton& SetText(std::string_view text);
    template <class = void> UIUserButton& SetIcon(Sprite icon);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>One pooled row on the user page: a strip of buttons, plus the foldout that keeps
/// the rest of a module's controls off the page.</summary>
class UIUserRow {
public:
    UIUserRow() = default;
    explicit UIUserRow(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(UIUserRow a, UIUserRow b) noexcept { return a.m_handle == b.m_handle; }

    /// <summary>A button in this row. The event carries player, api_user, user_id, display_name
    /// and kind, so a handler needs nothing else.</summary>
    template <class = void> UIUserButton AddButton(std::string_view text, Sprite icon,
                                                   std::function<void(const UserEvent&)> onClick);
    /// <summary>Put this row above VRChat's own action rows rather than after them. They
    /// share one container, so this is the top of the actions block: below the profile
    /// header, ahead of everything native. Two rows asking for it both land at the top.</summary>
    template <class = void> UIUserRow& SetFront(bool front = true);
    /// <summary>A collapsible section on the user page, placed after this row and shown to the
    /// same audience. It takes the same controls a page foldout does.</summary>
    /// <param name="title">Header text.</param>
    /// <param name="expanded">Whether it starts open.</param>
    /// <param name="background">Whether the content sits on the panel's backing plate.</param>
    /// <param name="autoSeparators">Whether a divider is inserted between controls.</param>
    template <class = void> UIFoldout AddFoldout(std::string_view title, bool expanded = true,
                                                 bool background = true,
                                                 bool autoSeparators = false);
    template <class = void> void Remove();
private:
    unix_handle m_handle{};
};

/// <summary>A custom row on a player's nameplate.</summary>
class Nameplate {
public:
    Nameplate() = default;
    explicit Nameplate(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(Nameplate a, Nameplate b) noexcept { return a.m_handle == b.m_handle; }

    /// <summary>False once the game rebuilt the nameplate and dropped this row.</summary>
    template <class = void> [[nodiscard]] bool IsAlive() const;
    template <class = void> Nameplate& SetText(std::string_view text);
    template <class = void> Nameplate& SetTextColor(Color c);
    template <class = void> Nameplate& SetIconColor(Color c);
    /// <summary>Show a sprite in the row's icon slot, ahead of the text. Sprite::None hides
    /// the slot and drops it out of the row's layout.</summary>
    template <class = void> Nameplate& SetIcon(Sprite icon);
    /// <summary>The same, from a live sprite object. Null hides the slot.</summary>
    template <class = void> Nameplate& SetIcon(void* sprite);
    template <class = void> Nameplate& SetPosition(Vec3 offset);
    template <class = void> Nameplate& SetWidth(float w);
    template <class = void> Nameplate& SetHeight(float h);
    template <class = void> Nameplate& SetBackground(bool on);
    template <class = void> [[nodiscard]] void* GetObject() const;
    template <class = void> [[nodiscard]] void* GetTextObject() const;
    template <class = void> [[nodiscard]] void* GetBackgroundObject() const;
    template <class = void> void Destroy();
private:
    unix_handle m_handle{};
};

/// <summary>A running interpolation owned by the host.</summary>
class Tween {
public:
    Tween() = default;
    explicit Tween(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(Tween a, Tween b) noexcept { return a.m_handle == b.m_handle; }

    /// <summary>Stop it. The completion callback fires with completed = false.</summary>
    template <class = void> void Cancel();
private:
    unix_handle m_handle{};
};

/// <summary>Subclass to handle toggle changes with a virtual instead of a lambda.
/// Address-stable, so the host can deliver to this object.</summary>
class UIToggleHandler {
public:
    UIToggleHandler() = default;
    UIToggleHandler(const UIToggleHandler&) = delete;
    UIToggleHandler& operator=(const UIToggleHandler&) = delete;
    virtual ~UIToggleHandler() = default;
    /// <summary>Create the control on a foldout and route its changes to OnChanged.</summary>
    template <class = void> UIToggle Attach(UIFoldout parent, std::string_view label,
                                            bool def, bool* bound = nullptr);
    /// <summary>Create the control on a page or sub-page and route its changes to OnChanged.</summary>
    template <class = void> UIToggle Attach(UISurface parent, std::string_view label,
                                            bool def, bool* bound = nullptr);
    [[nodiscard]] UIToggle Control() const noexcept { return m_control; }
protected:
    virtual void OnChanged(bool /*on*/) {}
private:
    UIToggle m_control{};
};

/// <summary>Subclass to handle slider changes with a virtual instead of a lambda.</summary>
class UISliderHandler {
public:
    UISliderHandler() = default;
    UISliderHandler(const UISliderHandler&) = delete;
    UISliderHandler& operator=(const UISliderHandler&) = delete;
    virtual ~UISliderHandler() = default;
    /// <summary>Create the control on a foldout and route its changes to OnChanged.</summary>
    template <class = void> UISlider Attach(UIFoldout parent, std::string_view label,
                                            float min, float max, float* bound = nullptr);
    [[nodiscard]] UISlider Control() const noexcept { return m_control; }
protected:
    virtual void OnChanged(float /*value*/) {}
private:
    UISlider m_control{};
};

/// <summary>Subclass to handle enum changes with a virtual instead of a lambda.</summary>
class UIEnumHandler {
public:
    UIEnumHandler() = default;
    UIEnumHandler(const UIEnumHandler&) = delete;
    UIEnumHandler& operator=(const UIEnumHandler&) = delete;
    virtual ~UIEnumHandler() = default;
    /// <summary>Create the control on a foldout and route its changes to OnChanged.</summary>
    template <class = void> UIEnumSelector Attach(UIFoldout parent, std::string_view label,
                                                  std::span<const char* const> options,
                                                  int32_t* bound = nullptr);
    [[nodiscard]] UIEnumSelector Control() const noexcept { return m_control; }
protected:
    virtual void OnChanged(int32_t /*index*/) {}
private:
    UIEnumSelector m_control{};
};

// ---------------------------------------------------------------------------------------
// Bodies. Out of line so a builder can return a type only forward-declared above.
// ---------------------------------------------------------------------------------------

namespace detail {

inline unix_handle AddToggleTo(unix_handle parent, std::string_view text, bool def,
                               bool* bound, const ToggleDesc& d) {
    Node* n = nullptr;
    if (d.OnChange || d.Apply) {
        n = AllocNode([apply = d.Apply, on = d.OnChange](const void* p) {
            const bool v = static_cast<const unix_widget_ev*>(p)->b;
            if (apply) apply(v);
            if (on) on(v);
        });
    }
    unix_toggle_desc t{};
    t.size          = sizeof(t);
    t.label         = Sv(text);
    t.config_key    = Sv(d.ConfigKey);
    t.def           = bound ? *bound : def;
    t.sub_indicator = d.SubIndicator;
    t.on_change     = n ? &TrampWidget : nullptr;
    t.ud            = n;
    t.bound         = bound;
    t.module_owns_write = static_cast<bool>(d.Apply);
    const unix_handle h = g_d.menu_add_toggle(g_self, parent, &t);
    OwnNode(h, n);
    return h;
}

inline unix_handle AddSliderTo(unix_handle parent, std::string_view label, float* bound,
                               const SliderDesc& d) {
    Node* n = nullptr;
    if (d.OnChange) {
        n = AllocNode([on = d.OnChange](const void* p) {
            on(static_cast<const unix_widget_ev*>(p)->f);
        });
    }
    unix_slider_desc s{};
    s.size          = sizeof(s);
    s.label         = Sv(label);
    s.format        = Sv(d.Format);
    s.config_key    = Sv(d.ConfigKey);
    s.value         = bound ? *bound : d.Value;
    s.min           = d.Min;
    s.max           = d.Max;
    s.power         = d.Power;
    s.display_scale = 1.f;
    s.sub_indicator = d.SubIndicator;
    s.on_change     = n ? &TrampWidget : nullptr;
    s.ud            = n;
    s.bound         = bound;
    const unix_handle h = g_d.menu_add_slider(g_self, parent, &s);
    OwnNode(h, n);
    return h;
}

inline unix_handle AddTextInputTo(unix_handle parent, std::string_view label,
                                  const TextInputDesc& d) {
    Node* n = nullptr;
    if (d.OnChange) {
        n = AllocNode([f = d.OnChange](const void* p) {
            const auto* e = static_cast<const unix_widget_ev*>(p);
            f(std::string_view{ e->s.data ? e->s.data : "", e->s.size });
        });
    }
    unix_text_desc desc{};
    desc.size       = sizeof(desc);
    desc.label      = Sv(label);
    desc.tooltip    = Sv(d.Tooltip);
    desc.value      = Sv(d.Value);
    desc.config_key = Sv(d.ConfigKey);
    desc.on_change  = n ? &TrampWidget : nullptr;
    desc.ud         = n;
    const unix_handle h = g_d.menu_add_text_input(g_self, parent, &desc);
    OwnNode(h, n);
    return h;
}

inline unix_handle AddListTo(unix_handle parent, std::function<void(uint32_t)> onPress) {
    Node* n = nullptr;
    if (onPress) {
        n = AllocNode([f = std::move(onPress)](const void* p) {
            f(static_cast<uint32_t>(static_cast<const unix_widget_ev*>(p)->i));
        });
    }
    const unix_handle h = g_d.menu_add_list(g_self, parent,
                                            n ? &TrampWidget : nullptr, n);
    OwnNode(h, n);
    return h;
}

inline unix_handle AddEnumTo(unix_handle parent, std::string_view label, int32_t* bound,
                             const EnumDesc& d) {
    Node* n = nullptr;
    if (d.OnChange) {
        n = AllocNode([on = d.OnChange](const void* p) {
            on(static_cast<const unix_widget_ev*>(p)->i);
        });
    }
    std::vector<unix_str> options;
    options.reserve(d.Options.size());
    for (const char* o : d.Options) options.push_back(Sv(o ? std::string_view{ o } : std::string_view{}));

    unix_enum_desc e{};
    e.size          = sizeof(e);
    e.label         = Sv(label);
    e.config_key    = Sv(d.ConfigKey);
    e.options       = options.empty() ? nullptr : options.data();
    e.count         = static_cast<uint32_t>(options.size());
    e.def           = bound ? *bound : d.Value;
    e.sub_indicator = d.SubIndicator;
    e.on_change     = n ? &TrampWidget : nullptr;
    e.ud            = n;
    e.bound         = bound;
    const unix_handle h = g_d.menu_add_enum(g_self, parent, &e);
    OwnNode(h, n);
    return h;
}

inline unix_handle AddCornerTo(unix_handle owner, unix_icon icon, unix_handle page,
                               std::function<void()> onClick) {
    Node* n = nullptr;
    if (onClick) n = AllocNode([f = std::move(onClick)](const void*) { f(); });
    const unix_handle h = g_d.menu_add_corner_button(g_self, owner, icon, page,
                                                     n ? &TrampWidget : nullptr, n);
    OwnNode(h, n);
    return h;
}

}   // namespace detail

template <class>
UIButton UISurface::AddButton(std::string_view text, std::function<void()> onClick) {
    UNIX_USE(menu, add_button);
    detail::Node* n = nullptr;
    if (onClick) n = detail::AllocNode([f = std::move(onClick)](const void*) { f(); });
    unix_button_desc d{};
    d.size     = sizeof(d);
    d.text     = Sv(text);
    d.icon     = detail::MakeIcon(Sprite::None);
    d.on_click = n ? &detail::TrampWidget : nullptr;
    d.ud       = n;
    const unix_handle h = detail::g_d.menu_add_button(detail::g_self, m_handle, &d);
    detail::OwnNode(h, n);
    return UIButton{ h };
}

template <class>
UIToggle UISurface::AddToggle(std::string_view text, bool def, const ToggleDesc& desc) {
    UNIX_USE(menu, add_toggle);
    return UIToggle{ detail::AddToggleTo(m_handle, text, def, nullptr, desc) };
}

template <class>
UIToggle UISurface::AddToggle(std::string_view text, bool* value, const ToggleDesc& desc) {
    UNIX_USE(menu, add_toggle);
    return UIToggle{ detail::AddToggleTo(m_handle, text, false, value, desc) };
}

template <class>
UIFoldout UISurface::AddFoldout(std::string_view title, bool expanded, bool background,
                                bool autoSeparators) {
    UNIX_USE(menu, add_foldout);
    unix_foldout_desc d{};
    d.size            = sizeof(d);
    d.title           = Sv(title);
    d.expanded        = expanded;
    d.background      = background;
    d.auto_separators = autoSeparators;
    return UIFoldout{ detail::g_d.menu_add_foldout(detail::g_self, m_handle, &d) };
}

template <class>
UIToggle UIFoldout::AddToggle(std::string_view text, bool def, const ToggleDesc& desc) {
    UNIX_USE(menu, add_toggle);
    return UIToggle{ detail::AddToggleTo(m_handle, text, def, nullptr, desc) };
}

template <class>
UIToggle UIFoldout::AddToggle(std::string_view text, bool* value, const ToggleDesc& desc) {
    UNIX_USE(menu, add_toggle);
    return UIToggle{ detail::AddToggleTo(m_handle, text, false, value, desc) };
}

template <class>
UISlider UIFoldout::AddSlider(std::string_view label, const SliderDesc& desc) {
    UNIX_USE(menu, add_slider);
    return UISlider{ detail::AddSliderTo(m_handle, label, nullptr, desc) };
}

template <class>
UISlider UIFoldout::AddSlider(std::string_view label, float* value, const SliderDesc& desc) {
    UNIX_USE(menu, add_slider);
    return UISlider{ detail::AddSliderTo(m_handle, label, value, desc) };
}

template <class>
UISlider UIFoldout::AddSlider(std::string_view label, float* value, float min, float max) {
    UNIX_USE(menu, add_slider);
    SliderDesc d{};
    d.Min = min;
    d.Max = max;
    return UISlider{ detail::AddSliderTo(m_handle, label, value, d) };
}

template <class>
UIEnumSelector UIFoldout::AddEnum(std::string_view label, const EnumDesc& desc) {
    UNIX_USE(menu, add_enum);
    return UIEnumSelector{ detail::AddEnumTo(m_handle, label, nullptr, desc) };
}

template <class>
UIEnumSelector UIFoldout::AddEnum(std::string_view label, int32_t* value, const EnumDesc& desc) {
    UNIX_USE(menu, add_enum);
    return UIEnumSelector{ detail::AddEnumTo(m_handle, label, value, desc) };
}

template <class>
UIEnumSelector UIFoldout::AddEnum(std::string_view label, int32_t* value,
                                  std::span<const char* const> options) {
    UNIX_USE(menu, add_enum);
    EnumDesc d{};
    d.Options = options;
    return UIEnumSelector{ detail::AddEnumTo(m_handle, label, value, d) };
}

template <class>
UILabel UIFoldout::AddLabel(std::string_view text) {
    UNIX_USE(menu, add_label);
    return UILabel{ detail::g_d.menu_add_label(detail::g_self, m_handle, Sv(text)) };
}

template <class>
UISeparator UIFoldout::AddSeparator() {
    UNIX_USE(menu, add_separator);
    return UISeparator{ detail::g_d.menu_add_separator(detail::g_self, m_handle) };
}

// ---- controls straight on a surface -------------------------------------------------
// The host takes a page handle wherever it takes a foldout handle, so these are the foldout
// bodies with a different parent.

template <class>
UISlider UISurface::AddSlider(std::string_view label, const SliderDesc& desc) {
    UNIX_USE(menu, add_slider);
    return UISlider{ detail::AddSliderTo(m_handle, label, nullptr, desc) };
}

template <class>
UISlider UISurface::AddSlider(std::string_view label, float* value, const SliderDesc& desc) {
    UNIX_USE(menu, add_slider);
    return UISlider{ detail::AddSliderTo(m_handle, label, value, desc) };
}

template <class>
UISlider UISurface::AddSlider(std::string_view label, float* value, float min, float max) {
    UNIX_USE(menu, add_slider);
    SliderDesc d{};
    d.Min = min;
    d.Max = max;
    return UISlider{ detail::AddSliderTo(m_handle, label, value, d) };
}

template <class>
UIEnumSelector UISurface::AddEnum(std::string_view label, const EnumDesc& desc) {
    UNIX_USE(menu, add_enum);
    return UIEnumSelector{ detail::AddEnumTo(m_handle, label, nullptr, desc) };
}

template <class>
UIEnumSelector UISurface::AddEnum(std::string_view label, int32_t* value, const EnumDesc& desc) {
    UNIX_USE(menu, add_enum);
    return UIEnumSelector{ detail::AddEnumTo(m_handle, label, value, desc) };
}

template <class>
UIEnumSelector UISurface::AddEnum(std::string_view label, int32_t* value,
                                  std::span<const char* const> options) {
    UNIX_USE(menu, add_enum);
    EnumDesc d{};
    d.Options = options;
    return UIEnumSelector{ detail::AddEnumTo(m_handle, label, value, d) };
}

template <class>
UILabel UISurface::AddLabel(std::string_view text) {
    UNIX_USE(menu, add_label);
    return UILabel{ detail::g_d.menu_add_label(detail::g_self, m_handle, Sv(text)) };
}

template <class>
UISeparator UISurface::AddSeparator() {
    UNIX_USE(menu, add_separator);
    return UISeparator{ detail::g_d.menu_add_separator(detail::g_self, m_handle) };
}

template <class>
UITextInput UISurface::AddTextInput(std::string_view label, const TextInputDesc& desc) {
    UNIX_USE(menu, add_text_input);
    return UITextInput{ detail::AddTextInputTo(m_handle, label, desc) };
}

template <class>
UIList UISurface::AddList(std::function<void(uint32_t)> onPress) {
    UNIX_USE(menu, add_list);
    return UIList{ detail::AddListTo(m_handle, std::move(onPress)) };
}

template <class>
UITextInput UIFoldout::AddTextInput(std::string_view label, const TextInputDesc& desc) {
    UNIX_USE(menu, add_text_input);
    return UITextInput{ detail::AddTextInputTo(m_handle, label, desc) };
}

template <class>
UIList UIFoldout::AddList(std::function<void(uint32_t)> onPress) {
    UNIX_USE(menu, add_list);
    return UIList{ detail::AddListTo(m_handle, std::move(onPress)) };
}

template <class>
void UIFoldout::SetExpanded(bool expanded) {
    UNIX_USE(menu, set_foldout_expanded);
    detail::g_d.menu_set_foldout_expanded(detail::g_self, m_handle, expanded);
}

template <class>
void UIFoldout::OnChange(std::function<void(bool)> fn) {
    UNIX_USE(menu, on_foldout_change);
    detail::Node* n = nullptr;
    if (fn) n = detail::AllocNode([f = std::move(fn)](const void* p) {
        f(static_cast<const unix_widget_ev*>(p)->b);
    });
    detail::g_d.menu_on_foldout_change(detail::g_self, m_handle,
                                       n ? &detail::TrampWidget : nullptr, n);
    detail::OwnNode(m_handle, n);
}

template <class>
UIFoldout& UIFoldout::SetEnabled(bool on) {
    UNIX_USE(menu, set_control_enabled);
    detail::g_d.menu_set_control_enabled(detail::g_self, m_handle, on);
    return *this;
}

template <class>
UIFoldout& UIFoldout::SetVisible(bool on) {
    UNIX_USE(menu, set_control_visible);
    detail::g_d.menu_set_control_visible(detail::g_self, m_handle, on);
    return *this;
}

template <class>
bool UIFoldout::IsExpanded() const {
    UNIX_USE(menu, get_foldout_expanded);
    return detail::g_d.menu_get_foldout_expanded(detail::g_self, m_handle);
}

template <class>
void UIFoldout::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
UISubPage& UISubPage::SetNavText(std::string_view text) {
    UNIX_USE(menu, set_nav_text);
    detail::g_d.menu_set_nav_text(detail::g_self, m_handle, Sv(text));
    return *this;
}

template <class>
UISubPage& UISubPage::SetNavIcon(Sprite icon) {
    UNIX_USE(menu, set_nav_icon);
    detail::g_d.menu_set_nav_icon(detail::g_self, m_handle, detail::MakeIcon(icon));
    return *this;
}

template <class>
UISubPage& UISubPage::SetNavAlert(bool on) {
    UNIX_USE(menu, set_nav_alert);
    detail::g_d.menu_set_nav_alert(detail::g_self, m_handle, on);
    return *this;
}

template <class>
UIButton UISubPage::GetNavButton() const {
    UNIX_USE(menu, nav_button);
    return UIButton{ detail::g_d.menu_nav_button(detail::g_self, m_handle) };
}

template <class>
UICornerButton UISubPage::AddCornerButton(Sprite icon, std::function<void()> onClick) {
    UNIX_USE(menu, add_corner_button);
    const UIButton nav = GetNavButton();
    return UICornerButton{ detail::AddCornerTo(nav.Handle(), detail::MakeIcon(icon), unix_null,
                                               std::move(onClick)) };
}

template <class>
void UISubPage::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
UIPage::UIPage(std::string_view name, std::string_view tooltip) {
    UNIX_USE(menu, create_page);
    UNIX_USE(menu, on_page_event);
    m_handle = detail::g_d.menu_create_page(detail::g_self, Sv(name), Sv(tooltip));
    if (m_handle)
        m_events = detail::g_d.menu_on_page_event(detail::g_self, m_handle,
                                                  &UIPage::OnPageEvent, this);
}

inline UIPage::~UIPage() {
    detail::Cancel(m_events);
    m_events = unix_null;
}

inline void UIPage::DispatchPageEvent(const unix_page_ev& e) {
    switch (e.kind) {
        case unix_page_built:        OnBuild(); break;
        case unix_page_shown:        OnShow(); break;
        case unix_page_hidden:       OnHide(); break;
        case unix_page_tick:         OnTick(e.dt); break;
        case unix_page_scene_loaded: OnSceneLoaded(); break;
        default: break;
    }
}

inline void UNIX_CC UIPage::OnPageEvent(void* ud, const unix_page_ev* e) {
    if (ud && e) static_cast<UIPage*>(ud)->DispatchPageEvent(*e);
}

template <class>
UIPage UIPage::Find(std::string_view name) {
    UNIX_USE(menu, find_page);
    return UIPage{ detail::g_d.menu_find_page(detail::g_self, Sv(name)) };
}

template <class>
UISubPage UIPage::AddSubPage(std::string_view name) {
    UNIX_USE(menu, create_sub_page);
    return UISubPage{ detail::g_d.menu_create_sub_page(detail::g_self, m_handle, Sv(name)) };
}

template <class>
UIPage& UIPage::SetIcon(Sprite icon) {
    UNIX_USE(menu, set_page_icon);
    detail::g_d.menu_set_page_icon(detail::g_self, m_handle, detail::MakeIcon(icon));
    return *this;
}

template <class>
UIPage& UIPage::SetIcon(void* sprite) {
    UNIX_USE(menu, set_page_icon);
    detail::g_d.menu_set_page_icon(detail::g_self, m_handle, detail::MakeIcon(sprite));
    return *this;
}

template <class>
UIPage& UIPage::SetTitle(std::string_view title) {
    UNIX_USE(menu, set_page_title);
    detail::g_d.menu_set_page_title(detail::g_self, m_handle, Sv(title));
    return *this;
}

template <class>
UIPage& UIPage::SetBadge(bool visible, std::string_view text) {
    UNIX_USE(menu, set_page_badge);
    detail::g_d.menu_set_page_badge(detail::g_self, m_handle, visible, Sv(text), nullptr);
    return *this;
}

template <class>
UIPage& UIPage::SetBadge(bool visible, std::string_view text, Color bg) {
    UNIX_USE(menu, set_page_badge);
    detail::g_d.menu_set_page_badge(detail::g_self, m_handle, visible, Sv(text), &bg);
    return *this;
}

template <class>
void UIPage::NavigateTo() {
    UNIX_USE(menu, navigate_to);
    detail::g_d.menu_navigate_to(detail::g_self, m_handle);
}

template <class>
bool UIPage::IsVisible() const {
    UNIX_USE(menu, page_visible);
    return detail::g_d.menu_page_visible(detail::g_self, m_handle);
}

template <class>
UIPage& UIPage::SetTabVisible(bool visible) {
    UNIX_USE(menu, set_page_tab_visible);
    detail::g_d.menu_set_page_tab_visible(detail::g_self, m_handle, visible);
    return *this;
}

template <class>
void UIPage::Remove() {
    detail::Cancel(m_events);
    m_events = unix_null;
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
UISection UIMainMenuPage::AddSection(std::string_view name) {
    UNIX_USE(mainmenu, add_section);
    return UISection{ detail::g_d.mainmenu_add_section(detail::g_self, m_handle, Sv(name)) };
}

template <class>
void UIMainMenuPage::Open() {
    UNIX_USE(mainmenu, open_page);
    detail::g_d.mainmenu_open_page(detail::g_self, m_handle);
}

template <class>
UIMainMenuPage& UIMainMenuPage::SetTitle(std::string_view title) {
    UNIX_USE(mainmenu, set_page_title);
    detail::g_d.mainmenu_set_page_title(detail::g_self, m_handle, Sv(title));
    return *this;
}

template <class>
void UIMainMenuPage::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
void UISection::Select() {
    UNIX_USE(mainmenu, select_section);
    detail::g_d.mainmenu_select_section(detail::g_self, m_handle);
}

template <class>
UISection& UISection::SetName(std::string_view name) {
    UNIX_USE(mainmenu, set_section_name);
    detail::g_d.mainmenu_set_section_name(detail::g_self, m_handle, Sv(name));
    return *this;
}

template <class>
void UISection::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
UIButton& UIButton::SetText(std::string_view text) {
    UNIX_USE(menu, set_button_text);
    detail::g_d.menu_set_button_text(detail::g_self, m_handle, Sv(text));
    return *this;
}

template <class>
UIButton& UIButton::SetIcon(Sprite icon) {
    UNIX_USE(menu, set_button_icon);
    detail::g_d.menu_set_button_icon(detail::g_self, m_handle, detail::MakeIcon(icon));
    return *this;
}

template <class>
UIButton& UIButton::SetIcon(void* sprite) {
    UNIX_USE(menu, set_button_icon);
    detail::g_d.menu_set_button_icon(detail::g_self, m_handle, detail::MakeIcon(sprite));
    return *this;
}

template <class>
UIButton& UIButton::SetColor(Color c) {
    UNIX_USE(menu, set_button_color);
    detail::g_d.menu_set_button_color(detail::g_self, m_handle, c);
    return *this;
}

template <class>
UIButton& UIButton::SetEnabled(bool on) {
    UNIX_USE(menu, set_button_enabled);
    detail::g_d.menu_set_button_enabled(detail::g_self, m_handle, on);
    return *this;
}

template <class>
UIButton& UIButton::SetVisible(bool on) {
    UNIX_USE(menu, set_button_visible);
    detail::g_d.menu_set_button_visible(detail::g_self, m_handle, on);
    return *this;
}

template <class>
void* UIButton::GetObject() const {
    UNIX_USE(menu, button_object);
    return detail::g_d.menu_button_object(detail::g_self, m_handle);
}

template <class>
UICornerButton UIButton::AddCornerButton(Sprite icon, std::function<void()> onClick) {
    UNIX_USE(menu, add_corner_button);
    return UICornerButton{ detail::AddCornerTo(m_handle, detail::MakeIcon(icon), unix_null,
                                               std::move(onClick)) };
}

template <class>
UICornerButton UIButton::AddCornerPage(Sprite icon, UIPage& target) {
    UNIX_USE(menu, add_corner_button);
    return UICornerButton{ detail::AddCornerTo(m_handle, detail::MakeIcon(icon), target.Handle(),
                                               {}) };
}

template <class>
void UIButton::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
bool UIToggle::GetState() const {
    UNIX_USE(menu, get_toggle);
    return detail::g_d.menu_get_toggle(detail::g_self, m_handle);
}

template <class>
void UIToggle::SetState(bool on) {
    UNIX_USE(menu, set_toggle);
    detail::g_d.menu_set_toggle(detail::g_self, m_handle, on);
}

template <class>
void UIToggle::SetStateSilent(bool on) {
    UNIX_USE(menu, set_toggle_silent);
    detail::g_d.menu_set_toggle_silent(detail::g_self, m_handle, on);
}

template <class>
UIToggle& UIToggle::SetIcon(Sprite icon) {
    UNIX_USE(menu, set_button_icon);
    detail::g_d.menu_set_button_icon(detail::g_self, m_handle, detail::MakeIcon(icon));
    return *this;
}

template <class>
UIToggle& UIToggle::SetIcon(void* sprite) {
    UNIX_USE(menu, set_button_icon);
    detail::g_d.menu_set_button_icon(detail::g_self, m_handle, detail::MakeIcon(sprite));
    return *this;
}

template <class>
UIToggle& UIToggle::SetEnabled(bool on) {
    UNIX_USE(menu, set_button_enabled);
    detail::g_d.menu_set_button_enabled(detail::g_self, m_handle, on);
    return *this;
}

template <class>
UIToggle& UIToggle::SetVisible(bool on) {
    UNIX_USE(menu, set_button_visible);
    detail::g_d.menu_set_button_visible(detail::g_self, m_handle, on);
    return *this;
}

template <class>
void UIToggle::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
float UISlider::GetValue() const {
    UNIX_USE(menu, get_slider);
    return detail::g_d.menu_get_slider(detail::g_self, m_handle);
}

template <class>
void UISlider::SetValue(float v) {
    UNIX_USE(menu, set_slider);
    detail::g_d.menu_set_slider(detail::g_self, m_handle, v);
}

template <class>
void UISlider::SetValueSilent(float v) {
    UNIX_USE(menu, set_slider_silent);
    detail::g_d.menu_set_slider_silent(detail::g_self, m_handle, v);
}

template <class>
void UISlider::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
int32_t UIEnumSelector::GetIndex() const {
    UNIX_USE(menu, get_enum_index);
    return detail::g_d.menu_get_enum_index(detail::g_self, m_handle);
}

template <class>
void UIEnumSelector::SetIndex(int32_t i) {
    UNIX_USE(menu, set_enum_index);
    detail::g_d.menu_set_enum_index(detail::g_self, m_handle, i);
}

template <class>
void UIEnumSelector::SetIndexSilent(int32_t i) {
    UNIX_USE(menu, set_enum_silent);
    detail::g_d.menu_set_enum_silent(detail::g_self, m_handle, i);
}

template <class>
void UIEnumSelector::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
UILabel& UILabel::SetText(std::string_view text) {
    UNIX_USE(menu, set_label_text);
    detail::g_d.menu_set_label_text(detail::g_self, m_handle, Sv(text));
    return *this;
}

template <class>
void UILabel::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
void UISeparator::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

// ---- the shared control setters -----------------------------------------------------
// menu.set_control_enabled and menu.set_control_visible take any control id, so every row
// gets the pair the tiles already had.

#define UNIX_UI_CONTROL_STATE(TYPE)                                              \
    template <class>                                                             \
    TYPE& TYPE::SetEnabled(bool on) {                                            \
        UNIX_USE(menu, set_control_enabled);                                     \
        detail::g_d.menu_set_control_enabled(detail::g_self, m_handle, on);      \
        return *this;                                                            \
    }                                                                            \
    template <class>                                                             \
    TYPE& TYPE::SetVisible(bool on) {                                            \
        UNIX_USE(menu, set_control_visible);                                     \
        detail::g_d.menu_set_control_visible(detail::g_self, m_handle, on);      \
        return *this;                                                            \
    }

UNIX_UI_CONTROL_STATE(UISlider)
UNIX_UI_CONTROL_STATE(UIEnumSelector)
UNIX_UI_CONTROL_STATE(UILabel)
UNIX_UI_CONTROL_STATE(UITextInput)
UNIX_UI_CONTROL_STATE(UIList)
#undef UNIX_UI_CONTROL_STATE

template <class>
UISeparator& UISeparator::SetVisible(bool on) {
    UNIX_USE(menu, set_control_visible);
    detail::g_d.menu_set_control_visible(detail::g_self, m_handle, on);
    return *this;
}

// ---- text input ---------------------------------------------------------------------

template <class>
std::string UITextInput::GetText() const {
    UNIX_USE(menu, get_text_input);
    const uint32_t need = detail::g_d.menu_get_text_input(detail::g_self, m_handle, nullptr, 0);
    if (!need) return {};
    std::string out(static_cast<size_t>(need) + 1, '\0');   // room for the host's terminator
    const uint32_t got = detail::g_d.menu_get_text_input(detail::g_self, m_handle, out.data(),
                                                         need + 1);
    out.resize(got < need ? got : need);
    return out;
}

template <class>
UITextInput& UITextInput::SetText(std::string_view text) {
    UNIX_USE(menu, set_text_input);
    detail::g_d.menu_set_text_input(detail::g_self, m_handle, Sv(text));
    return *this;
}

template <class>
void UITextInput::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

// ---- list ---------------------------------------------------------------------------

template <class>
void UIList::SetItems(std::span<const std::string_view> items) {
    UNIX_USE(menu, set_list_items);
    std::vector<unix_str> rows;
    rows.reserve(items.size());
    for (auto& s : items) rows.push_back(Sv(s));
    detail::g_d.menu_set_list_items(detail::g_self, m_handle,
                                    rows.empty() ? nullptr : rows.data(),
                                    static_cast<uint32_t>(rows.size()));
}

template <class>
void UIList::SetItems(std::span<const char* const> items) {
    UNIX_USE(menu, set_list_items);
    std::vector<unix_str> rows;
    rows.reserve(items.size());
    for (const char* s : items) rows.push_back(Sv(s ? std::string_view{ s } : std::string_view{}));
    detail::g_d.menu_set_list_items(detail::g_self, m_handle,
                                    rows.empty() ? nullptr : rows.data(),
                                    static_cast<uint32_t>(rows.size()));
}

template <class>
void UIList::Clear() {
    UNIX_USE(menu, clear_list);
    detail::g_d.menu_clear_list(detail::g_self, m_handle);
}

template <class>
uint32_t UIList::Count() const {
    UNIX_USE(menu, list_count);
    return detail::g_d.menu_list_count(detail::g_self, m_handle);
}

template <class>
UIButton UIList::Row(uint32_t index) const {
    UNIX_USE(menu, list_row);
    return UIButton{ detail::g_d.menu_list_row(detail::g_self, m_handle, index) };
}

template <class>
void UIList::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
UICornerButton& UICornerButton::SetColor(Color c) {
    UNIX_USE(menu, set_button_color);
    detail::g_d.menu_set_button_color(detail::g_self, m_handle, c);
    return *this;
}

template <class>
UICornerButton& UICornerButton::SetIcon(Sprite icon) {
    UNIX_USE(menu, set_button_icon);
    detail::g_d.menu_set_button_icon(detail::g_self, m_handle, detail::MakeIcon(icon));
    return *this;
}

template <class>
void UICornerButton::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
UIUserButton& UIUserButton::SetText(std::string_view text) {
    UNIX_USE(users, set_button_text);
    detail::g_d.users_set_button_text(detail::g_self, m_handle, Sv(text));
    return *this;
}

template <class>
UIUserButton& UIUserButton::SetIcon(Sprite icon) {
    UNIX_USE(users, set_button_icon);
    detail::g_d.users_set_button_icon(detail::g_self, m_handle, detail::MakeIcon(icon));
    return *this;
}

template <class>
void UIUserButton::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
UIUserButton UIUserRow::AddButton(std::string_view text, Sprite icon,
                                  std::function<void(const UserEvent&)> onClick) {
    UNIX_USE(users, add_row_button);
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
    // The user-page rows deliver a unix_user_ev through this slot, not a unix_widget_ev.
    d.on_click = n ? reinterpret_cast<unix_cb_widget>(&detail::TrampUser) : nullptr;
    d.ud       = n;
    const unix_handle h = detail::g_d.users_add_row_button(detail::g_self, m_handle, &d);
    detail::OwnNode(h, n);
    return UIUserButton{ h };
}

template <class>
UIUserRow& UIUserRow::SetFront(bool front) {
    UNIX_USE(users, set_row_front);
    detail::g_d.users_set_row_front(detail::g_self, m_handle, front);
    return *this;
}

template <class>
UIFoldout UIUserRow::AddFoldout(std::string_view title, bool expanded, bool background,
                                bool autoSeparators) {
    UNIX_USE(users, add_foldout);
    unix_foldout_desc d{};
    d.size            = sizeof(d);
    d.title           = Sv(title);
    d.expanded        = expanded;
    d.background      = background;
    d.auto_separators = autoSeparators;
    return UIFoldout{ detail::g_d.users_add_foldout(detail::g_self, m_handle, &d) };
}

template <class>
void UIUserRow::Remove() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
bool Nameplate::IsAlive() const {
    UNIX_USE(plates, alive);
    return detail::g_d.plates_alive(detail::g_self, m_handle);
}

template <class>
Nameplate& Nameplate::SetText(std::string_view text) {
    UNIX_USE(plates, set_text);
    detail::g_d.plates_set_text(detail::g_self, m_handle, Sv(text));
    return *this;
}

template <class>
Nameplate& Nameplate::SetTextColor(Color c) {
    UNIX_USE(plates, set_text_color);
    detail::g_d.plates_set_text_color(detail::g_self, m_handle, c);
    return *this;
}

template <class>
Nameplate& Nameplate::SetIconColor(Color c) {
    UNIX_USE(plates, set_icon_color);
    detail::g_d.plates_set_icon_color(detail::g_self, m_handle, c);
    return *this;
}

template <class>
Nameplate& Nameplate::SetIcon(Sprite icon) {
    UNIX_USE(plates, set_icon);
    detail::g_d.plates_set_icon(detail::g_self, m_handle, detail::MakeIcon(icon));
    return *this;
}

template <class>
Nameplate& Nameplate::SetIcon(void* sprite) {
    UNIX_USE(plates, set_icon);
    detail::g_d.plates_set_icon(detail::g_self, m_handle, detail::MakeIcon(sprite));
    return *this;
}

template <class>
Nameplate& Nameplate::SetPosition(Vec3 offset) {
    UNIX_USE(plates, set_position);
    detail::g_d.plates_set_position(detail::g_self, m_handle, offset);
    return *this;
}

template <class>
Nameplate& Nameplate::SetWidth(float w) {
    UNIX_USE(plates, set_width);
    detail::g_d.plates_set_width(detail::g_self, m_handle, w);
    return *this;
}

template <class>
Nameplate& Nameplate::SetHeight(float h) {
    UNIX_USE(plates, set_height);
    detail::g_d.plates_set_height(detail::g_self, m_handle, h);
    return *this;
}

template <class>
Nameplate& Nameplate::SetBackground(bool on) {
    UNIX_USE(plates, set_background_enabled);
    detail::g_d.plates_set_background_enabled(detail::g_self, m_handle, on);
    return *this;
}

template <class>
void* Nameplate::GetObject() const {
    UNIX_USE(plates, object);
    return detail::g_d.plates_object(detail::g_self, m_handle);
}

template <class>
void* Nameplate::GetTextObject() const {
    UNIX_USE(plates, text_object);
    return detail::g_d.plates_text_object(detail::g_self, m_handle);
}

template <class>
void* Nameplate::GetBackgroundObject() const {
    UNIX_USE(plates, background_object);
    return detail::g_d.plates_background_object(detail::g_self, m_handle);
}

template <class>
void Nameplate::Destroy() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
void Tween::Cancel() {
    detail::Cancel(m_handle);
    m_handle = unix_null;
}

template <class>
UIToggle UIToggleHandler::Attach(UIFoldout parent, std::string_view label, bool def, bool* bound) {
    ToggleDesc d{};
    d.OnChange = [this](bool on) { OnChanged(on); };
    m_control = bound ? parent.AddToggle(label, bound, d) : parent.AddToggle(label, def, d);
    return m_control;
}

template <class>
UIToggle UIToggleHandler::Attach(UISurface parent, std::string_view label, bool def, bool* bound) {
    ToggleDesc d{};
    d.OnChange = [this](bool on) { OnChanged(on); };
    m_control = bound ? parent.AddToggle(label, bound, d) : parent.AddToggle(label, def, d);
    return m_control;
}

template <class>
UISlider UISliderHandler::Attach(UIFoldout parent, std::string_view label, float min, float max,
                                 float* bound) {
    SliderDesc d{};
    d.Min = min;
    d.Max = max;
    d.OnChange = [this](float v) { OnChanged(v); };
    m_control = parent.AddSlider(label, bound, d);
    return m_control;
}

template <class>
UIEnumSelector UIEnumHandler::Attach(UIFoldout parent, std::string_view label,
                                     std::span<const char* const> options, int32_t* bound) {
    EnumDesc d{};
    d.Options = options;
    d.OnChange = [this](int32_t i) { OnChanged(i); };
    m_control = parent.AddEnum(label, bound, d);
    return m_control;
}


// ============================================================ Action Menu

/// <summary>One item on VRChat's radial Action Menu.
///
/// The wheel rebuilds a page from its Build() on every open and destroys the previous items,
/// so an item is only valid inside the Build() that made it and inside the callbacks that
/// Build() installed. Keep state in your module, not in an item.</summary>
class ActionItem {
public:
    ActionItem() = default;
    explicit ActionItem(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }

    template <class = void> ActionItem& SetLabel(std::string_view text);
    /// <summary>Label by localization key, the way VRChat labels its own pedals — the wheel's
    /// "Options" carries the key, not the word, so it follows the player's language.</summary>
    /// <param name="key">Localization key; empty uses the fallback verbatim.</param>
    template <class = void> ActionItem& SetLabelKey(std::string_view key,
                                                    std::string_view fallback = {});
    /// <summary>Text drawn inside the pedal instead of on the ring around it. VRChat leaves
    /// this empty on its own root items and uses it to show values.</summary>
    /// <param name="ringLabel">The ring label, set in the same call because the game exposes
    /// the pair as a single method.</param>
    template <class = void> ActionItem& SetCenterLabel(std::string_view text,
                                                       std::string_view ringLabel = {});
    /// <param name="name">A field of VRChat's own menu icon table: home, options, config,
    /// expressions, tools, inventory, props, back, close, toggleOn, toggleOff, arrowUp,
    /// arrowDown, arrowLeft, arrowRight, folder, micOn, micOff, camera, delete, ...</param>
    template <class = void> ActionItem& SetIcon(std::string_view name);
    /// <param name="texture">A UnityEngine.Texture2D.</param>
    template <class = void> ActionItem& SetIcon(void* texture);
    template <class = void> ActionItem& SetEnabled(bool enabled);
    /// <summary>Radial fill, 0..1.</summary>
    template <class = void> ActionItem& SetValue(float value);
    /// <summary>The item's own component, for anything this API does not wrap.</summary>
    template <class = void> [[nodiscard]] void* Object() const;

private:
    unix_handle m_handle{};
};

namespace detail {

/// Stable storage for one item's callbacks. The host is handed a pointer to a slot as the
/// item's user data, and clears its own item table at the top of every build, so the slots
/// are safe to recycle there too.
struct ActionSlot {
    std::function<void()>      click;
    std::function<bool()>      state;
    std::function<float()>     radialGet;
    std::function<void(float)> radialSet;
};
inline std::deque<ActionSlot> g_amSlots;

inline void UNIX_CC AmClickThunk(void* ud, const unix_widget_ev*) {
    auto* s = static_cast<ActionSlot*>(ud);
    if (s && s->click) s->click();
}
inline bool UNIX_CC AmStateThunk(void* ud) {
    auto* s = static_cast<ActionSlot*>(ud);
    return s && s->state ? s->state() : false;
}
inline float UNIX_CC AmRadialGetThunk(void* ud) {
    auto* s = static_cast<ActionSlot*>(ud);
    return s && s->radialGet ? s->radialGet() : 0.0f;
}
inline void UNIX_CC AmRadialSetThunk(void* ud, float value) {
    auto* s = static_cast<ActionSlot*>(ud);
    if (s && s->radialSet) s->radialSet(value);
}

}   // namespace detail

/// <summary>A page on VRChat's radial Action Menu — the wheel, not the QuickMenu.
///
/// Subclass it and override Build(). A page is a builder rather than a retained widget tree:
/// Build() runs every time the page is opened and must add every item the page shows. Toggles
/// are predicates the menu polls, so there is no state to push.
///
/// The root wheel belongs to VRChat and wipes its items whenever it rebuilds, so an entry on
/// it is a standing request: AddToRootMenu re-adds it for as long as the page lives.</summary>
class ActionPage {
public:
    ActionPage() = default;
    /// <param name="name">Identifies the page in logs; the wheel shows item labels, not it.</param>
    template <class = void> explicit ActionPage(std::string_view name);
    ActionPage(const ActionPage&) = delete;
    ActionPage& operator=(const ActionPage&) = delete;
    virtual ~ActionPage();

    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }

    /// <summary>Whether the Action Menu resolved on this build of the game.</summary>
    template <class = void> [[nodiscard]] static bool IsReady();
    /// <summary>One of the menu's own icons, by name. Null when the name is unknown.</summary>
    template <class = void> [[nodiscard]] static void* BuiltinIcon(std::string_view name);

    /// <summary>Push the page onto the wheel, running Build().</summary>
    template <class = void> void Open();
    /// <summary>Pop it, running the menu's own close path.</summary>
    template <class = void> void Close();
    template <class = void> void Remove();

    /// <summary>Give the page an entry on VRChat's root wheel.</summary>
    /// <param name="everyPage">Place it on whichever page is showing, VRChat's own sub-pages
    /// included, rather than only on the root.</param>
    template <class = void> void AddToRootMenu(std::string_view label,
                                               std::string_view icon = {},
                                               bool everyPage = false);
    template <class = void> void RemoveFromRootMenu();
    /// <summary>Art for the root entry, which AddToRootMenu cannot take: the wheel replaces
    /// that pedal on every rebuild, and a Texture2D cannot be made before the graphics device
    /// is up. Call it once you have one.</summary>
    /// <param name="texture">A UnityEngine.Texture2D.</param>
    template <class = void> void SetRootIcon(void* texture);

    // ---- only inside Build() ----

    template <class = void> ActionItem AddButton(std::string_view label,
                                                 std::function<void()> onClick = {});
    /// <param name="state">Polled by the menu for the tick. Keep it cheap and pure.</param>
    template <class = void> ActionItem AddToggle(std::string_view label,
                                                 std::function<bool()> state,
                                                 std::function<void()> onClick);
    template <class = void> ActionItem AddSubPage(std::string_view label,
                                                  const ActionPage& page);
    /// <summary>A pedal that opens the wheel's drag ring on a value of your own. The ring is
    /// VRChat's radial puppet; the host binds it to a parameter it owns, so nothing on the
    /// avatar is read or written.</summary>
    /// <param name="get">Current value as 0..1. Polled while the ring is being dragged.</param>
    /// <param name="set">Receives the new 0..1 value as the ring moves.</param>
    template <class = void> ActionItem AddRadial(std::string_view label,
                                                 std::function<float()> get,
                                                 std::function<void(float)> set);
    /// <summary>An explicit back item. The menu already adds one below the root, so this is
    /// only for a page that wants a second.</summary>
    template <class = void> ActionItem AddBack();

protected:
    /// <summary>Add the page's items. Runs on the Unity main thread, on every open.</summary>
    virtual void Build() {}

private:
    static void UNIX_CC OnBuildThunk(void* ud);
    unix_handle m_handle{};
    unix_handle m_root{};
};

// ---- ActionItem ----

template <class>
ActionItem& ActionItem::SetLabel(std::string_view text) {
    UNIX_USE(action, set_item_label);
    detail::g_d.action_set_item_label(detail::g_self, m_handle, Sv(text));
    return *this;
}

template <class>
ActionItem& ActionItem::SetLabelKey(std::string_view key, std::string_view fallback) {
    UNIX_USE(action, set_item_label_key);
    detail::g_d.action_set_item_label_key(detail::g_self, m_handle, Sv(key), Sv(fallback));
    return *this;
}

template <class>
ActionItem& ActionItem::SetCenterLabel(std::string_view text, std::string_view ringLabel) {
    UNIX_USE(action, set_item_center);
    detail::g_d.action_set_item_center(detail::g_self, m_handle, Sv(text), Sv(ringLabel));
    return *this;
}

template <class>
ActionItem& ActionItem::SetIcon(std::string_view name) {
    UNIX_USE(action, set_item_icon);
    detail::g_d.action_set_item_icon(detail::g_self, m_handle, Sv(name), nullptr);
    return *this;
}

template <class>
ActionItem& ActionItem::SetIcon(void* texture) {
    UNIX_USE(action, set_item_icon);
    detail::g_d.action_set_item_icon(detail::g_self, m_handle, Sv({}), texture);
    return *this;
}

template <class>
ActionItem& ActionItem::SetEnabled(bool enabled) {
    UNIX_USE(action, set_item_enabled);
    detail::g_d.action_set_item_enabled(detail::g_self, m_handle, enabled);
    return *this;
}

template <class>
ActionItem& ActionItem::SetValue(float value) {
    UNIX_USE(action, set_item_value);
    detail::g_d.action_set_item_value(detail::g_self, m_handle, value);
    return *this;
}

template <class>
void* ActionItem::Object() const {
    UNIX_USE(action, item_object);
    return detail::g_d.action_item_object(detail::g_self, m_handle);
}

// ---- ActionPage ----

template <class>
ActionPage::ActionPage(std::string_view name) {
    UNIX_USE(action, create_page);
    m_handle = detail::g_d.action_create_page(detail::g_self, Sv(name),
                                              &ActionPage::OnBuildThunk, this);
}

inline ActionPage::~ActionPage() {
    // The host holds a pointer to this object as the build callback's user data, so the
    // registration has to go with the object even when the caller never called Remove.
    if (m_root) {
        UNIX_USE(action, remove_root_item);
        detail::g_d.action_remove_root_item(detail::g_self, m_root);
        m_root = unix_null;
    }
    if (m_handle) {
        UNIX_USE(action, remove_page);
        detail::g_d.action_remove_page(detail::g_self, m_handle);
        m_handle = unix_null;
    }
}

inline void UNIX_CC ActionPage::OnBuildThunk(void* ud) {
    // The host dropped this module's items just before calling, so the slots they pointed at
    // are free; recycling them here is what keeps one deque from growing per menu open.
    detail::g_amSlots.clear();
    if (ud) static_cast<ActionPage*>(ud)->Build();
}

template <class>
bool ActionPage::IsReady() {
    UNIX_USE(action, is_ready);
    return detail::g_d.action_is_ready(detail::g_self);
}

template <class>
void* ActionPage::BuiltinIcon(std::string_view name) {
    UNIX_USE(action, builtin_icon);
    return detail::g_d.action_builtin_icon(detail::g_self, Sv(name));
}

template <class>
void ActionPage::Open() {
    UNIX_USE(action, open_page);
    detail::g_d.action_open_page(detail::g_self, m_handle);
}

template <class>
void ActionPage::Close() {
    UNIX_USE(action, close_page);
    detail::g_d.action_close_page(detail::g_self, m_handle);
}

template <class>
void ActionPage::Remove() {
    UNIX_USE(action, remove_page);
    detail::g_d.action_remove_page(detail::g_self, m_handle);
    m_handle = unix_null;
}

template <class>
void ActionPage::AddToRootMenu(std::string_view label, std::string_view icon, bool everyPage) {
    UNIX_USE(action, add_root_item);
    UNIX_USE(action, set_root_scope);
    if (m_root) RemoveFromRootMenu();
    m_root = detail::g_d.action_add_root_item(detail::g_self, m_handle, Sv(label), Sv(icon));
    if (m_root && everyPage && detail::g_d.action_set_root_scope)
        detail::g_d.action_set_root_scope(detail::g_self, m_root, true);
}

template <class>
void ActionPage::RemoveFromRootMenu() {
    UNIX_USE(action, remove_root_item);
    if (!m_root) return;
    detail::g_d.action_remove_root_item(detail::g_self, m_root);
    m_root = unix_null;
}

template <class>
void ActionPage::SetRootIcon(void* texture) {
    UNIX_USE(action, set_root_icon);
    if (m_root) detail::g_d.action_set_root_icon(detail::g_self, m_root, texture);
}

template <class>
ActionItem ActionPage::AddButton(std::string_view label, std::function<void()> onClick) {
    UNIX_USE(action, add_item);
    auto& slot = detail::g_amSlots.emplace_back();
    slot.click = std::move(onClick);
    unix_am_desc d{};
    d.size     = sizeof(d);
    d.label    = Sv(label);
    d.on_click = slot.click ? &detail::AmClickThunk : nullptr;
    d.ud       = &slot;
    return ActionItem{ detail::g_d.action_add_item(detail::g_self, &d) };
}

template <class>
ActionItem ActionPage::AddToggle(std::string_view label, std::function<bool()> state,
                                 std::function<void()> onClick) {
    UNIX_USE(action, add_item);
    auto& slot = detail::g_amSlots.emplace_back();
    slot.click = std::move(onClick);
    slot.state = std::move(state);
    unix_am_desc d{};
    d.size     = sizeof(d);
    d.label    = Sv(label);
    d.on_click = slot.click ? &detail::AmClickThunk : nullptr;
    d.state    = slot.state ? &detail::AmStateThunk : nullptr;
    d.ud       = &slot;
    return ActionItem{ detail::g_d.action_add_item(detail::g_self, &d) };
}

template <class>
ActionItem ActionPage::AddSubPage(std::string_view label, const ActionPage& page) {
    UNIX_USE(action, add_item);
    unix_am_desc d{};
    d.size  = sizeof(d);
    d.label = Sv(label);
    d.page  = page.Handle();
    return ActionItem{ detail::g_d.action_add_item(detail::g_self, &d) };
}

template <class>
ActionItem ActionPage::AddRadial(std::string_view label, std::function<float()> get,
                                 std::function<void(float)> set) {
    UNIX_USE(action, add_radial);
    auto& slot = detail::g_amSlots.emplace_back();
    slot.radialGet = std::move(get);
    slot.radialSet = std::move(set);
    return ActionItem{ detail::g_d.action_add_radial(
        detail::g_self, Sv(label),
        slot.radialGet ? &detail::AmRadialGetThunk : nullptr,
        slot.radialSet ? &detail::AmRadialSetThunk : nullptr, &slot) };
}

template <class>
ActionItem ActionPage::AddBack() {
    UNIX_USE(action, add_item);
    unix_am_desc d{};
    d.size = sizeof(d);
    d.back = true;
    return ActionItem{ detail::g_d.action_add_item(detail::g_self, &d) };
}

}   // namespace UNIx
