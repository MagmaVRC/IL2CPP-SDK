#include <VRChat/VRCPlayerApi.hpp>
#include <VRChat/VRCPlayer.hpp>
#include <VRChat/HostBridge.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/Reflection.hpp>
#include <IL2CPP.Module/include/Unity/GameObject.hpp>
#include <IL2CPP.Module/include/ICall.hpp>
#include <IL2CPP.Module/include/System/String.hpp>
#include <atomic>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

    namespace {
        // Class pointer and field offset packed into one word so a reader never sees a torn pair.
        int VRCPlayerApi_CachedFieldOffset(std::atomic<uint64_t>& cache, void* obj, const char* name) {
            void* klass = *static_cast<void**>(obj);
            const uint64_t key = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(klass)) << 16;
            const uint64_t cached = cache.load(std::memory_order_acquire);
            if (cached && (cached & ~uint64_t{ 0xFFFF }) == key) return static_cast<int>(cached & 0xFFFF);
            IL2CPP::Module::Field f = IL2CPP::Module::Class{ klass }.get_field(name);
            if (!f) return -1;
            const int off = f.offset();
            if (off < 0) return -1;
            if (off <= 0xFFFF) cache.store(key | static_cast<uint64_t>(off), std::memory_order_release);
            return off;
        }
    }

    std::string VRCPlayerApi::GetDisplayName() {
        if (!valid()) return "";
        static std::atomic<uint64_t> s_off{ 0 };
        const int off = VRCPlayerApi_CachedFieldOffset(s_off, raw(), "displayName");
        if (off < 0) return "";
        void* str = read<void*>(off);
        if (!str) return "";
        return IL2CPP::Module::System::String(str).to_string();
    }

    void VRCPlayerApi::SetDisplayName(std::string_view name) {
        set_string_field("displayName", name);
    }

    int VRCPlayerApi::GetPlayerId() {
        if (!valid()) return -1;
        static std::atomic<uint64_t> s_off{ 0 };
        if (const int off = VRCPlayerApi_CachedFieldOffset(s_off, raw(), "mPlayerId"); off >= 0)
            if (const int id = read<int>(off); id != -1) return id;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "get_playerId", 0);
        return MethodHandler::invoke<int>(m, raw());
    }

    bool VRCPlayerApi::IsLocal() {
        if (!valid()) return false;
        static std::atomic<uint64_t> s_off{ 0 };
        const int off = VRCPlayerApi_CachedFieldOffset(s_off, raw(), "isLocal");
        return off >= 0 ? read<bool>(off) : false;
    }

    bool VRCPlayerApi::IsMaster() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "get_isMaster", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool VRCPlayerApi::IsInstanceOwner() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "get_isInstanceOwner", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool VRCPlayerApi::IsModerator() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "get_isModerator", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool VRCPlayerApi::IsSuper() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "get_isSuper", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool VRCPlayerApi::IsSuspended() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "get_isSuspended", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool VRCPlayerApi::IsValid() {
        if (!valid()) return false;
        static std::atomic<uint64_t> s_off{ 0 };
        if (const int off = VRCPlayerApi_CachedFieldOffset(s_off, raw(), "_isValid"); off >= 0) return read<bool>(off);
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "IsValid", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool VRCPlayerApi::IsUserInVR() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "IsUserInVR", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool VRCPlayerApi::IsPlayerGrounded() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "IsPlayerGrounded", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    VRCPlayerApi::TrackingData VRCPlayerApi::GetTrackingData(TrackingDataType tt) {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetTrackingData", 1);
        void* params[1] = { &tt };
        return MethodHandler::invoke<TrackingData>(m, raw(), params);
    }

    IL2CPP::Vector3 VRCPlayerApi::GetVelocity() {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetVelocity", 0);
        return MethodHandler::invoke<IL2CPP::Vector3>(m, raw());
    }

    IL2CPP::Vector3 VRCPlayerApi::GetPosition() {
        if (!valid()) return {};
        using IL2CPP::Module::ICall;
        static ICall icTransform;
        static ICall icPosition;
        if (!icTransform.ready()) icTransform.resolve("UnityEngine.GameObject", "get_transform_Injected", 1,
            "UnityEngine.GameObject::get_transform_Injected(System.IntPtr)");
        if (!icPosition.ready()) icPosition.resolve("UnityEngine.Transform", "get_position_Injected", 2,
            "UnityEngine.Transform::get_position_Injected(System.IntPtr,UnityEngine.Vector3&)");
        auto trFn = icTransform.as<void*(IL2CPP_CALLTYPE)(void*)>();
        auto posFn = icPosition.as<void(IL2CPP_CALLTYPE)(void*, IL2CPP::Vector3*)>();
        if (trFn && posFn) {
            static std::atomic<uint64_t> s_goOff{ 0 };
            if (const int off = VRCPlayerApi_CachedFieldOffset(s_goOff, raw(), "gameObject"); off >= 0)
                if (void* goSelf = ICall::unity_self(read<void*>(off)))
                    if (void* trSelf = ICall::unity_self(ICall::unmarshal_object(trFn(goSelf)))) {
                        IL2CPP::Vector3 v{};
                        posFn(trSelf, &v);
                        return v;
                    }
        }
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetPosition", 0);
        return MethodHandler::invoke<IL2CPP::Vector3>(m, raw());
    }

    IL2CPP::Quaternion VRCPlayerApi::GetRotation() {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetRotation", 0);
        return MethodHandler::invoke<IL2CPP::Quaternion>(m, raw());
    }

    IL2CPP::Vector3 VRCPlayerApi::GetBonePosition(HumanBodyBones bone) {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetBonePosition", 1);
        void* params[1] = { &bone };
        return MethodHandler::invoke<IL2CPP::Vector3>(m, raw(), params);
    }

    IL2CPP::Quaternion VRCPlayerApi::GetBoneRotation(HumanBodyBones bone) {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetBoneRotation", 1);
        void* params[1] = { &bone };
        return MethodHandler::invoke<IL2CPP::Quaternion>(m, raw(), params);
    }

    void VRCPlayerApi::SetVelocity(IL2CPP::Vector3 v) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetVelocity", 1);
        void* params[1] = { &v };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    namespace { thread_local int t_teleportDepth = 0; }

    TeleportScope::TeleportScope() noexcept { ++t_teleportDepth; }
    TeleportScope::~TeleportScope() noexcept { --t_teleportDepth; }

    bool VRCPlayerApi::TeleportInProgress() noexcept { return t_teleportDepth > 0; }

    void VRCPlayerApi::TeleportTo(IL2CPP::Vector3 pos, IL2CPP::Quaternion rot) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "TeleportTo", 2);
        void* params[2] = { &pos, &rot };
        TeleportScope scope;
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::TeleportTo(IL2CPP::Vector3 pos, IL2CPP::Quaternion rot, SpawnOrientation o) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "TeleportTo", 3);
        void* params[3] = { &pos, &rot, &o };
        TeleportScope scope;
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::TeleportTo(IL2CPP::Vector3 pos, IL2CPP::Quaternion rot, SpawnOrientation o, bool lerpOnRemote) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "TeleportTo", 4);
        void* params[4] = { &pos, &rot, &o, &lerpOnRemote };
        TeleportScope scope;
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::Respawn() {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "Respawn", 0);
        MethodHandler::invoke<void>(m, raw());
    }

    void VRCPlayerApi::Respawn(int idx) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "Respawn", 1);
        void* params[1] = { &idx };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    float VRCPlayerApi::GetGravityStrength() {
        if (!valid()) return 1.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetGravityStrength", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    float VRCPlayerApi::GetRunSpeed() {
        if (!valid()) return 4.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetRunSpeed", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    float VRCPlayerApi::GetWalkSpeed() {
        if (!valid()) return 2.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetWalkSpeed", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    float VRCPlayerApi::GetStrafeSpeed() {
        if (!valid()) return 2.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetStrafeSpeed", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    float VRCPlayerApi::GetJumpImpulse() {
        if (!valid()) return 3.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetJumpImpulse", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    void VRCPlayerApi::SetGravityStrength(float s) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetGravityStrength", 1);
        void* params[1] = { &s };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetRunSpeed(float s) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetRunSpeed", 1);
        void* params[1] = { &s };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetWalkSpeed(float s) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetWalkSpeed", 1);
        void* params[1] = { &s };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetStrafeSpeed(float s) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetStrafeSpeed", 1);
        void* params[1] = { &s };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetJumpImpulse(float i) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetJumpImpulse", 1);
        void* params[1] = { &i };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::UseLegacyLocomotion() {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "UseLegacyLocomotion", 0);
        MethodHandler::invoke<void>(m, raw());
    }

    void VRCPlayerApi::UseAttachedStation() {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "UseAttachedStation", 0);
        MethodHandler::invoke<void>(m, raw());
    }

    void VRCPlayerApi::Immobilize(bool i) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "Immobilize", 1);
        void* params[1] = { &i };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    float VRCPlayerApi::GetVoiceGain() {
        if (!valid()) return 1.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetVoiceGain", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    float VRCPlayerApi::GetVoiceDistanceNear() {
        if (!valid()) return 0.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetVoiceDistanceNear", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    float VRCPlayerApi::GetVoiceDistanceFar() {
        if (!valid()) return 25.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetVoiceDistanceFar", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    float VRCPlayerApi::GetVoiceVolumetricRadius() {
        if (!valid()) return 0.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetVoiceVolumetricRadius", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    bool VRCPlayerApi::GetVoiceLowpass() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetVoiceLowpass", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    void VRCPlayerApi::SetVoiceGain(float g) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetVoiceGain", 1);
        void* params[1] = { &g };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetVoiceDistanceNear(float n) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetVoiceDistanceNear", 1);
        void* params[1] = { &n };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetVoiceDistanceFar(float f) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetVoiceDistanceFar", 1);
        void* params[1] = { &f };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetVoiceVolumetricRadius(float r) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetVoiceVolumetricRadius", 1);
        void* params[1] = { &r };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetVoiceLowpass(bool e) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetVoiceLowpass", 1);
        void* params[1] = { &e };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetAvatarAudioGain(float g) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarAudioGain", 1);
        void* params[1] = { &g };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetAvatarAudioFarRadius(float d) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarAudioFarRadius", 1);
        void* params[1] = { &d };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetAvatarAudioNearRadius(float d) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarAudioNearRadius", 1);
        void* params[1] = { &d };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetAvatarAudioVolumetricRadius(float r) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarAudioVolumetricRadius", 1);
        void* params[1] = { &r };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetAvatarAudioForceSpatial(bool f) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarAudioForceSpatial", 1);
        void* params[1] = { &f };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetAvatarAudioCustomCurve(bool a) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarAudioCustomCurve", 1);
        void* params[1] = { &a };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    float VRCPlayerApi::GetAvatarEyeHeightAsMeters() {
        if (!valid()) return 0.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetAvatarEyeHeightAsMeters", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    float VRCPlayerApi::GetAvatarEyeHeightMaximumAsMeters() {
        if (!valid()) return 0.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetAvatarEyeHeightMaximumAsMeters", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    float VRCPlayerApi::GetAvatarEyeHeightMinimumAsMeters() {
        if (!valid()) return 0.0f;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetAvatarEyeHeightMinimumAsMeters", 0);
        return MethodHandler::invoke<float>(m, raw());
    }

    bool VRCPlayerApi::GetManualAvatarScalingAllowed() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetManualAvatarScalingAllowed", 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    void VRCPlayerApi::SetAvatarEyeHeightByMeters(float m_) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarEyeHeightByMeters", 1);
        void* params[1] = { &m_ };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetAvatarEyeHeightByMultiplier(float m_) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarEyeHeightByMultiplier", 1);
        void* params[1] = { &m_ };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetAvatarEyeHeightMaximumByMeters(float m_) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarEyeHeightMaximumByMeters", 1);
        void* params[1] = { &m_ };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetAvatarEyeHeightMinimumByMeters(float m_) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetAvatarEyeHeightMinimumByMeters", 1);
        void* params[1] = { &m_ };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetManualAvatarScalingAllowed(bool e) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetManualAvatarScalingAllowed", 1);
        void* params[1] = { &e };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::SetNamePlateColor(IL2CPP::Color c) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetNamePlateColor", 1);
        void* params[1] = { &c };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::RestoreNamePlateColor() {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "RestoreNamePlateColor", 0);
        MethodHandler::invoke<void>(m, raw());
    }

    void VRCPlayerApi::SetNamePlateVisibility(bool f) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetNamePlateVisibility", 1);
        void* params[1] = { &f };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::RestoreNamePlateVisibility() {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "RestoreNamePlateVisibility", 0);
        MethodHandler::invoke<void>(m, raw());
    }

    bool VRCPlayerApi::IsOwner(IL2CPP::Module::Unity::GameObject obj) {
        if (!valid() || !obj) return false;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "IsOwner", 1);
        void* params[1] = { obj.raw() };
        return MethodHandler::invoke<bool>(m, raw(), params);
    }

    void VRCPlayerApi::TakeOwnership(IL2CPP::Module::Unity::GameObject obj) {
        if (!valid() || !obj) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "TakeOwnership", 1);
        void* params[1] = { obj.raw() };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::EnablePickups(bool e) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "EnablePickups", 1);
        void* params[1] = { &e };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    IL2CPP::Module::ManagedObject VRCPlayerApi::GetPickupInHand(PickupHand hand) {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetPickupInHand", 1);
        if (!m) m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetPickupInHand", 1);
        if (!m) return {};
        void* params[1] = { &hand };
        return IL2CPP::Module::ManagedObject(MethodHandler::invoke<void*>(m, raw(), params));
    }

    std::string VRCPlayerApi::GetPlayerTag(std::string_view tagName) {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetPlayerTag", 1);
        if (!m) m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetPlayerTag", 1);
        if (!m) return {};
        auto str = IL2CPP::Module::System::String::create(tagName);
        void* params[1] = { str.raw() };
        void* result = MethodHandler::invoke<void*>(m, raw(), params);
        return result ? IL2CPP::Module::System::String(result).to_string() : std::string{};
    }

    void VRCPlayerApi::SetPlayerTag(std::string_view tagName, std::string_view tagValue) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetPlayerTag", 2);
        if (!m) m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "SetPlayerTag", 2);
        if (!m) return;
        auto name = IL2CPP::Module::System::String::create(tagName);
        auto value = IL2CPP::Module::System::String::create(tagValue);
        void* params[2] = { name.raw(), value.raw() };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void VRCPlayerApi::ClearPlayerTags() {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "ClearPlayerTags", 0);
        if (!m) m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "ClearPlayerTags", 0);
        if (!m) return;
        MethodHandler::invoke<void>(m, raw());
    }

    void VRCPlayerApi::PlayHapticEventInHand(PickupHand hand, float duration, float amplitude, float frequency) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "PlayHapticEventInHand", 4);
        if (!m) m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "PlayHapticEventInHand", 4);
        if (!m) return;
        void* params[4] = { &hand, &duration, &amplitude, &frequency };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    bool VRCPlayerApi::IsVRCPlus() {
        if (!valid()) return false;
        static IL2CPP::Module::Method m{};
        if (!m) m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "get_isVRCPlus", 0);
        if (!m) return false;
        return MethodHandler::invoke<bool>(m, raw());
    }

    IL2CPP::Module::Unity::GameObject VRCPlayerApi::GetGameObject() {
        if (!valid()) return {};
        static std::atomic<uint64_t> s_off{ 0 };
        const int off = VRCPlayerApi_CachedFieldOffset(s_off, raw(), "gameObject");
        if (off < 0) return {};
        return IL2CPP::Module::Unity::GameObject(read<void*>(off));
    }

    VRCPlayer VRCPlayerApi::GetVRCPlayer() {
        if (!valid()) return {};
        auto go = GetGameObject();
        if (!go) return {};
        static IL2CPP::Module::Class klass{};
        if (!klass) klass = IL2CPP::Module::Class::find("VRCPlayer");
        if (!klass) return {};
        return VRCPlayer(IL2CPP::Module::Unity::GameObject(go).GetComponent(klass).raw());
    }

    int VRCPlayerApi::GetPlayerCount() {
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetPlayerCount", 0);
        if (!m) return 0;
        return MethodHandler::invoke<int>(m, nullptr);
    }

    int VRCPlayerApi::GetPlayerId(VRCPlayerApi p) {
        if (!p) return -1;
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetPlayerId", 1);
        if (!m) return -1;
        void* params[1] = { p.raw() };
        return MethodHandler::invoke<int>(m, nullptr, params);
    }

    VRCPlayerApi VRCPlayerApi::GetPlayerByGameObject(IL2CPP::Module::Unity::GameObject o) {
        if (!o) return VRCPlayerApi();
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetPlayerByGameObject", 1);
        void* params[1] = { o.raw() };
        void* result = MethodHandler::invoke<void*>(m, nullptr, params);
        return result ? VRCPlayerApi(result) : VRCPlayerApi();
    }

    VRCPlayerApi VRCPlayerApi::GetPlayerById(int id) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.VRCPlayerApi", "GetPlayerById", 1);
        void* params[1] = { &id };
        void* result = MethodHandler::invoke<void*>(m, nullptr, params);
        return result ? VRCPlayerApi(result) : VRCPlayerApi();
    }

    VRCPlayerApi VRCPlayerApi::GetLocalPlayer() {
        return VRCPlayerApi(Bridge::LocalPlayerApi());
    }

} // namespace IL2CPP::VRChat
