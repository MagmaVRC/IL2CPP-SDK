#pragma once
#include "../MethodHandler.hpp"
#include "../System/String.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <string>
#include <string_view>

namespace IL2CPP::Module { [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept; }

namespace IL2CPP::Module::Unity {

    enum class RuntimePlatform : int {
        OSXEditor = 0, OSXPlayer = 1, WindowsPlayer = 2, WindowsEditor = 7,
        IPhonePlayer = 8, Android = 11, LinuxPlayer = 13, LinuxEditor = 16,
        WebGLPlayer = 17, PS4 = 25, XboxOne = 27, Switch = 32
    };

    class Application {
    public:
        Application() = delete;

        [[nodiscard]] static std::string GetDataPath() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_dataPath"), 0);
            void* str = MethodHandler::invoke<void*>(m, nullptr);
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] static std::string GetPersistentDataPath() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_persistentDataPath"), 0);
            void* str = MethodHandler::invoke<void*>(m, nullptr);
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] static std::string GetStreamingAssetsPath() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_streamingAssetsPath"), 0);
            void* str = MethodHandler::invoke<void*>(m, nullptr);
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] static bool GetIsPlaying() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_isPlaying"), 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static bool GetIsEditor() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_isEditor"), 0);
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        [[nodiscard]] static RuntimePlatform GetPlatform() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_platform"), 0);
            return static_cast<RuntimePlatform>(MethodHandler::invoke<int>(m, nullptr));
        }

        [[nodiscard]] static std::string GetVersion() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_version"), 0);
            void* str = MethodHandler::invoke<void*>(m, nullptr);
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] static std::string GetUnityVersion() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_unityVersion"), 0);
            void* str = MethodHandler::invoke<void*>(m, nullptr);
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] static std::string GetProductName() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_productName"), 0);
            void* str = MethodHandler::invoke<void*>(m, nullptr);
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        [[nodiscard]] static std::string GetCompanyName() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_companyName"), 0);
            void* str = MethodHandler::invoke<void*>(m, nullptr);
            if (!str) return "";
            return System::String{ str }.to_string();
        }

        static void Quit() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("Quit"), 0);
            MethodHandler::invoke(m, nullptr);
        }

        static void Quit(int exitCode) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("Quit"), 1);
            void* params[] = { &exitCode };
            MethodHandler::invoke(m, nullptr, params);
        }

        static void OpenURL(std::string_view url) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("OpenURL"), 1);
            auto* exports = GetExports();
            if (!exports || !exports->m_stringNew) return;
            void* il2cppStr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(exports->m_stringNew)(
                std::string(url).c_str());
            void* params[] = { il2cppStr };
            MethodHandler::invoke(m, nullptr, params);
        }

        [[nodiscard]] static int GetTargetFrameRate() {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("get_targetFrameRate"), 0);
            return MethodHandler::invoke<int>(m, nullptr);
        }

        static void SetTargetFrameRate(int frameRate) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.Application"), IL2CPP_STR("set_targetFrameRate"), 1);
            void* params[] = { &frameRate };
            MethodHandler::invoke(m, nullptr, params);
        }
    };

} // namespace IL2CPP::Module::Unity
