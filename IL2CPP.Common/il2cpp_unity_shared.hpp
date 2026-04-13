#pragma once
#include <cstdint>

namespace IL2CPP {


    struct objectFunctions_t {
        void* m_Destroy = nullptr;
        void* m_DestroyImmediate = nullptr;
        void* m_FindObjectsOfType = nullptr;
        void* m_FindObjectOfType = nullptr;
        void* m_GetName = nullptr;
        void* m_SetName = nullptr;
        void* m_Internal_CloneSingle = nullptr;
        void* m_Internal_CloneSingleWithParent = nullptr;
        void* m_DontDestroyOnLoad = nullptr;
    };

    struct gameObjectFunctions_t {
        void* m_AddComponent = nullptr;
        void* m_CreatePrimitive = nullptr;
        void* m_Find = nullptr;
        void* m_FindGameObjectsWithTag = nullptr;
        void* m_FindGameObjectWithTag = nullptr;
        void* m_GetComponent = nullptr;
        void* m_GetComponents = nullptr;
        void* m_GetComponentByName = nullptr;
        void* m_GetComponentInChildren = nullptr;
        void* m_GetComponentsInChildren = nullptr;
        void* m_GetComponentInParent = nullptr;
        void* m_GetComponentsInParent = nullptr;
        void* m_GetActive = nullptr;
        void* m_GetActiveSelf = nullptr;
        void* m_GetActiveInHierarchy = nullptr;
        void* m_GetLayer = nullptr;
        void* m_GetTransform = nullptr;
        void* m_GetTag = nullptr;
        void* m_GetName = nullptr;
        void* m_GetScene = nullptr;
        void* m_SetActive = nullptr;
        void* m_SetLayer = nullptr;
        void* m_SetTag = nullptr;
        void* m_SetName = nullptr;
        void* m_CompareTag = nullptr;
        void* m_SendMessage = nullptr;
        void* m_SendMessageUpwards = nullptr;
        void* m_BroadcastMessage = nullptr;
        void* m_TryGetComponent = nullptr;
    };

    struct componentFunctions_t {
        void* m_GetTransform = nullptr;
        void* m_GetGameObject = nullptr;
        void* m_GetComponent = nullptr;
        void* m_GetComponentByName = nullptr;
        void* m_GetComponents = nullptr;
        void* m_TryGetComponent = nullptr;
        void* m_GetComponentInChildren = nullptr;
        void* m_GetComponentsInChildren = nullptr;
        void* m_GetComponentInParent = nullptr;
        void* m_GetComponentsInParent = nullptr;
    };

    struct colliderFunctions_t {
        void* m_GetEnabled = nullptr; void* m_SetEnabled = nullptr;
        void* m_GetIsTrigger = nullptr; void* m_SetIsTrigger = nullptr;
        void* m_GetAttachedRigidbody = nullptr;
    };

    struct behaviourFunctions_t {
        void* m_GetEnabled = nullptr; void* m_SetEnabled = nullptr;
        void* m_GetIsActiveAndEnabled = nullptr;
    };

    struct monoBehaviourFunctions_t {
        void* m_Invoke = nullptr;
        void* m_CancelInvoke = nullptr;
        void* m_IsInvoking = nullptr;
        void* m_IsInvokingWithName = nullptr;
        void* m_StartCoroutine = nullptr;
    };

    struct transformFunctions_t {
        void* m_GetPosition = nullptr;
        void* m_SetPosition = nullptr;
        void* m_GetLocalPosition = nullptr;
        void* m_SetLocalPosition = nullptr;
        void* m_GetRight = nullptr;
        void* m_SetRight = nullptr;
        void* m_GetUp = nullptr;
        void* m_SetUp = nullptr;
        void* m_GetForward = nullptr;
        void* m_SetForward = nullptr;
        void* m_GetRotation = nullptr;
        void* m_SetRotation = nullptr;
        void* m_GetLocalRotation = nullptr;
        void* m_SetLocalRotation = nullptr;
        void* m_GetLocalScale = nullptr;
        void* m_SetLocalScale = nullptr;
        void* m_GetParent = nullptr;
        void* m_SetParent = nullptr;
        void* m_GetWorldToLocalMatrix = nullptr;
        void* m_GetLocalToWorldMatrix = nullptr;
        void* m_GetRoot = nullptr;
        void* m_GetChildCount = nullptr;
        void* m_GetLossyScale = nullptr;
        void* m_GetHierarchyCount = nullptr;
        void* m_GetChild = nullptr;
        void* m_SetPositionAndRotation = nullptr;
        void* m_SetLocalPositionAndRotation = nullptr;
        void* m_TransformDirection = nullptr;
        void* m_InverseTransformDirection = nullptr;
        void* m_TransformPoint = nullptr;
        void* m_DetachChildren = nullptr;
        void* m_Find = nullptr;
        void* m_FindChild = nullptr;
        void* m_IsChildOf = nullptr;
        void* m_RotateAround = nullptr;
        void* m_RotateAroundLocal = nullptr;
    };

