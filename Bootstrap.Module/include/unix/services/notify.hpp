#pragma once
#ifndef UNIX_USE
#  error "<unix/services/notify.hpp> is included by <unix/module.hpp>; include that instead"
#endif

#include <deque>

namespace UNIx {

/// <summary>Which of VRChat's HUD widgets draws the notification.</summary>
enum class NotifyStyle : uint32_t {
    Auto         = unix_notify_auto,
    Card         = unix_notify_card,
    Pill         = unix_notify_pill,
    Banner       = unix_notify_banner,
    Toast        = unix_notify_toast,
    Event        = unix_notify_event,
    Voyager      = unix_notify_voyager,
    VoyagerBig   = unix_notify_voyager_big,
    /// <summary>A menu page in VRChat's own styling, the shape of its News window: heading,
    /// body and up to two buttons. It stays up until one is pressed rather than expiring.</summary>
    Announcement = unix_notify_announcement,
};

/// <summary>Which HUD anchor a style hangs off. Centre is the only one on screen in desktop.</summary>
enum class NotifyAnchor : uint32_t {
    Left = unix_notify_left, Centre = unix_notify_centre, Right = unix_notify_right,
};

/// <summary>Where a notification sits in the queue when the widget is busy. Every module shares
/// these widgets, so High jumps your own and other modules' Normal rows, never their High ones.
/// </summary>
enum class NotifyPriority : uint32_t {
    Low = unix_notify_low, Normal = unix_notify_normal, High = unix_notify_high,
};

namespace detail {
/// <summary>Fold a channel name into the tag the host groups by.</summary>
constexpr uint32_t ChannelTag(std::string_view name) noexcept {
    uint32_t h = 2166136261u;
    for (char c : name) { h ^= static_cast<uint8_t>(c); h *= 16777619u; }
    return h ? h : 1u;
}

// An announcement outlives the builder that described it, so its handlers live here rather
// than in the temporary. One slot per announcement still on screen; the host answers exactly
// once, which is when the slot is freed.
struct NotifyActions {
    std::vector<std::function<void()>> buttons;
    std::function<void()> on_close;
    bool live = false;
};
inline std::vector<NotifyActions>& NotifySlots() {
    static std::vector<NotifyActions> slots;
    return slots;
}
inline void UNIX_CC NotifyActionTramp(void* ud, uint32_t button) {
    auto& slots = NotifySlots();
    const size_t i = reinterpret_cast<size_t>(ud);
    if (i >= slots.size() || !slots[i].live) return;

    // A toggle is not the answer: run its handler and leave the slot for the button that is.
    if (button & unix_notify_toggled) {
        const uint32_t row = button & ~unix_notify_toggled;
        if (row < slots[i].buttons.size() && slots[i].buttons[row]) slots[i].buttons[row]();
        return;
    }

    NotifyActions taken = std::move(slots[i]);
    slots[i] = {};
    if (button == unix_notify_no_answer) { if (taken.on_close) taken.on_close(); return; }
    if (button < taken.buttons.size() && taken.buttons[button]) taken.buttons[button]();
}
inline void* ClaimNotifySlot(NotifyActions a) {
    a.live = true;
    auto& slots = NotifySlots();
    for (size_t i = 0; i < slots.size(); ++i) {
        if (slots[i].live) continue;
        slots[i] = std::move(a);
        return reinterpret_cast<void*>(i);
    }
    slots.push_back(std::move(a));
    return reinterpret_cast<void*>(slots.size() - 1);
}
}   // namespace detail

/// <summary>A notification that is on screen. Dismiss() takes it down early; a handle whose
/// notification has already expired is inert rather than an error.</summary>
class Notice {
public:
    constexpr Notice() noexcept = default;
    constexpr explicit Notice(unix_handle h) noexcept : id_(h) {}

    [[nodiscard]] constexpr unix_handle Id() const noexcept { return id_; }
    [[nodiscard]] constexpr bool Valid() const noexcept { return id_ != unix_null; }
    constexpr explicit operator bool() const noexcept { return Valid(); }

    template <class = void> bool Dismiss() const {
        UNIX_USE(notify, dismiss);
        return id_ && detail::g_d.notify_dismiss(detail::g_self, id_) == unix_ok;
    }

private:
    unix_handle id_ = unix_null;
};

/// <summary>One notification, built a field at a time and then shown. Every setter returns the
/// builder, and a field the chosen style has no slot for is ignored rather than refused.</summary>
class Notification {
public:
    Notification() { desc_.size = sizeof(desc_); }
    explicit Notification(std::string_view text) : Notification() { Text(text); }

