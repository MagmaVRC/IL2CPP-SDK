#pragma once
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <IL2CPP.Common/il2cpp_shared.hpp>

namespace IL2CPP {

	[[nodiscard]] __forceinline bool IsValid(const void* ptr) noexcept
	{
		if (!ptr) return false;
		constexpr uintptr_t minAddr = 0x10000;
		constexpr uintptr_t maxAddr = 0x7FFFFFFFFFFF;
		uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
		return addr >= minAddr && addr <= maxAddr;
	}

	[[nodiscard]] __forceinline void* Unbox(void* obj) noexcept
	{
		return static_cast<char*>(obj) + 0x10;
	}

	template<typename>
	struct is_const_member_fn : std::false_type {};

	template<typename R, typename C, typename... Args>
	struct is_const_member_fn<R(C::*)(Args...) const> : std::true_type {};

	struct il2cppFieldInfo;
	struct il2cppMethodInfo;
	struct il2cppClass;
	
	struct il2cppImage
	{
		const char* m_pName;
		const char* m_pNameNoExt;
	};

	struct il2cppAssemblyName
	{
		const char* m_pName;
		const char* m_pCulture;
		const char* m_pHash;
		const char* m_pPublicKey;
		unsigned int m_uHash;
		int m_iHashLength;
		unsigned int m_uFlags;
		int m_iMajor;
		int m_iMinor;
		int m_iBuild;
		int m_iRevision;
		unsigned char m_uPublicKeyToken[8];
		uint8_t _pad[4] = {};
	};

	struct il2cppAssembly
	{
		uint8_t _opaque[0x80] = {};

		/// Returns the assembly's image using the runtime-discovered offset.
		il2cppImage* get_image() const;
	};

#ifdef UNITY_VERSION_2022_3_8_HIGHER_
	struct il2cppType
	{
		void* data;
		unsigned int bits;
	};
#else
	struct il2cppType
	{
		union
		{
			void* m_pDummy;
			unsigned int m_uClassIndex;
			il2cppType* m_pType;
			void* m_pArray;
			unsigned int m_uGenericParameterIndex;
			void* m_pGenericClass;
		};
		unsigned int m_uAttributes : 16;
		unsigned int m_uType : 8;
		unsigned int m_uMods : 6;
		unsigned int m_uByref : 1;
		unsigned int m_uPinned : 1;
		uint32_t _pad = 0;
	};
#endif

	struct il2cppVirtualInvokeData
	{
		void* m_pMethodPointer;
		const il2cppMethodInfo* m_pMethod;
	};

	union il2cppRGCTXData
	{
		void* m_pDummy;
		const il2cppMethodInfo* m_pMethod;
		const il2cppType* m_pType;
		il2cppClass* m_pKlass;
	};

	struct il2cppRuntimeInterfaceOffsetPair
	{
		il2cppClass* m_pInterfaceType;
		int32_t m_iOffset;
		uint32_t _pad = 0;
	};

