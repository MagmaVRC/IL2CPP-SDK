#pragma once

#include <IL2CPP.Common/il2cpp_shared.hpp>

IL2CPP_PREDEFINE
#include <IL2CPP.Common/il2cpp_structs.hpp>
#include <IL2CPP.Common/il2cpp_unity_shared.hpp>
#include <IL2CPP.Common/il2cpp_types.hpp>

#include <cstdint>
#include <string_view>
#include <vector>
#include <string>
#include <format>
#include <mutex>
#include <unordered_map>

namespace IL2CPP::Module {

    struct ThreadAttachment {
        void* thread = nullptr;
        uint8_t owned = 0;
    };

    static_assert(sizeof(ThreadAttachment) == 16);

    [[nodiscard]] bool Connect();
    [[nodiscard]] bool Disconnect();
    [[nodiscard]] bool IsConnected() noexcept;
    [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept;
    [[nodiscard]] unity_functions const* GetUnityFunctions() noexcept;
    [[nodiscard]] bool IsUnityConnected() noexcept;

    [[nodiscard]] void*             GetDomain();
    [[nodiscard]] il2cppAssembly**  GetAssemblies(size_t* count);

    void* ThreadAttach(void* domain);
    void  ThreadDetach(void* thread);
    [[nodiscard]] void* ThreadCurrent();
    [[nodiscard]] ThreadAttachment EnsureThreadAttached();
    void ReleaseThreadAttachment(ThreadAttachment attachment);

    [[nodiscard]] uint32_t GCHandleNew(il2cppObject* object, bool pinned = false);
    [[nodiscard]] il2cppObject* GCHandleGetTarget(uint32_t handle);
    void GCHandleFree(uint32_t handle);

    [[nodiscard]] il2cppClass* FindClass(std::string_view fullName);
    [[nodiscard]] il2cppClass* GetClassFromName(il2cppImage* image, const char* ns, const char* name);
    [[nodiscard]] il2cppType* GetType(il2cppClass* klass);
    [[nodiscard]] il2cppClass* ClassFromType(il2cppType* type);
    [[nodiscard]] il2cppClass* ClassFromSystemType(il2cppSystemType* type);
    [[nodiscard]] il2cppClass* GetParent(il2cppClass* klass);
    [[nodiscard]] il2cppSystemType* GetSystemType(il2cppClass* klass);
    [[nodiscard]] bool IsAssignableFrom(il2cppClass* target, il2cppClass* candidate);
    [[nodiscard]] il2cppClass* GetObjectClass(il2cppObject* object);
    [[nodiscard]] il2cppMethodInfo* GetVirtualMethod(il2cppObject* object, il2cppMethodInfo* method);
    [[nodiscard]] il2cppClass* GetClassOrElementClass(il2cppType* type);
    [[nodiscard]] il2cppClass* GetInterfaces(il2cppClass* klass, void** iter);
    [[nodiscard]] int32_t GetClassValueSize(il2cppClass* klass, uint32_t* alignment = nullptr);
    [[nodiscard]] bool ClassHasReferences(il2cppClass* klass);

    [[nodiscard]] il2cppFieldInfo* GetFields(il2cppClass* klass, void** iter);
    [[nodiscard]] il2cppFieldInfo* GetFieldByName(il2cppClass* klass, const char* name);
    [[nodiscard]] int GetFieldOffset(il2cppClass* klass, const char* fieldName);
    void GetStaticFieldValue(il2cppFieldInfo* field, void* outValue);
    void SetStaticFieldValue(il2cppFieldInfo* field, void* value);
    void GetFieldValue(il2cppObject* obj, il2cppFieldInfo* field, void* outValue);
    void SetFieldValue(il2cppObject* obj, il2cppFieldInfo* field, void* value);
    [[nodiscard]] uint32_t GetFieldFlags(il2cppFieldInfo* field);
    [[nodiscard]] uint32_t GetFieldToken(il2cppFieldInfo* field);

    [[nodiscard]] il2cppMethodInfo* GetMethods(il2cppClass* klass, void** iter);
    [[nodiscard]] il2cppMethodInfo* GetMethodByName(il2cppClass* klass, const char* name, int argc = -1);
    [[nodiscard]] void* GetMethodPointer(il2cppClass* klass, const char* name, int argc = -1);
    [[nodiscard]] il2cppType* GetMethodParamType(il2cppMethodInfo* method, uint32_t index);
    [[nodiscard]] const char* GetMethodParamName(il2cppMethodInfo* method, uint32_t index);
    [[nodiscard]] uint32_t GetMethodToken(il2cppMethodInfo* method);
    [[nodiscard]] uint16_t GetMethodSlot(il2cppMethodInfo* method);
    [[nodiscard]] il2cppObject* GetMethodObject(il2cppMethodInfo* method, il2cppClass* reflectedClass = nullptr);
    [[nodiscard]] il2cppMethodInfo* GetMethodFromReflection(il2cppObject* reflectionMethod);

    [[nodiscard]] il2cppPropertyInfo* GetProperties(il2cppClass* klass, void** iter);
    [[nodiscard]] il2cppPropertyInfo* GetPropertyByName(il2cppClass* klass, const char* name);
    [[nodiscard]] uint32_t GetPropertyFlags(il2cppPropertyInfo* property);
    [[nodiscard]] il2cppMethodInfo* GetPropertyGetter(il2cppPropertyInfo* property);
    [[nodiscard]] il2cppMethodInfo* GetPropertySetter(il2cppPropertyInfo* property);
    [[nodiscard]] const char* GetPropertyName(il2cppPropertyInfo* property);
    [[nodiscard]] il2cppClass* GetPropertyParent(il2cppPropertyInfo* property);

    [[nodiscard]] void* GetEvents(il2cppClass* klass, void** iter);
    [[nodiscard]] const char* GetEventName(void* eventInfo);
    [[nodiscard]] il2cppMethodInfo* GetEventAddMethod(void* eventInfo);
    [[nodiscard]] il2cppMethodInfo* GetEventRemoveMethod(void* eventInfo);
    [[nodiscard]] il2cppMethodInfo* GetEventRaiseMethod(void* eventInfo);
    [[nodiscard]] il2cppClass* GetEventParent(void* eventInfo);
    [[nodiscard]] uint32_t GetEventToken(void* eventInfo);

    [[nodiscard]] il2cppObject* NewObject(il2cppClass* klass);
    [[nodiscard]] il2cppObject* Box(il2cppClass* klass, void* data);
    il2cppObject* RuntimeInvoke(il2cppMethodInfo* method, void* obj, void** params, void** exc);

    [[nodiscard]] void* StringNew(const char* str);

    [[nodiscard]] uint64_t GetArrayLength(void* array);
    [[nodiscard]] void* NewArrayFull(il2cppClass* arrayClass, const uint64_t* lengths, const int64_t* lowerBounds);
    [[nodiscard]] uint32_t GetArrayElementSize(il2cppClass* arrayClass);
    [[nodiscard]] bool GetArrayBounds(void* array, uint32_t dimension, uint64_t* length, int64_t* lowerBound);
    [[nodiscard]] bool SetReferenceWithWriteBarrier(il2cppObject* object, void** targetAddress, void* value);

    [[nodiscard]] uint32_t GetTypeAttributes(il2cppType* type);
    [[nodiscard]] bool TypesEqual(il2cppType* lhs, il2cppType* rhs);

    void FormatException(il2cppObject* exception, char* buffer, uint32_t bufferSize);
    void FormatStackTrace(il2cppObject* exception, char* buffer, uint32_t bufferSize);

    [[nodiscard]] void* ResolveCall(std::string_view fullPath, bool isExtern = false);
    [[nodiscard]] void* ResolveExport(std::string_view name);

    [[nodiscard]] const char* GetStableName(const char* obfuscatedName);
    [[nodiscard]] const char* GetOriginalName(const char* stableName);

    size_t FetchFields(il2cppClass* klass, std::vector<il2cppFieldInfo*>& out);
    size_t FetchMethods(il2cppClass* klass, std::vector<il2cppMethodInfo*>& out);
    size_t FetchClasses(std::vector<il2cppClass*>& out, std::string_view assemblyName);

} // namespace IL2CPP::Module

#include "Reflection.hpp"
#include "ManagedObject.hpp"
#include "Deobfuscation.hpp"
#include "ClassResolver.hpp"
#include "ClassCache.hpp"
#include "MethodHandler.hpp"
#include "Dispatcher.hpp"

#include "Unity/Object.hpp"
#include "Unity/Component.hpp"
#include "Unity/GameObject.hpp"
#include "Unity/Behaviour.hpp"
#include "Unity/MonoBehaviour.hpp"
#include "Unity/Transform.hpp"
#include "Unity/Camera.hpp"
#include "Unity/Rigidbody.hpp"
#include "Unity/Collider.hpp"
#include "Unity/Time.hpp"
#include "Unity/Input.hpp"
#include "Unity/RectTransform.hpp"
#include "Unity/Canvas.hpp"
#include "Unity/CanvasGroup.hpp"

#include "Unity/Renderer.hpp"
#include "Unity/MeshRenderer.hpp"
#include "Unity/SkinnedMeshRenderer.hpp"
#include "Unity/LineRenderer.hpp"
#include "Unity/TrailRenderer.hpp"
#include "Unity/Light.hpp"
#include "Unity/ParticleSystem.hpp"
#include "Unity/Mesh.hpp"

#include "Unity/AudioSource.hpp"
#include "Unity/AudioClip.hpp"
#include "Unity/Animator.hpp"
#include "Unity/AnimationClip.hpp"
#include "Unity/RuntimeAnimatorController.hpp"
#include "Unity/AnimatorOverrideController.hpp"

#include "Unity/BoxCollider.hpp"
#include "Unity/SphereCollider.hpp"
#include "Unity/MeshCollider.hpp"

#include "Unity/UIBehaviour.hpp"
#include "Unity/Graphic.hpp"
#include "Unity/MaskableGraphic.hpp"
#include "Unity/Image.hpp"
#include "Unity/RawImage.hpp"
#include "Unity/TMP_Text.hpp"
#include "Unity/TextMeshProUGUI.hpp"
#include "Unity/UnityEvent.hpp"
#include "Unity/Selectable.hpp"
#include "Unity/Button.hpp"
#include "Unity/Toggle.hpp"
#include "Unity/Slider.hpp"
#include "Unity/InputField.hpp"
#include "Unity/TMP_InputField.hpp"

#include "Unity/CanvasScaler.hpp"
#include "Unity/ContentSizeFitter.hpp"
#include "Unity/LayoutGroup.hpp"
#include "Unity/HorizontalLayoutGroup.hpp"
#include "Unity/VerticalLayoutGroup.hpp"
#include "Unity/LayoutElement.hpp"
#include "Unity/ScrollRect.hpp"
#include "Unity/RectMask2D.hpp"

#include "Unity/Shader.hpp"
#include "Unity/Material.hpp"
#include "Unity/Texture2D.hpp"
#include "Unity/GraphicsBuffer.hpp"
#include "Unity/Sprite.hpp"
#include "Unity/Resources.hpp"
#include "Unity/AsyncOperation.hpp"
#include "Unity/AssetBundle.hpp"
#include "Unity/AssetBundleCreateRequest.hpp"

#include "Unity/UnityWebRequest.hpp"
#include "Unity/PlayerPrefs.hpp"

#include "Unity/UnityCallbacks.hpp"
#include "Unity/SceneManager.hpp"

#include "System/String.hpp"
#include "System/Array.hpp"
#include "System/List.hpp"
#include "System/Dictionary.hpp"
#include "System/HashSet.hpp"
#include "System/Queue.hpp"
#include "System/Stack.hpp"
#include "System/Delegate.hpp"
#include "System/IO.hpp"
#include "System/Encoding.hpp"
