#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>

#ifndef IL2CPP_STR
#   define IL2CPP_STR(x) x
#endif

#ifndef IL2CPP_PREDEFINE
#   define IL2CPP_PREDEFINE
#endif

namespace IL2CPP {

    struct Il2CppSystemType;

#ifndef IL2CPP_CALLTYPE
#   ifdef _WIN64
#       define IL2CPP_CALLTYPE __fastcall*
#   else
#       define IL2CPP_CALLTYPE __stdcall*
#   endif
#endif

    enum class UnityEvent : uint8_t {
        Update = 0,
        LateUpdate,
        FixedUpdate,
        PostLateUpdate,
        OnSceneLoaded,
        OnSceneUnloaded
    };

    constexpr uint16_t FIELD_ATTRIBUTE_ACCESS_MASK   = 0x0007;
    constexpr uint16_t FIELD_ATTRIBUTE_PRIVATE       = 0x0001;
    constexpr uint16_t FIELD_ATTRIBUTE_FAM_AND_ASSEM = 0x0002;
    constexpr uint16_t FIELD_ATTRIBUTE_ASSEMBLY      = 0x0003;
    constexpr uint16_t FIELD_ATTRIBUTE_FAMILY        = 0x0004;
    constexpr uint16_t FIELD_ATTRIBUTE_FAM_OR_ASSEM  = 0x0005;
    constexpr uint16_t FIELD_ATTRIBUTE_PUBLIC        = 0x0006;
    constexpr uint16_t FIELD_ATTRIBUTE_STATIC        = 0x0010;
    constexpr uint16_t FIELD_ATTRIBUTE_INIT_ONLY     = 0x0020;
    constexpr uint16_t FIELD_ATTRIBUTE_LITERAL       = 0x0040;
    constexpr uint16_t FIELD_ATTRIBUTE_HAS_DEFAULT   = 0x8000;

    constexpr uint16_t METHOD_ATTRIBUTE_ACCESS_MASK  = 0x0007;
    constexpr uint16_t METHOD_ATTRIBUTE_PRIVATE      = 0x0001;
    constexpr uint16_t METHOD_ATTRIBUTE_FAM_AND_ASSEM= 0x0002;
    constexpr uint16_t METHOD_ATTRIBUTE_ASSEMBLY     = 0x0003;
    constexpr uint16_t METHOD_ATTRIBUTE_FAMILY       = 0x0004;
    constexpr uint16_t METHOD_ATTRIBUTE_FAM_OR_ASSEM = 0x0005;
    constexpr uint16_t METHOD_ATTRIBUTE_PUBLIC       = 0x0006;
    constexpr uint16_t METHOD_ATTRIBUTE_STATIC       = 0x0010;
    constexpr uint16_t METHOD_ATTRIBUTE_FINAL        = 0x0020;
    constexpr uint16_t METHOD_ATTRIBUTE_VIRTUAL      = 0x0040;
    constexpr uint16_t METHOD_ATTRIBUTE_ABSTRACT     = 0x0400;

    struct il2cpp_exports {
        void*       m_hModule;

        void*       m_domainGet;
        void*       m_domainGetAssemblies;

        void*       m_threadAttach;
        void*       m_threadDetach;

        void*       m_init;
        void*       m_shutdown;

        void*       m_classFromName;
        void*       m_classFromIl2cppType;
        void*       m_classFromSystemType;
        void*       m_classGetType;
        void*       m_classGetParent;
        void*       m_classGetFields;
        void*       m_classGetFieldFromName;
        void*       m_classGetMethods;
        void*       m_classGetMethodFromName;
        void*       m_classGetProperties;
        void*       m_classGetPropertyFromName;
        void*       m_classGetEvents;
        void*       m_classEnumBasetype;
        void*       m_classIsGeneric;
        void*       m_classIsSubclassOf;
        void*       m_classIsEnum;
        void*       m_classGetStaticFieldData;

