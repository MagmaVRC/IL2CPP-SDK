#pragma once
#include "Behaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>

namespace IL2CPP::Module::Unity {

    class MonoBehaviour : public Behaviour {
    public:
        using Behaviour::Behaviour;

        void Invoke(std::string_view methodName, float time) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MonoBehaviour"), IL2CPP_STR("Invoke"), 2);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(methodName).c_str());
            void* params[] = { str, &time };
            MethodHandler::invoke(m, raw(), params);
        }

        void CancelInvoke() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MonoBehaviour"), IL2CPP_STR("CancelInvoke"), 0);
            MethodHandler::invoke(m, raw());
        }

        [[nodiscard]] bool IsInvoking() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MonoBehaviour"), IL2CPP_STR("IsInvoking"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        /// <summary>Start a coroutine by method name.</summary>
        [[nodiscard]] ManagedObject StartCoroutine(std::string_view method) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.MonoBehaviour"), IL2CPP_STR("StartCoroutine"), 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return ManagedObject{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(method).c_str());
            void* params[] = { str };
            return ManagedObject{ MethodHandler::invoke<void*>(m, raw(), params) };
        }
    };

} // namespace IL2CPP::Module::Unity
