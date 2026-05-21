#pragma once
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::Unity {

    class Resources {
    public:
        Resources() = delete;

        [[nodiscard]] static void* Load(std::string_view path) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Resources"), IL2CPP_STR("Load"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return nullptr;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(path).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<void*>(m, nullptr, params);
        }

        [[nodiscard]] static void* FindObjectsOfTypeAll(void* systemType) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Resources"), IL2CPP_STR("FindObjectsOfTypeAll"), 1);
            if (!systemType) return nullptr;
            void* params[] = { systemType };
            return MethodHandler::invoke<void*>(m, nullptr, params);
        }

        [[nodiscard]] static void* UnloadUnusedAssets() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Resources"), IL2CPP_STR("UnloadUnusedAssets"), 0);
            return MethodHandler::invoke<void*>(m, nullptr);
        }
    };

} // namespace IL2CPP::Module::Unity
