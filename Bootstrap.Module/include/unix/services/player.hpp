#pragma once
#ifndef UNIX_USE
#  error "<unix/services/player.hpp> is included by <unix/module.hpp>; include that instead"
#endif

namespace UNIx {

using PlayerInfo   = unix_player_info;
using Locomotion   = unix_locomotion;
using VoiceSettings = unix_voice;
using AvatarAudio  = unix_avatar_audio;
using AvatarScale  = unix_avatar_scale;
using StorageUsage = unix_storage_usage;

/// <summary>Which tracked point on a player to read.</summary>
enum class TrackingPoint : uint32_t {
    Head = 0, LeftHand = 1, RightHand = 2, Origin = 3, AvatarRoot = 4,
};

/// <summary>Which hand a pickup or haptic pulse applies to.</summary>
enum class Hand : uint32_t { Right = 0, Left = 1 };

/// <summary>How a teleport orients the player.</summary>
enum class SpawnOrientation : uint32_t {
    Default = 0, AlignRoomWithSpawnPoint = 1, AlignPlayerWithSpawnPoint = 2,
};

/// <summary>Whether a nameplate is forced on, forced off, or left to VRChat.</summary>
enum class PlateVisibility : int32_t {
    Restore = unix_plate_restore, Hide = unix_plate_hide, Show = unix_plate_show,
};

/// <summary>One player's VRCPlayerApi. Every method takes the api pointer that
/// Players().LocalApi(), ById() or ByObject() returns, and runs on the Unity main thread.</summary>
class PlayerApi {
public:
    /// <summary>The player with this id, or null when nobody in the instance has it.</summary>
    template <class = void> [[nodiscard]] void* ById(int32_t playerId) const {
        UNIX_USE(player, by_id);
        return detail::g_d.player_by_id(detail::g_self, playerId);
    }

    /// <summary>The player a GameObject belongs to, or null.</summary>
    template <class = void> [[nodiscard]] void* ByObject(void* gameObject) const {
        UNIX_USE(player, by_object);
        return detail::g_d.player_by_object(detail::g_self, gameObject);
    }

    /// <summary>How many players are in the instance.</summary>
    template <class = void> [[nodiscard]] uint32_t Count() const {
        UNIX_USE(player, count);
        return detail::g_d.player_count(detail::g_self);
    }

    /// <summary>Identity and every state flag in one call.</summary>
    template <class = void> [[nodiscard]] std::optional<PlayerInfo> GetInfo(void* api) const {
        UNIX_USE(player, info);
        PlayerInfo i{};
        i.size = sizeof(i);
        if (detail::g_d.player_info(detail::g_self, api, &i) != unix_ok) return std::nullopt;
        return i;
    }

    template <class = void> [[nodiscard]] void* GetGameObject(void* api) const {
        UNIX_USE(player, game_object);
        return detail::g_d.player_game_object(detail::g_self, api);
    }

    /// <summary>Change the name this client shows for a player. Local only.</summary>
    template <class = void> Status SetDisplayName(void* api, std::string_view name) {
        UNIX_USE(player, set_display_name);
        return detail::g_d.player_set_display_name(detail::g_self, api, Sv(name));
    }

    template <class = void> [[nodiscard]] Vec3 GetPosition(void* api) const {
        UNIX_USE(player, position);
        return detail::g_d.player_position(detail::g_self, api);
    }

    template <class = void> [[nodiscard]] Vec4 GetRotation(void* api) const {
        UNIX_USE(player, rotation);
        return detail::g_d.player_rotation(detail::g_self, api);
    }

    template <class = void> [[nodiscard]] Vec3 GetVelocity(void* api) const {
        UNIX_USE(player, velocity);
        return detail::g_d.player_velocity(detail::g_self, api);
    }

    template <class = void> [[nodiscard]] bool IsGrounded(void* api) const {
        UNIX_USE(player, grounded);
        return detail::g_d.player_grounded(detail::g_self, api);
    }

    /// <summary>Only the local player's velocity can be set; VRChat ignores the rest.</summary>
    template <class = void> Status SetVelocity(void* api, Vec3 velocity) {
        UNIX_USE(player, set_velocity);
        return detail::g_d.player_set_velocity(detail::g_self, api, velocity);
    }

