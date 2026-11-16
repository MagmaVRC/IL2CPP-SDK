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
            static Method m;
            if (!m) m = MethodHandler::resolve(IL2CPP_STR("TMPro.TextMeshProUGUI"), IL2CPP_STR("set_text"), 1);
            InvokeStringSetter(m, raw(), value);
        }


        void SetTextDirect(std::string_view text) {
            static Method m;
            if (!m) m = MethodHandler::resolve(IL2CPP_STR("TMPro.TextMeshProUGUI"), IL2CPP_STR("SetText"), 1);
            InvokeStringSetter(m, raw(), text);
        }
    };

} // namespace IL2CPP::Module::Unity
