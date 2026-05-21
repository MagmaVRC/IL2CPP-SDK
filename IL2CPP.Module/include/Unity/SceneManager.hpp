#pragma once
#include "../MethodHandler.hpp"
#include "../System/String.hpp"
#include "../System/Array.hpp"
#include "Object.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace IL2CPP::Module::Unity {

    class GameObject;

    class SceneWrapper {
        Scene m_scene{};
    public:
        SceneWrapper() = default;
        explicit SceneWrapper(Scene s) : m_scene(s) {}

        [[nodiscard]] int GetHandle() const { return m_scene.handle; }

        [[nodiscard]] std::string GetName() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.Scene"), IL2CPP_STR("GetNameInternal"), 1);
            int handle = m_scene.handle;
            void* p[] = { &handle };
            void* str = MethodHandler::invoke<void*>(m, nullptr, p);
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] int GetBuildIndex() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.Scene"), IL2CPP_STR("GetBuildIndexInternal"), 1);
            int handle = m_scene.handle;
            void* params[] = { &handle };
            return MethodHandler::invoke<int>(m, nullptr, params);
        }

        [[nodiscard]] bool GetIsLoaded() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.Scene"), IL2CPP_STR("GetIsLoadedInternal"), 1);
            int handle = m_scene.handle;
            void* params[] = { &handle };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] bool IsValid() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.Scene"), IL2CPP_STR("IsValidInternal"), 1);
            int handle = m_scene.handle;
            void* params[] = { &handle };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }

        [[nodiscard]] int GetRootCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.Scene"), IL2CPP_STR("GetRootCountInternal"), 1);
            int handle = m_scene.handle;
            void* params[] = { &handle };
            return MethodHandler::invoke<int>(m, nullptr, params);
        }

        [[nodiscard]] std::vector<Object> GetRootGameObjects() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.Scene"), IL2CPP_STR("GetRootGameObjectsInternal"), 1);
            static auto m2 = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.Scene"), IL2CPP_STR("GetRootGameObjects"), 0);
            auto* e = GetExports();
            if (!e) return {};
            void* sceneClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)("UnityEngine.SceneManagement.Scene");
            if (!sceneClass) return {};
            Scene s = m_scene;
            void* boxed = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, void*)>(e->m_valueBox)(sceneClass, &s);
            if (!boxed) return {};
            void* arr = MethodHandler::invoke<void*>(m2, boxed);
            return Object::FromArray<Object>(arr);
        }

        [[nodiscard]] Scene raw() const { return m_scene; }
        [[nodiscard]] operator bool() const { return m_scene.handle != 0; }
    };

    class SceneManager {
    public:
        SceneManager() = delete;

        [[nodiscard]] static SceneWrapper GetActiveScene() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.SceneManager"), IL2CPP_STR("GetActiveScene"), 0);
            return SceneWrapper{ MethodHandler::invoke<Scene>(m, nullptr) };
        }

        [[nodiscard]] static int GetSceneCount() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.SceneManager"), IL2CPP_STR("get_sceneCount"), 0);
            return MethodHandler::invoke<int>(m, nullptr);
        }

        [[nodiscard]] static int GetSceneCountInBuildSettings() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.SceneManager"), IL2CPP_STR("get_sceneCountInBuildSettings"), 0);
            return MethodHandler::invoke<int>(m, nullptr);
        }

        [[nodiscard]] static SceneWrapper GetSceneAt(int index) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.SceneManager"), IL2CPP_STR("GetSceneAt"), 1);
            void* params[] = { &index };
            return SceneWrapper{ MethodHandler::invoke<Scene>(m, nullptr, params) };
        }

        [[nodiscard]] static SceneWrapper GetSceneByName(std::string_view name) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.SceneManager"), IL2CPP_STR("GetSceneByName"), 1);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return SceneWrapper{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str };
            return SceneWrapper{ MethodHandler::invoke<Scene>(m, nullptr, params) };
        }

        [[nodiscard]] static SceneWrapper GetSceneByBuildIndex(int buildIndex) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.SceneManager"), IL2CPP_STR("GetSceneByBuildIndex"), 1);
            void* params[] = { &buildIndex };
            return SceneWrapper{ MethodHandler::invoke<Scene>(m, nullptr, params) };
        }

        static void LoadScene(int sceneBuildIndex, LoadSceneMode mode = LoadSceneMode::Single) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.SceneManager"), IL2CPP_STR("LoadScene"), 2);
            int modeInt = static_cast<int>(mode);
            void* params[] = { &sceneBuildIndex, &modeInt };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void LoadScene(std::string_view sceneName, LoadSceneMode mode = LoadSceneMode::Single) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.SceneManager"), IL2CPP_STR("LoadScene"), 2);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return;
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(sceneName).c_str());
            int modeInt = static_cast<int>(mode);
            void* params[] = { str, &modeInt };
            MethodHandler::invoke(m, nullptr, params);
        }

        [[nodiscard]] static bool SetActiveScene(SceneWrapper scene) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.SceneManagement.SceneManager"), IL2CPP_STR("SetActiveScene"), 1);
            Scene s = scene.raw();
            void* params[] = { &s };
            return MethodHandler::invoke<bool>(m, nullptr, params);
        }
    };

} // namespace IL2CPP::Module::Unity
