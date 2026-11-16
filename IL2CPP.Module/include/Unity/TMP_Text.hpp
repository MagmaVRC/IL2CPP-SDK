#pragma once
#include "MaskableGraphic.hpp"
#include "../MethodHandler.hpp"
#include "../System/String.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <cstring>
#include <string>
#include <string_view>

namespace IL2CPP::Module::Unity {

    class TMP_Text : public MaskableGraphic {
    public:
        using MaskableGraphic::MaskableGraphic;

        [[nodiscard]] std::string GetText() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_text"), 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        void SetText(std::string_view value) {
            static Method m;
            if (!m) m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_text"), 1);
            InvokeStringSetter(m, raw(), value);
        }

        [[nodiscard]] float GetFontSize() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_fontSize"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetFontSize(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_fontSize"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Color GetColor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_color"), 0);
            return MethodHandler::invoke<Color>(m, raw());
        }
        void SetColor(const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_color"), 1);
            Color c = value;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetAlignment() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_alignment"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetAlignment(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_alignment"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetEnableWordWrapping() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_enableWordWrapping"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetEnableWordWrapping(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_enableWordWrapping"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        /// <summary>TextOverflowModes: 0 Overflow, 1 Ellipsis, 3 Truncate.</summary>
        void SetOverflowMode(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_overflowMode"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetFontStyle() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_fontStyle"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetFontStyle(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_fontStyle"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetFontSizeMin() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_fontSizeMin"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetFontSizeMin(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_fontSizeMin"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetFontSizeMax() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_fontSizeMax"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetFontSizeMax(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_fontSizeMax"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetEnableAutoSizing() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_enableAutoSizing"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetEnableAutoSizing(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_enableAutoSizing"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        /// <summary>Extra spacing between characters, in font units (1 = 1/100 em).</summary>
        void SetCharacterSpacing(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_characterSpacing"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetRichText() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_richText"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetRichText(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_richText"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetTextDirect(std::string_view text) {
            static Method m;
            if (!m) m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("SetText"), 1);
            InvokeStringSetter(m, raw(), text);
        }

        void ForceMeshUpdate() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("ForceMeshUpdate"), 0);
            MethodHandler::invoke(m, raw());
        }

        [[nodiscard]] void* GetFontRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_font"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetFontRaw(void* fontAsset) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_font"), 1);
            void* params[] = { fontAsset };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] void* GetFontSharedMaterialRaw() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("get_fontSharedMaterial"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetFontSharedMaterialRaw(void* material) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("TMPro.TMP_Text"), IL2CPP_STR("set_fontSharedMaterial"), 1);
            void* params[] = { material };
            MethodHandler::invoke(m, raw(), params);
        }

    protected:
        static void InvokeStringSetter(const Method& m, void* self, std::string_view value) {
            if (!m) return;
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            char inlineBuf[256];
            std::string heapBuf;
            const char* text = inlineBuf;
            if (value.size() < sizeof(inlineBuf)) {
                if (!value.empty()) std::memcpy(inlineBuf, value.data(), value.size());
                inlineBuf[value.size()] = '\0';
            } else {
                heapBuf.assign(value);
                text = heapBuf.c_str();
            }
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(text);
            void* params[] = { il2cppStr };
            MethodHandler::invoke(m, self, params);
        }
    };

} // namespace IL2CPP::Module::Unity
