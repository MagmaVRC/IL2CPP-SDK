#pragma once
#include "Behaviour.hpp"
#include "../MethodHandler.hpp"
#include <IL2CPP.Common/il2cpp_types.hpp>

namespace IL2CPP::Module::Unity {

    class AudioSource : public Behaviour {
    public:
        using Behaviour::Behaviour;

        [[nodiscard]] void* GetClip() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_clip", 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetClip(void* clip) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_clip", 1);
            void* params[] = { clip };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetVolume() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_volume", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetVolume(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_volume", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetPitch() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_pitch", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetPitch(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_pitch", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetLoop() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_loop", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetLoop(bool value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_loop", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetMute() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_mute", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetMute(bool value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_mute", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetIsPlaying() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_isPlaying", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }

        [[nodiscard]] float GetTime() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_time", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetTime(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_time", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetSpatialBlend() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_spatialBlend", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetSpatialBlend(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_spatialBlend", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetMinDistance() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_minDistance", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetMinDistance(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_minDistance", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetMaxDistance() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_maxDistance", 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetMaxDistance(float value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_maxDistance", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetPlayOnAwake() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_playOnAwake", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetPlayOnAwake(bool value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_playOnAwake", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] int GetPriority() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_priority", 0);
            return MethodHandler::invoke<int>(m, raw());
        }
        void SetPriority(int value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_priority", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] bool GetSpatialize() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_spatialize", 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetSpatialize(bool value) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_spatialize", 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] void* GetOutputAudioMixerGroup() const {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "get_outputAudioMixerGroup", 0);
            return MethodHandler::invoke<void*>(m, raw());
        }
        void SetOutputAudioMixerGroup(void* group) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "set_outputAudioMixerGroup", 1);
            void* params[] = { group };
            MethodHandler::invoke(m, raw(), params);
        }

        void Play() {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "Play", 0);
            MethodHandler::invoke(m, raw());
        }
        void Play(uint64_t delay) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "Play", 1);
            void* params[] = { &delay };
            MethodHandler::invoke(m, raw(), params);
        }

        void Pause() {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "Pause", 0);
            MethodHandler::invoke(m, raw());
        }
        void UnPause() {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "UnPause", 0);
            MethodHandler::invoke(m, raw());
        }
        void Stop() {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "Stop", 0);
            MethodHandler::invoke(m, raw());
        }

        void PlayOneShot(void* clip, float volumeScale = 1.0f) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "PlayOneShot", 2);
            void* params[] = { clip, &volumeScale };
            MethodHandler::invoke(m, raw(), params);
        }

        static void PlayClipAtPoint(void* clip, const Vector3& position, float volume = 1.0f) {
            static auto m = MethodHandler::resolve("UnityEngine.AudioSource", "PlayClipAtPoint", 3);
            Vector3 pos = position;
            float vol = volume;
            void* params[] = { clip, &pos, &vol };
            MethodHandler::invoke(m, nullptr, params);
        }
    };

} // namespace IL2CPP::Module::Unity
