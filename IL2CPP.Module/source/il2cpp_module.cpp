#include <il2cpp_module.hpp>
#include <IL2CPP.Common/il2cpp_unity_shared.hpp>
#include <SharedMemory.Common/shared_memory.hpp>
#include <windows.h>
#include <atomic>
#include <climits>
#include <cstring>

namespace IL2CPP::Module {

    namespace {

        struct ConnectionState {
            std::atomic<bool>           connected{ false };
            IL2CPP::il2cpp_exports const*       exports = nullptr;
            unity_functions const*      unity = nullptr;
        };

        ConnectionState g_conn;

        inline auto* E() { return g_conn.exports; }

        static il2cppClass* SafeGetParent(il2cppClass* klass) noexcept {
            if (!klass || !E()) return nullptr;
            if (E()->m_offClassParent >= 0) {
                __try {
                    auto* parent = *reinterpret_cast<il2cppClass**>(
                        reinterpret_cast<char*>(klass) + E()->m_offClassParent);
                    return parent;
                } __except(1) {
                    return nullptr;
                }
            }
            if (!E()->m_classGetParent) return nullptr;
            __try {
                return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*)>(
                    E()->m_classGetParent)(klass);
            } __except(1) {
                return nullptr;
            }
        }

        static bool SafeCStringEquals(const char* lhs, const char* rhs) noexcept {
            if (!lhs || !rhs) return false;
            __try {
                return std::strcmp(lhs, rhs) == 0;
            } __except(1) {
                return false;
            }
        }

        static il2cppPropertyInfo* FindPropertyByIteration(
            il2cppClass* klass, const char* name) noexcept
        {
            if (!E() || !E()->m_classGetProperties || !klass || !name) return nullptr;

            auto* current = klass;
            while (current) {
                void* iter = nullptr;
                __try {
                    while (auto* prop = reinterpret_cast<il2cppPropertyInfo*(IL2CPP_CALLTYPE)(void*, void**)>(
                        E()->m_classGetProperties)(current, &iter))
                    {
                        if (!IsValid(prop)) continue;
                        const char* propName = prop->get_name();
                        if (!propName || !*propName) continue;
                        if (SafeCStringEquals(propName, name))
                            return prop;
                    }
                } __except(1) {
                    // Skip this class level and continue walking parents.
                }

                current = SafeGetParent(current);
            }

            return nullptr;
        }

        static bool NeedsFieldMaterialization(il2cppClass* klass) noexcept {
            if (!klass || !E()) return false;
            if (E()->m_offClassFields < 0 || E()->m_offClassFieldCount < 0) return false;

            __try {
                auto* klassBytes = reinterpret_cast<const char*>(klass);
                uintptr_t fieldsPtr = *reinterpret_cast<const uintptr_t*>(
                    klassBytes + E()->m_offClassFields);
                uint32_t countDword = *reinterpret_cast<const uint32_t*>(
                    klassBytes + E()->m_offClassFieldCount);
                uint16_t fieldCount = static_cast<uint16_t>(countDword & 0xFFFF);
                return fieldCount != 0 && fieldsPtr == 0;
            } __except(1) {
                return false;
            }
        }

        static bool TryRuntimeClassInit(void* rawFn, void* klass) noexcept {
            if (!rawFn || !klass) return false;
            using InitFn = void(IL2CPP_CALLTYPE)(void*);
            __try {
                reinterpret_cast<InitFn>(rawFn)(klass);
                return true;
            } __except(1) {
                return false;
            }
        }