    /// <param name="lerpOnRemote">Smooth the move for other clients instead of snapping.</param>
    template <class = void> Status TeleportTo(void* api, Vec3 position, Vec4 rotation,
                                              SpawnOrientation orientation = SpawnOrientation::Default,
                                              bool lerpOnRemote = false) {
        UNIX_USE(player, teleport);
        return detail::g_d.player_teleport(detail::g_self, api, position, rotation,
                                           (uint32_t)orientation, lerpOnRemote);
    }

    /// <param name="spawnIndex">Which of the world's spawn points, or -1 for VRChat's choice.</param>
    template <class = void> Status Respawn(void* api, int32_t spawnIndex = -1) {
        UNIX_USE(player, respawn);
        return detail::g_d.player_respawn(detail::g_self, api, spawnIndex);
    }

    /// <summary>Where one humanoid bone is. Both outputs are filled or neither is.</summary>
    /// <param name="bone">Unity's HumanBodyBones value.</param>
    /// <returns>False when the player has no avatar, or no such bone on it.</returns>
    template <class = void> bool GetBone(void* api, uint32_t bone, Vec3& position, Vec4& rotation) const {
        UNIX_USE(player, bone);
        return detail::g_d.player_bone(detail::g_self, api, bone, &position, &rotation) == unix_ok;
    }

    /// <summary>Where one tracked point is.</summary>
    template <class = void> bool GetTracking(void* api, TrackingPoint point,
                                             Vec3& position, Vec4& rotation) const {
        UNIX_USE(player, tracking);
        return detail::g_d.player_tracking(detail::g_self, api, (uint32_t)point,
                                           &position, &rotation) == unix_ok;
    }

    /// <summary>Gravity, the three speeds and the jump impulse in one call.</summary>
    template <class = void> [[nodiscard]] std::optional<Locomotion> GetLocomotion(void* api) const {
        UNIX_USE(player, locomotion);
        Locomotion l{};
        l.size = sizeof(l);
        if (detail::g_d.player_locomotion(detail::g_self, api, &l) != unix_ok) return std::nullopt;
        return l;
    }

    /// <summary>Apply gravity, the three speeds and the jump impulse. `immobilized` is applied
    /// too; GetLocomotion never reports it, because VRChat exposes no getter.</summary>
    template <class = void> Status SetLocomotion(void* api, const Locomotion& l) {
        UNIX_USE(player, set_locomotion);
        return detail::g_d.player_set_locomotion(detail::g_self, api, &l);
    }

    template <class = void> Status Immobilize(void* api, bool immobile) {
        UNIX_USE(player, immobilize);
        return detail::g_d.player_immobilize(detail::g_self, api, immobile);
    }

    template <class = void> Status UseLegacyLocomotion(void* api) {
        UNIX_USE(player, use_legacy_locomotion);
        return detail::g_d.player_use_legacy_locomotion(detail::g_self, api);
    }

    template <class = void> [[nodiscard]] std::optional<VoiceSettings> GetVoice(void* api) const {
        UNIX_USE(player, voice);
        VoiceSettings v{};
        v.size = sizeof(v);
        if (detail::g_d.player_voice(detail::g_self, api, &v) != unix_ok) return std::nullopt;
        return v;
    }

    template <class = void> Status SetVoice(void* api, const VoiceSettings& v) {
        UNIX_USE(player, set_voice);
        return detail::g_d.player_set_voice(detail::g_self, api, &v);
    }

    /// <summary>Apply avatar audio settings. There is no matching getter: VRChat ships none.</summary>
    template <class = void> Status SetAvatarAudio(void* api, const AvatarAudio& a) {
        UNIX_USE(player, set_avatar_audio);
        return detail::g_d.player_set_avatar_audio(detail::g_self, api, &a);
    }

    /// <summary>Eye height, its bounds, and whether the player may scale themselves.</summary>
    template <class = void> [[nodiscard]] std::optional<AvatarScale> GetAvatarScale(void* api) const {
        UNIX_USE(player, avatar_scale);
        AvatarScale s{};
        s.size = sizeof(s);
        if (detail::g_d.player_avatar_scale(detail::g_self, api, &s) != unix_ok) return std::nullopt;
        return s;
    }