    /// <summary>The upper, smaller line. Styles with one line show Text and drop this.</summary>
    Notification& Title(std::string_view s) { title_ = s; return *this; }
    /// <summary>The main line, and the only one the single-line styles show.</summary>
    Notification& Text(std::string_view s)  { text_  = s; return *this; }
    /// <summary>The trailing pill on the event toast, beside BadgeIcon.</summary>
    Notification& Badge(std::string_view s) { badge_ = s; return *this; }

    Notification& Icon(Sprite s)      { desc_.icon = detail::MakeIcon(s); return *this; }
    Notification& Icon(void* sprite)  { desc_.icon = { unix_icon_sprite_ptr, sprite, 0 }; return *this; }
    Notification& BadgeIcon(Sprite s) { desc_.badge_icon = detail::MakeIcon(s); return *this; }
    Notification& BadgeIcon(void* sprite) {
        desc_.badge_icon = { unix_icon_sprite_ptr, sprite, 0 };
        return *this;
    }

    /// <summary>Fill the panel behind the text with a downloaded image. Setting this promotes a
    /// Pill to a Banner, which is the styling VRChat gives group and event announcements.</summary>
    Notification& Image(std::string_view url) { image_ = url; return *this; }

    /// <summary>Tint the widget's own background. Left unset, it keeps VRChat's colour.</summary>
    Notification& Tint(Color c) { desc_.tint = c; return *this; }
    /// <summary>Colour the text lines. Left unset, they keep VRChat's colour.</summary>
    Notification& TextColor(Color c) { desc_.text_color = c; return *this; }
    /// <summary>Scale the text lines' font size: 0.8 is four fifths of VRChat's.</summary>
    Notification& TextScale(float scale) { desc_.text_scale = scale; return *this; }

    /// <summary>How long it stays up. Left unset, each style keeps its own default.</summary>
    Notification& For(float seconds) { desc_.seconds = seconds; return *this; }
    Notification& At(NotifyAnchor a) {
        desc_.placement = static_cast<unix_notify_placement>(a);
        return *this;
    }
    Notification& As(NotifyStyle s) { desc_.style = static_cast<unix_notify_style>(s); return *this; }
    /// <summary>Play VRChat's notification chime with it. Off by default.</summary>
    Notification& WithSound(bool on = true) { desc_.sound = on; return *this; }

    /// <summary>Where this sits in the queue while another module's notification is up.</summary>
    Notification& Priority(NotifyPriority p) {
        desc_.priority = static_cast<unix_notify_priority>(p);
        return *this;
    }
    /// <summary>Group rows so Replace, Coalesce and CancelChannel can act on them together.
    /// The tag is scoped to this module: two modules may use the same name freely.</summary>
    Notification& Channel(std::string_view name) {
        desc_.channel = detail::ChannelTag(name);
        return *this;
    }
    /// <summary>Drop this module's queued rows on the same channel first, so a status line
    /// that updates faster than it displays shows the newest text rather than a backlog.</summary>
    Notification& Replace(bool on = true) {
        desc_.flags = on ? (desc_.flags | unix_notify_f_replace)
                         : (desc_.flags & ~unix_notify_f_replace);
        return *this;
    }
    /// <summary>Fold into an identical queued row instead of adding another.</summary>
    Notification& Coalesce(bool on = true) {
        desc_.flags = on ? (desc_.flags | unix_notify_f_coalesce)
                         : (desc_.flags & ~unix_notify_f_coalesce);
        return *this;
    }
    /// <summary>Drop this one when the widget is busy rather than waiting for a turn.</summary>
    Notification& SkipIfBusy(bool on = true) {
        desc_.flags = on ? (desc_.flags | unix_notify_f_skip_busy)
                         : (desc_.flags & ~unix_notify_f_skip_busy);
        return *this;
    }
    /// <summary>Cut the queue and take the widget now. For one module interrupting itself,
    /// not for talking over other modules.</summary>
    Notification& Immediate(bool on = true) {
        desc_.flags = on ? (desc_.flags | unix_notify_f_now) : (desc_.flags & ~unix_notify_f_now);
        return *this;
    }