        void*       m_imageGetClassCount;
        void*       m_imageGetClass;
        void*       m_assemblyGetImage;

        void*       m_typeGetObject;
        void*       m_typeGetType;
        void*       m_typeGetName;
        void*       m_typeIsByref;
        void*       m_typeGetAssemblyQualifiedName;
        void*       m_typeGetReflectionName;

        void*       m_fieldGetValue;
        void*       m_fieldSetValue;
        void*       m_fieldStaticGetValue;
        void*       m_fieldStaticSetValue;

        void*       m_methodGetParam;
        void*       m_methodGetParamName;
        void*       m_methodGetFlags;

        void*       m_objectNew;
        void*       m_valueBox;

        void*       m_runtimeInvoke;
        void*       m_resolveFunction;

        void*       m_stringNew;

        void*       m_arrayNew;

        void*       m_alloc;
        void*       m_free;

        void*       m_gcCollect;

        void*       m_helperFindClass;
        void*       m_helperResolveCall;
        void*       m_helperGetStableName;
        void*       m_helperGetOriginalName;
        void*       m_helperAddMapping;
        void*       m_helperGetClassStableName;
        void*       m_helperGetClassStableNamespace;
        void*       m_helperGetClassStableFullName;
        void*       m_helperGetTypeStableName;
        void*       m_helperGetTypeStableNamespace;
        void*       m_helperGetTypeStableFullName;

        void*       m_helperClassFromType;
        void*       m_helperGetMethodParamType;
        void*       m_helperGetMethodReturnType;

        void*       m_helperCacheLoad;
        void*       m_helperCacheLoadMultiple;
        void*       m_helperCacheRelease;
        void*       m_helperCacheReleaseOne;
        void*       m_helperCacheGetClasses;
        void*       m_helperCacheFilter;
        void*       m_helperCacheFilterAll;

        void*       m_helperCallbackRegister;
        void*       m_helperCallbackInitialize;
        void*       m_helperCallbackUninitialize;

        void*       m_helperMethodResolve;
        void*       m_helperMethodInvoke;

        void*       m_helperGetStableNameByShort;

        void*       m_helperSymbolPublish;   // (kind,human,member,p0,p1) -> void
        void*       m_helperSymbolLookup;    // (kind,human,member) -> void*

        uint32_t    m_uVersion;
        uint32_t    m_uSize = 0;

        // Runtime-discovered struct offsets (populated by DiscoverStructOffsets/DiscoverClassOffsets)
        // -1 = not yet discovered. Used by both Core and Module.
        int32_t     m_offAsmImage       = -1;   // assembly → Il2CppImage*
        int32_t     m_offImgClassCount  = -1;   // image → uint32_t classCount
        int32_t     m_offClassName      = -1;   // class → const char* name
        int32_t     m_offClassNamespace = -1;   // class → const char* namespace
        int32_t     m_offClassFlags     = -1;   // class → uint32_t flags
        int32_t     m_offClassParent    = -1;   // class → il2cppClass* parent
        int32_t     m_offClassImage     = -1;   // class → il2cppImage*
        int32_t     m_offClassFields     = -1;  // class → FieldInfo* array (klass+0x98 in 1832)
        int32_t     m_offClassFieldCount = -1;  // class → uint16_t field_count (runtime-discovered)
        int32_t     m_offClassMethods     = -1; // class → MethodInfo** array (runtime-discovered)
        int32_t     m_offClassMethodCount = -1; // class → uint16_t method_count (runtime-discovered)

        // Assembly offsets
        int32_t     m_offAsmName         = -1;  // assembly → const char* name

        // Image offsets
        int32_t     m_offImgName         = -1;  // image → const char* name

