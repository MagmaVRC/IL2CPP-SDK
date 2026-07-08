#include <VRChat/LocalizableString.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

    std::string LocalizableString::GetKey() {
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableString", "get_Key", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string LocalizableString::GetFallbackText() {
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableString", "get_FallbackText", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    bool LocalizableString::HasKey() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableString", "get_HasKey", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool LocalizableString::IsLocalized() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableString", "get_IsLocalized", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool LocalizableString::IsEmpty() {
        if (!valid()) return true;
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableString", "get_IsEmpty", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    std::string LocalizableString::ToString() {
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableString", "ToString", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    std::string LocalizableString::GetTranslation(bool& success) {
        if (!valid()) { success = false; return {}; }
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableString", "GetTranslation", 1);
        bool outSuccess = false;
        void* params[1] = { &outSuccess };
        void* result = MethodHandler::invoke<void*>(m, raw(), params);
        success = outSuccess;
        if (!result) return {};
        return IL2CPP::Module::System::String(result).to_string();
    }

    LocalizableString LocalizableString::Empty() {
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableString", "get_Empty", 0);
        void* result = MethodHandler::invoke<void*>(m, nullptr);
        return LocalizableString{ result };
    }

    LocalizableString LocalizableString::Localize(std::string_view text) {
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableStringExtensions", "Localize", 4);
        auto str = IL2CPP::Module::System::String::create(text);
        void* params[1] = { str.raw() };
        void* result = MethodHandler::invoke<void*>(m, nullptr, params);
        return LocalizableString{ result };
    }

    LocalizableString LocalizableString::LocalizeWithFallback(std::string_view key, std::string_view fallback) {
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableStringExtensions", "LocalizeWithFallback", 5);
        auto keyStr = IL2CPP::Module::System::String::create(key);
        auto fallbackStr = IL2CPP::Module::System::String::create(fallback);
        void* params[2] = { keyStr.raw(), fallbackStr.raw() };
        void* result = MethodHandler::invoke<void*>(m, nullptr, params);
        return LocalizableString{ result };
    }

    LocalizableString LocalizableString::NoTranslation(std::string_view text) {
        static auto m = MethodHandler::resolve("VRC.Localization.LocalizableStringExtensions", "NoTranslation", 4);
        auto str = IL2CPP::Module::System::String::create(text);
        void* params[4] = { str.raw(), nullptr, nullptr, nullptr };
        void* result = MethodHandler::invoke<void*>(m, nullptr, params);
        return LocalizableString{ result };
    }

} // namespace IL2CPP::VRChat
