#pragma once

#include <cstdint>

namespace IL2CPP::VRChat {

    enum class TrackingDataType : int {
        Head,
        LeftHand,
        RightHand,
        Origin,
        AvatarRoot
    };

    enum class HumanBodyBones : int {
        Hips = 0,
        LeftUpperLeg = 1,
        RightUpperLeg = 2,
        LeftLowerLeg = 3,
        RightLowerLeg = 4,
        LeftFoot = 5,
        RightFoot = 6,
        Spine = 7,
        Chest = 8,
        UpperChest = 54,
        Neck = 9,
        Head = 10,
        LeftShoulder = 11,
        RightShoulder = 12,
        LeftUpperArm = 13,
        RightUpperArm = 14,
        LeftLowerArm = 15,
        RightLowerArm = 16,
        LeftHand = 17,
        RightHand = 18,
        LeftToes = 19,
        RightToes = 20,
        LeftEye = 21,
        RightEye = 22,
        Jaw = 23,
        LeftThumbProximal = 24,
        LeftThumbIntermediate = 25,
        LeftThumbDistal = 26,
        LeftIndexProximal = 27,
        LeftIndexIntermediate = 28,
        LeftIndexDistal = 29,
        LeftMiddleProximal = 30,
        LeftMiddleIntermediate = 31,
        LeftMiddleDistal = 32,
        LeftRingProximal = 33,
        LeftRingIntermediate = 34,
        LeftRingDistal = 35,
        LeftLittleProximal = 36,
        LeftLittleIntermediate = 37,
        LeftLittleDistal = 38,
        RightThumbProximal = 39,
        RightThumbIntermediate = 40,
        RightThumbDistal = 41,
        RightIndexProximal = 42,
        RightIndexIntermediate = 43,
        RightIndexDistal = 44,
        RightMiddleProximal = 45,
        RightMiddleIntermediate = 46,
        RightMiddleDistal = 47,
        RightRingProximal = 48,
        RightRingIntermediate = 49,
        RightRingDistal = 50,
        RightLittleProximal = 51,
        RightLittleIntermediate = 52,
        RightLittleDistal = 53,
        LastBone = 55
    };

    enum class SpawnOrientation : int {
        Default,
        AlignPlayerWithSpawnPoint,
        AlignRoomWithSpawnPoint
    };

    enum class SpawnOrder : int {
        First,
        Sequential,
        Random,
        Demo
    };

    enum class PickupHand : int {
        None,
        Left,
        Right
    };

    enum class PickupOrientation : int {
        Any,
        Grip,
        Gun
    };

    enum class AutoHoldMode : int {
        AutoDetect,
        Yes,
        No
    };

    enum class StationMobility : int {
        Mobile,
        Immobilize,
        ImmobilizeForVehicle
    };

    enum class SerializationMethod : int {
        None = 0,
        All,
        OwnerOnly
    };

    using VRC_SerializationMethod [[deprecated("use SerializationMethod")]] = SerializationMethod;

    enum class EventTiming : int {
        Update = 0,
        LateUpdate,
        PostLateUpdate,
        FixedUpdate
    };

    enum class NetworkEventTarget : int {
        All = 0,
        Owner = 1,
        Others = 2,
        Self = 3
    };

    enum class SyncType : int {
        Unknown = 0,
        None,
        Continuous,
        Manual
    };

    enum class RpcDestination : int {
        All = 0,
        Others = 1,
        Owner = 2,
        Master = 3,
        AllBuffered = 4,
        OthersBuffered = 5,
        Local = 6,
        AllBufferOne = 7,
        OthersBufferOne = 8,
        TargetPlayer = 9
    };

    enum class VrcBroadcastType : int {
        Always = 0,
        Master = 1,
        Local = 2,
        Owner = 3,
        AlwaysUnbuffered = 4,
        MasterUnbuffered = 5,
        OwnerUnbuffered = 6,
        AlwaysBufferOne = 7,
        MasterBufferOne = 8,
        OwnerBufferOne = 9
    };

} // namespace IL2CPP::VRChat
