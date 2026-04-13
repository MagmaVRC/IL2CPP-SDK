#pragma once
#include "Behaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module::Unity {

    class Animator : public Behaviour {
    public:
        using Behaviour::Behaviour;

        [[nodiscard]] float GetSpeed() const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "get_speed", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetSpeed(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "set_speed", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetPlaybackTime() const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "get_playbackTime", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetPlaybackTime(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "set_playbackTime", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetRuntimeAnimatorController() const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "get_runtimeAnimatorController", 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetRuntimeAnimatorController(void* controller) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "set_runtimeAnimatorController", 1);
            void* params[] = { controller };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetAvatar() const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "get_avatar", 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetAvatar(void* avatar) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "set_avatar", 1);
            void* params[] = { avatar };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetApplyRootMotion() const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "get_applyRootMotion", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetApplyRootMotion(bool value) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "set_applyRootMotion", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetUpdateMode() const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "get_updateMode", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetUpdateMode(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "set_updateMode", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetHasRootMotion() const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "get_hasRootMotion", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] bool GetIsHuman() const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "get_isHuman", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        void SetFloat(std::string_view name, float value) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "SetFloat", 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] float GetFloat(std::string_view name) const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "GetFloat", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return 0.f;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<float>(m, raw(), params);
        }

        void SetInteger(std::string_view name, int value) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "SetInteger", 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] int GetInteger(std::string_view name) const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "GetInteger", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return 0;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<int>(m, raw(), params);
        }

        void SetBool(std::string_view name, bool value) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "SetBool", 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] bool GetBool(std::string_view name) const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "GetBool", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return false;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }

        void SetTrigger(std::string_view name) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "SetTrigger", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, raw(), params);
        }
        void ResetTrigger(std::string_view name) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "ResetTrigger", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, raw(), params);
        }

        void Play(std::string_view stateName) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "Play", 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(stateName).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, raw(), params);
        }
        void CrossFade(std::string_view stateName, float normalizedTransitionDuration) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "CrossFade", 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(stateName).c_str());
            void* params[] = { str, &normalizedTransitionDuration };
            MethodHandler::invoke(m, raw(), params);
        }

        void Rebind() {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "Rebind", 0);
            MethodHandler::invoke(m, raw());
        }
        void Update(float deltaTime) {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "Update", 1);
            void* params[] = { &deltaTime };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] bool IsInTransition(int layerIndex) const {
            static auto m = MethodHandler::resolve("UnityEngine.Animator", "IsInTransition", 1);
            void* params[] = { &layerIndex };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
