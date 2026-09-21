#pragma once

#include "unix_module_abi.hpp"

#include <cstdint>

namespace UNIx {

    /// <summary>Menu lifecycle phase, in firing order. Subscribing after a one-shot phase
    /// fired invokes the handler immediately.</summary>
    enum class MenuPhase : uint32_t {
        QuickMenuPreSetup  = unix_menu_qm_pre_setup,  QuickMenuSetup   = unix_menu_qm_setup,
        QuickMenuPostSetup = unix_menu_qm_post_setup, PreReady         = unix_menu_pre_ready,
        Ready              = unix_menu_ready,         LateReady        = unix_menu_late_ready,
        Il2CppReady        = unix_menu_il2cpp_ready,  WorldLoaded      = unix_menu_world_loaded,
        PlayerManagerReady = unix_menu_players_ready, RendererReady    = unix_menu_renderer_ready,
        MainMenuSetup      = unix_menu_mm_setup,
    };
    static_assert(static_cast<uint32_t>(MenuPhase::MainMenuSetup) == unix_menu_mm_setup);

    /// <summary>What kind of image this module was loaded as.</summary>
    enum class HostKind : uint32_t {
        Native = unix_host_native, Vm = unix_host_vm, Static = unix_host_static,
    };
    static_assert(static_cast<uint32_t>(HostKind::Static) == unix_host_static);

    /// <summary>Which of the host's threads is running the current call.</summary>
    enum class ThreadKind : uint32_t {
        Unknown = unix_thread_unknown,     UnityMain = unix_thread_unity_main,
        Render  = unix_thread_render,      Photon    = unix_thread_photon,
        Loader  = unix_thread_loader,
    };
    static_assert(static_cast<uint32_t>(ThreadKind::Loader) == unix_thread_loader);

    /// <summary>How early a module asks to be loaded.</summary>
    enum class ModulePhase : uint32_t {
        PreIl2Cpp  = unix_phase_pre_il2cpp,
        PostIl2Cpp = unix_phase_post_il2cpp,
        MenuReady  = unix_phase_menu_ready,
    };
    static_assert(static_cast<uint32_t>(ModulePhase::MenuReady) == unix_phase_menu_ready);

    /// <summary>A point in one player's lifetime in the instance.</summary>
    enum class PlayerPhase : uint32_t {
        Awake          = unix_player_awake,
        Joined         = unix_player_joined,
        JoinedComplete = unix_player_joined_complete,
        Left           = unix_player_left,
        Update         = unix_player_update,
    };
    static_assert(static_cast<uint32_t>(PlayerPhase::Update) == unix_player_update);

    /// <summary>Entering or leaving a world.</summary>
    enum class WorldPhase : uint32_t { Entered = unix_world_entered, Left = unix_world_left };
    static_assert(static_cast<uint32_t>(WorldPhase::Left) == unix_world_left);

    /// <summary>A point in a UIPage's lifetime.</summary>
    enum class PageEventKind : uint32_t {
        Built = unix_page_built, Shown = unix_page_shown,       Hidden = unix_page_hidden,
        Tick  = unix_page_tick,  SceneLoaded = unix_page_scene_loaded,
    };
    static_assert(static_cast<uint32_t>(PageEventKind::SceneLoaded) == unix_page_scene_loaded);

    /// <summary>Whether a control changed because the user moved it, or because the host put it
    /// back after a refused change.</summary>
    enum class WidgetKind : uint32_t { Change = unix_wk_change, Restore = unix_wk_restore };
    static_assert(static_cast<uint32_t>(WidgetKind::Restore) == unix_wk_restore);

    /// <summary>Where an icon's image comes from.</summary>
    enum class IconKind : uint32_t {
        None      = unix_icon_none,       Bundle = unix_icon_bundle,
        SpritePtr = unix_icon_sprite_ptr, VrcId  = unix_icon_vrc_id,
    };
    static_assert(static_cast<uint32_t>(IconKind::VrcId) == unix_icon_vrc_id);