        // Field struct offsets (populated by DiscoverFieldMethodOffsets or fingerprint decoding)
        int32_t     m_offFieldType         = -1; // field → Il2CppType*
        int32_t     m_offFieldOffset       = -1; // field → int32_t offset
        int32_t     m_offFieldName         = -1; // field → const char* name
        int32_t     m_offFieldParent       = -1; // field → Il2CppClass* parent
        int32_t     m_offFieldToken        = -1; // field → uint32_t metadata token (1830+)
        int32_t     m_offFieldTypeInflated = -1; // field → Il2CppType* generic-inflated (1832+)
        int32_t     m_fieldInfoStride      = -1; // FieldInfo sizeof (0x20 2022 / 0x28 1830 / 0x30 1832)

        // Method struct offsets (populated by DiscoverFieldMethodOffsets)
        int32_t     m_offMethodPointer    = -1; // method → void* native code pointer
        int32_t     m_offMethodVirtual    = -1; // method → void* virtual/invoker pointer
        int32_t     m_offMethodName       = -1; // method → const char* name
        int32_t     m_offMethodRetType    = -1; // method → Il2CppType* return_type
        int32_t     m_offMethodDeclType   = -1; // method → Il2CppClass* declaring_type
        int32_t     m_offMethodArgCount   = -1; // method → uint8_t arg count (param count)
        int32_t     m_offMethodFlags      = -1; // method → uint16_t MethodAttributes flags
        int32_t     m_offMethodSlot       = -1; // method → uint16_t vtable slot (0xFFFF if non-virtual)
        int32_t     m_offMethodToken      = -1; // method → uint32_t or uint64_t metadata token
        int32_t     m_offMethodParameters = -1; // method → void* parameters array
        int32_t     m_methodInfoStride    = -1; // MethodInfo sizeof (0x50 1830 / 0x60 1832)

        // Property struct offsets (populated by DiscoverPropertyOffsets)
        int32_t     m_offPropName       = -1;   // property → const char* name
        int32_t     m_offPropGetter     = -1;   // property → il2cppMethodInfo* get
        int32_t     m_offPropSetter     = -1;   // property → il2cppMethodInfo* set

        void*       m_fnFieldGetName    = nullptr;
        void*       m_fnFieldGetType    = nullptr;
        void*       m_fnFieldGetOffset  = nullptr;
        void*       m_fnFieldGetParent  = nullptr;
        void*       m_fnMethodGetName   = nullptr;

        void*       m_formatException   = nullptr;
        void*       m_objectGetClass    = nullptr;
        void*       m_classGetName      = nullptr;

        void*       m_threadCurrent = nullptr;
        void*       m_gcHandleNew = nullptr;
        void*       m_gcHandleGetTarget = nullptr;
        void*       m_gcHandleFree = nullptr;

        void*       m_classIsAssignableFrom = nullptr;
        void*       m_objectGetVirtualMethod = nullptr;

        void*       m_arrayLength = nullptr;
        void*       m_arrayNewFull = nullptr;
        void*       m_arrayElementSize = nullptr;
        void*       m_gcWBarrierSetField = nullptr;
        void*       m_classGetRank = nullptr;
        void*       m_offsetOfArrayLength = nullptr;
        void*       m_offsetOfArrayBounds = nullptr;
        void*       m_helperArrayGetBounds = nullptr;

        void*       m_methodGetToken = nullptr;
        void*       m_methodGetSlot = nullptr;
        void*       m_methodGetObject = nullptr;
        void*       m_methodGetFromReflection = nullptr;

        void*       m_typeGetAttrs = nullptr;
        void*       m_typeEquals = nullptr;
        void*       m_typeGetClassOrElementClass = nullptr;

        void*       m_classGetInterfaces = nullptr;
        void*       m_classValueSize = nullptr;
        void*       m_classHasReferences = nullptr;

        void*       m_propertyGetFlags = nullptr;
        void*       m_propertyGetGetMethod = nullptr;
        void*       m_propertyGetSetMethod = nullptr;
        void*       m_propertyGetName = nullptr;
        void*       m_propertyGetParent = nullptr;