    struct rigidbodyFunctions_t {
        void* m_SetDensity = nullptr;
        void* m_GetMass = nullptr; void* m_SetMass = nullptr;
        void* m_GetDrag = nullptr; void* m_SetDrag = nullptr;
        void* m_GetAngularDrag = nullptr; void* m_SetAngularDrag = nullptr;
        void* m_GetVelocity = nullptr; void* m_SetVelocity = nullptr;
        void* m_GetAngularVelocity = nullptr; void* m_SetAngularVelocity = nullptr;
        void* m_GetFreezeRotation = nullptr; void* m_SetFreezeRotation = nullptr;
        void* m_GetDetectCollisions = nullptr; void* m_SetDetectCollisions = nullptr;
        void* m_GetPosition = nullptr; void* m_SetPosition = nullptr;
        void* m_GetRotation = nullptr; void* m_SetRotation = nullptr;
        void* m_GetCenterOfMass = nullptr; void* m_SetCenterOfMass = nullptr;
        void* m_GetInterpolation = nullptr; void* m_SetInterpolation = nullptr;
        void* m_GetExcludedLayers = nullptr; void* m_SetExcludedLayers = nullptr;
        void* m_GetIncludedLayers = nullptr; void* m_SetIncludedLayers = nullptr;
        void* m_MovePosition = nullptr;
        void* m_MoveRotation = nullptr;
        void* m_Move = nullptr;
        void* m_Sleep = nullptr;
        void* m_IsSleeping = nullptr;
        void* m_WakeUp = nullptr;
        void* m_GetRelativePointVelocity = nullptr;
        void* m_GetPointVelocity = nullptr;
        void* m_AddForce = nullptr;
        void* m_AddTorque = nullptr;
        void* m_AddForceAtPosition = nullptr;
        void* m_AddExplosionForce = nullptr;
        void* m_ClosestPointOnBounds = nullptr;
        void* m_GetUseGravity = nullptr; void* m_SetUseGravity = nullptr;
        void* m_GetIsKinematic = nullptr; void* m_SetIsKinematic = nullptr;
        void* m_GetCollisionDetectionMode = nullptr; void* m_SetCollisionDetectionMode = nullptr;
        void* m_GetConstraints = nullptr; void* m_SetConstraints = nullptr;
    };

    struct timeFunctions_t {
        void* m_GetTime = nullptr;
        void* m_GetTimeAsDouble = nullptr;
        void* m_GetTimeSinceLevelLoad = nullptr;
        void* m_GetTimeSinceLevelLoadAsDouble = nullptr;
        void* m_GetDeltaTime = nullptr;
        void* m_GetFixedTime = nullptr;
        void* m_GetFixedTimeAsDouble = nullptr;
        void* m_GetUnscaledTime = nullptr;
        void* m_GetUnscaledTimeAsDouble = nullptr;
        void* m_GetFixedUnscaledTime = nullptr;
        void* m_GetFixedUnscaledTimeAsDouble = nullptr;
        void* m_GetUnscaledDeltaTime = nullptr;
        void* m_GetFixedUnscaledDeltaTime = nullptr;
        void* m_GetFixedDeltaTime = nullptr;
        void* m_GetMaximumDeltaTime = nullptr;
        void* m_GetSmoothDeltaTime = nullptr;
        void* m_GetTimeScale = nullptr;
        void* m_SetTimeScale = nullptr;
        void* m_GetFrameCount = nullptr;
        void* m_GetRenderedFrameCount = nullptr;
        void* m_GetRealtimeSinceStartup = nullptr;
        void* m_GetRealtimeSinceStartupAsDouble = nullptr;
        void* m_GetCaptureDeltaTime = nullptr;
        void* m_GetCaptureFramerate = nullptr;
    };

    struct inputFunctions_t {
        void* m_GetAxis = nullptr;
        void* m_GetAxisRaw = nullptr;
        void* m_GetButton = nullptr;
        void* m_GetButtonDown = nullptr;
        void* m_GetButtonUp = nullptr;
        void* m_GetKey = nullptr;
        void* m_GetKeyDown = nullptr;
        void* m_GetKeyUp = nullptr;
        void* m_GetMouseButton = nullptr;
        void* m_GetMouseButtonDown = nullptr;
        void* m_GetMouseButtonUp = nullptr;
        void* m_GetAnyKey = nullptr;
        void* m_GetAnyKeyDown = nullptr;
        void* m_GetMousePosition = nullptr;
        void* m_GetMouseScrollDelta = nullptr;
        void* m_GetMousePresent = nullptr;
        void* m_GetTouchCount = nullptr;
        void* m_GetTouchPresent = nullptr;
        void* m_GetAcceleration = nullptr;
        void* m_GetTouches = nullptr;
    };