	/// Field offsets are shuffled between builds.
	/// Use safe_name()/safe_namespace()/safe_parent() instead of direct field access.
	struct il2cppClass
	{
		void* m_pImage;
		void* m_pGC;
		const char* m_pName;
		const char* m_pNamespace;
		il2cppType m_byvalArg;
		il2cppType m_thisArg;
		il2cppClass* m_pElementClass;
		il2cppClass* m_pCastClass;
		il2cppClass* m_pDeclareClass;
		il2cppClass* m_pParentClass;
		void* m_pGenericClass;
		void* m_pTypeDefinition;
		void* m_pInteropData;
		il2cppClass* m_pKlass;
		void* m_pFields;
		void* m_pEvents;
		void* m_pProperties;
		void* m_pMethods;
		il2cppClass** m_pNestedTypes;
		il2cppClass** m_ImplementedInterfaces;
		il2cppRuntimeInterfaceOffsetPair* m_pInterfaceOffsets;
		void* m_pStaticFields;
		il2cppRGCTXData* m_pRGCTX;
		il2cppClass** m_pTypeHierarchy;
		void* m_pUnityUserData;
		uint32_t m_uInitExceptionGCHandle;
		uint32_t m_uCCtorStarted;
		uint32_t m_uCCtorFinished;
		uint32_t _pad1 = 0;
		size_t m_uCCtorThread;
		void* m_pGenericContainerHandle;
		uint32_t m_uInstanceSize;
		uint32_t m_uActualSize;
		uint32_t m_uElementSize;
		int32_t m_iNativeSize;
		uint32_t m_uStaticFieldsSize;
		uint32_t m_uThreadStaticFieldsSize;
		int32_t m_iThreadStaticFieldDataOffset;
		uint32_t m_uFlags;
		uint32_t m_uToken;
		uint16_t m_uMethodCount;
		uint16_t m_uPropertyCount;
		uint16_t m_uFieldCount;
		uint16_t m_uEventCount;
		uint16_t m_uNestedTypesCount;
		uint16_t m_uVTableCount;
		uint16_t m_uInterfacesCount;
		uint16_t m_uInterfaceOffsetsCount;
		uint8_t m_uTypeHierarchyDepth;
		uint8_t m_uGenericRecursionDepth;
		uint8_t m_uRank;
		uint8_t m_uMinAlignment;
		uint8_t m_uNaturalAlignment;
		uint8_t m_uPackingSize;
		uint8_t m_bInitialized : 1;
		uint8_t m_bEnumType : 1;
		uint8_t m_bNullable : 1;
		uint8_t m_bMarshaledFromNative : 1;
		uint8_t m_bSerializable : 1;
		uint8_t m_bBlittable : 1;
		uint8_t m_bIsImportOrWindow : 1;
		uint8_t m_bCCtor : 1;
		uint8_t m_bValueType : 1;
		uint8_t m_bIsAbstract : 1;
		uint8_t m_bIsInterface : 1;
		uint8_t m_bIsSealed : 1;
		uint8_t _pad2[4] = {};
		il2cppVirtualInvokeData m_vtable[255];

		[[nodiscard]] const char* safe_name() const noexcept;
		[[nodiscard]] const char* safe_namespace() const noexcept;
		[[nodiscard]] il2cppClass* safe_parent() const noexcept;
	};

	struct il2cppSystemType {
		il2cppClass* m_pClass = nullptr;
		void* m_pMonitor = nullptr;
	};

	struct il2cpp_exports;

	/// Runtime-discovered struct offsets, set by IL2CPP.Core during init.
	inline const il2cpp_exports* g_structOffsets = nullptr;

	/// Opaque struct — field layout is shuffled between builds.
	struct il2cppFieldInfo
	{
		uint8_t _opaque[0x20];

		[[nodiscard]] const char*  name() const noexcept;
		[[nodiscard]] il2cppClass* parent() const noexcept;
		[[nodiscard]] il2cppType*  type() const noexcept;
		[[nodiscard]] int          offset() const noexcept;
		[[nodiscard]] unsigned int token() const noexcept;

		__declspec(property(get=name))    const char*  m_pName;
		__declspec(property(get=parent))  il2cppClass* m_pParent;
		__declspec(property(get=type))    il2cppType*  m_pType;
		__declspec(property(get=offset))  int          m_iOffset;
		__declspec(property(get=token))   unsigned int m_uToken;
	};

	struct il2cppParameterInfo
	{
		const char* m_pName;
		int m_iPosition;
		unsigned int m_uToken;
		il2cppType* m_pParameterType;
	};

	/// Opaque struct — method field layout is shuffled between builds.
	struct il2cppMethodInfo
	{
		uint8_t _opaque[0x58];

		[[nodiscard]] void*        code_pointer() const noexcept;
		[[nodiscard]] void*        virtual_pointer() const noexcept;
		[[nodiscard]] const char*  name() const noexcept;
		[[nodiscard]] il2cppType*  return_type() const noexcept;
		[[nodiscard]] il2cppClass* declaring_type() const noexcept;
		[[nodiscard]] uint8_t      arg_count() const noexcept;

		__declspec(property(get=code_pointer))     void*        m_pMethodPointer;
		__declspec(property(get=virtual_pointer))   void*        m_pVirtualMethodPointer;
		__declspec(property(get=name))              const char*  m_pName;
		__declspec(property(get=return_type))       il2cppType*  m_pReturnType;
		__declspec(property(get=declaring_type))    il2cppClass* m_pDeclaringType;
		__declspec(property(get=arg_count))         uint8_t      m_uArgCount;
	};