    /// <summary>Hand it to the HUD. An empty Notice means no widget could take it.</summary>
    template <class = void> Notice Show() const {
        UNIX_USE(notify, show);
        unix_notification d = desc_;
        d.title       = Sv(title_);
        d.text        = Sv(text_);
        d.badge_text  = Sv(badge_);
        d.image_url   = Sv(image_);
        return Notice{ detail::g_d.notify_show(detail::g_self, &d) };
    }

    /// <summary>The raw record, for a caller that would rather fill it itself.</summary>
    [[nodiscard]] const unix_notification& Desc() const noexcept { return desc_; }

private:
    unix_notification desc_{};
    std::string title_, text_, badge_, image_;
};

/// <summary>A page in the shape of VRChat's News window, described as rows from top to bottom:
/// a tag, a heading, paragraphs, an image, separators, a toggle, buttons, small print. Rows are
/// drawn in the order they are added, so the layout is the module's rather than a fixed set of
/// slots, and the page is built from the menu's own row templates.</summary>
class Announcement {
public:
    Announcement() { desc_.size = sizeof(desc_); desc_.style = unix_notify_announcement; }
    explicit Announcement(std::string_view heading) : Announcement() { Heading(heading); }

    /// <summary>The window's own title bar, the way VRChat's reads "News".</summary>
    Announcement& Header(std::string_view s) { header_ = s; return *this; }
    /// <summary>Whether the corner close button is offered. On by default.</summary>
    Announcement& Closable(bool on = true) { desc_.closable = on; return *this; }
    /// <summary>Runs when the page closes with no button pressed.</summary>
    Announcement& OnClose(std::function<void()> fn) { acts_.on_close = std::move(fn); return *this; }

    /// <summary>The small tag above the heading, "Announcement" in VRChat's own.</summary>
    Announcement& Pill(std::string_view text, Color tint = {}) {
        return Row(unix_ann_pill, text, tint);
    }
    Announcement& Heading(std::string_view text) { return Row(unix_ann_heading, text); }
    /// <summary>A paragraph. Add as many as the story needs.</summary>
    Announcement& Text(std::string_view text) { return Row(unix_ann_text, text); }
    /// <summary>Small print under the buttons.</summary>
    Announcement& Footer(std::string_view text) { return Row(unix_ann_footer, text); }
    Announcement& Separator() { return Row(unix_ann_separator, {}); }

    /// <summary>A picture, `height` pixels tall.</summary>
    Announcement& Image(Sprite sprite, float height = 160.0f) {
        unix_ann_row r = MakeRow(unix_ann_image, {});
        r.icon = detail::MakeIcon(sprite);
        r.height = height;
        rows_.push_back(r);
        return *this;
    }

    /// <summary>A button. The first one added with a tint is drawn as the call to action.</summary>
    Announcement& Button(std::string_view text, std::function<void()> on_press = {},
                         Sprite icon = Sprite::None, Color tint = {}) {
        unix_ann_row r = MakeRow(unix_ann_button, text);
        r.icon = detail::MakeIcon(icon);
        r.tint = tint;
        rows_.push_back(r);
        acts_.buttons.push_back(std::move(on_press));
        return *this;
    }

    /// <summary>A switch the reader can set before answering, "Don't show this again" and the
    /// like. Its press reports through the same handler as a button.</summary>
    Announcement& Toggle(std::string_view text, bool on = false,
                         std::function<void()> on_change = {}) {
        unix_ann_row r = MakeRow(unix_ann_toggle, text);
        r.on = on;
        rows_.push_back(r);
        acts_.buttons.push_back(std::move(on_change));
        return *this;
    }

    /// <summary>Put it on screen. It stays until answered.</summary>
    template <class = void> Notice Show() {
        UNIX_USE(notify, show);
        unix_notification d = desc_;
        d.header = Sv(header_);
        d.rows = rows_.empty() ? nullptr : rows_.data();
        d.row_count = static_cast<uint32_t>(rows_.size());
        d.on_action = &detail::NotifyActionTramp;
        d.action_ud = detail::ClaimNotifySlot(acts_);
        return Notice{ detail::g_d.notify_show(detail::g_self, &d) };
    }

private:
    // Every row borrows its text, so the strings live in a deque: it never moves what it
    // already holds, which a vector would the first time it grew.
    unix_ann_row MakeRow(unix_ann_row_kind kind, std::string_view text) {
        owned_.emplace_back(text);
        unix_ann_row r{};
        r.size = sizeof(r);
        r.kind = kind;
        r.text = Sv(owned_.back());
        return r;
    }
    Announcement& Row(unix_ann_row_kind kind, std::string_view text, Color tint = {}) {
        unix_ann_row r = MakeRow(kind, text);
        r.tint = tint;
        rows_.push_back(r);
        return *this;
    }

