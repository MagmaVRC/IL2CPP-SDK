#pragma once
#include <cstdint>

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
        void*       m_classGetDeclaringType;
        void*       m_classGetElementClass;
        void*       m_classGetFields;
        void*       m_classGetFieldFromName;
        void*       m_classGetMethods;
        void*       m_classGetMethodFromName;
        void*       m_classGetProperties;
        void*       m_classGetPropertyFromName;
        void*       m_classGetEvents;
        void*       m_classGetNestedTypes;
        void*       m_classGetInterfaces;
        void*       m_classEnumBasetype;
        void*       m_classIsGeneric;
        void*       m_classIsSubclassOf;
        void*       m_classIsEnum;
        void*       m_classHasAttribute;
        void*       m_classGetStaticFieldData;
        void*       m_classGetBitmap;
        void*       m_classGetBitmapSize;

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
        void*       m_fieldHasAttribute;

        void*       m_methodGetObject;
        void*       m_methodGetParam;
        void*       m_methodGetParamName;
        void*       m_methodGetFlags;
        void*       m_methodHasAttribute;
        void*       m_methodGetName;

        void*       m_objectNew;
        void*       m_valueBox;

        void*       m_runtimeInvoke;
        void*       m_resolveFunction;

        void*       m_stringNew;

        void*       m_arrayClassGet;
        void*       m_arrayNew;
        void*       m_arrayNewSpecific;
        void*       m_arrayNewFull;

        void*       m_alloc;
        void*       m_free;

        void*       m_gcCollect;
        void*       m_gcCollectALittle;

        // Field accessor function pointers (populated by fingerprint tier)
        void*       m_fieldGetOffset    = nullptr;  // il2cpp_field_get_offset(FieldInfo*) → int
        void*       m_fieldGetName      = nullptr;  // il2cpp_field_get_name(FieldInfo*) → const char*
        void*       m_fieldGetParent    = nullptr;  // il2cpp_field_get_parent(FieldInfo*) → Il2CppClass*

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

        uint32_t    m_uVersion;
        uint32_t    _reserved = 0;

        // Runtime-discovered struct offsets (populated by DiscoverStructOffsets/DiscoverClassOffsets)
        // -1 = not yet discovered. Used by both Core and Module.
        int32_t     m_offAsmImage       = -1;   // assembly → Il2CppImage*
        int32_t     m_offImgClassCount  = -1;   // image → uint32_t classCount
        int32_t     m_offClassName      = -1;   // class → const char* name
        int32_t     m_offClassNamespace = -1;   // class → const char* namespace
        int32_t     m_offClassFlags     = -1;   // class → uint32_t flags
        int32_t     m_offClassParent    = -1;   // class → il2cppClass* parent
        int32_t     m_offClassImage     = -1;   // class → il2cppImage*

        // Field struct offsets (populated by DiscoverFieldMethodOffsets or fingerprint decoding)
        int32_t     m_offFieldType      = -1;   // field → Il2CppType*
        int32_t     m_offFieldOffset    = -1;   // field → int32_t offset
        int32_t     m_offFieldName      = -1;   // field → const char* name
        int32_t     m_offFieldParent    = -1;   // field → Il2CppClass* parent

        // Method struct offsets (populated by DiscoverFieldMethodOffsets)
        int32_t     m_offMethodPointer  = -1;   // method → void* native code pointer
        int32_t     m_offMethodVirtual  = -1;   // method → void* virtual/invoker pointer
        int32_t     m_offMethodName     = -1;   // method → const char* name
        int32_t     m_offMethodRetType  = -1;   // method → Il2CppType* return_type
        int32_t     m_offMethodDeclType = -1;   // method → Il2CppClass* declaring_type
        int32_t     m_offMethodArgCount = -1;   // method → uint8_t arg count

        // Property struct offsets (populated by DiscoverPropertyOffsets)
        int32_t     m_offPropName       = -1;   // property → const char* name
        int32_t     m_offPropGetter     = -1;   // property → il2cppMethodInfo* get
        int32_t     m_offPropSetter     = -1;   // property → il2cppMethodInfo* set
    };

    constexpr uint32_t exports_version = 12;  // bumped for method code pointer + virtual pointer offsets

} // namespace IL2CPP