	/// Opaque struct — property layout is shuffled between builds.
	struct il2cppPropertyInfo
	{
		uint8_t _opaque[0x30];

		[[nodiscard]] const char* get_name() const noexcept;
		[[nodiscard]] il2cppMethodInfo* get_getter() const noexcept;
		[[nodiscard]] il2cppMethodInfo* get_setter() const noexcept;

		__declspec(property(get=get_getter)) il2cppMethodInfo* m_pGet;
		__declspec(property(get=get_setter)) il2cppMethodInfo* m_pSet;
		__declspec(property(get=get_name))   const char* m_pName;
	};

	struct il2cppArrayBounds
	{
		uintptr_t m_uLength;
		int m_iLowerBound;
		uint32_t _pad = 0;
	};

	struct il2cppObject {
		il2cppClass* m_pClass = nullptr;
		void* m_pMonitor = nullptr;

		il2cppFieldInfo* GetFields(void** m_pIterator);
		il2cppMethodInfo* GetMethods(void** m_pIterator);

		il2cppFieldInfo* GetField(std::string_view m_pFieldName);
		il2cppMethodInfo* GetMethod(std::string_view m_pMethodName, int m_iArgs = -1);

		il2cppPropertyInfo* GetProperty(std::string_view m_pPropertyName);

		void* GetPropertyValue(std::string_view m_pPropertyName);
		void SetPropertyValue(std::string_view m_pPropertyName, void* m_tValue);
		void* GetMethodPointer(std::string_view m_pMethodName, int m_iArgs = -1);

		void* CallMethod_Impl(void* m_pMethod);
		void* CallMethod_Impl(void* m_pMethod, void* m_pArg);
		void* CallMethod_Impl(void* m_pMethod, void* m_pArg, void* m_pArg2);

		static __forceinline bool IsAlive(il2cppObject* obj) {
			return obj && obj->m_pClass;
		}

		template<typename T = void*>
		[[nodiscard]] __forceinline T* FieldByOffset(std::uintptr_t offset) {
			return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
		}
		template<typename T = void*>
		[[nodiscard]] __forceinline T _FieldByOffset(std::uintptr_t offset) {
			return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
		}

		template<typename TReturn>
		TReturn CallMethod(void* m_pMethod) {
			return *reinterpret_cast<TReturn*>(CallMethod_Impl(m_pMethod));
		}

		template<typename TReturn, typename... TArgs>
		TReturn CallMethod(void* m_pMethod, TArgs... tArgs) {
			return *reinterpret_cast<TReturn*>(CallMethod_Impl(m_pMethod, tArgs...));
		}
		template<typename TReturn, typename... TArgs>
		TReturn CallMethod(std::string_view m_pMethodName, TArgs... tArgs) {
			void* method = GetMethodPointer(m_pMethodName);
			return CallMethod<TReturn>(method, tArgs...);
		}
		template<typename TReturn, typename... TArgs>
		TReturn CallMethodSafe(void* m_pMethod, TArgs... tArgs) {
			if (m_pMethod == nullptr) [[unlikely]]
				return TReturn{};
			return CallMethod<TReturn>(m_pMethod, tArgs...);
		}
		template<typename TReturn, typename... TArgs>
		TReturn CallMethodSafe(std::string_view m_pMethodName, TArgs... tArgs) {
			return CallMethodSafe<TReturn>(GetMethodPointer(m_pMethodName), tArgs...);
		}

