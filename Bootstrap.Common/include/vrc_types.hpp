#pragma once
#include <cstdint>

/// VRChat runtime type descriptors.
/// `void*` fields are IL2CPP class/singleton pointers.
/// `int` fields are byte offsets into IL2CPP instances.
namespace Bootstrap {

    enum class PlayerEvent : uint8_t {
        Awake = 0,
        Join = 1,
        JoinComplete = 2,
        Leave = 3,
        Unknown = 255
    };

    struct PlayerNameplateData {
        void* Class;
        int gameObject_contents;
        int gameObject_subText;
        int gameObject_textContainer;
        int gameObject_mainContainer;
        int gameObject_icon;
        int gameObject_quickStats;
        int gameObject_platform;
        int gameObject_interactionStatus;
        int gameObject_pronouns;
        int gameObject_playerStatusIcons;
        int gameObject_userMuted;
        int gameObject_userVolume;
        int gameObject_listenBlocked;
        int gameObject_earmuffsIcon;
        int gameObject_focusViewIcon;
        int gameObject_devBanner;
        int gameObject_devIcon;
        int gameObject_groupInfo;
        int gameObject_avatarProgress;
        int gameObject_friendIcon;
    };

    struct PlayerData {
        void* Class;
        void* Singleton;
        int VRCPlayerApi;
        int VRCPlayer;
        int APIUser;
        int USpeaker;
    };

    struct VRCPlayerData {
        void* Class;
        void* Singleton;
        int VRCPlayerApi;
        int Player;
        int PlayerNameplate;
        int PoseRecorder;
        int Serializer;
        int SyncPhysics;
        int ApiAvatar;
        int ApiAvatarFallback;

        void* Color_Administrator;
        void* Color_Friend;
        void* Color_Visitor;
        void* Color_NewUser;
        void* Color_User;
        void* Color_KnownUser;
        void* Color_Trusted;
        void* Color_Nuisance;

        int gameObject_nameplate;
        int gameObject_avatar;
    };

    struct LoadBalancingClientData {
        void* Class;
        void* OnEvent;
        void* OpRaiseEvent;
        int   photonPeerOffset;             // LoadBalancingClient.field_PhotonPeer_0

        // Singleton holder: a class with a single static Photon.Realtime.LoadBalancingClient field.
        void* SingletonHolderClass;
        int   singletonHolderStaticOffset;  // offset of the static field within static-data

        // PhotonPeer inherits PeerBase.roundTripTime (Single, ms).
        void* PhotonPeerClass;
        int   roundTripTimeOffset;
    };

    struct FlatBufferNetworkSerializerData {
        void* Class;
        int RequireFastRate;
    };

    struct PhotonViewData {
        void* Class;
        int ViewId;
    };

    struct VRCPlayerStepHeightData {
        void* Class;
        int maxStepHeight;
        int stepUpThreshold;
        int offsetDistance;
        int characterController;
        int groundedLayers;
    };

    struct VRCMotionStateData {
        void* Class;
        int characterController;
    };

    struct ONSPAudioData {
        void* Class;
        int audioSource;
        int onspLowPassFilter;
        int onspAudioSource;
    };

    struct RoomManagerData {
        void* Class;
        int apiWorld;
        int apiWorldInstance;
    };

    struct VRCRectMask2DData {
        void* Class;
        int mask;
        int clippingEnabled;
    };

    struct VRCDroneData {
        void* Class;
        int vrcPickup;
        int trailRenderer;
        int worldCollider;
        int pickupCollider;
        int PlayerOwner;
    };

} // namespace Bootstrap