    template <class = void> Status SetEyeHeight(void* api, float meters) {
        UNIX_USE(player, set_eye_height);
        return detail::g_d.player_set_eye_height(detail::g_self, api, meters);
    }

    template <class = void> Status SetNameplateColor(void* api, Color color) {
        UNIX_USE(player, set_plate_color);
        return detail::g_d.player_set_plate_color(detail::g_self, api, color);
    }

    /// <summary>Give the nameplate its VRChat colour back.</summary>
    template <class = void> Status RestoreNameplateColor(void* api) {
        UNIX_USE(player, restore_plate_color);
        return detail::g_d.player_restore_plate_color(detail::g_self, api);
    }

    template <class = void> Status SetNameplateVisibility(void* api, PlateVisibility visibility) {
        UNIX_USE(player, set_plate_visible);
        return detail::g_d.player_set_plate_visible(detail::g_self, api, (int32_t)visibility);
    }

    template <class = void> [[nodiscard]] bool IsOwner(void* api, void* gameObject) const {
        UNIX_USE(player, is_owner);
        return detail::g_d.player_is_owner(detail::g_self, api, gameObject);
    }

    template <class = void> Status TakeOwnership(void* api, void* gameObject) {
        UNIX_USE(player, take_ownership);
        return detail::g_d.player_take_ownership(detail::g_self, api, gameObject);
    }

    template <class = void> Status EnablePickups(void* api, bool enabled) {
        UNIX_USE(player, enable_pickups);
        return detail::g_d.player_enable_pickups(detail::g_self, api, enabled);
    }

    /// <summary>What the player is holding in one hand, or null.</summary>
    template <class = void> [[nodiscard]] void* GetPickupInHand(void* api, Hand hand) const {
        UNIX_USE(player, pickup_in_hand);
        return detail::g_d.player_pickup_in_hand(detail::g_self, api, (uint32_t)hand);
    }

    template <class = void> Status PlayHaptic(void* api, Hand hand, float duration,
                                              float amplitude, float frequency) {
        UNIX_USE(player, haptic);
        return detail::g_d.player_haptic(detail::g_self, api, (uint32_t)hand,
                                         duration, amplitude, frequency);
    }

    /// <summary>Read one VRChat player tag.</summary>
    template <class = void> [[nodiscard]] std::string GetTag(void* api, std::string_view name) const {
        UNIX_USE(player, tag);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.player_tag(detail::g_self, api, Sv(name), b, c);
        });
    }

    template <class = void> Status SetTag(void* api, std::string_view name, std::string_view value) {
        UNIX_USE(player, set_tag);
        return detail::g_d.player_set_tag(detail::g_self, api, Sv(name), Sv(value));
    }

    template <class = void> Status ClearTags(void* api) {
        UNIX_USE(player, clear_tags);
        return detail::g_d.player_clear_tags(detail::g_self, api);
    }
};

/// <summary>VRC.SDKBase.Networking: instance roles, object ownership and server time.
/// Unity main thread only.</summary>
class Net {
public:
    /// <summary>Whether the instance has finished settling. Ownership answers are unreliable
    /// before it does.</summary>
    template <class = void> [[nodiscard]] bool IsSettled() const {
        UNIX_USE(net, is_settled);
        return detail::g_d.net_is_settled(detail::g_self);
    }

    template <class = void> [[nodiscard]] bool IsMaster() const {
        UNIX_USE(net, is_master);
        return detail::g_d.net_is_master(detail::g_self);
    }

    /// <summary>Whether the network is backed up. Sending while clogged makes it worse.</summary>
    template <class = void> [[nodiscard]] bool IsClogged() const {
        UNIX_USE(net, is_clogged);
        return detail::g_d.net_is_clogged(detail::g_self);
    }

    template <class = void> [[nodiscard]] bool IsInstanceOwner() const {
        UNIX_USE(net, is_instance_owner);
        return detail::g_d.net_is_instance_owner(detail::g_self);
    }

    template <class = void> [[nodiscard]] void* LocalPlayer() const {
        UNIX_USE(net, local_player);
        return detail::g_d.net_local_player(detail::g_self);
    }