		template<typename T>
		__inline T GetValue(int m_iOffset) {
			return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + m_iOffset);
		}
		template<typename T>
		__inline void SetValue(int m_iOffset, T m_tValue) {
			*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + m_iOffset) = m_tValue;
		}

		template<typename T>
		T GetValue(il2cppFieldInfo* m_pField) {
			return GetValue<T>(m_pField->offset());
		}
		template<typename T>
		void SetValue(il2cppFieldInfo* m_pField, T m_tValue) {
			SetValue<T>(m_pField->offset(), m_tValue);
		}

		template<typename T>
		T GetValue(std::string_view m_pMemberName) {
			il2cppFieldInfo* pField = GetField(m_pMemberName);
			if (pField) {
				int off = pField->offset();
				if (off >= 0)
					return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + off);
			}
			else if (auto prop = GetProperty(m_pMemberName); prop && prop->m_pGet)
				return reinterpret_cast<T>(CallMethod_Impl(prop->m_pGet->m_pMethodPointer, nullptr));
			return T{};
		}
		template<typename T>
		void SetValue(std::string_view m_pMemberName, T m_tValue) {
			il2cppFieldInfo* pField = GetField(m_pMemberName);
			if (pField) {
				int off = pField->offset();
				if (off >= 0) {
					*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + off) = m_tValue;
					return;
				}
			}
			if constexpr (std::is_pointer_v<T>) {
				SetPropertyValue(m_pMemberName, const_cast<void*>(
					static_cast<const void*>(m_tValue)));
			}
			else {
				SetPropertyValue(m_pMemberName, reinterpret_cast<void*>(&m_tValue));
			}
		}
	};

	inline const char* il2cppFieldInfo::name() const noexcept {
		if (g_structOffsets && g_structOffsets->m_fieldGetName) {
			return reinterpret_cast<const char*(__fastcall*)(const void*)>(
				g_structOffsets->m_fieldGetName)(this);
		}
		if (g_structOffsets && g_structOffsets->m_offFieldName >= 0) {
			auto ptr = *reinterpret_cast<const char* const*>(
				reinterpret_cast<const char*>(this) + g_structOffsets->m_offFieldName);
			return (reinterpret_cast<uintptr_t>(ptr) >= 0x10000 &&
					reinterpret_cast<uintptr_t>(ptr) <= 0x7FFFFFFFFFFF) ? ptr : nullptr;
		}
		return nullptr;
	}

	inline il2cppClass* il2cppFieldInfo::parent() const noexcept {
		if (g_structOffsets && g_structOffsets->m_fieldGetParent) {
			return reinterpret_cast<il2cppClass*(__fastcall*)(const void*)>(
				g_structOffsets->m_fieldGetParent)(this);
		}
		if (g_structOffsets && g_structOffsets->m_offFieldParent >= 0) {
			auto ptr = *reinterpret_cast<il2cppClass* const*>(
				reinterpret_cast<const char*>(this) + g_structOffsets->m_offFieldParent);
			return (reinterpret_cast<uintptr_t>(ptr) >= 0x10000 &&
					reinterpret_cast<uintptr_t>(ptr) <= 0x7FFFFFFFFFFF) ? ptr : nullptr;
		}
		return nullptr;
	}

	inline il2cppType* il2cppFieldInfo::type() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offFieldType >= 0)
			? g_structOffsets->m_offFieldType : 0x08;
		return *reinterpret_cast<il2cppType* const*>(
			reinterpret_cast<const char*>(this) + off);
	}

	inline int il2cppFieldInfo::offset() const noexcept {
		int val = -1;
		if (g_structOffsets && g_structOffsets->m_fieldGetOffset) {
			val = static_cast<int>(reinterpret_cast<int64_t(__fastcall*)(const void*)>(
				g_structOffsets->m_fieldGetOffset)(this));
		} else if (g_structOffsets && g_structOffsets->m_offFieldOffset >= 0) {
			val = *reinterpret_cast<const int*>(
				reinterpret_cast<const char*>(this) + g_structOffsets->m_offFieldOffset);
		}
		// Reject unresolved metadata tokens (high byte set) — real offsets are < 0x10000.
		if (val < 0 || val > 0x10000) return -1;
		return val;
	}

	inline unsigned int il2cppFieldInfo::token() const noexcept {
		return *reinterpret_cast<const unsigned int*>(
			reinterpret_cast<const char*>(this) + 0x00);
	}

	inline void* il2cppMethodInfo::code_pointer() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offMethodPointer >= 0)
			? g_structOffsets->m_offMethodPointer : 0x00;
		return *reinterpret_cast<void* const*>(
			reinterpret_cast<const char*>(this) + off);
	}

	inline void* il2cppMethodInfo::virtual_pointer() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offMethodVirtual >= 0)
			? g_structOffsets->m_offMethodVirtual : 0x08;
		return *reinterpret_cast<void* const*>(
			reinterpret_cast<const char*>(this) + off);
	}

	inline const char* il2cppMethodInfo::name() const noexcept {
		if (g_structOffsets && g_structOffsets->m_methodGetName) {
			return reinterpret_cast<const char*(__fastcall*)(const void*)>(
				g_structOffsets->m_methodGetName)(this);
		}
		if (g_structOffsets && g_structOffsets->m_offMethodName >= 0) {
			return *reinterpret_cast<const char* const*>(
				reinterpret_cast<const char*>(this) + g_structOffsets->m_offMethodName);
		}
		return nullptr;
	}

	inline il2cppType* il2cppMethodInfo::return_type() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offMethodRetType >= 0)
			? g_structOffsets->m_offMethodRetType : 0x20;
		return *reinterpret_cast<il2cppType* const*>(
			reinterpret_cast<const char*>(this) + off);
	}

	inline il2cppClass* il2cppMethodInfo::declaring_type() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offMethodDeclType >= 0)
			? g_structOffsets->m_offMethodDeclType : 0x18;
		return *reinterpret_cast<il2cppClass* const*>(
			reinterpret_cast<const char*>(this) + off);
	}

	inline uint8_t il2cppMethodInfo::arg_count() const noexcept {
		if (g_structOffsets && g_structOffsets->m_offMethodArgCount >= 0)
			return *reinterpret_cast<const uint8_t*>(
				reinterpret_cast<const char*>(this) + g_structOffsets->m_offMethodArgCount);
		return 0;
	}

	inline const char* il2cppClass::safe_name() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offClassName >= 0)
			? g_structOffsets->m_offClassName : 0x10;
		auto ptr = *reinterpret_cast<const char* const*>(
			reinterpret_cast<const char*>(this) + off);
		return (reinterpret_cast<uintptr_t>(ptr) >= 0x10000 &&
		        reinterpret_cast<uintptr_t>(ptr) <= 0x7FFFFFFFFFFF) ? ptr : nullptr;
	}

	inline const char* il2cppClass::safe_namespace() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offClassNamespace >= 0)
			? g_structOffsets->m_offClassNamespace : 0x18;
		auto ptr = *reinterpret_cast<const char* const*>(
			reinterpret_cast<const char*>(this) + off);
		return (reinterpret_cast<uintptr_t>(ptr) >= 0x10000 &&
		        reinterpret_cast<uintptr_t>(ptr) <= 0x7FFFFFFFFFFF) ? ptr : nullptr;
	}

	inline il2cppClass* il2cppClass::safe_parent() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offClassParent >= 0)
			? g_structOffsets->m_offClassParent : 0x58;
		auto ptr = *reinterpret_cast<il2cppClass* const*>(
			reinterpret_cast<const char*>(this) + off);
		if (ptr == this) return nullptr;
		return (reinterpret_cast<uintptr_t>(ptr) >= 0x10000 &&
		        reinterpret_cast<uintptr_t>(ptr) <= 0x7FFFFFFFFFFF) ? ptr : nullptr;
	}

	inline const char* il2cppPropertyInfo::get_name() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offPropName >= 0)
			? g_structOffsets->m_offPropName : 0x08;
		return *reinterpret_cast<const char* const*>(
			reinterpret_cast<const char*>(this) + off);
	}

	inline il2cppMethodInfo* il2cppPropertyInfo::get_getter() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offPropGetter >= 0)
			? g_structOffsets->m_offPropGetter : 0x10;
		return *reinterpret_cast<il2cppMethodInfo* const*>(
			reinterpret_cast<const char*>(this) + off);
	}

	inline il2cppMethodInfo* il2cppPropertyInfo::get_setter() const noexcept {
		int32_t off = (g_structOffsets && g_structOffsets->m_offPropSetter >= 0)
			? g_structOffsets->m_offPropSetter : 0x18;
		return *reinterpret_cast<il2cppMethodInfo* const*>(
			reinterpret_cast<const char*>(this) + off);
	}

} // namespace IL2CPP
