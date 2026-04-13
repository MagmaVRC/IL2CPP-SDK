#pragma once
#include "TMP_Text.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module::Unity {

    class TextMeshProUGUI : public TMP_Text {
    public:
        using TMP_Text::TMP_Text;


        void SetText(std::string_view value) {
            static auto m = MethodHandler::resolve("TMPro.TextMeshProUGUI", "set_text", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(
                e->m_stringNew)(std::string(value).c_str());
            void* params[] = { il2cppStr };
            MethodHandler::invoke(m, raw(), params);
        }


        void SetTextDirect(std::string_view text) {
            static auto m = MethodHandler::resolve("TMPro.TextMeshProUGUI", "SetText", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(
                e->m_stringNew)(std::string(text).c_str());
            void* params[] = { il2cppStr };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