    template <class = void> [[nodiscard]] void* Master() const {
        UNIX_USE(net, master);
        return detail::g_d.net_master(detail::g_self);
    }

    template <class = void> [[nodiscard]] void* InstanceOwner() const {
        UNIX_USE(net, instance_owner);
        return detail::g_d.net_instance_owner(detail::g_self);
    }

    /// <summary>Who owns a networked object, or null.</summary>
    template <class = void> [[nodiscard]] void* GetOwner(void* gameObject) const {
        UNIX_USE(net, owner);
        return detail::g_d.net_owner(detail::g_self, gameObject);
    }

    /// <summary>Hand a networked object to a player. Only the current owner may give it away.</summary>
    template <class = void> Status SetOwner(void* api, void* gameObject) {
        UNIX_USE(net, set_owner);
        return detail::g_d.net_set_owner(detail::g_self, api, gameObject);
    }

    template <class = void> [[nodiscard]] bool IsOwner(void* api, void* gameObject) const {
        UNIX_USE(net, is_owner);
        return detail::g_d.net_is_owner(detail::g_self, api, gameObject);
    }

    /// <summary>Whether a networked object has finished initialising.</summary>
    template <class = void> [[nodiscard]] bool IsObjectReady(void* gameObject) const {
        UNIX_USE(net, is_object_ready);
        return detail::g_d.net_is_object_ready(detail::g_self, gameObject);
    }

    /// <summary>The stable network name of an object, the one every client agrees on.</summary>
    template <class = void> [[nodiscard]] std::string GetUniqueName(void* gameObject) const {
        UNIX_USE(net, unique_name);
        return detail::ReadStr([&](char* b, uint32_t c) {
            return detail::g_d.net_unique_name(detail::g_self, gameObject, b, c);
        });
    }

    /// <summary>Destroy a networked object for everyone. Owner only.</summary>
    template <class = void> Status Destroy(void* gameObject) {
        UNIX_USE(net, destroy);
        return detail::g_d.net_destroy(detail::g_self, gameObject);
    }

    template <class = void> [[nodiscard]] int32_t ServerTimeMs() const {
        UNIX_USE(net, server_time_ms);
        return detail::g_d.net_server_time_ms(detail::g_self);
    }

    template <class = void> [[nodiscard]] double ServerTimeSeconds() const {
        UNIX_USE(net, server_time_seconds);
        return detail::g_d.net_server_time_seconds(detail::g_self);
    }

    /// <summary>The network clock as .NET ticks.</summary>
    template <class = void> [[nodiscard]] int64_t NetworkDateTime() const {
        UNIX_USE(net, network_datetime);
        return detail::g_d.net_network_datetime(detail::g_self);
    }

    /// <summary>When an object's owner last simulated it.</summary>
    template <class = void> [[nodiscard]] float SimulationTime(void* gameObject) const {
        UNIX_USE(net, simulation_time);
        return detail::g_d.net_simulation_time(detail::g_self, gameObject);
    }

    template <class = void> [[nodiscard]] float SimulationTimeOf(void* api) const {
        UNIX_USE(net, player_simulation_time);
        return detail::g_d.net_player_simulation_time(detail::g_self, api);
    }

    /// <summary>Move this client to another instance.</summary>
    template <class = void> bool GoToRoom(std::string_view roomId) {
        UNIX_USE(net, go_to_room);
        return detail::g_d.net_go_to_room(detail::g_self, Sv(roomId));
    }

    /// <summary>How much persistence storage a player is using, against the limits.</summary>
    template <class = void> [[nodiscard]] std::optional<StorageUsage> GetStorageUsage(void* api) const {
        UNIX_USE(net, storage_usage);
        StorageUsage u{};
        u.size = sizeof(u);
        if (detail::g_d.net_storage_usage(detail::g_self, api, &u) != unix_ok) return std::nullopt;
        return u;
    }

    /// <summary>Ask VRChat to refresh the storage figures. They are cached until it does.</summary>
    template <class = void> Status RequestStorageUpdate() {
        UNIX_USE(net, request_storage_update);
        return detail::g_d.net_request_storage_update(detail::g_self);
    }
};

} // namespace UNIx
