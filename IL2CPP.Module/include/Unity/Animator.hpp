#pragma once
#include "Behaviour.hpp"
#include "Transform.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module::Unity {

    struct AnimatorStateInfo {
        int   shortNameHash = 0;
        int   nameHash = 0;
        int   fullPathHash = 0;
        float normalizedTime = 0.f;
        float length = 0.f;
        float speed = 0.f;
        float speedMultiplier = 0.f;
        int   tagHash = 0;
        int   loop = 0;
    };

    class Animator : public Behaviour {
    public:
        using Behaviour::Behaviour;

        [[nodiscard]] float GetSpeed() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_speed"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetSpeed(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("set_speed"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetPlaybackTime() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_playbackTime"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetPlaybackTime(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("set_playbackTime"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetRuntimeAnimatorController() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_runtimeAnimatorController"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetRuntimeAnimatorController(void* controller) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("set_runtimeAnimatorController"), 1);
            void* params[] = { controller };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetAvatar() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_avatar"), 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetAvatar(void* avatar) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("set_avatar"), 1);
            void* params[] = { avatar };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetApplyRootMotion() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_applyRootMotion"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetApplyRootMotion(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("set_applyRootMotion"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetUpdateMode() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_updateMode"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetUpdateMode(int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("set_updateMode"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetHasRootMotion() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_hasRootMotion"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        /// <summary>The factor HumanPose positions are expressed in, not the transform scale.</summary>
        [[nodiscard]] float GetHumanScale() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_humanScale"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }

        /// <summary>The transform driving a humanoid bone.</summary>
        /// <param name="humanBone">A UnityEngine.HumanBodyBones value.</param>
        /// <returns>The bone transform, or an invalid Transform when the rig has none.</returns>
        [[nodiscard]] Transform GetBoneTransform(int humanBone) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("GetBoneTransform"), 1);
            void* params[] = { &humanBone };
            return Transform{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        [[nodiscard]] bool GetIsHuman() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_isHuman"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        /// <summary>Whether the rig is a humanoid avatar that can be posed.</summary>
        [[nodiscard]] bool IsHuman() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_isHuman"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] int GetParameterCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("get_parameterCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        /// <summary>Get parameter metadata by index (UnityEngine.AnimatorControllerParameter).</summary>
        [[nodiscard]] ManagedObject GetParameter(int index) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("GetParameter"), 1);
            void* params[] = { &index };
            return ManagedObject{ MethodHandler::invoke<void*>(m, raw(), params) };
        }

        void SetFloat(std::string_view name, float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("SetFloat"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] float GetFloat(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("GetFloat"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return 0.f;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<float>(m, raw(), params);
        }

        void SetInteger(std::string_view name, int value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("SetInteger"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] int GetInteger(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("GetInteger"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return 0;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<int>(m, raw(), params);
        }

        void SetBool(std::string_view name, bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("SetBool"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, &value };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] bool GetBool(std::string_view name) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("GetBool"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return false;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }

        void SetTrigger(std::string_view name) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("SetTrigger"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, raw(), params);
        }
        void ResetTrigger(std::string_view name) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("ResetTrigger"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, raw(), params);
        }

        void Play(std::string_view stateName) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("Play"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(stateName).c_str());
            void* params[] = { str };
            MethodHandler::invoke(m, raw(), params);
        }
        void CrossFade(std::string_view stateName, float normalizedTransitionDuration) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("CrossFade"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(stateName).c_str());
            void* params[] = { str, &normalizedTransitionDuration };
            MethodHandler::invoke(m, raw(), params);
        }

        void Rebind() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("Rebind"), 0);
            MethodHandler::invoke(m, raw());
        }
        void Update(float deltaTime) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("Update"), 1);
            void* params[] = { &deltaTime };
            MethodHandler::invoke(m, raw(), params);
        }
        [[nodiscard]] bool IsInTransition(int layerIndex) const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), IL2CPP_STR("IsInTransition"), 1);
            void* params[] = { &layerIndex };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }

        /// <summary>Whether the animator has bound its controller and can answer state queries.</summary>
        [[nodiscard]] bool GetIsInitialized() const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("get_isInitialized"), 0)) return false;
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] int GetLayerCount() const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("get_layerCount"), 0)) return 0;
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] std::string GetLayerName(int layerIndex) const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("GetLayerName"), 1)) return {};
            void* params[] = { &layerIndex };
            void* str = MethodHandler::invoke<void*>(m, raw(), params);
            return str ? System::String{ str }.to_string() : std::string();
        }

        [[nodiscard]] float GetLayerWeight(int layerIndex) const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("GetLayerWeight"), 1)) return 0.f;
            void* params[] = { &layerIndex };
            return MethodHandler::invoke<float>(m, raw(), params);
        }

        [[nodiscard]] AnimatorStateInfo GetCurrentAnimatorStateInfo(int layerIndex) const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("GetCurrentAnimatorStateInfo"), 1)) return {};
            void* params[] = { &layerIndex };
            return MethodHandler::invoke<AnimatorStateInfo>(m, raw(), params);
        }

        /// <summary>Get the controller's parameters as a managed UnityEngine.AnimatorControllerParameter[].</summary>
        /// <returns>The raw array, or null.</returns>
        [[nodiscard]] void* GetParameters() const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("get_parameters"), 0)) return nullptr;
            return MethodHandler::invoke<void*>(m, raw());
        }

        /// <summary>Read a float parameter by its name hash, through the int-id path only.</summary>
        [[nodiscard]] float GetFloatById(int id) const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("GetFloatID"), 1)) return 0.f;
            void* params[] = { &id };
            return MethodHandler::invoke<float>(m, raw(), params);
        }

        [[nodiscard]] int GetIntegerById(int id) const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("GetIntegerID"), 1)) return 0;
            void* params[] = { &id };
            return MethodHandler::invoke<int>(m, raw(), params);
        }

        [[nodiscard]] bool GetBoolById(int id) const {
            static Method m;
            if (!Lazy(m, IL2CPP_STR("GetBoolID"), 1)) return false;
            void* params[] = { &id };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }

        /// <summary>Hash a parameter or state name the way the animator does.</summary>
        [[nodiscard]] static int StringToHash(std::string_view name) {
            static Method m;
            auto* e = GetExports();
            if (!Lazy(m, IL2CPP_STR("StringToHash"), 1) || !e || !e->m_stringNew) return 0;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return MethodHandler::invoke<int>(m, nullptr, params);
        }

    private:
        static bool Lazy(Method& m, const char* name, int argc) {
            if (!m) m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Animator"), name, argc);
            return static_cast<bool>(m);
        }
    };

} // namespace IL2CPP::Module::Unity
