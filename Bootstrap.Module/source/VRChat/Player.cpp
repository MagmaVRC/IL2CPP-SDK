#include <VRChat/Player.hpp>
#include <VRChat/VRCPlayerApi.hpp>
#include <VRChat/VRCPlayer.hpp>
#include <VRChat/APIUser.hpp>
#include <VRChat/HostBridge.hpp>

namespace IL2CPP::VRChat {

    namespace {
        std::optional<unix_player_offsets> player_table() {
            return Bridge::Offsets<unix_player_offsets>(unix_offsets_player);
        }
    }

    Player Player::GetLocalPlayer() {
        return Player(Bridge::LocalPlayer());
    }

    VRCPlayerApi Player::GetVRCPlayerApi() {
        if (!valid()) return {};
        const auto o = player_table();
        if (!o) return {};
        return VRCPlayerApi(Bridge::MemberAt(raw(), o->vrc_player_api));
    }

    VRCPlayer Player::GetVRCPlayer() {
        if (!valid()) return {};
        const auto o = player_table();
        if (!o) return {};
        return VRCPlayer(Bridge::MemberAt(raw(), o->vrc_player));
    }

    APIUser Player::GetAPIUser() {
        if (!valid()) return {};
        const auto o = player_table();
        if (!o) return {};
        if (o->api_user > 0) {
            // Same guard the model hop already carried: the detoured avatar setters fire
            // on a background thread, where _player can point at an object that is no
            // longer a live Player, and the slot then reads back as non-pointer bytes.
            void* user = Bridge::MemberAt(raw(), o->api_user);
            const auto address = reinterpret_cast<uintptr_t>(user);
            if (address < 0x10000u || address >= 0x7FFFFFFE0000ull || (address & 7u) != 0) return {};
            return APIUser(user);
        }
        // Builds that moved the user behind a data model: Player -> model -> APIUser.
        if (o->size < sizeof(unix_player_offsets) || o->user_model <= 0) return {};

        void* model = Bridge::MemberAt(raw(), o->user_model);
        // Non-null is not enough: a wrong user_model offset yields whatever bytes live there,
        // and the second hop then reads off an address that is not an object at all.
        const auto address = reinterpret_cast<uintptr_t>(model);
        if (address < 0x10000u || address >= 0x7FFFFFFE0000ull || (address & 7u) != 0) return {};

        return APIUser(Bridge::MemberAt(model, o->user_model_api_user));
    }

    IL2CPP::Module::ManagedObject Player::GetUSpeaker() {
        if (!valid()) return {};
        const auto o = player_table();
        if (!o) return {};
        return IL2CPP::Module::ManagedObject(Bridge::MemberAt(raw(), o->uspeaker));
    }

    PlayerRank Player::GetPlayerRank() {
        if (!valid()) return PlayerRank::Visitor;
        return static_cast<PlayerRank>(Bridge::Rank(GetAPIUser().raw()));
    }

    unix_color Player::GetRankColor(PlayerRank rank) {
        const unix_color c = Bridge::RankColor(static_cast<uint32_t>(rank));
        if (c.a <= 0.f) return unix_color{ 1.f, 1.f, 1.f, 1.f };
        return c;
    }

} // namespace IL2CPP::VRChat
