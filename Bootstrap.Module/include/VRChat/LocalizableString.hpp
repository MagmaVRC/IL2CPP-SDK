#pragma once

#include <IL2CPP.Module/include/il2cpp_module.hpp>
#include <string>

namespace IL2CPP::VRChat {

    class LocalizableString : public IL2CPP::Module::ManagedObject {
    public:
        using ManagedObject::ManagedObject;

        [[nodiscard]] std::string GetKey();
        [[nodiscard]] std::string GetFallbackText();
        [[nodiscard]] bool HasKey();
        [[nodiscard]] bool IsLocalized();
        [[nodiscard]] bool IsEmpty();

        [[nodiscard]] std::string ToString();
        [[nodiscard]] std::string GetTranslation(bool& success);

        static LocalizableString Empty();

        static LocalizableString Localize(std::string_view text);
        static LocalizableString LocalizeWithFallback(std::string_view key, std::string_view fallback);
        static LocalizableString NoTranslation(std::string_view text);
    };

} // namespace IL2CPP::VRChat