    struct layerMaskFunctions_t {
        void* m_LayerToName = nullptr;
        void* m_NameToLayer = nullptr;
    };

    struct cameraFunctions_t {
        void* m_GetMain = nullptr;
        void* m_GetCurrent = nullptr;
        void* m_WorldToScreenPoint = nullptr;
        void* m_GetWorldToCameraMatrix = nullptr;
        void* m_GetProjectionMatrix = nullptr;
        void* m_GetFieldOfView = nullptr; void* m_SetFieldOfView = nullptr;
        void* m_GetNearClipPlane = nullptr; void* m_SetNearClipPlane = nullptr;
        void* m_GetFarClipPlane = nullptr; void* m_SetFarClipPlane = nullptr;
        void* m_GetOrthographic = nullptr; void* m_SetOrthographic = nullptr;
        void* m_GetOrthographicSize = nullptr; void* m_SetOrthographicSize = nullptr;
        void* m_GetDepth = nullptr; void* m_SetDepth = nullptr;
        void* m_GetCullingMask = nullptr; void* m_SetCullingMask = nullptr;
        void* m_GetPixelRect = nullptr;
        void* m_GetRect = nullptr; void* m_SetRect = nullptr;
        void* m_ScreenToWorldPoint = nullptr;
        void* m_ScreenPointToRay = nullptr;
        void* m_ViewportToWorldPoint = nullptr;
        void* m_WorldToViewportPoint = nullptr;
        void* m_GetAllCameras = nullptr;
        void* m_GetAllCamerasCount = nullptr;
    };

    struct physicsFunctions_t {
        void* m_Raycast = nullptr;
        void* m_SphereCast = nullptr;
        void* m_RaycastAll = nullptr;
        void* m_OverlapSphere = nullptr;
        void* m_CheckSphere = nullptr;
        void* m_Linecast = nullptr;
        void* m_GetGravity = nullptr; void* m_SetGravity = nullptr;
    };

    struct screenFunctions_t {
        void* m_GetWidth = nullptr;
        void* m_GetHeight = nullptr;
        void* m_GetDpi = nullptr;
        void* m_GetFullScreen = nullptr; void* m_SetFullScreen = nullptr;
        void* m_GetFullScreenMode = nullptr; void* m_SetFullScreenMode = nullptr;
        void* m_GetCurrentResolution = nullptr;
        void* m_SetResolution = nullptr;
    };

    struct applicationFunctions_t {
        void* m_GetDataPath = nullptr;
        void* m_GetPersistentDataPath = nullptr;
        void* m_GetStreamingAssetsPath = nullptr;
        void* m_GetIsPlaying = nullptr;
        void* m_GetIsEditor = nullptr;
        void* m_GetPlatform = nullptr;
        void* m_GetVersion = nullptr;
        void* m_GetProductName = nullptr;
        void* m_GetCompanyName = nullptr;
        void* m_Quit = nullptr;
        void* m_QuitWithExitCode = nullptr;
        void* m_OpenURL = nullptr;
        void* m_GetTargetFrameRate = nullptr; void* m_SetTargetFrameRate = nullptr;
    };

    struct cursorFunctions_t {
        void* m_GetVisible = nullptr; void* m_SetVisible = nullptr;
        void* m_GetLockState = nullptr; void* m_SetLockState = nullptr;
    };

    struct unity_functions {
        objectFunctions_t       object;
        gameObjectFunctions_t   gameObject;
        componentFunctions_t    component;
        colliderFunctions_t     collider;
        behaviourFunctions_t    behaviour;
        monoBehaviourFunctions_t monoBehaviour;
        transformFunctions_t    transform;
        rigidbodyFunctions_t    rigidbody;
        timeFunctions_t         time;
        inputFunctions_t        input;
        layerMaskFunctions_t    layerMask;
        cameraFunctions_t       camera;
        physicsFunctions_t      physics;
        screenFunctions_t       screen;
        applicationFunctions_t  application;
        cursorFunctions_t       cursor;
        uint32_t                m_uVersion;
        uint32_t                _reserved = 0;
    };

    constexpr uint32_t       unity_version      = 2;


} // namespace IL2CPP