    /// <summary>What a bind note reports about one symbol.</summary>
    enum class NoteKind : uint32_t {
        Deprecated        = unix_note_deprecated,
        Experimental      = unix_note_experimental,
        Removed           = unix_note_removed,
        Unknown           = unix_note_unknown,
        HostTooOld        = unix_note_host_too_old,
        SignatureMismatch = unix_note_signature_mismatch,
        Denied            = unix_note_denied,
        UndeclaredCap     = unix_note_undeclared_cap,
        UnusedCap         = unix_note_unused_cap,
        UnknownCap        = unix_note_unknown_cap,
        SlotBudget        = unix_note_slot_budget,
        UnavailableCount  = unix_note_unavailable_summary,
        BadRequest        = unix_note_bad_request,
    };
    static_assert(static_cast<uint32_t>(NoteKind::BadRequest) == unix_note_bad_request);

    /// <summary>What a call refused to do. `Status` is the raw ABI spelling of the same values.</summary>
    enum class StatusCode : int32_t {
        Ok      = unix_ok,        Absent = unix_e_absent,  Denied = unix_e_denied,
        BadArg  = unix_e_arg,     State  = unix_e_state,   Thread = unix_e_thread,
        Removed = unix_e_removed, Busy   = unix_e_busy,
    };
    static_assert(static_cast<int32_t>(StatusCode::Busy) == unix_e_busy);

    /// <summary>Severity carried by log.write and by Unity's own log callback.</summary>
    enum class LogLevel : uint32_t { Error = 0, Assert, Warning, Log, Exception };

    static_assert(static_cast<uint32_t>(LogLevel::Error) == 0);
    static_assert(static_cast<uint32_t>(LogLevel::Assert) == 1);
    static_assert(static_cast<uint32_t>(LogLevel::Warning) == 2);
    static_assert(static_cast<uint32_t>(LogLevel::Log) == 3);
    static_assert(static_cast<uint32_t>(LogLevel::Exception) == 4);

    /// <summary>Rank in VRChat's display vocabulary, as used by the VRCPlayer rank colour fields.</summary>
    enum class PlayerRank : uint32_t {
        Visitor = 0, NewUser, User, Known, Trusted, Administrator, Troll
    };

    static_assert(static_cast<uint32_t>(PlayerRank::Visitor) == 0);
    static_assert(static_cast<uint32_t>(PlayerRank::NewUser) == 1);
    static_assert(static_cast<uint32_t>(PlayerRank::User) == 2);
    static_assert(static_cast<uint32_t>(PlayerRank::Known) == 3);
    static_assert(static_cast<uint32_t>(PlayerRank::Trusted) == 4);
    static_assert(static_cast<uint32_t>(PlayerRank::Administrator) == 5);
    static_assert(static_cast<uint32_t>(PlayerRank::Troll) == 6);

    /// <summary>The same ladder in the API tag vocabulary. VRChat's tag names sit one step off its display names.</summary>
    enum class ApiPlayerRank : uint8_t {
        Visitor = 0, Basic, Known, Trusted, Veteran, Administrator, Troll
    };

    /// <summary>Which selected users a user-page row is shown for.</summary>
    enum class UserPageAudience : uint32_t {
        Self           = 1u << 0,  // your own page
        InstancePlayer = 1u << 1,  // another player in the instance
        ExternalUser   = 1u << 2,  // a user outside the instance
        Others         = InstancePlayer | ExternalUser,
        All            = Self | InstancePlayer | ExternalUser,
    };