    unix_notification desc_{};
    std::string header_;
    std::deque<std::string> owned_;
    std::vector<unix_ann_row> rows_;
    detail::NotifyActions acts_;
};

/// <summary>A named stream of notifications from this module: set the style, icon and duration
/// once, then send one-liners. Every widget is shared with every other module, so a channel is
/// also the unit the host replaces and cancels by.</summary>
class NotifyChannel {
public:
    explicit NotifyChannel(std::string_view name) : name_(name) { base_.Channel(name); }

    NotifyChannel& As(NotifyStyle s)          { base_.As(s);          return *this; }
    NotifyChannel& Icon(Sprite s)             { base_.Icon(s);        return *this; }
    NotifyChannel& Icon(void* sprite)         { base_.Icon(sprite);   return *this; }
    NotifyChannel& For(float seconds)         { base_.For(seconds);   return *this; }
    NotifyChannel& At(NotifyAnchor a)         { base_.At(a);          return *this; }
    NotifyChannel& Tint(Color c)              { base_.Tint(c);        return *this; }
    NotifyChannel& TextColor(Color c)         { base_.TextColor(c);   return *this; }
    NotifyChannel& TextScale(float scale)     { base_.TextScale(scale); return *this; }
    NotifyChannel& Priority(NotifyPriority p) { base_.Priority(p);    return *this; }
    NotifyChannel& WithSound(bool on = true)  { base_.WithSound(on);  return *this; }
    /// <summary>Newest text wins: this channel's queued rows are dropped as each is sent.</summary>
    NotifyChannel& Replace(bool on = true)    { base_.Replace(on);    return *this; }
    NotifyChannel& Coalesce(bool on = true)   { base_.Coalesce(on);   return *this; }
    NotifyChannel& SkipIfBusy(bool on = true) { base_.SkipIfBusy(on); return *this; }

    template <class = void> Notice Say(std::string_view text) const {
        return Notification(base_).Text(text).Show();
    }
    template <class = void> Notice Tell(std::string_view title, std::string_view body) const {
        return Notification(base_).Title(title).Text(body).Show();
    }

    /// <summary>A notification carrying this channel's defaults, to finish by hand.</summary>
    [[nodiscard]] Notification New() const { return base_; }

    /// <summary>Drop everything this channel still has queued.</summary>
    template <class = void> bool Cancel() const {
        UNIX_USE(notify, cancel_chan);
        return detail::g_d.notify_cancel_chan(detail::g_self,
                                              detail::ChannelTag(name_)) == unix_ok;
    }

    [[nodiscard]] std::string_view Name() const noexcept { return name_; }

private:
    Notification base_;
    std::string name_;
};

/// <summary>VRChat's HUD notifications. Every call is on the Unity main thread.
///
/// The client carries four generations of notification widget at once and this service drives
/// all of them: the legacy card, the centre pill and its image banner, the carousel toast, and
/// the Voyager stack the open beta is moving to. NotifyStyle::Auto picks the newest one the
/// running client actually has, which is what a module should normally ask for.</summary>
class Notify {
public:
    /// <summary>Show a built notification.</summary>
    template <class = void> Notice Show(const Notification& n) const { return n.Show(); }

    /// <summary>A reusable stream with this module's own defaults. Hold one per kind of
    /// message rather than repeating the style on every call.</summary>
    template <class = void> [[nodiscard]] NotifyChannel Channel(std::string_view name) const {
        return NotifyChannel{ name };
    }

    /// <summary>One line, an icon, and a duration, on whichever stack this client has.</summary>
    template <class = void> Notice Say(std::string_view text, Sprite icon = Sprite::None,
                                       float seconds = 0.0f) const {
        return Notification(text).Icon(icon).For(seconds).Show();
    }

    /// <summary>Title over body, on the widest two-line widget this client has.</summary>
    template <class = void> Notice Tell(std::string_view title, std::string_view body,
                                        Sprite icon = Sprite::None, float seconds = 0.0f) const {
        return Notification(body).Title(title).Icon(icon).For(seconds)
                                 .As(NotifyStyle::Auto).Show();
    }

    /// <summary>The centre pill: one line, no countdown, tinted however you like.</summary>
    template <class = void> Notice Pill(std::string_view text, Sprite icon = Sprite::None,
                                        Color tint = {}, float seconds = 0.0f) const {
        return Notification(text).Icon(icon).Tint(tint).For(seconds)
                                 .As(NotifyStyle::Pill).Show();
    }

