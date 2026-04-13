#pragma once
#include "Object.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_shared.hpp>
#include <cstring>
#include <string>
#include <string_view>

namespace IL2CPP::Module::Unity {

    class AudioClip : public Object {
    public:
        using Object::Object;

        [[nodiscard]] float GetLength() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "get_length", 0);
            return MethodHandler::invoke<float>(m, raw());
        }

        [[nodiscard]] int GetSamples() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "get_samples", 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] int GetChannels() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "get_channels", 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] int GetFrequency() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "get_frequency", 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] int GetLoadState() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "get_loadState", 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] int GetLoadType() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "get_loadType", 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] bool LoadAudioData() {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "LoadAudioData", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        bool UnloadAudioData() {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "UnloadAudioData", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        bool SetData(const float* data, size_t sampleCount, int offsetSamples = 0) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "SetData", 2);
            auto* e = GetExports();
            if (!e || !data || sampleCount == 0) return false;
            void* floatClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)("System.Single");
            if (!floatClass) return false;
            void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(e->m_arrayNew)(floatClass, sampleCount);
            if (!arr) return false;
            std::memcpy(static_cast<char*>(arr) + 0x20, data, sampleCount * sizeof(float));
            void* params[] = { arr, &offsetSamples };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }

        bool GetData(float* outData, size_t sampleCount, int offsetSamples = 0) const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "GetData", 2);
            auto* e = GetExports();
            if (!e || !outData || sampleCount == 0) return false;
            void* floatClass = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_helperFindClass)("System.Single");
            if (!floatClass) return false;
            void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(e->m_arrayNew)(floatClass, sampleCount);
            if (!arr) return false;
            void* params[] = { arr, &offsetSamples };
            bool ok = MethodHandler::invoke<bool>(m, raw(), params);
            if (ok) std::memcpy(outData, static_cast<char*>(arr) + 0x20, sampleCount * sizeof(float));
            return ok;
        }

        [[nodiscard]] static AudioClip Create(std::string_view name, int lengthSamples, int channels, int frequency, bool stream) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioClip", "Create", 5);
            auto* e = GetExports();
            if (!e || !e->m_stringNew) return AudioClip{};
            void* str = reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(e->m_stringNew)(std::string(name).c_str());
            void* params[] = { str, &lengthSamples, &channels, &frequency, &stream };
            return AudioClip{ MethodHandler::invoke<void*>(m, nullptr, params) };
        }
    };

} // namespace IL2CPP::Module::Unity