        int32_t     m_offEventName = -1;
        int32_t     m_offEventAdd = -1;
        int32_t     m_offEventRemove = -1;
        int32_t     m_offEventRaise = -1;
        int32_t     m_offEventParent = -1;
        int32_t     m_offEventToken = -1;
        int32_t     _eventReserved0 = 0;
        int32_t     _eventReserved1 = 0;

        void*       m_eventGetName = nullptr;
        void*       m_eventGetAddMethod = nullptr;
        void*       m_eventGetRemoveMethod = nullptr;
        void*       m_eventGetRaiseMethod = nullptr;
        void*       m_eventGetParent = nullptr;
        void*       m_eventGetToken = nullptr;

        void*       m_formatStackTrace = nullptr;
        void*       m_fieldGetFlags = nullptr;
        void*       m_fieldGetToken = nullptr;
        void*       m_helperResolveExport = nullptr;

        // Centralized runtime-layout offsets (single source of truth; the Module
        // reads these instead of hardcoding constants). -1 = Module falls back to
        // its compiled default. Core populates them in Initialize.
        int32_t     m_offDelegateMethodPtr  = -1;  // System.Delegate → method_ptr      (default 0x10)
        int32_t     m_offDelegateInvokeImpl = -1;  // System.Delegate → invoke_impl      (default 0x18)
        int32_t     m_offDelegateTarget     = -1;  // System.Delegate → m_target         (default 0x20)
        int32_t     m_offDelegateMethodInfo = -1;  // System.Delegate → method (MethodInfo*) (default 0x28)
        int32_t     m_offDelegateExtraArg   = -1;  // System.Delegate → extra_arg        (default 0x38)
        int32_t     m_offArrayBounds        = -1;  // Il2CppArray → bounds               (default 0x10)
        int32_t     m_offArrayMaxLength     = -1;  // Il2CppArray → max_length           (default 0x18)
        int32_t     m_offArrayData          = -1;  // Il2CppArray → vector (inline data)  (default 0x20)
        int32_t     m_offObjectCachedPtr    = -1;  // UnityEngine.Object → m_CachedPtr    (default 0x10)
        int32_t     _offsetsPadTail         = 0;   // keeps 8-byte tail alignment

        // Unity dispatcher — main-thread queue drained from UpdateHook, worker thread pool.
        // fn: plain C trampoline; ud: opaque payload (Module heap-boxes the lambda + deletes in trampoline).
        void*       m_dispatchPostMain      = nullptr;   // bool(*)(void(*fn)(void*), void(*destroy)(void*), void* ud)
        void*       m_dispatchPostWorker    = nullptr;   // bool(*)(void(*fn)(void*), void(*destroy)(void*), void* ud)
        void*       m_dispatchIsMainThread  = nullptr;   // bool(*)()
        void*       m_dispatchWorkerCount   = nullptr;   // uint32_t(*)()  — pool size (for tuning)
    };

    constexpr uint32_t exports_version = 21;
    constexpr uint32_t exports_size = static_cast<uint32_t>(sizeof(il2cpp_exports));

    static_assert(std::is_standard_layout_v<il2cpp_exports>);
    static_assert(std::is_trivially_copyable_v<il2cpp_exports>);
    static_assert(sizeof(void*) == sizeof(uint64_t));
    static_assert(offsetof(il2cpp_exports, m_uSize) == offsetof(il2cpp_exports, m_uVersion) + sizeof(uint32_t));
    static_assert(offsetof(il2cpp_exports, m_threadCurrent) > offsetof(il2cpp_exports, m_classGetName));
    static_assert(offsetof(il2cpp_exports, m_offObjectCachedPtr) > offsetof(il2cpp_exports, m_helperResolveExport));
    static_assert(offsetof(il2cpp_exports, m_dispatchWorkerCount) + sizeof(void*) == sizeof(il2cpp_exports));
    static_assert(exports_version == 21);

} // namespace IL2CPP
