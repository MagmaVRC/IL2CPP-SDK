#pragma once
#ifndef UNIX_USE
#  error "<unix/services/hud.hpp> is included by <unix/module.hpp>; include that instead"
#endif

namespace UNIx {

/// <summary>One sample of a HUD widget.</summary>
struct HudReading {
    /// <summary>Shown as is when Numeric is false; at most 63 bytes are kept.</summary>
    std::string Text;
    /// <summary>Shown rounded and followed by Suffix when Numeric is true.</summary>
    float Value = 0.f;
    bool Numeric = false;
    /// <summary>At most 15 bytes are kept.</summary>
    std::string Suffix;
    /// <summary>0 bad to 1 good for the value colour mode; below 0 is unrated.</summary>
    float Quality = -1.f;
    /// <summary>Sprite::None keeps the registered icon.</summary>
    Sprite Icon = Sprite::None;
};

/// <summary>A cell on the client's HUD stats strip.</summary>
struct HudWidgetDesc {
    /// <summary>Stable id, unique within this module; also its settings key.</summary>
    std::string_view Id, Title;
    Sprite Icon = Sprite::Bundle_Pulse;
    /// <summary>Text width in HUD units.</summary>
    float Width = 72.f;
    /// <summary>Seconds between samples.</summary>
    float Interval = 1.f;
    /// <summary>Whether the cell is on before the user picks.</summary>
    bool DefaultOn = false;
    /// <summary>Called on the Unity main thread every Interval. Nothing leaves the cell blank.</summary>
    std::function<std::optional<HudReading>()> Sample;
};

/// <summary>A registered HUD widget.</summary>
class HudWidget {
public:
    HudWidget() = default;
    explicit HudWidget(unix_handle h) noexcept : m_handle(h) {}
    [[nodiscard]] explicit operator bool() const noexcept { return m_handle != unix_null; }
    [[nodiscard]] unix_handle Handle() const noexcept { return m_handle; }
    friend bool operator==(HudWidget a, HudWidget b) noexcept { return a.m_handle == b.m_handle; }

    /// <summary>Turn the cell on or off, as the user's own HUD setting does.</summary>
    template <class = void> HudWidget& SetEnabled(bool on) {
        UNIX_USE(hud, set_widget_enabled);
        detail::g_d.hud_set_widget_enabled(detail::g_self, m_handle, on);
        return *this;
    }
    /// <summary>Unregister the widget. Safe from any thread. After it returns, the callback is
    /// never called again.</summary>
    template <class = void> void Remove() {
        detail::Cancel(m_handle);
        m_handle = unix_null;
    }
private:
    unix_handle m_handle{};
};

namespace detail {
inline bool UNIX_CC TrampHudSample(void* ud, unix_hud_sample* out) {
    auto* n = static_cast<Node*>(ud);
    if (!n || !out) return false;
    NodeScope s(n);
    return s && n->ret ? n->ret(out) : false;
}
}   // namespace detail

/// <summary>The client's HUD stats strip. Needs unix_cap_hud.</summary>
class Hud {
public:
    /// <summary>Add a cell. Empty when Id or Sample is empty.</summary>
    template <class = void> HudWidget AddWidget(const HudWidgetDesc& desc) {
        UNIX_USE(hud, add_widget);
        if (desc.Id.empty() || !desc.Sample) return HudWidget{};
        detail::Node* n = detail::AllocNodeRaw();
        n->ret = [f = desc.Sample](const void* p) -> bool {
            auto* out = static_cast<unix_hud_sample*>(const_cast<void*>(p));
            std::optional<HudReading> r = f();
            if (!r) return false;
            detail::CopyFixed(out->text, r->Text);
            detail::CopyFixed(out->suffix, r->Suffix);
            out->value   = r->Value;
            out->numeric = r->Numeric;
            out->quality = r->Quality;
            out->icon    = detail::MakeIcon(r->Icon);
            return true;
        };
        unix_hud_widget_desc d{};
        d.size       = sizeof(d);
        d.id         = Sv(desc.Id);
        d.title      = Sv(desc.Title);
        d.icon       = detail::MakeIcon(desc.Icon);
        d.width      = desc.Width;
        d.interval   = desc.Interval;
        d.default_on = desc.DefaultOn;
        d.sample     = &detail::TrampHudSample;
        d.ud         = n;
        const unix_handle h = detail::g_d.hud_add_widget(detail::g_self, &d);
        detail::OwnNode(h, n);
        return HudWidget{ h };
    }
};

}   // namespace UNIx
