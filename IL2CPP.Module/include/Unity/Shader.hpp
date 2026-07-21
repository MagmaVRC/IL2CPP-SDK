#pragma once
#include "Object.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::Unity {

    class Shader : public Object {
        static void* Str(std::string_view s) {
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return nullptr;
            return reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(s).c_str());
        }

    public:
        using Object::Object;

        /// <summary>Find a shader by name (e.g. "Standard", "Sprites/Default").</summary>
        [[nodiscard]] static Shader Find(std::string_view name) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("Find"), 1);
            void* str = Str(name);
            if (!m || !str) return {};
            void* params[] = { str };
            return Shader{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }

        /// <summary>Map a shader property name to its integer ID (cache this).</summary>
        [[nodiscard]] static int PropertyToID(std::string_view name) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("PropertyToID"), 1);
            void* str = Str(name);
            if (!m || !str) return 0;
            void* params[] = { str };
            return MethodHandler::invoke<int>(m, nullptr, params);
        }

        [[nodiscard]] bool IsSupported() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("get_isSupported"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] int GetRenderQueue() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("get_renderQueue"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        static void SetGlobalFloat(std::string_view name, float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("SetGlobalFloat"), 2);
            void* str = Str(name);
            if (!m || !str) return;
            void* params[] = { str, &value };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void SetGlobalColor(std::string_view name, const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("SetGlobalColor"), 2);
            void* str = Str(name);
            if (!m || !str) return;
            Color c = value;
            void* params[] = { str, &c };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void SetGlobalVector(std::string_view name, const Vector4& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("SetGlobalVector"), 2);
            void* str = Str(name);
            if (!m || !str) return;
            Vector4 v = value;
            void* params[] = { str, &v };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void SetGlobalTexture(std::string_view name, void* texture) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("SetGlobalTexture"), 2);
            void* str = Str(name);
            if (!m || !str) return;
            void* params[] = { str, texture };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void EnableGlobalKeyword(std::string_view keyword) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("EnableKeyword"), 1);
            void* str = Str(keyword);
            if (!m || !str) return;
            void* params[] = { str };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void DisableGlobalKeyword(std::string_view keyword) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Shader"), IL2CPP_STR("DisableKeyword"), 1);
            void* str = Str(keyword);
            if (!m || !str) return;
            void* params[] = { str };
            MethodHandler::invoke(m, nullptr, params);
        }
    };

} // namespace IL2CPP::Module::Unity
