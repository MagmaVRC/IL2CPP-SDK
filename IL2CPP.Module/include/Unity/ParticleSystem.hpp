#pragma once
#include "Component.hpp"
#include "../MethodHandler.hpp"

namespace IL2CPP::Module::Unity {

    class ParticleSystem : public Component {
    public:
        using Component::Component;

        [[nodiscard]] bool GetIsPlaying() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("get_isPlaying"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        [[nodiscard]] bool GetIsEmitting() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("get_isEmitting"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        [[nodiscard]] bool GetIsStopped() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("get_isStopped"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        [[nodiscard]] bool GetIsPaused() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("get_isPaused"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        [[nodiscard]] int GetParticleCount() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("get_particleCount"), 0);
            return MethodHandler::invoke<int>(m, raw());
        }

        [[nodiscard]] float GetTime() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("get_time"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }
        void SetTime(float value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("set_time"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        [[nodiscard]] float GetDuration() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("get_duration"), 0);
            return MethodHandler::invoke<float>(m, raw());
        }

        [[nodiscard]] bool GetLoop() const {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("get_loop"), 0);
            return MethodHandler::invoke<bool>(m, raw());
        }
        void SetLoop(bool value) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("set_loop"), 1);
            void* params[] = { &value };
            MethodHandler::invoke(m, raw(), params);
        }

        void Play(bool withChildren = true) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("Play"), 1);
            void* params[] = { &withChildren };
            MethodHandler::invoke(m, raw(), params);
        }
        void Pause(bool withChildren = true) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("Pause"), 1);
            void* params[] = { &withChildren };
            MethodHandler::invoke(m, raw(), params);
        }
        void Stop(bool withChildren = true) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("Stop"), 1);
            void* params[] = { &withChildren };
            MethodHandler::invoke(m, raw(), params);
        }
        void Clear(bool withChildren = true) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("Clear"), 1);
            void* params[] = { &withChildren };
            MethodHandler::invoke(m, raw(), params);
        }
        void Emit(int count) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("Emit"), 1);
            void* params[] = { &count };
            MethodHandler::invoke(m, raw(), params);
        }
        void Simulate(float t, bool withChildren = true, bool restart = true) {
            static auto m = MethodHandler::resolve(IL2CPP_STR("UnityEngine.ParticleSystem"), IL2CPP_STR("Simulate"), 3);
            void* params[] = { &t, &withChildren, &restart };
            MethodHandler::invoke(m, raw(), params);
        }
    };

} // namespace IL2CPP::Module::Unity