        static void EnsureClassInitializedForFields(il2cppClass* klass) noexcept {
            if (!NeedsFieldMaterialization(klass)) return;

            static void* initFn = reinterpret_cast<void*>(
                ResolveCall("il2cpp_runtime_class_init"));
            if (!initFn) return;

            TryRuntimeClassInit(initFn, klass);
        }

    }

    void InitLayoutOffsets() noexcept {
        auto* e = g_conn.exports;
        if (!e) return;
        auto& o = g_layoutOffsets;
        if (e->m_offDelegateMethodPtr  >= 0) o.delegateMethodPtr  = e->m_offDelegateMethodPtr;
        if (e->m_offDelegateInvokeImpl >= 0) o.delegateInvokeImpl = e->m_offDelegateInvokeImpl;
        if (e->m_offDelegateTarget     >= 0) o.delegateTarget     = e->m_offDelegateTarget;
        if (e->m_offDelegateMethodInfo >= 0) o.delegateMethodInfo = e->m_offDelegateMethodInfo;
        if (e->m_offArrayMaxLength     >= 0) o.arrayMaxLength     = e->m_offArrayMaxLength;
        if (e->m_offArrayData          >= 0) o.arrayData          = e->m_offArrayData;
        if (e->m_offObjectCachedPtr    >= 0) o.objectCachedPtr    = e->m_offObjectCachedPtr;
    }

    bool Connect() {
        if (g_conn.connected.exchange(true, std::memory_order_acq_rel))
            return true;

        g_conn.exports = SharedMemory::Resolve<IL2CPP::il2cpp_exports>("IL2CPP.Exports");
        uint32_t version = 0;
        if (g_conn.exports) {
            auto& versionStorage = const_cast<uint32_t&>(g_conn.exports->m_uVersion);
            version = std::atomic_ref<uint32_t>(versionStorage).load(std::memory_order_acquire);
        }
        if (!g_conn.exports || version != exports_version || g_conn.exports->m_uSize != exports_size) {
            g_conn.exports = nullptr;
            g_conn.connected.store(false, std::memory_order_release);
            return false;
        }

        IL2CPP::g_structOffsets = g_conn.exports;
        InitLayoutOffsets();

        g_conn.unity = SharedMemory::Resolve<unity_functions>("IL2CPP.Unity");
        if (g_conn.unity && g_conn.unity->m_uVersion != unity_version) {
            g_conn.unity = nullptr;
        }

        return true;
    }

    bool Disconnect() {
        if (!g_conn.connected.exchange(false, std::memory_order_acq_rel))
            return true;
        if (!Unity::AsyncOperation::CancelAllSubscriptions()) {
            g_conn.connected.store(true, std::memory_order_release);
            return false;
        }
        g_conn.unity = nullptr;
        g_conn.exports = nullptr;
        IL2CPP::g_structOffsets = nullptr;   // keep GetExports() (inline, reads this) consistent
        return true;
    }

    [[nodiscard]] bool IsConnected() noexcept {
        return g_conn.connected.load(std::memory_order_acquire) && g_conn.exports;
    }

    // GetExports() is now defined inline in ManagedObject.hpp (returns IL2CPP::g_structOffsets).

    void* GetDomain() {
        if (!E() || !E()->m_domainGet) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)()>(E()->m_domainGet)();
    }

    il2cppAssembly** GetAssemblies(size_t* count) {
        if (!E() || !E()->m_domainGetAssemblies) return nullptr;
        return reinterpret_cast<il2cppAssembly**(IL2CPP_CALLTYPE)(void*, size_t*)>(
            E()->m_domainGetAssemblies)(GetDomain(), count);
    }

    void* ThreadAttach(void* domain) {
        if (!E() || !E()->m_threadAttach) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*)>(E()->m_threadAttach)(domain);
    }

    void ThreadDetach(void* thread) {
        if (!E() || !E()->m_threadDetach) return;
        reinterpret_cast<void(IL2CPP_CALLTYPE)(void*)>(E()->m_threadDetach)(thread);
    }

    void* ThreadCurrent() {
        if (!E() || !E()->m_threadCurrent) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)()>(E()->m_threadCurrent)();
    }

    ThreadAttachment EnsureThreadAttached() {
        if (!E() || !E()->m_threadCurrent) return {};
        if (void* current = ThreadCurrent()) return { current, 0 };
        void* domain = GetDomain();
        if (!domain) return {};
        void* attached = ThreadAttach(domain);
        return { attached, attached ? uint8_t{1} : uint8_t{0} };
    }

    void ReleaseThreadAttachment(ThreadAttachment attachment) {
        if (attachment.owned != 0 && attachment.thread)
            ThreadDetach(attachment.thread);
    }

    uint32_t GCHandleNew(il2cppObject* object, bool pinned) {
        if (!E() || !E()->m_gcHandleNew || !object) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*, uint8_t)>(
            E()->m_gcHandleNew)(object, pinned ? uint8_t{1} : uint8_t{0});
    }

    il2cppObject* GCHandleGetTarget(uint32_t handle) {
        if (!E() || !E()->m_gcHandleGetTarget || handle == 0) return nullptr;
        return reinterpret_cast<il2cppObject*(IL2CPP_CALLTYPE)(uint32_t)>(
            E()->m_gcHandleGetTarget)(handle);
    }

    void GCHandleFree(uint32_t handle) {
        if (!E() || !E()->m_gcHandleFree || handle == 0) return;
        reinterpret_cast<void(IL2CPP_CALLTYPE)(uint32_t)>(E()->m_gcHandleFree)(handle);
    }

    il2cppClass* FindClass(std::string_view fullName) {
        if (!E() || !E()->m_helperFindClass) return nullptr;
        return static_cast<il2cppClass*>(
            reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(E()->m_helperFindClass)(
                std::string(fullName).c_str()));
    }

    il2cppClass* GetClassFromName(il2cppImage* image, const char* ns, const char* name) {
        if (!E() || !E()->m_classFromName) return nullptr;
        return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*, const char*, const char*)>(
            E()->m_classFromName)(image, ns, name);
    }

    il2cppType* GetType(il2cppClass* klass) {
        if (!E() || !E()->m_classGetType || !klass) return nullptr;
        return reinterpret_cast<il2cppType*(IL2CPP_CALLTYPE)(void*)>(E()->m_classGetType)(klass);
    }

    il2cppClass* ClassFromType(il2cppType* type) {
        if (!E() || !E()->m_classFromIl2cppType || !type) return nullptr;
        auto addr = reinterpret_cast<uintptr_t>(type);
        if (addr < 0x10000 || addr > 0x7FFFFFFFFFFF) return nullptr;
        return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*)>(E()->m_classFromIl2cppType)(type);
    }

    il2cppClass* ClassFromSystemType(il2cppSystemType* type) {
        if (!E() || !E()->m_classFromSystemType || !type) return nullptr;
        __try {
            return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*)>(
                E()->m_classFromSystemType)(type);
        } __except(1) {
            return nullptr;
        }
    }

    il2cppClass* GetParent(il2cppClass* klass) {
        if (!E() || !E()->m_classGetParent || !klass) return nullptr;
        return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*)>(E()->m_classGetParent)(klass);
    }

    il2cppSystemType* GetSystemType(il2cppClass* klass) {
        if (!E() || !E()->m_typeGetObject || !E()->m_classGetType || !klass) return nullptr;
        auto* type = reinterpret_cast<il2cppType*(IL2CPP_CALLTYPE)(void*)>(E()->m_classGetType)(klass);
        if (!type) return nullptr;
        return reinterpret_cast<il2cppSystemType*(IL2CPP_CALLTYPE)(void*)>(E()->m_typeGetObject)(type);
    }

    bool IsAssignableFrom(il2cppClass* target, il2cppClass* candidate) {
        if (!E() || !E()->m_classIsAssignableFrom || !target || !candidate) return false;
        return reinterpret_cast<uint8_t(IL2CPP_CALLTYPE)(void*, void*)>(
            E()->m_classIsAssignableFrom)(target, candidate) != 0;
    }

    il2cppClass* GetObjectClass(il2cppObject* object) {
        if (!E() || !E()->m_objectGetClass || !object) return nullptr;
        return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_objectGetClass)(object);
    }

    il2cppMethodInfo* GetVirtualMethod(il2cppObject* object, il2cppMethodInfo* method) {
        if (!E() || !E()->m_objectGetVirtualMethod || !object || !method) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*(IL2CPP_CALLTYPE)(void*, void*)>(
            E()->m_objectGetVirtualMethod)(object, method);
    }

    il2cppClass* GetClassOrElementClass(il2cppType* type) {
        if (!E() || !E()->m_typeGetClassOrElementClass || !type) return nullptr;
        return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_typeGetClassOrElementClass)(type);
    }

    il2cppClass* GetInterfaces(il2cppClass* klass, void** iter) {
        if (!E() || !E()->m_classGetInterfaces || !klass || !iter) return nullptr;
        __try {
            return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*, void**)>(
                E()->m_classGetInterfaces)(klass, iter);
        } __except(1) {
            return nullptr;
        }
    }

    int32_t GetClassValueSize(il2cppClass* klass, uint32_t* alignment) {
        if (alignment) *alignment = 0;
        if (!E() || !E()->m_classValueSize || !klass) return -1;
        return reinterpret_cast<int32_t(IL2CPP_CALLTYPE)(void*, uint32_t*)>(
            E()->m_classValueSize)(klass, alignment);
    }

    bool ClassHasReferences(il2cppClass* klass) {
        if (!E() || !E()->m_classHasReferences || !klass) return true;
        return reinterpret_cast<uint8_t(IL2CPP_CALLTYPE)(void*)>(
            E()->m_classHasReferences)(klass) != 0;
    }

    il2cppFieldInfo* GetFields(il2cppClass* klass, void** iter) {
        if (!E() || !E()->m_classGetFields || !klass) return nullptr;
        EnsureClassInitializedForFields(klass);
        return reinterpret_cast<il2cppFieldInfo*(IL2CPP_CALLTYPE)(void*, void**)>(
            E()->m_classGetFields)(klass, iter);
    }

    il2cppFieldInfo* GetFieldByName(il2cppClass* klass, const char* name) {
        if (!E() || !E()->m_classGetFieldFromName || !klass || !name) return nullptr;
        return reinterpret_cast<il2cppFieldInfo*(IL2CPP_CALLTYPE)(void*, const char*)>(
            E()->m_classGetFieldFromName)(klass, name);
    }

    int GetFieldOffset(il2cppClass* klass, const char* fieldName) {
        auto* field = GetFieldByName(klass, fieldName);
        if (!field) return -1;
        // Stride-0x30 (1832): offset@+0x20. Stride-0x20 (Unity 2022 canonical): offset@+0x18.
        // Default to 0x20 to match Reflection.cpp Field::offset() fallback.
        int32_t off = (E() && E()->m_offFieldOffset >= 0) ? E()->m_offFieldOffset : 0x20;
        return *reinterpret_cast<int32_t*>(reinterpret_cast<char*>(field) + off);
    }

    void GetStaticFieldValue(il2cppFieldInfo* field, void* outValue) {
        if (!E() || !E()->m_fieldStaticGetValue || !field || !outValue) return;
        __try {
            reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void*)>(E()->m_fieldStaticGetValue)(field, outValue);
        } __except(1) { }
    }

    void SetStaticFieldValue(il2cppFieldInfo* field, void* value) {
        if (!E() || !E()->m_fieldStaticSetValue || !field || !value) return;
        reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void*)>(E()->m_fieldStaticSetValue)(field, value);
    }

    void GetFieldValue(il2cppObject* obj, il2cppFieldInfo* field, void* outValue) {
        if (!E() || !E()->m_fieldGetValue || !obj || !field || !outValue) return;
        reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void*, void*)>(E()->m_fieldGetValue)(obj, field, outValue);
    }

    void SetFieldValue(il2cppObject* obj, il2cppFieldInfo* field, void* value) {
        if (!E() || !E()->m_fieldSetValue || !obj || !field || !value) return;
        reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void*, void*)>(E()->m_fieldSetValue)(obj, field, value);
    }

    uint32_t GetFieldFlags(il2cppFieldInfo* field) {
        if (!E() || !E()->m_fieldGetFlags || !field) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*)>(E()->m_fieldGetFlags)(field);
    }

    uint32_t GetFieldToken(il2cppFieldInfo* field) {
        if (!E() || !E()->m_fieldGetToken || !field) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*)>(E()->m_fieldGetToken)(field);
    }

    il2cppMethodInfo* GetMethods(il2cppClass* klass, void** iter) {
        if (!E() || !E()->m_classGetMethods || !klass) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*(IL2CPP_CALLTYPE)(void*, void**)>(
            E()->m_classGetMethods)(klass, iter);
    }

    il2cppMethodInfo* GetMethodByName(il2cppClass* klass, const char* name, int argc) {
        if (!E() || !E()->m_classGetMethodFromName || !klass || !name) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*(IL2CPP_CALLTYPE)(void*, const char*, int)>(
            E()->m_classGetMethodFromName)(klass, name, argc);
    }

    void* GetMethodPointer(il2cppClass* klass, const char* name, int argc) {
        auto* method = GetMethodByName(klass, name, argc);
        if (!method) return nullptr;
        return *reinterpret_cast<void**>(method);
    }

    il2cppType* GetMethodParamType(il2cppMethodInfo* method, uint32_t index) {
        if (!E() || !E()->m_methodGetParam || !method) return nullptr;
        return reinterpret_cast<il2cppType*(IL2CPP_CALLTYPE)(void*, uint32_t)>(
            E()->m_methodGetParam)(method, index);
    }

    const char* GetMethodParamName(il2cppMethodInfo* method, uint32_t index) {
        if (!E() || !E()->m_methodGetParamName || !method) return nullptr;
        const char* name = reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*, uint32_t)>(
            E()->m_methodGetParamName)(method, index);
        if (name && *name) {
            for (const unsigned char* p = reinterpret_cast<const unsigned char*>(name); *p; ++p) {
                if (*p > 0x7E || *p < 0x20) {
                    static thread_local char buf[32];
                    snprintf(buf, sizeof(buf), "arg_%u", index);
                    return buf;
                }
            }
        }
        return name;
    }

    uint32_t GetMethodToken(il2cppMethodInfo* method) {
        if (!E() || !E()->m_methodGetToken || !method) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*)>(E()->m_methodGetToken)(method);
    }

    uint16_t GetMethodSlot(il2cppMethodInfo* method) {
        if (!E() || !E()->m_methodGetSlot || !method) return UINT16_MAX;
        return reinterpret_cast<uint16_t(IL2CPP_CALLTYPE)(void*)>(E()->m_methodGetSlot)(method);
    }

    il2cppObject* GetMethodObject(il2cppMethodInfo* method, il2cppClass* reflectedClass) {
        if (!E() || !E()->m_methodGetObject || !method) return nullptr;
        return reinterpret_cast<il2cppObject*(IL2CPP_CALLTYPE)(void*, void*)>(
            E()->m_methodGetObject)(method, reflectedClass);
    }

    il2cppMethodInfo* GetMethodFromReflection(il2cppObject* reflectionMethod) {
        if (!E() || !E()->m_methodGetFromReflection || !reflectionMethod) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_methodGetFromReflection)(reflectionMethod);
    }

    il2cppPropertyInfo* GetProperties(il2cppClass* klass, void** iter) {
        if (!E() || !E()->m_classGetProperties || !klass) return nullptr;
        return reinterpret_cast<il2cppPropertyInfo*(IL2CPP_CALLTYPE)(void*, void**)>(
            E()->m_classGetProperties)(klass, iter);
    }

    il2cppPropertyInfo* GetPropertyByName(il2cppClass* klass, const char* name) {
        if (!E() || !klass || !name) return nullptr;
        return FindPropertyByIteration(klass, name);
    }

    uint32_t GetPropertyFlags(il2cppPropertyInfo* property) {
        if (!E() || !E()->m_propertyGetFlags || !property) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*)>(E()->m_propertyGetFlags)(property);
    }

    il2cppMethodInfo* GetPropertyGetter(il2cppPropertyInfo* property) {
        if (!E() || !E()->m_propertyGetGetMethod || !property) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_propertyGetGetMethod)(property);
    }

    il2cppMethodInfo* GetPropertySetter(il2cppPropertyInfo* property) {
        if (!E() || !E()->m_propertyGetSetMethod || !property) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_propertyGetSetMethod)(property);
    }

    const char* GetPropertyName(il2cppPropertyInfo* property) {
        if (!E() || !E()->m_propertyGetName || !property) return nullptr;
        return reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_propertyGetName)(property);
    }

    il2cppClass* GetPropertyParent(il2cppPropertyInfo* property) {
        if (!E() || !E()->m_propertyGetParent || !property) return nullptr;
        return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_propertyGetParent)(property);
    }

    void* GetEvents(il2cppClass* klass, void** iter) {
        if (!E() || !E()->m_classGetEvents || !klass || !iter) return nullptr;
        __try {
            return reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, void**)>(
                E()->m_classGetEvents)(klass, iter);
        } __except(1) {
            return nullptr;
        }
    }

    const char* GetEventName(void* eventInfo) {
        if (!E() || !E()->m_eventGetName || !eventInfo) return nullptr;
        return reinterpret_cast<const char*(IL2CPP_CALLTYPE)(void*)>(E()->m_eventGetName)(eventInfo);
    }

    il2cppMethodInfo* GetEventAddMethod(void* eventInfo) {
        if (!E() || !E()->m_eventGetAddMethod || !eventInfo) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_eventGetAddMethod)(eventInfo);
    }

    il2cppMethodInfo* GetEventRemoveMethod(void* eventInfo) {
        if (!E() || !E()->m_eventGetRemoveMethod || !eventInfo) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_eventGetRemoveMethod)(eventInfo);
    }

    il2cppMethodInfo* GetEventRaiseMethod(void* eventInfo) {
        if (!E() || !E()->m_eventGetRaiseMethod || !eventInfo) return nullptr;
        return reinterpret_cast<il2cppMethodInfo*(IL2CPP_CALLTYPE)(void*)>(
            E()->m_eventGetRaiseMethod)(eventInfo);
    }

    il2cppClass* GetEventParent(void* eventInfo) {
        if (!E() || !E()->m_eventGetParent || !eventInfo) return nullptr;
        return reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*)>(E()->m_eventGetParent)(eventInfo);
    }

    uint32_t GetEventToken(void* eventInfo) {
        if (!E() || !E()->m_eventGetToken || !eventInfo) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*)>(E()->m_eventGetToken)(eventInfo);
    }

    il2cppObject* NewObject(il2cppClass* klass) {
        if (!E() || !E()->m_objectNew || !klass) return nullptr;
        return reinterpret_cast<il2cppObject*(IL2CPP_CALLTYPE)(void*)>(E()->m_objectNew)(klass);
    }

    il2cppObject* Box(il2cppClass* klass, void* data) {
        if (!E() || !E()->m_valueBox || !klass || !data) return nullptr;
        return reinterpret_cast<il2cppObject*(IL2CPP_CALLTYPE)(void*, void*)>(E()->m_valueBox)(klass, data);
    }

    il2cppObject* RuntimeInvoke(il2cppMethodInfo* method, void* obj, void** params, void** exc) {
        if (!E() || !E()->m_runtimeInvoke || !method) return nullptr;
        return reinterpret_cast<il2cppObject*(IL2CPP_CALLTYPE)(void*, void*, void**, void**)>(
            E()->m_runtimeInvoke)(method, obj, params, exc);
    }

    void* StringNew(const char* str) {
        if (!E() || !E()->m_stringNew || !str) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(E()->m_stringNew)(str);
    }

    uint64_t GetArrayLength(void* array) {
        if (!E() || !E()->m_arrayLength || !array) return 0;
        return reinterpret_cast<uint64_t(IL2CPP_CALLTYPE)(void*)>(E()->m_arrayLength)(array);
    }

    void* NewArrayFull(il2cppClass* arrayClass, const uint64_t* lengths, const int64_t* lowerBounds) {
        if (!E() || !E()->m_arrayNewFull || !arrayClass || !lengths) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, const uint64_t*, const int64_t*)>(
            E()->m_arrayNewFull)(arrayClass, lengths, lowerBounds);
    }

    uint32_t GetArrayElementSize(il2cppClass* arrayClass) {
        if (!E() || !E()->m_arrayElementSize || !arrayClass) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*)>(E()->m_arrayElementSize)(arrayClass);
    }

    bool GetArrayBounds(void* array, uint32_t dimension, uint64_t* length, int64_t* lowerBound) {
        if (length) *length = 0;
        if (lowerBound) *lowerBound = 0;
        if (!E() || !E()->m_helperArrayGetBounds || !array || !length || !lowerBound) return false;
        return reinterpret_cast<uint8_t(IL2CPP_CALLTYPE)(void*, uint32_t, uint64_t*, int64_t*)>(
            E()->m_helperArrayGetBounds)(array, dimension, length, lowerBound) != 0;
    }

    bool SetReferenceWithWriteBarrier(il2cppObject* object, void** targetAddress, void* value) {
        if (!E() || !E()->m_gcWBarrierSetField || !targetAddress) return false;
        reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void**, void*)>(
            E()->m_gcWBarrierSetField)(object, targetAddress, value);
        return true;
    }

    uint32_t GetTypeAttributes(il2cppType* type) {
        if (!E() || !E()->m_typeGetAttrs || !type) return 0;
        return reinterpret_cast<uint32_t(IL2CPP_CALLTYPE)(void*)>(E()->m_typeGetAttrs)(type);
    }

    bool TypesEqual(il2cppType* lhs, il2cppType* rhs) {
        if (!E() || !E()->m_typeEquals || !lhs || !rhs) return false;
        return reinterpret_cast<uint8_t(IL2CPP_CALLTYPE)(void*, void*)>(E()->m_typeEquals)(lhs, rhs) != 0;
    }

    void FormatException(il2cppObject* exception, char* buffer, uint32_t bufferSize) {
        if (!E() || !E()->m_formatException || !exception || !buffer || bufferSize == 0 || bufferSize > static_cast<uint32_t>(INT32_MAX)) return;
        buffer[bufferSize - 1] = '\0';
        reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, char*, int32_t)>(
            E()->m_formatException)(exception, buffer, static_cast<int32_t>(bufferSize));
        buffer[bufferSize - 1] = '\0';
    }

    void FormatStackTrace(il2cppObject* exception, char* buffer, uint32_t bufferSize) {
        if (!E() || !E()->m_formatStackTrace || !exception || !buffer || bufferSize == 0) return;
        reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, char*, int32_t)>(
            E()->m_formatStackTrace)(exception, buffer, static_cast<int32_t>(bufferSize));
    }

    void* ResolveCall(std::string_view fullPath, bool isExtern) {
        if (!E() || !E()->m_helperResolveCall) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*, bool)>(
            E()->m_helperResolveCall)(std::string(fullPath).c_str(), isExtern);
    }

    void* ResolveExport(std::string_view name) {
        if (!E() || !E()->m_helperResolveExport || name.empty()) return nullptr;
        return reinterpret_cast<void*(IL2CPP_CALLTYPE)(const char*)>(
            E()->m_helperResolveExport)(std::string(name).c_str());
    }

    const char* GetStableName(const char* obfuscatedName) {
        if (!E() || !E()->m_helperGetStableName || !obfuscatedName) return obfuscatedName;
        return reinterpret_cast<const char*(IL2CPP_CALLTYPE)(const char*)>(
            E()->m_helperGetStableName)(obfuscatedName);
    }

    const char* GetOriginalName(const char* stableName) {
        if (!E() || !E()->m_helperGetOriginalName || !stableName) return stableName;
        return reinterpret_cast<const char*(IL2CPP_CALLTYPE)(const char*)>(
            E()->m_helperGetOriginalName)(stableName);
    }

    size_t FetchFields(il2cppClass* klass, std::vector<il2cppFieldInfo*>& out) {
        out.clear();
        if (!klass) return 0;

        void* iter = nullptr;
        while (auto* field = GetFields(klass, &iter))
            out.emplace_back(field);
        return out.size();
    }

    size_t FetchMethods(il2cppClass* klass, std::vector<il2cppMethodInfo*>& out) {
        out.clear();
        if (!klass) return 0;

        void* iter = nullptr;
        while (auto* method = GetMethods(klass, &iter))
            out.emplace_back(method);
        return out.size();
    }

    size_t FetchClasses(std::vector<il2cppClass*>& out, std::string_view assemblyName) {
        out.clear();
        if (!E() || !E()->m_domainGetAssemblies || !E()->m_imageGetClassCount || !E()->m_imageGetClass)
            return 0;

        size_t assemblyCount = 0;
        auto** assemblies = GetAssemblies(&assemblyCount);
        if (!assemblies || assemblyCount == 0) return 0;

        for (size_t a = 0; a < assemblyCount; ++a) {
            auto* assembly = assemblies[a];
            if (!assembly) continue;

            auto* image = reinterpret_cast<il2cppImage*(IL2CPP_CALLTYPE)(void*)>(
                E()->m_assemblyGetImage)(assembly);
            if (!image) continue;

            bool nameMatch = false;
            auto* imgBytes = reinterpret_cast<const char*>(image);
            for (int i = 0; i < 56; ++i) {
                if (imgBytes[i] >= 'A' && imgBytes[i] <= 'z') {
                    std::string_view candidate(imgBytes + i);
                    if (candidate.find(assemblyName) != std::string_view::npos) {
                        nameMatch = true;
                        break;
                    }
                    break;
                }
            }
            if (!nameMatch) continue;

            size_t classCount = reinterpret_cast<size_t(IL2CPP_CALLTYPE)(void*)>(
                E()->m_imageGetClassCount)(image);

            for (size_t c = 0; c < classCount; ++c) {
                auto* klass = reinterpret_cast<il2cppClass*(IL2CPP_CALLTYPE)(void*, size_t)>(
                    E()->m_imageGetClass)(image, c);
                if (klass) out.emplace_back(klass);
            }
            break;
        }

        return out.size();
    }

    unity_functions const* GetUnityFunctions() noexcept {
        return g_conn.unity;
    }

    bool IsUnityConnected() noexcept {
        return g_conn.unity != nullptr;
    }

} // namespace IL2CPP::Module