    constexpr UserPageAudience operator|(UserPageAudience a, UserPageAudience b) {
        return static_cast<UserPageAudience>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    constexpr UserPageAudience operator&(UserPageAudience a, UserPageAudience b) {
        return static_cast<UserPageAudience>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    /// <summary>Which kind of user the open page is showing.</summary>
    enum class UserPageKind : uint32_t {
        None           = 0,
        Self           = 1,
        InstancePlayer = 2,
        ExternalUser   = 3,
    };

    enum class PerfSetting : int32_t {
        MasterTextureLimit = 0,
        AntiAliasing = 1,
        ShadowResolution = 2,
        PixelLightCount = 3,
        ShaderMaxLOD = 4,
        VSyncCount = 5,
        StreamingMipmapsBudget = 100,
        LodBias = 101
    };

    /// <summary>Declared type of a world script's public variable.</summary>
    enum class WorldVarType : uint32_t {
        Bool = 0,
        Int = 1,
        Float = 2,
        String = 3,
    };

    /// <summary>Tween interpolation curve.</summary>
    enum class Ease : uint8_t { Linear, EaseInOut, EaseOutBack, EaseOut, EaseIn };

    static_assert(static_cast<uint8_t>(Ease::Linear) == 0);
    static_assert(static_cast<uint8_t>(Ease::EaseInOut) == 1);
    static_assert(static_cast<uint8_t>(Ease::EaseOutBack) == 2);
    static_assert(static_cast<uint8_t>(Ease::EaseOut) == 3);
    static_assert(static_cast<uint8_t>(Ease::EaseIn) == 4);

    /// <summary>Studio sidebar grouping. Pages sort by group, then by registration order.</summary>
    enum class PageGroup : int32_t {
        Scripting = 0,
        Udon      = 1,
        Runtime   = 2,
        System    = 3,
    };

    static_assert(static_cast<int32_t>(PageGroup::Scripting) == 0);
    static_assert(static_cast<int32_t>(PageGroup::Udon) == 1);
    static_assert(static_cast<int32_t>(PageGroup::Runtime) == 2);
    static_assert(static_cast<int32_t>(PageGroup::System) == 3);

    /// <summary>Unity key code. Value-identical to IL2CPP::KeyCode: static_cast between them.</summary>
    enum class Key : int32_t {
        None = 0, Backspace = 8, Tab = 9, Clear = 12, Return = 13, Pause = 19, Escape = 27,
        Space = 32, Exclaim = 33, DoubleQuote = 34, Hash = 35, Dollar = 36, Ampersand = 38,
        Quote = 39, LeftParen = 40, RightParen = 41, Asterisk = 42, Plus = 43, Comma = 44,
        Minus = 45, Period = 46, Slash = 47,
        Alpha0 = 48, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,
        Colon = 58, Semicolon, Less, Equals, Greater, Question, At,
        LeftBracket = 91, Backslash, RightBracket, Caret, Underscore, BackQuote,
        A = 97, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Delete = 127,
        Keypad0 = 256, Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,
        KeypadPeriod, KeypadDivide, KeypadMultiply, KeypadMinus, KeypadPlus, KeypadEnter, KeypadEquals,
        UpArrow = 273, DownArrow, RightArrow, LeftArrow, Insert, Home, End, PageUp, PageDown,
        F1 = 282, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15,
        Numlock = 300, CapsLock, ScrollLock, RightShift, LeftShift, RightControl, LeftControl,
        RightAlt, LeftAlt, RightApple, LeftApple = 310, LeftWindows, RightWindows, AltGr = 313,
        Help = 315, Print, SysReq, Break, Menu,
        Mouse0 = 323, Mouse1, Mouse2, Mouse3, Mouse4, Mouse5, Mouse6,
    };

    static_assert(static_cast<int32_t>(Key::None) == 0);
    static_assert(static_cast<int32_t>(Key::F6) == 287);
    static_assert(static_cast<int32_t>(Key::F7) == 288);
    static_assert(static_cast<int32_t>(Key::F8) == 289);
    static_assert(static_cast<int32_t>(Key::RightShift) == 303);

    /// <summary>Hotkey modifier mask. LeftControl and RightControl both collapse to Ctrl.</summary>
    enum class KeyMods : uint32_t { None = 0, Ctrl = 1u << 0, Shift = 1u << 1, Alt = 1u << 2 };

    constexpr KeyMods  operator| (KeyMods a, KeyMods b) noexcept { return KeyMods(uint32_t(a) | uint32_t(b)); }
    constexpr KeyMods  operator& (KeyMods a, KeyMods b) noexcept { return KeyMods(uint32_t(a) & uint32_t(b)); }
    constexpr KeyMods& operator|=(KeyMods& a, KeyMods b) noexcept { return a = a | b; }
    constexpr KeyMods& operator&=(KeyMods& a, KeyMods b) noexcept { return a = a & b; }
    constexpr bool     Any(KeyMods m) noexcept { return uint32_t(m) != 0; }

    /// <summary>VRChat sprite bundled with the client. None leaves the icon alone.</summary>
    enum class Sprite : int32_t {
        None = -1,
        Rocket, Notification, MapIcon, Sound,
        VRCPlus, VRCPlusExperiment, Store, Settings, Report,
        Planet, Avatars, Social, Groups, Home, Reload, Target, Shield, Calibrate, SitStand,
        Screenshot, Light, MultiLayer, Gallery, Folder,
        Gift, UserIcon, Prints, Camera, Emojis, Stickers,
        Drone, CameraDolly, SelfieExpression, Backgrounds,
        UIThemes, ManageSubscription,
        MicOff, TeleportTo, TeleportToMe,
        KickUser, BanUser, GainUp, GainDown, Logout,

        Bundle_BatteryFull, Bundle_FileCss, Bundle_ThermometerCold, Bundle_FileCloud,
        Bundle_Stack, Bundle_BatteryVerticalFull, Bundle_Vision, Bundle_Bookmarks,
        Bundle_EggCrack, Bundle_HeartHalf, Bundle_Boot, Bundle_FileSql,
        Bundle_ArrowBendLeftUp, Bundle_Alien, Bundle_EyeSlash, Bundle_FilePdf,
        Bundle_Recycle, Bundle_Virus, Bundle_FileVue, Bundle_Bread,
        Bundle_CellSignalX, Bundle_CodesandboxLogo, Bundle_BatteryVerticalEmpty, Bundle_Bell,
        Bundle_StackMinus, Bundle_WarningDiamond, Bundle_FileMagnifyingGlass, Bundle_Files,
        Bundle_ArrowArcLeft, Bundle_FlyingSaucer, Bundle_BatteryEmpty, Bundle_ArrowCircleLeft,
        Bundle_BracketsCurly, Bundle_BatteryVerticalHigh, Bundle_FilePng, Bundle_WifiNone,
        Bundle_BatteryPlus, Bundle_ArrowsInCardinal, Bundle_SealQuestion, Bundle_Books,
        Bundle_HandPeace, Bundle_ArrowLineUpRight, Bundle_FileLock, Bundle_ArrowBendUpRight,
        Bundle_BatteryWarning, Bundle_Spinner, Bundle_FalloutShelter, Bundle_FileTs,
        Bundle_Coins, Bundle_DotsThreeOutlineVertical, Bundle_Wall, Bundle_Thermometer,
        Bundle_ArrowULeftDown, Bundle_Book, Bundle_SpeakerX, Bundle_ArrowBendDownRight,
        Bundle_StackSimple, Bundle_Bandaids, Bundle_ArrowFatLineUp, Bundle_BookOpen,
        Bundle_Ear, Bundle_FileXls, Bundle_Prohibit, Bundle_FileCsv,
        Bundle_BookmarkSimple, Bundle_Selection, Bundle_Browsers, Bundle_ArrowLeft,
        Bundle_ArrowLineDown, Bundle_Balloon, Bundle_ArrowsOutLineVertical, Bundle_BracketsSquare,
        Bundle_CellSignalNone, Bundle_BatteryVerticalLow, Bundle_Video, Bundle_EarSlash,
        Bundle_FileJsx, Bundle_SpeakerSlash, Bundle_WarningCircle, Bundle_SecurityCamera,
        Bundle_CellSignalSlash, Bundle_Egg, Bundle_FileIni, Bundle_ArrowFatUp,
        Bundle_Rocket, Bundle_BookmarksSimple, Bundle_ArrowFatDown, Bundle_ArrowBendUpLeft,
        Bundle_BatteryLow, Bundle_StarHalf, Bundle_Fish, Bundle_SpeakerNone,
        Bundle_Biohazard, Bundle_File, Bundle_Seal, Bundle_FileCode,
        Bundle_BellSlash, Bundle_WifiX, Bundle_FileArrowDown, Bundle_GenderMale,
        Bundle_Bluetooth, Bundle_Coin, Bundle_FileJpg, Bundle_FileMinus,
        Bundle_XCircle, Bundle_IdentificationCards, Bundle_FileImage, Bundle_FileText,
        Bundle_HeartBreak, Bundle_SealWarning, Bundle_TriangleDashed, Bundle_Wallet,
        Bundle_ArrowLineDownLeft, Bundle_ArrowLineLeft, Bundle_StackPlus, Bundle_ArrowFatLeft,
        Bundle_BellSimple, Bundle_Diamond, Bundle_Pulse, Bundle_ArrowBendLeftDown,
        Bundle_ArrowFatRight, Bundle_BatteryPlusVertical, Bundle_ArrowLineUpLeft, Bundle_FileCpp,
        Bundle_ThermometerHot, Bundle_CellSignalMedium, Bundle_SpeakerLow, Bundle_WifiSlash,
        Bundle_ArrowArcRight, Bundle_VinylRecord, Bundle_BracketsAngle, Bundle_Feather,
        Bundle_BatteryHigh, Bundle_ArrowSquareIn, Bundle_BluetoothConnected, Bundle_Brain,
        Bundle_CheckFat, Bundle_ArrowsHorizontal, Bundle_Flashlight, Bundle_ArrowCircleDown,
        Bundle_Service, Bundle_AirplaneTilt, Bundle_ArrowFatLinesRight, Bundle_ArrowCounterClockwise,
        Bundle_Fingerprint, Bundle_Flask, Bundle_Check, Bundle_DotsThreeVertical,
        Bundle_Triangle, Bundle_CloudArrowUp, Bundle_HandFist, Bundle_Snowflake,
        Bundle_ArrowFatLineDown, Bundle_FileArchive, Bundle_FileC, Bundle_FileTxt,
        Bundle_HandsClapping, Bundle_BatteryVerticalMedium, Bundle_ArrowBendDownLeft, Bundle_FileRs,
        Bundle_FileAudio, Bundle_CellSignalHigh, Bundle_FileZip, Bundle_CodeSimple,
        Bundle_SpinnerGap, Bundle_Gear, Bundle_ArrowsDownUp, Bundle_ArrowRight,
        Bundle_ArrowFatLineLeft, Bundle_BatteryMedium, Bundle_CloudArrowDown, Bundle_Toolbox,
        Bundle_Heart, Bundle_ChartLineUp, Bundle_ArrowUDownRight, Bundle_FileHtml,
        Bundle_PuzzlePiece, Bundle_ArrowCircleUp, Bundle_ArrowBendRightUp, Bundle_FileJs,
        Bundle_Control, Bundle_Waves, Bundle_ArrowCircleRight, Bundle_SpeakerHigh,
        Bundle_TextT, Bundle_WashingMachine, Bundle_ArrowLineUp, Bundle_WifiHigh,
        Bundle_ArrowFatLinesLeft, Bundle_FileSvg, Bundle_HandHeart, Bundle_ArrowsInLineHorizontal,
        Bundle_FileArrowUp, Bundle_SealPercent, Bundle_Checks, Bundle_Drop,
        Bundle_GenderFemale, Bundle_ArrowLineRight, Bundle_FileDoc, Bundle_FileMd,
        Bundle_Cloud, Bundle_ArrowsIn, Bundle_FilePlus, Bundle_FolderX,
        Bundle_FileX, Bundle_FileVideo, Bundle_DotsThreeOutline, Bundle_HandPalm,
        Bundle_ArrowFatLinesUp, Bundle_Ghost, Bundle_ArrowLineDownRight, Bundle_Nut,
        Bundle_ArrowsClockwise, Bundle_CheckCircle, Bundle_SlidersHorizontal, Bundle_Bomb,
        Bundle_ArrowBendRightDown, Bundle_Bug, Bundle_Power, Bundle_CellSignalFull,
        Bundle_WarningSign, Bundle_BatteryChargingVertical, Bundle_Bone, Bundle_ArrowFatLineRight,
        Bundle_ArrowFatLinesDown, Bundle_FileTsx, Bundle_Shield, Bundle_WarningOctagon,
        Bundle_WifiMedium, Bundle_ChartLine, Bundle_X, Bundle_Hexagon,
        Bundle_BracketsRound, Bundle_Handshake, Bundle_HeartStraightBreak, Bundle_Sliders,
        Bundle_FilePpt, Bundle_FirstAidKit, Bundle_Airplane, Bundle_ArrowSquareDown,
        Bundle_StarFour, Bundle_ArrowsOut, Bundle_CellSignalLow, Bundle_CornersIn,
        Bundle_Detector, Bundle_FilePy, Bundle_SealCheck, Bundle_Star,
        Bundle_Translate, Bundle_ChartLineDown, Bundle_LocationMarker, Bundle_BookBookmark,
        Bundle_Sun, Bundle_DropSimple, Bundle_Bookmark, Bundle_ArrowsCounterClockwise,
        Bundle_TrafficCone, Bundle_ArrowUDownLeft, Bundle_BellSimpleSlash, Bundle_Fan,
        Bundle_Flame, Bundle_ArrowClockwise, Bundle_FileCSharp, Bundle_TextTSlash,
        Bundle_BatteryWarningVertical, Bundle_CodepenLogo, Bundle_ArrowsLeftRight, Bundle_HandGrabbing,
        Bundle_BugBeetle, Bundle_FirstAid, Bundle_SelectionAll, Bundle_ArrowsMerge,
        Bundle_Heartbeat, Bundle_StarAndCrescent, Bundle_HeartStraight, Bundle_BellRinging,
        Bundle_ArrowsOutCardinal, Bundle_HandPointing, Bundle_Certificate, Bundle_DropSlash,
        Bundle_ArrowsInLineVertical, Bundle_FileDashed, Bundle_Scroll, Bundle_SketchLogo,
        Bundle_Binary, Bundle_ArrowsInSimple, Bundle_ArrowsOutLineHorizontal, Bundle_BatteryCharging,
        Bundle_BellSimpleRinging,

        _COUNT
    };

    static_assert(static_cast<int32_t>(Sprite::None) == -1);
    static_assert(static_cast<int32_t>(Sprite::Rocket) == 0);

} // namespace UNIx

// Deprecated for 7.1: one-release compatibility spelling for the pre-7.0 Bootstrap namespace.
namespace Bootstrap {

    using UNIx::Sprite;
    using UNIx::PlayerRank;
    using UNIx::ApiPlayerRank;
    using UNIx::UserPageAudience;
    using UNIx::UserPageKind;
    using UNIx::PerfSetting;
    using UNIx::WorldVarType;
    using UNIx::LogLevel;
    using UnityLogType = UNIx::LogLevel;

    constexpr uint32_t invalid_id = ~0u;

    using fn_menu_button_callback = void(__cdecl*)(uint32_t button_id);
    using fn_menu_slider_callback = void(__cdecl*)(uint32_t slider_id, float value);
    using fn_menu_enum_callback   = void(__cdecl*)(uint32_t selector_id, int32_t index);
    using fn_menu_text_callback   = void(__cdecl*)(uint32_t input_id, char const* value,
                                                   uint32_t value_len);
    using fn_menu_list_callback   = void(__cdecl*)(uint32_t list_id, uint32_t row_index);

    using fn_user_select_callback = void(__cdecl*)(void* player, void* api_user,
        char const* display_name, uint32_t name_len,
        char const* user_id, uint32_t user_id_len);
    using fn_user_button_callback = void(__cdecl*)(uint32_t button_id, void* player, void* api_user,
        char const* user_id, uint32_t user_id_len);

} // namespace Bootstrap
