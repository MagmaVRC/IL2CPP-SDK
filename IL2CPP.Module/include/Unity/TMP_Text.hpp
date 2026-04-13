#pragma once
#include "MaskableGraphic.hpp"
#include "../MethodHandler.hpp"
#include "../System/String.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module::Unity {

    class TMP_Text : public MaskableGraphic {
    public:
        using MaskableGraphic::MaskableGraphic;

        [[nodiscard]] std::string GetText() const {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "get_text", 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        void SetText(std::string_view value) {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "set_text", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(
                e->m_stringNew)(std::string(value).c_str());
            void* params[] = { il2cppStr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetFontSize() const {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "get_fontSize", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetFontSize(float value) {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "set_fontSize", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Color GetColor() const {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "get_color", 0);
            return MethodHandler::invoke<Color>(m, raw());
        }
        void SetColor(const Color& value) {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "set_color", 1);
            Color c = value;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetAlignment() const {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "get_alignment", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetAlignment(int value) {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "set_alignment", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetEnableWordWrapping() const {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "get_enableWordWrapping", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetEnableWordWrapping(bool value) {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "set_enableWordWrapping", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetFontStyle() const {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "get_fontStyle", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetFontStyle(int value) {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "set_fontStyle", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetRichText() const {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "get_richText", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetRichText(bool value) {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "set_richText", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetTextDirect(std::string_view text) {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "SetText", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(
                e->m_stringNew)(std::string(text).c_str());
            void* params[] = { il2cppStr };
            MethodHandler::invoke(m, raw(), params);
        }

        void ForceMeshUpdate() {
            static auto m = MethodHandler::resolve("TMPro.TMP_Text", "ForceMeshUpdate", 0);
            MethodHandler::invoke(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