    /// <summary>The pill over a downloaded image panel, the way group announcements look.</summary>
    template <class = void> Notice Banner(std::string_view text, std::string_view image_url,
                                          Sprite icon = Sprite::None, float seconds = 0.0f) const {
        return Notification(text).Icon(icon).Image(image_url).For(seconds)
                                 .As(NotifyStyle::Banner).Show();
    }

    /// <summary>The carousel card: icon, title over body, and a badge on the right.</summary>
    template <class = void> Notice Event(std::string_view title, std::string_view body,
                                         Sprite icon = Sprite::None,
                                         std::string_view badge = {},
                                         Sprite badge_icon = Sprite::None,
                                         float seconds = 0.0f) const {
        return Notification(body).Title(title).Icon(icon).Badge(badge).BadgeIcon(badge_icon)
                                 .For(seconds).As(NotifyStyle::Event).Show();
    }

    /// <summary>The shorthand News window: a heading, a paragraph and two buttons.</summary>
    /// <param name="action">Call-to-action label; empty leaves only the dismiss button.</param>
    template <class = void> Notice Announce(std::string_view title, std::string_view body,
                                            std::string_view action = {},
                                            std::function<void()> on_action = {}) const {
        ::UNIx::Announcement a(title);
        a.Text(body);
        if (!action.empty()) a.Button(action, std::move(on_action), Sprite::None, { 0, 0.62f, 0.68f, 1 });
        a.Button("Dismiss");
        return a.Show();
    }

    /// <summary>An announcement to lay out row by row.</summary>
    template <class = void> [[nodiscard]] ::UNIx::Announcement Announcement(
        std::string_view heading = {}) const {
        return heading.empty() ? ::UNIx::Announcement{} : ::UNIx::Announcement{ heading };
    }

    /// <summary>Take one notification down early.</summary>
    template <class = void> bool Dismiss(Notice n) const { return n.Dismiss(); }

    /// <summary>Clear everything one style is showing or has queued. Auto clears every style.</summary>
    template <class = void> bool DismissAll(NotifyStyle style = NotifyStyle::Auto) const {
        UNIX_USE(notify, dismiss_all);
        return detail::g_d.notify_dismiss_all(detail::g_self,
                                              static_cast<uint32_t>(style)) == unix_ok;
    }

    /// <summary>Whether this client has that widget wired up and ready to draw.</summary>
    template <class = void> [[nodiscard]] bool Has(NotifyStyle style) const {
        UNIX_USE(notify, available);
        return detail::g_d.notify_available(detail::g_self, static_cast<uint32_t>(style));
    }

    /// <summary>How many of this module's notifications are waiting for a turn. Auto counts
    /// every style.</summary>
    template <class = void> [[nodiscard]] uint32_t Pending(
        NotifyStyle style = NotifyStyle::Auto) const {
        UNIX_USE(notify, pending);
        return detail::g_d.notify_pending(detail::g_self, static_cast<uint32_t>(style));
    }

    /// <summary>Drop this module's queued notifications on one channel. Whatever is already on
    /// screen stays up; Dismiss takes that down.</summary>
    template <class = void> bool CancelChannel(std::string_view name) const {
        UNIX_USE(notify, cancel_chan);
        return detail::g_d.notify_cancel_chan(detail::g_self, detail::ChannelTag(name)) == unix_ok;
    }

    /// <summary>Move a style to another HUD anchor for the rest of the session. Centre is where
    /// VRChat itself puts these; the side anchors sit off screen in desktop.</summary>
    template <class = void> bool SetAnchor(NotifyStyle style, NotifyAnchor anchor) const {
        UNIX_USE(notify, set_anchor);
        return detail::g_d.notify_set_anchor(detail::g_self, static_cast<uint32_t>(style),
                                             static_cast<uint32_t>(anchor)) == unix_ok;
    }

    /// <summary>The sprite VRChat itself would put on that style, for reuse on your own rows.
    /// Null before the HUD is built.</summary>
    template <class = void> [[nodiscard]] void* DefaultIcon(
        NotifyStyle style = NotifyStyle::Auto) const {
        UNIX_USE(notify, default_icon);
        return detail::g_d.notify_default_icon(detail::g_self, static_cast<uint32_t>(style));
    }
};

}   // namespace UNIx
