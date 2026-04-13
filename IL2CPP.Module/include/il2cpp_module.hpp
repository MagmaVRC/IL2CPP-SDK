#pragma once

#include <IL2CPP.Common/il2cpp_shared.hpp>
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

    [[nodiscard]] bool Connect();
    void Disconnect();
    [[nodiscard]] bool IsConnected() noexcept;
    [[nodiscard]] IL2CPP::il2cpp_exports const* GetExports() noexcept;
    [[nodiscard]] unity_functions const* GetUnityFunctions() noexcept;
    [[nodiscard]] bool IsUnityConnected() noexcept;

    [[nodiscard]] void*             GetDomain();
    [[nodiscard]] il2cppAssembly**  GetAssemblies(size_t* count);

    void* ThreadAttach(void* domain);
    void  ThreadDetach(void* thread);

    [[nodiscard]] il2cppClass* FindClass(std::string_view fullName);
    [[nodiscard]] il2cppClass* GetClassFromName(il2cppImage* image, const char* ns, const char* name);
    [[nodiscard]] il2cppType* GetType(il2cppClass* klass);
    [[nodiscard]] il2cppClass* ClassFromType(il2cppType* type);
    [[nodiscard]] il2cppClass* GetParent(il2cppClass* klass);
    [[nodiscard]] il2cppSystemType* GetSystemType(il2cppClass* klass);

    [[nodiscard]] il2cppFieldInfo* GetFields(il2cppClass* klass, void** iter);
    [[nodiscard]] il2cppFieldInfo* GetFieldByName(il2cppClass* klass, const char* name);
    [[nodiscard]] int GetFieldOffset(il2cppClass* klass, const char* fieldName);
    void GetStaticFieldValue(il2cppFieldInfo* field, void* outValue);
    void SetStaticFieldValue(il2cppFieldInfo* field, void* value);
    void GetFieldValue(il2cppObject* obj, il2cppFieldInfo* field, void* outValue);
    void SetFieldValue(il2cppObject* obj, il2cppFieldInfo* field, void* value);

    [[nodiscard]] il2cppMethodInfo* GetMethods(il2cppClass* klass, void** iter);
    [[nodiscard]] il2cppMethodInfo* GetMethodByName(il2cppClass* klass, const char* name, int argc = -1);
    [[nodiscard]] void* GetMethodPointer(il2cppClass* klass, const char* name, int argc = -1);
    [[nodiscard]] il2cppType* GetMethodParamType(il2cppMethodInfo* method, uint32_t index);
    [[nodiscard]] const char* GetMethodParamName(il2cppMethodInfo* method, uint32_t index);

    [[nodiscard]] il2cppPropertyInfo* GetProperties(il2cppClass* klass, void** iter);
    [[nodiscard]] il2cppPropertyInfo* GetPropertyByName(il2cppClass* klass, const char* name);

    [[nodiscard]] il2cppObject* NewObject(il2cppClass* klass);
    [[nodiscard]] il2cppObject* Box(il2cppClass* klass, void* data);
    il2cppObject* RuntimeInvoke(il2cppMethodInfo* method, void* obj, void** params, void** exc);

    [[nodiscard]] void* StringNew(const char* str);

    [[nodiscard]] void* ResolveCall(std::string_view fullPath, bool isExtern = false);

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

#include "Unity/Material.hpp"
#include "Unity/Texture2D.hpp"
#include "Unity/Sprite.hpp"
#include "Unity/Resources.hpp"
#include "Unity/AssetBundle.hpp"
#include "Unity/AssetBundleCreateRequest.hpp"

#include "Unity/UnityWebRequest.hpp"
#include "Unity/PlayerPrefs.hpp"

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
