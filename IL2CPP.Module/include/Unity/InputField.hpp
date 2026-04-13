#pragma once
#include "Selectable.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>
#include <windows.h>

namespace IL2CPP::Module::Unity {

    class InputField : public Selectable {
    public:
        using Selectable::Selectable;

        [[nodiscard]] std::string GetText() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_text", 0);
            void* str = MethodHandler::invoke<void*>(m, raw());
            if (!str) return "";
            int len = *reinterpret_cast<int*>(static_cast<char*>(str) + 0x10);
            if (len <= 0) return "";
            const wchar_t* wstr = reinterpret_cast<const wchar_t*>(static_cast<char*>(str) + 0x14);
            int bytes = WideCharToMultiByte(CP_UTF8, 0, wstr, len, nullptr, 0, nullptr, nullptr);
            if (bytes <= 0) return "";
            std::string out(static_cast<size_t>(bytes), '\0');
            WideCharToMultiByte(CP_UTF8, 0, wstr, len, out.data(), bytes, nullptr, nullptr);
            return out;
        }
        void SetText(std::string_view value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_text", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(
                e->m_stringNew)(std::string(value).c_str());
            void* params[] = { il2cppStr };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetPlaceholder() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_placeholder", 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetPlaceholder(void* graphic) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_placeholder", 1);
            void* params[] = { graphic };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetCaretPosition() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_caretPosition", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetCaretPosition(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_caretPosition", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetSelectionAnchorPosition() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_selectionAnchorPosition", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetSelectionAnchorPosition(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_selectionAnchorPosition", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetSelectionFocusPosition() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_selectionFocusPosition", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetSelectionFocusPosition(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_selectionFocusPosition", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetCharacterLimit() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_characterLimit", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetCharacterLimit(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_characterLimit", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetContentType() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_contentType", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetContentType(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_contentType", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetLineType() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_lineType", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetLineType(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_lineType", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetInputType() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_inputType", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetInputType(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_inputType", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetReadOnly() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_readOnly", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetReadOnly(bool value) {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "set_readOnly", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetMultiLine() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_multiLine", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] bool GetIsFocused() const {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "get_isFocused", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        void ActivateInputField() {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "ActivateInputField", 0);
            MethodHandler::invoke(m, raw());
        }
        void DeactivateInputField() {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "DeactivateInputField", 0);
            MethodHandler::invoke(m, raw());
        }
        void ForceLabelUpdate() {
            static auto m = MethodHandler::resolve("UnityEngine.UI.InputField", "ForceLabelUpdate", 0);
            MethodHandler::invoke(m, raw());
        }
    };

} // namespace IL2CPP::Module::Unity
