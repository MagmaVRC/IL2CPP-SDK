#pragma once
#include "Object.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module::Unity {

    class Material : public Object {
    public:
        using Object::Object;

        [[nodiscard]] Color GetColor() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("get_color"), 0);
            return MethodHandler::invoke<Color>(m, raw());
        }
        void SetColor(const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("set_color"), 1);
            Color c = value;
            void* params[] = { &c };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetMainTexture() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("get_mainTexture"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetMainTexture(void* texture) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("set_mainTexture"), 1);
            void* params[] = { texture };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetMainTextureOffset() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("get_mainTextureOffset"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetMainTextureOffset(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("set_mainTextureOffset"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] Vector2 GetMainTextureScale() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("get_mainTextureScale"), 0);
            return MethodHandler::invoke<Vector2>(m, raw());
        }
        void SetMainTextureScale(const Vector2& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("set_mainTextureScale"), 1);
            Vector2 v = value;
            void* params[] = { &v };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetShader() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("get_shader"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetShader(void* shader) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("set_shader"), 1);
            void* params[] = { shader };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetRenderQueue() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("get_renderQueue"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetRenderQueue(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("set_renderQueue"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetPassCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("get_passCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        void SetColorByName(std::string_view name, const Color& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("SetColor"), 2);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            Color c = value; void* params[] = { str, &c };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] Color GetColorByName(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("GetColor"), 1);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return Color{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<Color>(m, raw(), params);
        }

        void SetFloatByName(std::string_view name, float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("SetFloat"), 2);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] float GetFloatByName(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("GetFloat"), 1);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return 0.f;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<float>(m, raw(), params);
        }

        void SetIntByName(std::string_view name, int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("SetInt"), 2);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] int GetIntByName(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("GetInt"), 1);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return 0;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<int>(m, raw(), params);
        }

        void SetTextureByName(std::string_view name, void* texture) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("SetTexture"), 2);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, texture };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] void* GetTextureByName(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("GetTexture"), 1);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return nullptr;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<void*>(m, raw(), params);
        }

        void SetVectorByName(std::string_view name, const Vector4& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("SetVector"), 2);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            Vector4 v = value; void* params[] = { str, &v };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] Vector4 GetVectorByName(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("GetVector"), 1);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return Vector4{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<Vector4>(m, raw(), params);
        }

        [[nodiscard]] bool HasProperty(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("HasProperty"), 1);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return false;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }

        void EnableKeyword(std::string_view keyword) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("EnableKeyword"), 1);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(keyword).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, raw(), params);
        }
        void DisableKeyword(std::string_view keyword) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("DisableKeyword"), 1);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(keyword).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, raw(), params);
        }

        void SetMatrix(std::string_view name, const Matrix4x4& value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Material"), IL2CPP_STR("SetMatrix"), 2);
            auto* e = GetExports(); if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            Matrix4x4 mat = value; void* params[] = { str, &mat };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
