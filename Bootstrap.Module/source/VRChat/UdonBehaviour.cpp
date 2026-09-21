#include <VRChat/UdonBehaviour.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>
#include <IL2CPP.Module/include/System/Array.hpp>
#include <IL2CPP.Module/include/System/Delegate.hpp>
#include <mutex>
#include <utility>
#include <IL2CPP.Module/include/ManagedObject.hpp>
#include <IL2CPP.Module/include/Unity/Object.hpp>
#include <IL2CPP.Module/include/il2cpp_module.hpp>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <format>
#include <unordered_map>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::Class;
    using IL2CPP::Module::Field;
    using IL2CPP::Module::ManagedObject;
    using IL2CPP::Module::Method;
    using IL2CPP::Module::MethodHandler;
    using IL2CPP::Module::Type;
    using IL2CPP::Module::System::String;

    namespace {

        enum : int {
            kTypeBoolean = 0x02, kTypeChar = 0x03,
            kTypeI1 = 0x04, kTypeU1 = 0x05, kTypeI2 = 0x06, kTypeU2 = 0x07,
            kTypeI4 = 0x08, kTypeU4 = 0x09, kTypeI8 = 0x0a, kTypeU8 = 0x0b,
            kTypeR4 = 0x0c, kTypeR8 = 0x0d, kTypeString = 0x0e,
            kTypeValueType = 0x11, kTypeClass = 0x12, kTypeArray = 0x14,
            kTypeI = 0x18, kTypeU = 0x19, kTypeObject = 0x1c, kTypeSzArray = 0x1d,
        };

        ManagedObject AutoProperty(ManagedObject obj, const char* name, const char* backingField) {
            if (!obj) return ManagedObject{};
            if (void* v = obj.get_property<void*>(name)) return ManagedObject{ v };
            return ManagedObject{ obj.get_field<void*>(backingField) };
        }

        ManagedObject EntryPointsOf(void* program) {
            return AutoProperty(ManagedObject{ program }, "EntryPoints", "<EntryPoints>k__BackingField");
        }

        ManagedObject SymbolTableOf(void* program) {
            return AutoProperty(ManagedObject{ program }, "SymbolTable", "<SymbolTable>k__BackingField");
        }

        Module::System::Array<void*> SymbolArray(ManagedObject table, const char* field, const char* method) {
            if (!table) return Module::System::Array<void*>{};

            void* raw = table.get_field<void*>(field);
            if (!raw) {
                // ImmutableArray<string> is a one-field struct, so the boxed return is the String[].
                if (void* boxed = table.call_method<void*>(method, nullptr, 0))
                    raw = *reinterpret_cast<void**>(IL2CPP::Unbox(boxed));
            }
            return Module::System::Array<void*>{ raw };
        }

        std::vector<std::string> ReadSymbols(ManagedObject table, const char* field, const char* method) {
            std::vector<std::string> out;
            if (!table) return out;

            Module::System::Array<void*> arr = SymbolArray(table, field, method);
            const uintptr_t n = arr.size();
            out.reserve(static_cast<size_t>(n));
            for (uintptr_t i = 0; i < n; ++i) {
                if (void* s = arr[i]) out.push_back(String(s).to_string());
            }
            return out;
        }

        Method ResolveByReturn(const Class& klass, const char* name, int argc, const char* returnType) {
            if (!klass) return Method{};
            for (const Method& m : klass.get_methods()) {
                if (static_cast<int>(m.param_count()) != argc) continue;
                const char* n = m.name();
                const char* r = m.return_type_name();
                if (n && r && std::strcmp(n, name) == 0 && std::strcmp(r, returnType) == 0) return m;
            }
            return Method{};
        }

        std::string TypeNameOf(void* systemType) {
            if (!systemType) return {};
            Class k = Class::from_system_type(systemType);
            return k ? k.full_name() : std::string{};
        }

        void SetSigned(UdonVariable& var, int64_t value) {
            var.kind = UdonValueKind::Integer;
            var.integer = value;
            var.number = static_cast<double>(value);
            var.display = std::to_string(value);
        }

        void SetUnsigned(UdonVariable& var, uint64_t value) {
            var.kind = UdonValueKind::Unsigned;
            var.integer = static_cast<int64_t>(value);
            var.number = static_cast<double>(value);
            var.display = std::to_string(value);
        }

        void SetFloat(UdonVariable& var, double value) {
            var.kind = UdonValueKind::Float;
            var.integer = static_cast<int64_t>(value);
            var.number = value;
            var.display = std::format("{}", value);
        }

        // Defined below, next to the rest of the collection walking.
        bool CollectionSummary(void* obj, std::string& out);

        // ToString() is never called here: it runs arbitrary game code, allocates a managed
        // string per slot, and throws on a destroyed UnityEngine.Object. The class name and
        // the address are free. StringifyHeapValue is the opt-in for one slot.
        void SetReference(UdonVariable& var, void* obj, bool stringify) {
            if (!obj) {
                var.kind = UdonValueKind::Null;
                var.display = "null";
                return;
            }
            var.kind = UdonValueKind::Object;
            var.boxed = obj;
            if (stringify) {
                var.display = ManagedObject(obj).to_string();
                return;
            }
            // A collection rendered as a pointer is unreadable, and its contents are a
            // handful of direct reads away.
            if (CollectionSummary(obj, var.display)) return;
            Class klass = ManagedObject(obj).get_class();
            std::string name = klass ? klass.full_name() : std::string{};
            if (name.empty()) name = "object";
            var.display = std::format("{} @0x{:X}", name, reinterpret_cast<uintptr_t>(obj));
        }

        /// <summary>Decode the Unity structs an Udon heap is mostly made of, straight out of
        /// the slot. `var.typeName` must already be set.</summary>
        /// <returns>False when the type is not one of them and the caller must fall back.</returns>
        bool DecodeKnownStruct(UdonVariable& var, const char* payload) {
            const std::string& t = var.typeName;
            const float* f = reinterpret_cast<const float*>(payload);
            auto set = [&](std::string text) {
                var.kind = UdonValueKind::Object;
                var.display = std::move(text);
                return true;
            };

            if (t == "UnityEngine.Vector2")
                return set(std::format("({}, {})", f[0], f[1]));
            if (t == "UnityEngine.Vector3")
                return set(std::format("({}, {}, {})", f[0], f[1], f[2]));
            if (t == "UnityEngine.Vector4" || t == "UnityEngine.Quaternion")
                return set(std::format("({}, {}, {}, {})", f[0], f[1], f[2], f[3]));
            if (t == "UnityEngine.Color")
                return set(std::format("RGBA({}, {}, {}, {})", f[0], f[1], f[2], f[3]));
            if (t == "UnityEngine.Color32") {
                const uint8_t* b = reinterpret_cast<const uint8_t*>(payload);
                return set(std::format("RGBA32({}, {}, {}, {})", b[0], b[1], b[2], b[3]));
            }
            return false;
        }

        /// <summary>Decode one stored value: `payload` is the value itself for a value type
        /// and the pointer field for a reference type. `var.typeName` is filled when empty.</summary>
        void DecodeValue(const Type& type, char* payload, UdonVariable& var, bool stringify) {
            if (var.typeName.empty()) var.typeName = type.full_name();

            switch (type.type_enum()) {
            case kTypeBoolean:
                var.kind = UdonValueKind::Bool;
                var.integer = *reinterpret_cast<uint8_t*>(payload) ? 1 : 0;
                var.number = static_cast<double>(var.integer);
                var.display = var.integer ? "true" : "false";
                break;
            case kTypeI1:  SetSigned(var, *reinterpret_cast<int8_t*>(payload)); break;
            case kTypeI2:  SetSigned(var, *reinterpret_cast<int16_t*>(payload)); break;
            case kTypeI4:  SetSigned(var, *reinterpret_cast<int32_t*>(payload)); break;
            case kTypeI8:
            case kTypeI:   SetSigned(var, *reinterpret_cast<int64_t*>(payload)); break;
            case kTypeU1:  SetUnsigned(var, *reinterpret_cast<uint8_t*>(payload)); break;
            case kTypeChar:
            case kTypeU2:  SetUnsigned(var, *reinterpret_cast<uint16_t*>(payload)); break;
            case kTypeU4:  SetUnsigned(var, *reinterpret_cast<uint32_t*>(payload)); break;
            case kTypeU8:
            case kTypeU:   SetUnsigned(var, *reinterpret_cast<uint64_t*>(payload)); break;
            case kTypeR4:  SetFloat(var, *reinterpret_cast<float*>(payload)); break;
            case kTypeR8:  SetFloat(var, *reinterpret_cast<double*>(payload)); break;
            case kTypeString: {
                void* str = *reinterpret_cast<void**>(payload);
                if (!str) { var.kind = UdonValueKind::Null; var.display = "null"; break; }
                var.kind = UdonValueKind::String;
                var.boxed = str;
                var.display = String(str).to_string();
                break;
            }
            case kTypeValueType: {
                // The common Unity structs are plain floats in the slot, so they are read
                // directly. Anything else would need a box plus a managed ToString per
                // slot, which is what made a full heap read unaffordable.
                if (DecodeKnownStruct(var, payload)) break;
                if (!stringify) {
                    var.kind = UdonValueKind::Object;
                    var.display = var.typeName.empty() ? "struct" : var.typeName;
                    break;
                }
                Class klass = type.get_class();
                if (!klass) break;
                ManagedObject boxed = ManagedObject::box_value(klass, payload);
                if (!boxed) break;
                var.kind = UdonValueKind::Object;
                var.boxed = boxed.raw();
                var.display = boxed.to_string();
                break;
            }
            case kTypeClass:
            case kTypeObject:
            case kTypeArray:
            case kTypeSzArray:
                SetReference(var, *reinterpret_cast<void**>(payload), stringify);
                break;
            default:
                break;
            }
        }

        // Every heap slot is a StrongBox<T>: value types sit inline at the Value offset,
        // reference types store a pointer there. The il2cpp type enum tells the two apart.
        void DecodeSlot(void* slot, UdonVariable& var, bool stringify) {
            ManagedObject box{ slot };
            Field value = box.get_field_info("Value");
            if (!value) return;

            Type type = value.type();
            if (!type) return;
            var.typeName = type.full_name();
            if (var.typeName.empty()) {
                if (const char* n = value.type_name()) var.typeName = n;
            }

            const int offset = value.offset();
            if (offset < 0) return;
            DecodeValue(type, static_cast<char*>(slot) + offset, var, stringify);
        }

        std::string_view Trimmed(std::string_view text) {
            while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front()))) text.remove_prefix(1);
            while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back()))) text.remove_suffix(1);
            return text;
        }

        // ── collections in a heap slot ──────────────────────────────────────────
        //
        // A slot holding an array, a List or a Dictionary decodes to a class name and a
        // pointer, which is useless to read and impossible to edit. These walk the storage
        // so the elements come back as values.

        Field FieldEither(const Class& klass, const char* a, const char* b) {
            if (!klass) return Field{};
            Field f = klass.get_field_deep(a);
            return f ? f : klass.get_field_deep(b);
        }

        // A value type's field offsets are quoted as if it were boxed, so reaching a field
        // of a struct stored inline -- an array element, a dictionary entry -- means taking
        // the object header back off.
        int UnboxedOffset(const Field& f) {
            if (!f) return -1;
            const int off = f.offset() - static_cast<int>(sizeof(il2cppObject));
            return off < 0 ? -1 : off;
        }

        int32_t ReadInt32Field(void* obj, const Field& f) {
            if (!obj || !f || f.offset() < 0) return 0;
            return *reinterpret_cast<int32_t*>(static_cast<char*>(obj) + f.offset());
        }

        void* ReadPointerField(void* obj, const Field& f) {
            if (!obj || !f || f.offset() < 0) return nullptr;
            return *reinterpret_cast<void**>(static_cast<char*>(obj) + f.offset());
        }

        /// The window of an il2cpp array: where the elements start, how wide each is, how many.
        struct ArrayStorage {
            char*    base = nullptr;
            uint32_t stride = 0;
            int      length = 0;
            Class    elementClass;
            Type     elementType;
            bool     valid = false;
        };

        ArrayStorage OpenArray(void* array) {
            ArrayStorage s;
            if (!array || !Module::IsValidPointer(array)) return s;
            Class arrayClass = ManagedObject{ array }.get_class();
            if (!arrayClass) return s;
            s.elementClass = arrayClass.get_type().get_class_or_element_class();
            if (!s.elementClass || s.elementClass.raw() == arrayClass.raw()) return s;
            s.elementType = s.elementClass.get_type();
            s.stride = Module::GetArrayElementSize(reinterpret_cast<il2cppClass*>(arrayClass.raw()));
            if (s.stride == 0) return s;
            s.length = static_cast<int>(Module::GetArrayLength(array));
            s.base = reinterpret_cast<char*>(Module::System::Array<uint8_t>{ array }.data());
            s.valid = s.base != nullptr;
            return s;
        }

        bool IsArrayObject(void* obj) {
            if (!obj || !Module::IsValidPointer(obj)) return false;
            Class k = ManagedObject{ obj }.get_class();
            if (!k) return false;
            const int e = k.get_type().type_enum();
            return e == kTypeSzArray || e == kTypeArray;
        }

        UdonShape ShapeOf(void* obj, std::string& typeName) {
            if (!obj || !Module::IsValidPointer(obj)) return UdonShape::None;
            Class k = ManagedObject{ obj }.get_class();
            if (!k) return UdonShape::None;
            typeName = k.full_name();
            if (IsArrayObject(obj)) return UdonShape::Array;
            if (typeName.starts_with("System.Collections.Generic.List`1")) return UdonShape::List;
            if (typeName.starts_with("System.Collections.Generic.Dictionary`2")) return UdonShape::Dictionary;
            return UdonShape::None;
        }

        bool KindIsWritable(UdonValueKind kind) {
            switch (kind) {
                case UdonValueKind::Bool:
                case UdonValueKind::Integer:
                case UdonValueKind::Unsigned:
                case UdonValueKind::Float:
                case UdonValueKind::String:
                case UdonValueKind::Null:
                    return true;
                default:
                    return false;
            }
        }

        // The structs DecodeKnownStruct renders, and the only ones ParseStorage reads back.
        bool IsKnownStructType(const std::string& t) {
            return t == "UnityEngine.Vector2" || t == "UnityEngine.Vector3" ||
                   t == "UnityEngine.Vector4" || t == "UnityEngine.Quaternion" ||
                   t == "UnityEngine.Color"   || t == "UnityEngine.Color32";
        }

        /// <summary>Pull up to `max` numbers out of text, ignoring any label, parentheses and
        /// separators around them, so "(1, 2, 3)" and "1 2 3" both read the same.</summary>
        int ParseFloats(const char* text, float* out, int max) {
            int n = 0;
            const char* p = text;
            while (*p && n < max) {
                while (*p && *p != '-' && *p != '+' && *p != '.' &&
                       !(*p >= '0' && *p <= '9')) ++p;
                if (!*p) break;
                char* stop = nullptr;
                const double v = std::strtod(p, &stop);
                if (stop == p) break;
                out[n++] = static_cast<float>(v);
                p = stop;
            }
            return n;
        }

        UdonElement DecodeElement(const Type& type, char* payload, int index) {
            UdonElement el;
            el.index = index;
            UdonVariable var;
            DecodeValue(type, payload, var, false);
            el.display = var.display.empty() ? "(unreadable)" : std::move(var.display);
            el.typeName = std::move(var.typeName);
            el.kind = var.kind;
            // A null reference is writable only when its declared type has a text form.
            el.writable = (KindIsWritable(var.kind) &&
                           (var.kind != UdonValueKind::Null || el.typeName == "System.String")) ||
                          IsKnownStructType(el.typeName);
            return el;
        }

        /// <summary>A short rendering of an array, List or Dictionary: shape, count and a few
        /// elements. Direct reads only, so it is affordable during a whole-heap sweep.</summary>
        /// <returns>False when the object is not one of the three.</returns>
        bool CollectionSummary(void* obj, std::string& out) {
            // An element can be a collection of its own; one level of nesting is enough
            // for a summary and keeps a jagged array from walking itself.
            static thread_local int depth = 0;
            if (depth > 1) return false;

            std::string typeName;
            const UdonShape shape = ShapeOf(obj, typeName);
            if (shape == UdonShape::None) return false;

            auto shortName = [](std::string full) {
                const size_t tick = full.find('`');
                if (tick != std::string::npos) full.resize(tick);
                const size_t dot = full.rfind('.');
                return dot == std::string::npos ? full : full.substr(dot + 1);
            };

            Class klass = ManagedObject{ obj }.get_class();
            if (shape == UdonShape::Dictionary) {
                const int total = ReadInt32Field(obj, FieldEither(klass, "_count", "count"));
                const int freed = ReadInt32Field(obj, FieldEither(klass, "_freeCount", "freeCount"));
                out = std::format("{}({})", shortName(typeName), total - freed);
                return true;
            }

            ArrayStorage st;
            int count = 0;
            if (shape == UdonShape::Array) {
                st = OpenArray(obj);
                count = st.length;
            } else {
                st = OpenArray(ReadPointerField(obj, FieldEither(klass, "_items", "items")));
                count = ReadInt32Field(obj, FieldEither(klass, "_size", "size"));
                if (count > st.length) count = st.length;
            }
            if (!st.valid || count < 0) return false;

            const std::string elem = shortName(st.elementClass.full_name());
            out = shape == UdonShape::Array ? std::format("{}[{}]", elem, count)
                                            : std::format("List<{}>({})", elem, count);

            constexpr int kPreview = 4;
            const int shown = count < kPreview ? count : kPreview;
            if (shown <= 0) return true;

            ++depth;
            out += " {";
            for (int i = 0; i < shown; ++i) {
                UdonVariable el;
                DecodeValue(st.elementType, st.base + static_cast<size_t>(i) * st.stride, el, false);
                if (i) out += ", ";
                if (el.display.size() > 24) { el.display.resize(24); el.display += "..."; }
                out += el.display.empty() ? "?" : el.display;
            }
            if (shown < count) out += ", ...";
            out += "}";
            --depth;
            return true;
        }

        /// One argument in the shape runtime_invoke wants: a value type by a pointer to its
        /// data, a reference type by the object pointer. Holds its own storage, so it must
        /// outlive the call and must not be copied.
        struct InvokeArg {
            alignas(16) char storage[16] = {};
            void* ptr = nullptr;

            InvokeArg() = default;
            InvokeArg(const InvokeArg&) = delete;
            InvokeArg& operator=(const InvokeArg&) = delete;

            bool Parse(const Class& klass, std::string_view text, std::string* error);
        };

        /// <summary>The object a heap slot holds, through the heap's own accessor so a
        /// generic instantiation comes back correctly whatever its declared type is.</summary>
        void* HeapValueObject(void* heap, uint32_t address) {
            ManagedObject h{ heap };
            if (!h) return nullptr;
            static Method getValue = ResolveByReturn(h.get_class(), "GetHeapVariable", 1, "Object");
            if (!getValue) return nullptr;
            void* params[1] = { &address };
            void* obj = MethodHandler::invoke<void*>(getValue, heap, params);
            return Module::IsValidPointer(obj) ? obj : nullptr;
        }

        /// <summary>Parse `text` as `klass` into `buffer`: the value itself for a primitive
        /// or enum, an il2cpp string pointer for System.String.</summary>
        /// <returns>False with a reason in `error` when the type has no text form or the
        /// text does not parse. `written` receives the number of bytes laid down.</returns>
        bool ParseStorage(const Class& klass, std::string_view text, void* buffer, size_t bufferSize,
                          size_t* written, std::string* error) {
            auto fail = [&](const char* why) { if (error) *error = why; return false; };
            if (!klass) return fail("unknown type");
            if (bufferSize < 16) return fail("buffer too small");

            std::string type = klass.full_name();
            const std::string value{ Trimmed(text) };

            if (IsKnownStructType(type)) {
                const int want = (type == "UnityEngine.Vector2") ? 2
                               : (type == "UnityEngine.Vector3") ? 3 : 4;
                float f[4] = { 0, 0, 0, 0 };
                if (ParseFloats(value.c_str(), f, want) != want)
                    return fail("expected that many numbers, e.g. (1, 2, 3)");
                if (type == "UnityEngine.Color32") {
                    uint8_t b[4];
                    for (int i = 0; i < 4; ++i)
                        b[i] = static_cast<uint8_t>(f[i] < 0 ? 0 : (f[i] > 255 ? 255 : f[i]));
                    std::memcpy(buffer, b, sizeof b);
                    if (written) *written = sizeof b;
                    return true;
                }
                const size_t bytes = static_cast<size_t>(want) * sizeof(float);
                std::memcpy(buffer, f, bytes);
                if (written) *written = bytes;
                return true;
            }

            if (type == "System.String") {
                void* str = String::create(value).raw();
                if (!str) return fail("could not create the string");
                std::memcpy(buffer, &str, sizeof(void*));
                if (written) *written = sizeof(void*);
                return true;
            }

            // An enum is stored as, and parses as, its underlying integer.
            if (klass.is_enum()) {
                const int32_t size = klass.value_size(nullptr);
                if (size <= 0 || size > 8) return fail("unsupported enum width");
                char* end = nullptr;
                long long parsed = std::strtoll(value.c_str(), &end, 0);
                if (end == value.c_str() || *end) return fail("expected an integer");
                std::memset(buffer, 0, 8);
                std::memcpy(buffer, &parsed, static_cast<size_t>(size));
                if (written) *written = static_cast<size_t>(size);
                return true;
            }

            auto store = [&](auto v) {
                std::memcpy(buffer, &v, sizeof v);
                if (written) *written = sizeof v;
                return true;
            };

            if (type == "System.Boolean") {
                bool v = value == "true" || value == "True" || value == "1";
                if (!v && value != "false" && value != "False" && value != "0")
                    return fail("expected true or false");
                return store(static_cast<uint8_t>(v ? 1 : 0));
            }
            if (type == "System.Char") {
                if (value.empty()) return fail("expected a character");
                return store(static_cast<uint16_t>(static_cast<unsigned char>(value[0])));
            }

            char* end = nullptr;
            // One parse per width: the value is copied by its own size, so a wider
            // temporary would drag whatever follows it into the destination.
            if (type == "System.UInt64") {
                uint64_t v = std::strtoull(value.c_str(), &end, 0);
                if (end == value.c_str() || *end) return fail("expected an integer");
                return store(v);
            }
            auto asInt = [&](auto sample) {
                using T = decltype(sample);
                long long parsed = std::strtoll(value.c_str(), &end, 0);
                if (end == value.c_str() || *end) return fail("expected an integer");
                return store(static_cast<T>(parsed));
            };
            if (type == "System.SByte")  return asInt(int8_t{});
            if (type == "System.Byte")   return asInt(uint8_t{});
            if (type == "System.Int16")  return asInt(int16_t{});
            if (type == "System.UInt16") return asInt(uint16_t{});
            if (type == "System.Int32")  return asInt(int32_t{});
            if (type == "System.UInt32") return asInt(uint32_t{});
            if (type == "System.Int64")  return asInt(int64_t{});

            if (type == "System.Single" || type == "System.Double") {
                double parsed = std::strtod(value.c_str(), &end);
                if (end == value.c_str() || *end) return fail("expected a number");
                if (type == "System.Single") return store(static_cast<float>(parsed));
                return store(parsed);
            }

            if (error) *error = "unsupported type: " + type;
            return false;
        }

        bool InvokeArg::Parse(const Class& klass, std::string_view text, std::string* error) {
            if (!ParseStorage(klass, text, storage, sizeof storage, nullptr, error)) return false;
            if (klass.full_name() == "System.String") std::memcpy(&ptr, storage, sizeof ptr);
            else                                      ptr = storage;
            return true;
        }

        /// <summary>The collection in a heap slot, when it has the wanted shape.</summary>
        void* CollectionAt(void* heap, uint32_t address, UdonShape want) {
            void* obj = HeapValueObject(heap, address);
            if (!obj) return nullptr;
            std::string typeName;
            return ShapeOf(obj, typeName) == want ? obj : nullptr;
        }

        Module::System::Array<void*> HeapSlots(void* heap) {
            if (!heap) return Module::System::Array<void*>{};
            return Module::System::Array<void*>{ ManagedObject(heap).get_field<void*>("_heap") };
        }

        UdonVariable ReadSlot(Module::System::Array<void*> slots, uint32_t address, bool stringify) {
            UdonVariable var;
            var.address = address;
            if (address >= slots.size()) return var;
            void* slot = slots[address];
            if (!slot) return var;
            var.initialized = true;
            DecodeSlot(slot, var, stringify);
            return var;
        }

        // Generic instantiations can be either a value or a reference type, so hand those
        // back to the heap itself, which boxes them correctly.
        void DecodeThroughApi(void* heap, UdonVariable& var, bool stringify) {
            ManagedObject obj{ heap };
            if (!obj) return;
            // GetHeapVariable has a generic twin of the same arity; only one returns System.Object.
            static Method getValue = ResolveByReturn(obj.get_class(), "GetHeapVariable", 1, "Object");
            static Method getType  = ResolveByReturn(obj.get_class(), "GetHeapVariableType", 1, "Type");

            void* params[1] = { &var.address };
            if (var.typeName.empty())
                var.typeName = TypeNameOf(MethodHandler::invoke<void*>(getType, heap, params));
            SetReference(var, MethodHandler::invoke<void*>(getValue, heap, params), stringify);
        }

        bool LooksLikeHash(std::string_view name) {
            if (name.size() < 24) return false;
            for (char c : name)
                if (!std::isxdigit(static_cast<unsigned char>(c))) return false;
            return true;
        }

        /// <summary>Whether a managed symbol name carries the "__refl_" marker, tested on the
        /// UTF-16 buffer so scanning a program's symbols allocates nothing.</summary>
        bool HasReflMarker(const String& str) {
            const int len = str.length();
            const wchar_t* wc = str.chars();
            if (!wc || len < 7) return false;
            static constexpr wchar_t kMarker[] = L"__refl_";
            constexpr int kMarkerLen = 7;
            for (int i = 0; i + kMarkerLen <= len; ++i) {
                int j = 0;
                for (; j < kMarkerLen; ++j) {
                    wchar_t c = wc[i + j];
                    if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
                    if (c != kMarker[j]) break;
                }
                if (j == kMarkerLen) return true;
            }
            return false;
        }

        bool IsReflTypeNameSymbol(std::string_view name) {
            std::string lower;
            lower.reserve(name.size());
            for (char c : name) lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
            if (lower.find("__refl_typename") != std::string::npos) return true;
            if (lower.find("__refl_udontypename") != std::string::npos) return true;
            return lower.starts_with("__refl_") && lower.ends_with("typename");
        }

        std::string CleanTypeName(std::string_view raw) {
            std::string_view name = Trimmed(raw);
            if (const size_t dot = name.rfind('.'); dot != std::string_view::npos) name.remove_prefix(dot + 1);
            if (name.starts_with("UdonSharpBehaviour") && name.size() > 18) name.remove_prefix(18);
            return std::string{ Trimmed(name) };
        }

        std::string AssetName(ManagedObject obj, const char* fieldName, bool rejectHash) {
            if (!obj) return {};
            void* asset = obj.get_field<void*>(fieldName);
            if (!asset) return {};

            // GetName reads objectCachedPtr, which only exists on a real UnityEngine.Object.
            Class unityBase = Class::find("UnityEngine.Object");
            Class assetClass = ManagedObject{ asset }.get_class();
            if (!unityBase || !assetClass || !assetClass.is_subclass_of(unityBase)) return {};

            Module::Unity::Object unityObj{ asset };
            if (!unityObj.IsValid()) return {};

            const std::string raw = unityObj.GetName();
            std::string_view name = Trimmed(raw);
            static constexpr std::string_view kAssetSuffix = " (SerializedUdonProgramAsset)";
            if (name.ends_with(kAssetSuffix))
                name = Trimmed(name.substr(0, name.size() - kAssetSuffix.size()));
            if (rejectHash && LooksLikeHash(name)) return {};
            return std::string{ name };
        }

        uint32_t AddressOf(ManagedObject symbolTable, std::string_view symbolName, bool& found) {
            uint32_t address = 0;
            found = false;
            if (!symbolTable) return 0;
            auto str = String::create(symbolName);
            void* params[2] = { str.raw(), &address };
            found = symbolTable.call_method<bool>("TryGetAddressFromSymbol", params, 2);
            return found ? address : 0;
        }

        Class SlotClass(ManagedObject heap, uint32_t address) {
            if (!heap) return Class{};
            static Method getType = ResolveByReturn(heap.get_class(), "GetHeapVariableType", 1, "Type");
            if (!getType) return Class{};
            void* params[1] = { &address };
            void* systemType = MethodHandler::invoke<void*>(getType, heap.raw(), params);
            return systemType ? Class::from_system_type(systemType) : Class{};
        }

        void* BoxFromText(const Class& klass, std::string_view text, std::string* error) {
            if (!klass) { if (error) *error = "unknown parameter type"; return nullptr; }
            if (klass.full_name() == "System.String")
                return String::create(std::string{ Trimmed(text) }).raw();

            alignas(16) char buffer[16] = {};
            if (!ParseStorage(klass, text, buffer, sizeof buffer, nullptr, error)) return nullptr;
            void* box = Module::Box(reinterpret_cast<il2cppClass*>(klass.raw()), buffer);
            if (!box && error) *error = "boxing failed";
            return box;
        }

        /// The receiving symbol could not be resolved, so the literal's own shape picks the
        /// type. Wrong for a byte or a double, right for the common bool/int/float/string.
        void* BoxSniffed(std::string_view text, std::string* error) {
            const std::string v{ Trimmed(text) };
            const char* type = "System.String";
            if (v == "true" || v == "false" || v == "True" || v == "False") {
                type = "System.Boolean";
            } else if (!v.empty()) {
                char* end = nullptr;
                std::strtoll(v.c_str(), &end, 0);
                if (end != v.c_str() && !*end) {
                    type = "System.Int32";
                } else {
                    std::strtod(v.c_str(), &end);
                    if (end != v.c_str() && !*end) type = "System.Single";
                }
            }
            return BoxFromText(Class::find(type), v, error);
        }

        std::string StringProperty(ManagedObject obj, const char* property, const char* field) {
            if (!obj) return {};
            if (void* s = obj.get_property<void*>(property)) return String(s).to_string();
            if (void* s = obj.get_field<void*>(field)) return String(s).to_string();
            return {};
        }

    } // namespace

    bool UdonBehaviour::GetSynchronizePosition() {
        if (!valid()) return false;
        return get_field<bool>("SynchronizePosition");
    }

    bool UdonBehaviour::GetSynchronizeAnimation() {
        if (!valid()) return false;
        return get_field<bool>("SynchronizeAnimation");
    }

    bool UdonBehaviour::GetReliable() {
        if (!valid()) return false;
        return get_field<bool>("Reliable");
    }

    SyncType UdonBehaviour::GetSyncMethod() {
        if (!valid()) return SyncType::Unknown;
        return get_field<SyncType>("_syncMethod");
    }

    bool UdonBehaviour::GetIsReady() {
        if (!valid()) return false;
        return get_field<bool>("_isReady");
    }

    bool UdonBehaviour::GetHasError() {
        if (!valid()) return false;
        return get_field<bool>("_hasError");
    }

    bool UdonBehaviour::GetInitialized() {
        if (!valid()) return false;
        return get_field<bool>("_initialized");
    }

    void UdonBehaviour::SendCustomEvent(std::string_view eventName) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "SendCustomEvent", 1);
        auto str = String::create(eventName);
        void* params[1] = { str.raw() };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::SendCustomNetworkEvent(NetworkEventTarget target, std::string_view eventName) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "SendCustomNetworkEvent", 2);
        auto str = String::create(eventName);
        int iTarget = static_cast<int>(target);
        void* params[2] = { &iTarget, str.raw() };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void* BoxLiteral(std::string_view text, std::string_view typeName, std::string* error) {
        if (typeName.empty()) return BoxSniffed(text, error);
        Class klass = Class::find(typeName);
        if (!klass) { if (error) *error = "unknown type: " + std::string{ typeName }; return nullptr; }
        return BoxFromText(klass, text, error);
    }

    void* MakeObjectArray(const std::vector<std::string>& literals,
                          const std::vector<std::string>& typeNames, std::string* error) {
        auto fail = [&](std::string why) -> void* { if (error) *error = std::move(why); return nullptr; };

        std::vector<void*> boxed;
        boxed.reserve(literals.size());
        for (size_t i = 0; i < literals.size(); ++i) {
            const std::string_view type = i < typeNames.size() ? std::string_view{ typeNames[i] }
                                                               : std::string_view{};
            std::string why;
            void* value = BoxLiteral(literals[i], type, &why);
            if (!value) return fail("parameter " + std::to_string(i) + ": " +
                                    (why.empty() ? "could not be parsed" : why));
            boxed.push_back(value);
        }

        auto arr = Module::System::Array<void*>::Create("System.Object", boxed.size());
        if (!arr) return fail("the parameter array could not be allocated");
        auto* e = Module::GetExports();
        void** slots = arr.data();
        if (!slots) return fail("the parameter array is unreadable");
        for (size_t i = 0; i < boxed.size(); ++i) {
            if (e && e->m_gcWBarrierSetField)
                reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void**, void*)>(e->m_gcWBarrierSetField)(
                    arr.raw(), &slots[i], boxed[i]);
            else
                slots[i] = boxed[i];
        }
        return arr.raw();
    }

    std::string UdonBehaviour::GetEventParameterName(std::string_view eventName,
                                                     std::string_view symbolName) const {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "GetEventParameterName", 2);
        if (!m) return {};
        auto ev = String::create(eventName);
        auto sym = String::create(symbolName);
        void* params[2] = { ev.raw(), sym.raw() };
        void* out = MethodHandler::invoke<void*>(m, raw(), params);
        return out ? String(out).to_string() : std::string{};
    }

    std::vector<UdonNetworkEvent> UdonBehaviour::GetNetworkCallableEvents() const {
        std::vector<UdonNetworkEvent> out;
        if (!valid()) return out;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "GetNetworkCallingMetadata", 0);
        if (!m) return out;

        Module::System::Array<void*> entries{ MethodHandler::invoke<void*>(m, raw()) };
        const uintptr_t count = entries.size();
        out.reserve(static_cast<size_t>(count));

        ManagedObject heap{ GetHeap() };
        for (uintptr_t i = 0; i < count; ++i) {
            ManagedObject entry{ entries[i] };
            if (!entry) continue;

            UdonNetworkEvent ev;
            ev.name = StringProperty(entry, "Name", "_name");
            if (ev.name.empty()) continue;
            ev.maxEventsPerSecond = entry.get_property<int32_t>("MaxEventsPerSecond");
            if (ev.maxEventsPerSecond == 0) ev.maxEventsPerSecond = entry.get_field<int32_t>("_maxEventsPerSecond");

            void* rawParams = entry.get_property<void*>("Parameters");
            if (!rawParams) rawParams = entry.get_field<void*>("_parameters");
            Module::System::Array<void*> params{ rawParams };
            const uintptr_t paramCount = params.size();
            ev.parameters.reserve(static_cast<size_t>(paramCount));
            for (uintptr_t p = 0; p < paramCount; ++p) {
                ManagedObject pm{ params[p] };
                UdonNetworkParam param;
                param.name = StringProperty(pm, "Name", "_name");
                param.symbol = GetEventParameterName(ev.name, param.name);
                uint32_t address = 0;
                if (!param.symbol.empty() && TryGetSymbolAddress(param.symbol, address)) {
                    Class klass = SlotClass(heap, address);
                    if (klass) param.typeName = klass.full_name();
                }
                ev.parameters.push_back(std::move(param));
            }
            out.push_back(std::move(ev));
        }
        return out;
    }

    bool UdonBehaviour::GetNetworkCallableEvent(std::string_view eventName, UdonNetworkEvent& out) const {
        for (auto& ev : GetNetworkCallableEvents()) {
            if (ev.name == eventName) { out = std::move(ev); return true; }
        }
        return false;
    }

    bool UdonBehaviour::CanSendNetworkEvent(std::string_view eventName) const {
        if (!valid()) return false;
        for (const auto& name : GetNetworkedEventNames())
            if (name == eventName) return true;
        UdonNetworkEvent ev;
        return GetNetworkCallableEvent(eventName, ev);
    }

    bool UdonBehaviour::SendCustomNetworkEventWithParams(NetworkEventTarget target,
                                                        std::string_view eventName,
                                                        const std::vector<std::string>& parameterTexts,
                                                        std::string* error) {
        auto fail = [&](std::string why) { if (error) *error = std::move(why); return false; };
        if (!valid()) return fail("the behaviour is gone");
        if (parameterTexts.empty()) {
            SendCustomNetworkEvent(target, eventName);
            return true;
        }
        if (parameterTexts.size() > 8) return fail("VRChat carries at most 8 parameters");

        UdonNetworkEvent meta;
        const bool hasMeta = GetNetworkCallableEvent(eventName, meta);
        if (hasMeta && meta.parameters.size() != parameterTexts.size())
            return fail(std::format("{} takes {} parameter(s), {} given",
                                    eventName, meta.parameters.size(), parameterTexts.size()));

        ManagedObject heap{ GetHeap() };
        std::vector<void*> boxed;
        boxed.reserve(parameterTexts.size());
        for (size_t i = 0; i < parameterTexts.size(); ++i) {
            std::string why;
            Class klass;
            if (hasMeta && !meta.parameters[i].symbol.empty()) {
                uint32_t address = 0;
                if (TryGetSymbolAddress(meta.parameters[i].symbol, address)) klass = SlotClass(heap, address);
            }
            void* value = klass ? BoxFromText(klass, parameterTexts[i], &why)
                                : BoxSniffed(parameterTexts[i], &why);
            if (!value) {
                const std::string name = hasMeta ? meta.parameters[i].name : std::to_string(i);
                return fail(name + ": " + (why.empty() ? "could not be parsed" : why));
            }
            boxed.push_back(value);
        }

        const int argc = static_cast<int>(boxed.size());
        const std::string method = "SendCustomNetworkEvent" + std::to_string(argc);
        Method m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", method, argc + 2);
        if (!m) return fail(method + " unresolved");

        auto str = String::create(eventName);
        int iTarget = static_cast<int>(target);
        void* params[10] = { &iTarget, str.raw() };
        for (int i = 0; i < argc; ++i) params[i + 2] = boxed[static_cast<size_t>(i)];
        MethodHandler::invoke<void>(m, raw(), params);
        return true;
    }

    void UdonBehaviour::RequestSerialization() {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "RequestSerialization", 0);
        MethodHandler::invoke<void>(m, raw());
    }

    void UdonBehaviour::SendCustomEventDelayedSeconds(std::string_view eventName, float delaySeconds, EventTiming eventTiming) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "SendCustomEventDelayedSeconds", 3);
        auto str = String::create(eventName);
        int iTiming = static_cast<int>(eventTiming);
        void* params[3] = { str.raw(), &delaySeconds, &iTiming };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::SendCustomEventDelayedFrames(std::string_view eventName, int delayFrames, EventTiming eventTiming) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "SendCustomEventDelayedFrames", 3);
        auto str = String::create(eventName);
        int iTiming = static_cast<int>(eventTiming);
        void* params[3] = { str.raw(), &delayFrames, &iTiming };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    std::string UdonBehaviour::GetInteractionText() {
        if (!valid()) return "";
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "get_InteractionText", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? String(str).to_string() : "";
    }

    void UdonBehaviour::SetInteractionText(std::string_view text) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "set_InteractionText", 1);
        auto str = String::create(text);
        void* params[1] = { str.raw() };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::Interact() {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "Interact", 0);
        if (!m) m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "Interact", 0);
        if (!m) return;
        MethodHandler::invoke<void>(m, raw());
    }

    bool UdonBehaviour::GetDisableInteractive() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "get_DisableInteractive", 0);
        if (!m) m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "get_DisableInteractive", 0);
        if (!m) return false;
        return MethodHandler::invoke<bool>(m, raw());
    }

    void UdonBehaviour::SetDisableInteractive(bool value) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "set_DisableInteractive", 1);
        if (!m) m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "set_DisableInteractive", 1);
        if (!m) return;
        void* params[1] = { &value };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    bool UdonBehaviour::GetIsInteractive() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "get_IsInteractive", 0);
        if (!m) return false;
        return MethodHandler::invoke<bool>(m, raw());
    }

    bool UdonBehaviour::GetDisableEventProcessing() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "get_DisableEventProcessing", 0);
        if (!m) return get_field<bool>("<DisableEventProcessing>k__BackingField");
        return MethodHandler::invoke<bool>(m, raw());
    }

    void UdonBehaviour::SetDisableEventProcessing(bool value) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "set_DisableEventProcessing", 1);
        if (!m) return;
        void* params[1] = { &value };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    bool UdonBehaviour::GetIsNetworkingSupported() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "get_IsNetworkingSupported", 0);
        if (!m) return false;
        return MethodHandler::invoke<bool>(m, raw());
    }

    void UdonBehaviour::SetIsNetworkingSupported(bool value) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "set_IsNetworkingSupported", 1);
        if (!m) return;
        void* params[1] = { &value };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::SetSyncMethod(SyncType value) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "set_SyncMethod", 1);
        if (!m) return;
        int iValue = static_cast<int>(value);
        void* params[1] = { &iValue };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::RunProgram(std::string_view eventName) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "RunProgram", 1);
        if (!m) return;
        auto str = String::create(eventName);
        void* params[1] = { str.raw() };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::RunProgramAt(uint32_t entryPoint) {
        if (!valid()) return;
        // RunProgram(String) and RunProgram(UInt32) share a name; the dump's RunProgram1 is
        // the second overload, and both take one argument, so the parameter type picks it.
        static Method m = [this] {
            Class klass = get_class();
            if (!klass) return Method{ nullptr };
            for (const Method& candidate : klass.get_methods()) {
                const char* n = candidate.name();
                if (!n || std::strcmp(n, "RunProgram") != 0) continue;
                if (candidate.param_count() != 1) continue;
                const Type t = candidate.get_param_type(0);
                if (t && t.full_name() == "System.UInt32") return candidate;
            }
            return Method{ nullptr };
        }();
        if (!m) return;
        void* params[1] = { &entryPoint };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    bool UdonBehaviour::RunEvent(std::string_view eventName, bool canRunBeforeStart) {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "RunEventAdvanced", 2);
        auto str = String::create(eventName);
        if (m) {
            void* params[2] = { str.raw(), &canRunBeforeStart };
            return MethodHandler::invoke<bool>(m, raw(), params);
        }
        static auto plain = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "RunEvent", 1);
        if (!plain) return false;
        void* params[1] = { str.raw() };
        return MethodHandler::invoke<bool>(plain, raw(), params);
    }

    bool UdonBehaviour::TryGetEntrypointHash(std::string_view eventName, uint32_t& hash) const {
        hash = 0;
        if (!valid()) return false;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "TryGetEntrypointHashFromName", 2);
        if (!m) return false;
        auto str = String::create(eventName);
        void* params[2] = { str.raw(), &hash };
        return MethodHandler::invoke<bool>(m, raw(), params);
    }

    std::string UdonBehaviour::GetEntrypointName(uint32_t hash) const {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "TryGetEntrypointNameFromHash", 2);
        if (!m) return {};
        void* name = nullptr;
        void* params[2] = { &hash, &name };
        if (!MethodHandler::invoke<bool>(m, raw(), params) || !name) return {};
        return String(name).to_string();
    }

    std::string UdonBehaviour::GetProgramVariableType(std::string_view symbolName) const {
        if (!valid()) return {};
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "GetProgramVariableType", 1);
        if (!m) return {};
        auto str = String::create(symbolName);
        void* params[1] = { str.raw() };
        return TypeNameOf(MethodHandler::invoke<void*>(m, raw(), params));
    }

    bool UdonBehaviour::RunEventWithParams(std::string_view eventName,
                                           const std::vector<std::pair<std::string, std::string>>& parameters,
                                           bool mangleParameterNames, bool canRunBeforeStart,
                                           std::string* error) {
        auto fail = [&](std::string why) { if (error) *error = std::move(why); return false; };
        if (!valid()) return fail("the behaviour is gone");
        if (parameters.empty()) return RunEvent(eventName, canRunBeforeStart);

        // RunEventAdvanced's overloads all share one IL name. The tuple-per-argument forms are
        // generic; only the array form is a plain method, so the array parameter picks it.
        static Method m = [this] {
            Class klass = get_class();
            if (!klass) return Method{ nullptr };
            for (const Method& candidate : klass.get_methods()) {
                const char* n = candidate.name();
                if (!n || std::strcmp(n, "RunEventAdvanced") != 0) continue;
                if (candidate.param_count() != 4) continue;
                const Type t = candidate.get_param_type(3);
                const char* tn = t ? t.name() : nullptr;
                if (tn && std::strstr(tn, "[]")) return candidate;
            }
            return Method{ nullptr };
        }();
        if (!m) return fail("RunEventAdvanced(string, bool, bool, ValueTuple[]) unresolved");

        Class arrayClass = m.get_param_type(3).get_class();
        if (!arrayClass) return fail("the parameter array type could not be resolved");
        auto* arrayRaw = reinterpret_cast<il2cppClass*>(arrayClass.raw());
        void* elementClass = arrayRaw ? arrayRaw->m_pElementClass : nullptr;
        auto* e = Module::GetExports();
        if (!elementClass || !e || !e->m_arrayNew) return fail("the runtime cannot allocate the array");

        // One ValueTuple<string, object>: two managed references, nothing else.
        struct TupleSO { void* item1; void* item2; };
        std::vector<TupleSO> staged;
        staged.reserve(parameters.size());

        ManagedObject heap{ GetHeap() };
        for (const auto& [name, text] : parameters) {
            const std::string symbol = mangleParameterNames ? GetEventParameterName(eventName, name) : name;
            if (symbol.empty()) return fail(name + ": no parameter symbol on this event");

            Class klass;
            uint32_t address = 0;
            if (TryGetSymbolAddress(symbol, address)) klass = SlotClass(heap, address);

            std::string why;
            void* value = klass ? BoxFromText(klass, text, &why) : BoxSniffed(text, &why);
            if (!value) return fail(name + ": " + (why.empty() ? "could not be parsed" : why));
            staged.push_back({ String::create(symbol).raw(), value });
        }

        void* arr = reinterpret_cast<void*(IL2CPP_CALLTYPE)(void*, uintptr_t)>(e->m_arrayNew)(
            elementClass, staged.size());
        if (!arr) return fail("the parameter array could not be allocated");

        Module::System::Array<TupleSO> typed{ arr };
        TupleSO* slots = typed.data();
        if (!slots) return fail("the parameter array is unreadable");
        for (size_t i = 0; i < staged.size(); ++i) {
            if (e->m_gcWBarrierSetField) {
                reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void**, void*)>(e->m_gcWBarrierSetField)(
                    arr, &slots[i].item1, staged[i].item1);
                reinterpret_cast<void(IL2CPP_CALLTYPE)(void*, void**, void*)>(e->m_gcWBarrierSetField)(
                    arr, &slots[i].item2, staged[i].item2);
            } else {
                slots[i] = staged[i];
            }
        }

        auto str = String::create(eventName);
        bool mangle = mangleParameterNames, early = canRunBeforeStart;
        void* params[4] = { str.raw(), &mangle, &early, arr };
        if (!MethodHandler::invoke<bool>(m, raw(), params))
            return fail("the behaviour refused to run the event");
        return true;
    }

    namespace {

        /// Build an UdonInputEventArgs by calling its constructor on stack storage, so the
        /// value type's field order never has to be guessed.
        bool MakeInputArgs(bool isFloat, bool boolValue, float floatValue, int handType, void* storage) {
            static Class klass = Class::find("VRC.Udon.Common.UdonInputEventArgs");
            if (!klass) return false;
            const char* want = isFloat ? "Single" : "Boolean";
            for (const Method& m : klass.get_methods()) {
                const char* n = m.name();
                if (!n || std::strcmp(n, ".ctor") != 0) continue;
                if (m.param_count() != 2) continue;
                const Type t = m.get_param_type(0);
                const char* tn = t ? t.name() : nullptr;
                if (!tn || std::strcmp(tn, want) != 0) continue;
                int hand = handType;
                void* params[2] = { isFloat ? static_cast<void*>(&floatValue)
                                            : static_cast<void*>(&boolValue), &hand };
                MethodHandler::invoke<void>(m, storage, params);
                return true;
            }
            return false;
        }

        bool InvokeInputEvent(void* behaviour, std::string_view eventName, void* args) {
            static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "RunInputEvent", 2);
            if (!m) return false;
            auto str = String::create(eventName);
            void* params[2] = { str.raw(), args };
            MethodHandler::invoke<void>(m, behaviour, params);
            return true;
        }

    } // namespace

    bool UdonBehaviour::RunInputEvent(std::string_view eventName, bool value, int handType) {
        if (!valid()) return false;
        alignas(16) char args[32] = {};
        if (!MakeInputArgs(false, value, 0.0f, handType, args)) return false;
        return InvokeInputEvent(raw(), eventName, args);
    }

    bool UdonBehaviour::RunInputEvent(std::string_view eventName, float value, int handType) {
        if (!valid()) return false;
        alignas(16) char args[32] = {};
        if (!MakeInputArgs(true, false, value, handType, args)) return false;
        return InvokeInputEvent(raw(), eventName, args);
    }

    UdonSyncInfo UdonBehaviour::GetSyncMetadata(std::string_view symbolName) const {
        UdonSyncInfo out;
        out.symbol = std::string{ symbolName };
        if (!valid()) return out;

        static auto getTable = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "get_SyncMetadataTable", 0);
        if (!getTable) return out;
        ManagedObject table{ MethodHandler::invoke<void*>(getTable, raw()) };
        if (!table) return out;

        auto str = String::create(symbolName);
        void* params[1] = { str.raw() };
        ManagedObject meta{ table.call_method<void*>("GetSyncMetadataFromSymbol", params, 1) };
        if (!meta) return out;

        out.synced = true;
        ManagedObject props{ meta.get_property<void*>("Properties") };
        if (!props) props = ManagedObject{ meta.get_field<void*>("<Properties>k__BackingField") };
        if (!props) return out;

        // List<IUdonSyncProperty>: the first property carries the interpolation for the symbol.
        void* items = props.get_field<void*>("_items");
        Module::System::Array<void*> arr{ items };
        if (arr.size() == 0 || !arr[0]) return out;

        ManagedObject prop{ arr[0] };
        const int32_t algorithm = prop.get_property<int32_t>("InterpolationAlgorithm");
        switch (algorithm) {
            case 1:  out.interpolation = "Linear"; break;
            case 2:  out.interpolation = "Smooth"; break;
            default: out.interpolation = "None";   break;
        }
        return out;
    }

    std::vector<UdonSyncInfo> UdonBehaviour::GetSyncedVariables() const {
        std::vector<UdonSyncInfo> out;
        if (!valid()) return out;
        for (const auto& sym : GetSymbolAddresses()) {
            UdonSyncInfo info = GetSyncMetadata(sym.name);
            if (info.synced) out.push_back(std::move(info));
        }
        return out;
    }

    void UdonBehaviour::SetProgramVariable(std::string_view symbolName, void* value) {
        uint32_t address = 0;
        if (!TryGetSymbolAddress(symbolName, address)) return;
        WriteHeap(address, value);
    }

    void* UdonBehaviour::GetProgramVariable(std::string_view symbolName) {
        UdonVariable var;
        if (!TryGetSymbolAddress(symbolName, var.address)) return nullptr;
        DecodeThroughApi(GetHeap(), var, false);
        return var.boxed;
    }

    bool UdonBehaviour::TryGetVariableValue(std::string_view symbolName, void** value) {
        if (!valid() || !value) return false;
        auto* table = get_field<void*>("publicVariables");
        if (!table) return false;
        ManagedObject tableObj(table);
        auto str = String::create(symbolName);
        void* params[2] = { str.raw(), value };
        return tableObj.call_method<bool>("TryGetVariableValue", params, 2);
    }

    bool UdonBehaviour::TrySetVariableValue(std::string_view symbolName, void* value) {
        if (!valid()) return false;
        auto* table = get_field<void*>("publicVariables");
        if (!table) return false;
        ManagedObject tableObj(table);
        auto str = String::create(symbolName);
        void* params[2] = { str.raw(), value };
        return tableObj.call_method<bool>("TrySetVariableValue", params, 2);
    }

    void* UdonBehaviour::GetProgram() const {
        if (!valid()) return nullptr;
        return get_field<void*>("_program");
    }

    void* UdonBehaviour::GetHeap() const {
        if (!valid()) return nullptr;
        ManagedObject vm{ get_field<void*>("_udonVM") };
        if (vm) {
            if (void* h = vm.get_field<void*>("_heap")) return h;
            if (void* h = vm.call_method<void*>("InspectHeap", nullptr, 0)) return h;
        }
        return AutoProperty(ManagedObject{ GetProgram() }, "Heap", "<Heap>k__BackingField").raw();
    }

    void* UdonBehaviour::GetVM() const {
        if (!valid()) return nullptr;
        return get_field<void*>("_udonVM");
    }

    std::string UdonBehaviour::GetScriptName() const {
        // A caller walking every behaviour in a world reaches this once per program, so it
        // decodes nothing it does not need: GetSymbols() would resolve a type and an address
        // per symbol (four managed calls each), and even converting every name to UTF-8 costs
        // a conversion per symbol. Only a name carrying the reflection marker is decoded.
        ManagedObject table = SymbolTableOf(GetProgram());
        Module::System::Array<void*> names = SymbolArray(table, "_symbols", "GetSymbols");
        const uintptr_t count = names.size();
        for (uintptr_t i = 0; i < count; ++i) {
            void* raw = names[i];
            if (!raw || !HasReflMarker(String(raw))) continue;
            std::string name = String(raw).to_string();
            if (!IsReflTypeNameSymbol(name)) continue;
            bool found = false;
            const uint32_t address = AddressOf(table, name, found);
            if (!found) continue;
            UdonVariable var = ReadHeap(address);
            if (var.kind != UdonValueKind::String) continue;
            std::string clean = CleanTypeName(var.display);
            if (!clean.empty()) return clean;
        }

        ManagedObject self{ raw() };
        // Only genuine UnityEngine.Object assets: _program is an IUdonProgram, and reading
        // objectCachedPtr off one crashes when nothing earlier resolved.
        if (std::string name = AssetName(self, "programSource", true); !name.empty()) return name;
        if (std::string name = AssetName(self, "serializedProgramAsset", true); !name.empty()) return name;
        return {};
    }

    uint32_t UdonBehaviour::GetByteCodeLength() const {
        ManagedObject vm{ GetVM() };
        if (!vm) return 0;
        return static_cast<uint32_t>(
            Module::System::Array<uint32_t>{ vm.get_field<void*>("_processedByteCode") }.size());
    }

    std::vector<uint32_t> UdonBehaviour::GetByteCode() const {
        ManagedObject vm{ GetVM() };
        if (!vm) return {};
        return Module::System::Array<uint32_t>{ vm.get_field<void*>("_processedByteCode") }.to_vector();
    }

    bool UdonBehaviour::SetByteCode(const std::vector<uint32_t>& words) {
        ManagedObject vm{ GetVM() };
        if (!vm) return false;

        Field field = vm.get_field_info("_processedByteCode");
        if (!field || field.offset() < 0) return false;

        Module::System::Array<uint32_t> existing{ vm.get_field<void*>(field) };
        if (existing && existing.size() == words.size()) {
            if (!words.empty()) std::memcpy(existing.data(), words.data(), words.size() * sizeof(uint32_t));
            return true;
        }

        auto replacement = Module::System::Array<uint32_t>::Create("System.UInt32", words.size());
        if (!replacement) return false;
        if (!words.empty()) std::memcpy(replacement.data(), words.data(), words.size() * sizeof(uint32_t));

        vm.set_field<void*>(field, replacement.raw());
        return true;
    }

    std::string UdonBehaviour::GetInstructionSetIdentifier() const {
        ManagedObject id = AutoProperty(ManagedObject{ GetProgram() },
                                        "InstructionSetIdentifier", "<InstructionSetIdentifier>k__BackingField");
        return id ? String(id.raw()).to_string() : std::string{};
    }

    std::vector<std::string> UdonBehaviour::GetEventNames() const {
        return ReadSymbols(EntryPointsOf(GetProgram()), "_symbols", "GetSymbols");
    }

    std::vector<std::string> UdonBehaviour::GetNetworkedEventNames() const {
        return ReadSymbols(EntryPointsOf(GetProgram()), "_exportedSymbols", "GetExportedSymbols");
    }

    bool UdonBehaviour::HasEvent(std::string_view eventName) const {
        ManagedObject entry = EntryPointsOf(GetProgram());
        if (!entry) return false;
        auto str = String::create(eventName);
        void* params[1] = { str.raw() };
        return entry.call_method<bool>("HasAddressForSymbol", params, 1);
    }

    std::vector<UdonSymbol> UdonBehaviour::GetEntryPoints() const {
        std::vector<UdonSymbol> out;
        ManagedObject table = EntryPointsOf(GetProgram());
        if (!table) return out;

        auto names = ReadSymbols(table, "_symbols", "GetSymbols");
        out.reserve(names.size());
        for (auto& name : names) {
            UdonSymbol sym;
            sym.name = std::move(name);

            bool found = false;
            sym.address = AddressOf(table, sym.name, found);
            if (!found) continue;
            out.push_back(std::move(sym));
        }
        return out;
    }

    std::vector<UdonSymbol> UdonBehaviour::GetSymbols() const {
        std::vector<UdonSymbol> out;
        ManagedObject table = SymbolTableOf(GetProgram());
        if (!table) return out;

        auto names = ReadSymbols(table, "_symbols", "GetSymbols");
        out.reserve(names.size());
        for (auto& name : names) {
            UdonSymbol sym;
            sym.name = std::move(name);

            auto str = String::create(sym.name);
            void* params[1] = { str.raw() };
            sym.typeName = TypeNameOf(table.call_method<void*>("GetSymbolType", params, 1));

            bool found = false;
            sym.address = AddressOf(table, sym.name, found);
            out.push_back(std::move(sym));
        }
        return out;
    }

    std::vector<UdonSymbol> UdonBehaviour::GetSymbolAddresses() const {
        std::vector<UdonSymbol> out;
        ManagedObject table = SymbolTableOf(GetProgram());
        if (!table) return out;

        auto names = ReadSymbols(table, "_symbols", "GetSymbols");
        out.reserve(names.size());
        for (auto& name : names) {
            UdonSymbol sym;
            sym.name = std::move(name);
            bool found = false;
            sym.address = AddressOf(table, sym.name, found);
            if (!found) continue;
            out.push_back(std::move(sym));
        }
        return out;
    }

    std::vector<std::string> UdonBehaviour::GetPublicVariableNames() const {
        return ReadSymbols(SymbolTableOf(GetProgram()), "_exportedSymbols", "GetExportedSymbols");
    }

    bool UdonBehaviour::TryGetSymbolAddress(std::string_view symbolName, uint32_t& address) const {
        bool found = false;
        address = AddressOf(SymbolTableOf(GetProgram()), symbolName, found);
        return found;
    }

    uint32_t UdonBehaviour::GetHeapCapacity() const {
        return static_cast<uint32_t>(HeapSlots(GetHeap()).size());
    }

    UdonVariable UdonBehaviour::ReadHeap(uint32_t address) const {
        void* heap = GetHeap();
        UdonVariable var = ReadSlot(HeapSlots(heap), address, false);
        if (var.kind == UdonValueKind::Unknown && var.initialized) DecodeThroughApi(heap, var, false);
        return var;
    }

    std::vector<UdonVariable> UdonBehaviour::ReadHeapSlots(const uint32_t* addresses, size_t count,
                                                          bool stringify) const {
        std::vector<UdonVariable> out;
        if (!addresses || count == 0) return out;

        void* heap = GetHeap();
        auto slots = HeapSlots(heap);
        out.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            UdonVariable var = ReadSlot(slots, addresses[i], stringify);
            if (var.kind == UdonValueKind::Unknown && var.initialized)
                DecodeThroughApi(heap, var, stringify);
            out.push_back(std::move(var));
        }
        return out;
    }

    std::string UdonBehaviour::StringifyHeapValue(uint32_t address) const {
        void* heap = GetHeap();
        UdonVariable var = ReadSlot(HeapSlots(heap), address, false);
        if (!var.initialized) return "(empty slot)";
        if (var.kind == UdonValueKind::Null) return "null";
        if (var.kind != UdonValueKind::Object) return var.display;
        // A value type carries no box until asked for one, so this is where it is paid for.
        if (!var.boxed) {
            UdonVariable boxedVar = ReadSlot(HeapSlots(heap), address, true);
            return boxedVar.display.empty() ? "(unreadable)" : boxedVar.display;
        }
        if (!Module::IsValidPointer(var.boxed)) return "(unreadable)";

        ManagedObject obj{ var.boxed };
        Class klass = obj.get_class();
        Class unityBase = Class::find("UnityEngine.Object");
        if (klass && unityBase && klass.is_subclass_of(unityBase) &&
            !Module::Unity::Object{ var.boxed }.IsValid())
            return "(destroyed)";

        std::string text = obj.to_string();
        constexpr size_t kMaxLength = 512;
        if (text.size() > kMaxLength) {
            text.resize(kMaxLength);
            text += "...";
        }
        return text;
    }

    UdonVariable UdonBehaviour::ReadVariable(std::string_view symbolName) const {
        uint32_t address = 0;
        if (!TryGetSymbolAddress(symbolName, address)) return {};
        UdonVariable var = ReadHeap(address);
        var.symbol.assign(symbolName);
        return var;
    }

    std::vector<UdonVariable> UdonBehaviour::DumpHeap(bool namedOnly) const {
        std::vector<UdonVariable> out;

        void* heap = GetHeap();
        auto slots = HeapSlots(heap);
        const uintptr_t capacity = slots.size();
        if (capacity == 0) return out;

        std::unordered_map<uint32_t, std::string> names;
        for (auto& sym : GetSymbolAddresses()) names.emplace(sym.address, std::move(sym.name));

        out.reserve(namedOnly ? names.size() : static_cast<size_t>(capacity));
        for (uint32_t address = 0; address < capacity; ++address) {
            auto it = names.find(address);
            if (namedOnly && it == names.end()) continue;

            UdonVariable var = ReadSlot(slots, address, false);
            if (var.kind == UdonValueKind::Unknown && var.initialized) DecodeThroughApi(heap, var, false);
            if (it != names.end()) var.symbol = it->second;
            out.push_back(std::move(var));
        }
        return out;
    }

    std::vector<UdonCachedExtern> UdonBehaviour::GetCachedExterns() const {
        std::vector<UdonCachedExtern> out;

        auto slots = HeapSlots(GetHeap());
        const uintptr_t capacity = slots.size();
        for (uint32_t address = 0; address < capacity; ++address) {
            void* slot = slots[address];
            if (!slot) continue;

            Field value = ManagedObject(slot).get_field_info("Value");
            if (!value || value.offset() < 0) continue;

            Type valueType = value.type();
            if (!valueType) continue;
            const int typeEnum = valueType.type_enum();
            if (typeEnum != kTypeClass && typeEnum != kTypeObject) continue;

            void* cached = *reinterpret_cast<void**>(static_cast<char*>(slot) + value.offset());
            if (!Module::IsValidPointer(cached)) continue;

            // The slot's declared type is not proof of what it holds, and these offsets are
            // read raw, so the runtime class is checked before anything is dereferenced.
            ManagedObject obj{ cached };
            Class cachedClass = obj.get_class();
            if (!cachedClass) continue;
            const char* className = cachedClass.name();
            if (!className || std::strcmp(className, "CachedUdonExternDelegate") != 0) continue;

            static void* resolvedFor = nullptr;
            static Field signatureField;
            static Field delegateField;
            static Field countField;
            if (resolvedFor != cachedClass.raw()) {
                Field s = obj.get_field_info("externSignature");
                Field d = obj.get_field_info("externDelegate");
                Field c = obj.get_field_info("parameterCount");
                if (!d || d.offset() < 0) continue;
                signatureField = s;
                delegateField = d;
                countField = c;
                resolvedFor = cachedClass.raw();
            }
            if (!delegateField || delegateField.offset() < 0) continue;

            void* del = *reinterpret_cast<void**>(static_cast<char*>(cached) + delegateField.offset());
            if (!Module::IsValidPointer(del)) continue;

            void* fn = *reinterpret_cast<void**>(static_cast<char*>(del) + Module::g_layoutOffsets.delegateMethodPtr);
            if (!Module::IsValidPointer(fn)) continue;

            UdonCachedExtern entry;
            entry.address = address;
            entry.delegateObject = del;
            entry.methodPointer = fn;
            if (signatureField && signatureField.offset() >= 0) {
                if (void* str = *reinterpret_cast<void**>(static_cast<char*>(cached) + signatureField.offset()))
                    entry.signature = String(str).to_string();
            }
            if (countField && countField.offset() >= 0)
                entry.parameterCount = *reinterpret_cast<int32_t*>(static_cast<char*>(cached) + countField.offset());
            out.push_back(std::move(entry));
        }
        return out;
    }

    bool UdonBehaviour::WriteHeap(uint32_t address, void* boxedValue) {
        ManagedObject heap{ GetHeap() };
        if (!heap) return false;

        static Method getType = ResolveByReturn(heap.get_class(), "GetHeapVariableType", 1, "Type");
        static Method setValue = ResolveByReturn(heap.get_class(), "SetHeapVariable", 3, "Void");
        if (!setValue) return false;

        void* typeParams[1] = { &address };
        void* type = MethodHandler::invoke<void*>(getType, heap.raw(), typeParams);
        if (!type) return false;

        void* params[3] = { &address, boxedValue, type };
        MethodHandler::invoke<void>(setValue, heap.raw(), params);
        return true;
    }

    std::string UdonBehaviour::GetHeapType(uint32_t address) const {
        ManagedObject heap{ GetHeap() };
        if (!heap) return {};
        static Method getType = ResolveByReturn(heap.get_class(), "GetHeapVariableType", 1, "Type");
        if (!getType) return {};
        void* params[1] = { &address };
        return TypeNameOf(MethodHandler::invoke<void*>(getType, heap.raw(), params));
    }

    bool UdonBehaviour::WriteHeapText(uint32_t address, std::string_view text, std::string* error) {
        auto fail = [&](const char* why) { if (error) *error = why; return false; };

        ManagedObject heap{ GetHeap() };
        if (!heap) return fail("no heap");
        static Method getType = ResolveByReturn(heap.get_class(), "GetHeapVariableType", 1, "Type");
        if (!getType) return fail("GetHeapVariableType unresolved");
        void* typeParams[1] = { &address };
        void* systemType = MethodHandler::invoke<void*>(getType, heap.raw(), typeParams);
        if (!systemType) return fail("slot has no declared type");

        Class klass = Class::from_system_type(systemType);
        const std::string type = klass ? klass.full_name() : std::string{};
        if (type.empty()) return fail("slot type is unreadable");

        if (type == "System.String") {
            void* str = String::create(std::string{ Trimmed(text) }).raw();
            if (!WriteHeap(address, str)) return fail("the heap refused the write");
            return true;
        }

        alignas(16) char buffer[16] = {};
        if (!ParseStorage(klass, text, buffer, sizeof buffer, nullptr, error)) return false;

        void* box = Module::Box(reinterpret_cast<il2cppClass*>(klass.raw()), buffer);
        if (!box) return fail("boxing failed");
        if (!WriteHeap(address, box)) return fail("the heap refused the write");
        return true;
    }

    UdonCollection UdonBehaviour::ReadHeapCollection(uint32_t address, int first, int max) const {
        UdonCollection out;
        out.first = first < 0 ? 0 : first;
        if (max <= 0) return out;

        void* obj = HeapValueObject(GetHeap(), address);
        if (!obj) return out;
        out.shape = ShapeOf(obj, out.typeName);
        if (out.shape == UdonShape::None) return out;

        Class klass = ManagedObject{ obj }.get_class();

        if (out.shape == UdonShape::Dictionary) {
            ArrayStorage st = OpenArray(ReadPointerField(obj, FieldEither(klass, "_entries", "entries")));
            if (!st.valid) return out;
            Field keyF  = st.elementClass.get_field("key");
            Field valF  = st.elementClass.get_field("value");
            Field hashF = FieldEither(st.elementClass, "hashCode", "_hashCode");
            if (!keyF || !valF) return out;
            out.keyType     = keyF.type().full_name();
            out.elementType = valF.type().full_name();
            const int keyOff = UnboxedOffset(keyF);
            const int valOff = UnboxedOffset(valF);
            const int hashOff = hashF ? UnboxedOffset(hashF) : -1;
            if (keyOff < 0 || valOff < 0) return out;

            int scan = ReadInt32Field(obj, FieldEither(klass, "_count", "count"));
            if (scan > st.length) scan = st.length;
            int live = 0;
            for (int i = 0; i < scan; ++i) {
                char* entry = st.base + static_cast<size_t>(i) * st.stride;
                // A negative hash marks a slot on the free list.
                if (hashOff >= 0 && *reinterpret_cast<int32_t*>(entry + hashOff) < 0) continue;
                if (live >= out.first && static_cast<int>(out.items.size()) < max) {
                    UdonElement el = DecodeElement(valF.type(), entry + valOff, i);
                    UdonVariable key;
                    DecodeValue(keyF.type(), entry + keyOff, key, false);
                    el.key = std::move(key.display);
                    out.items.push_back(std::move(el));
                }
                ++live;
            }
            out.count = live;
            return out;
        }

        ArrayStorage st = out.shape == UdonShape::Array
            ? OpenArray(obj)
            : OpenArray(ReadPointerField(obj, FieldEither(klass, "_items", "items")));
        if (!st.valid) return out;
        out.elementType = st.elementClass.full_name();
        out.count = st.length;
        if (out.shape == UdonShape::Array) {
            out.resizable = true;
        } else {
            const int size = ReadInt32Field(obj, FieldEither(klass, "_size", "size"));
            out.count = size < 0 ? 0 : (size < st.length ? size : st.length);
        }
        for (int i = out.first; i < out.count && static_cast<int>(out.items.size()) < max; ++i)
            out.items.push_back(DecodeElement(st.elementType, st.base + static_cast<size_t>(i) * st.stride, i));
        return out;
    }

    bool UdonBehaviour::WriteHeapElement(uint32_t address, int index, std::string_view text, std::string* error) {
        auto fail = [&](const char* why) { if (error) *error = why; return false; };
        if (index < 0) return fail("index out of range");

        void* obj = HeapValueObject(GetHeap(), address);
        if (!obj) return fail("the slot holds nothing");
        std::string typeName;
        const UdonShape shape = ShapeOf(obj, typeName);
        if (shape == UdonShape::None) return fail("the slot does not hold an array, a List or a Dictionary");

        Class klass = ManagedObject{ obj }.get_class();
        char* dst = nullptr;
        Class elementClass;

        if (shape == UdonShape::Dictionary) {
            ArrayStorage st = OpenArray(ReadPointerField(obj, FieldEither(klass, "_entries", "entries")));
            if (!st.valid) return fail("dictionary storage is unreadable");
            if (index >= st.length) return fail("index out of range");
            Field valF = st.elementClass.get_field("value");
            const int valOff = valF ? UnboxedOffset(valF) : -1;
            if (valOff < 0) return fail("dictionary entry has no value field");
            dst = st.base + static_cast<size_t>(index) * st.stride + valOff;
            elementClass = valF.type().get_class();
        } else {
            ArrayStorage st = shape == UdonShape::Array
                ? OpenArray(obj)
                : OpenArray(ReadPointerField(obj, FieldEither(klass, "_items", "items")));
            if (!st.valid) return fail("element storage is unreadable");
            int limit = st.length;
            if (shape == UdonShape::List) {
                const int size = ReadInt32Field(obj, FieldEither(klass, "_size", "size"));
                limit = size < 0 ? 0 : (size < st.length ? size : st.length);
            }
            if (index >= limit) return fail("index out of range");
            dst = st.base + static_cast<size_t>(index) * st.stride;
            elementClass = st.elementClass;
        }

        alignas(16) char buffer[16] = {};
        size_t written = 0;
        if (!ParseStorage(elementClass, text, buffer, sizeof buffer, &written, error)) return false;

        if (elementClass.full_name() == "System.String") {
            void* str = nullptr;
            std::memcpy(&str, buffer, sizeof str);
            if (!Module::SetReferenceWithWriteBarrier(reinterpret_cast<il2cppObject*>(obj),
                                                      reinterpret_cast<void**>(dst), str))
                std::memcpy(dst, &str, sizeof str);
            return true;
        }
        std::memcpy(dst, buffer, written);
        return true;
    }

    bool UdonBehaviour::ResizeHeapArray(uint32_t address, int length, std::string* error) {
        auto fail = [&](const char* why) { if (error) *error = why; return false; };
        if (length < 0) return fail("length must not be negative");

        void* obj = HeapValueObject(GetHeap(), address);
        if (!obj || !IsArrayObject(obj)) return fail("the slot does not hold an array");
        ArrayStorage st = OpenArray(obj);
        if (!st.valid) return fail("array storage is unreadable");
        if (st.length == length) return true;

        Class arrayClass = ManagedObject{ obj }.get_class();
        const uint64_t lengths[1] = { static_cast<uint64_t>(length) };
        void* fresh = Module::NewArrayFull(reinterpret_cast<il2cppClass*>(arrayClass.raw()), lengths, nullptr);
        if (!fresh) return fail("the array could not be allocated");

        ArrayStorage dst = OpenArray(fresh);
        if (!dst.valid || dst.stride != st.stride) return fail("the new array has a different layout");
        const int keep = st.length < length ? st.length : length;
        if (keep > 0) std::memcpy(dst.base, st.base, static_cast<size_t>(keep) * st.stride);

        if (!WriteHeap(address, fresh)) return fail("the heap refused the write");
        return true;
    }

    // Adding and removing goes through the collection's own methods rather than its storage:
    // a dictionary's buckets, hash codes and free list are invariants that a raw write would
    // silently break, and List keeps a count the array does not know about.
    bool UdonBehaviour::SetHeapEntry(uint32_t address, std::string_view keyText,
                                     std::string_view valueText, std::string* error) {
        auto fail = [&](const char* why) { if (error) *error = why; return false; };

        void* obj = CollectionAt(GetHeap(), address, UdonShape::Dictionary);
        if (!obj) return fail("the slot does not hold a Dictionary");

        Method setItem = ManagedObject{ obj }.get_class().get_method("set_Item", 2);
        if (!setItem) return fail("set_Item unresolved");

        // The parameter types carry the instantiation, so this works on an empty dictionary
        // that has no entry array to read the types off yet.
        InvokeArg key, value;
        if (!key.Parse(setItem.get_param_type(0).get_class(), keyText, error)) return false;
        if (!value.Parse(setItem.get_param_type(1).get_class(), valueText, error)) return false;

        void* params[2] = { key.ptr, value.ptr };
        MethodHandler::invoke<void>(setItem, obj, params);
        return true;
    }

    bool UdonBehaviour::RemoveHeapEntry(uint32_t address, std::string_view keyText, std::string* error) {
        auto fail = [&](const char* why) { if (error) *error = why; return false; };

        void* obj = CollectionAt(GetHeap(), address, UdonShape::Dictionary);
        if (!obj) return fail("the slot does not hold a Dictionary");

        Method remove = ManagedObject{ obj }.get_class().get_method("Remove", 1);
        if (!remove) return fail("Remove unresolved");

        InvokeArg key;
        if (!key.Parse(remove.get_param_type(0).get_class(), keyText, error)) return false;

        void* params[1] = { key.ptr };
        if (!MethodHandler::invoke<bool>(remove, obj, params)) return fail("no entry with that key");
        return true;
    }

    bool UdonBehaviour::AppendHeapElement(uint32_t address, std::string_view text, std::string* error) {
        auto fail = [&](const char* why) { if (error) *error = why; return false; };

        void* obj = CollectionAt(GetHeap(), address, UdonShape::List);
        if (!obj) return fail("the slot does not hold a List");

        Method add = ManagedObject{ obj }.get_class().get_method("Add", 1);
        if (!add) return fail("Add unresolved");

        InvokeArg value;
        if (!value.Parse(add.get_param_type(0).get_class(), text, error)) return false;

        void* params[1] = { value.ptr };
        MethodHandler::invoke<void>(add, obj, params);
        return true;
    }

    bool UdonBehaviour::RemoveHeapElement(uint32_t address, int index, std::string* error) {
        auto fail = [&](const char* why) { if (error) *error = why; return false; };
        if (index < 0) return fail("index out of range");

        void* obj = CollectionAt(GetHeap(), address, UdonShape::List);
        if (!obj) return fail("the slot does not hold a List");

        Class klass = ManagedObject{ obj }.get_class();
        const int size = ReadInt32Field(obj, FieldEither(klass, "_size", "size"));
        if (index >= size) return fail("index out of range");

        Method removeAt = klass.get_method("RemoveAt", 1);
        if (!removeAt) return fail("RemoveAt unresolved");

        int32_t at = index;
        void* params[1] = { &at };
        MethodHandler::invoke<void>(removeAt, obj, params);
        return true;
    }

    bool UdonBehaviour::WriteVariableText(std::string_view symbolName, std::string_view text, std::string* error) {
        uint32_t address = 0;
        if (!TryGetSymbolAddress(symbolName, address)) {
            if (error) *error = "no symbol named " + std::string{ symbolName };
            return false;
        }
        return WriteHeapText(address, text, error);
    }

    namespace UdonNetworkCalling {

        namespace {
            constexpr const char* kNetworkCalling = "VRC.SDK3.UdonNetworkCalling.NetworkCalling";
        }

        bool InNetworkCall() {
            static auto m = MethodHandler::resolve(kNetworkCalling, "get_InNetworkCall", 0);
            if (!m) return false;
            return MethodHandler::invoke<bool>(m, nullptr);
        }

        void* CallingPlayer() {
            static auto m = MethodHandler::resolve(kNetworkCalling, "get_CallingPlayer", 0);
            if (!m) return nullptr;
            return MethodHandler::invoke<void*>(m, nullptr);
        }

        int GetQueuedEvents(const UdonBehaviour& behaviour, std::string_view eventName) {
            if (!behaviour) return 0;
            static auto m = MethodHandler::resolve(kNetworkCalling, "GetQueuedEvents", 2);
            if (!m) return 0;
            auto str = String::create(eventName);
            void* params[2] = { behaviour.raw(), str.raw() };
            return MethodHandler::invoke<int32_t>(m, nullptr, params);
        }

        int GetAllQueuedEvents() {
            static auto m = MethodHandler::resolve(kNetworkCalling, "GetAllQueuedEvents", 0);
            if (!m) return 0;
            return MethodHandler::invoke<int32_t>(m, nullptr);
        }

        namespace {
            // WithNetworkCallingContext runs the action synchronously, so a thread-local hand-off
            // is enough and the delegate stays a plain static with no captured state.
            thread_local void (*t_ctxFn)(void*) = nullptr;
            thread_local void*  t_ctxUser = nullptr;

            void __fastcall CallingContextTrampoline(void*, void*) {
                if (auto* fn = t_ctxFn) fn(t_ctxUser);
            }
        }

        bool WithCallingPlayer(void* vrcPlayerApi, void (*fn)(void* user), void* user) {
            if (!fn) return false;
            static auto m = MethodHandler::resolve(kNetworkCalling, "WithNetworkCallingContext", 2);
            if (!m) return false;
            static Module::System::Action action =
                Module::System::Action::CreateNative(&CallingContextTrampoline);
            if (!action) return false;

            void (*savedFn)(void*) = t_ctxFn;
            void* savedUser = t_ctxUser;
            t_ctxFn = fn;
            t_ctxUser = user;
            void* params[2] = { vrcPlayerApi, action.raw() };
            MethodHandler::invoke<void>(m, nullptr, params);
            t_ctxFn = savedFn;
            t_ctxUser = savedUser;
            return true;
        }

    } // namespace UdonNetworkCalling

    namespace UdonEvents {

        namespace {

            struct Subscriber { uint32_t token; Kind kind; Callback callback; void* user; };

            std::mutex             g_hookMutex;
            std::vector<Subscriber> g_subscribers;
            uint32_t               g_nextToken = 1;
            bool                   g_installed[2] = { false, false };

            void Dispatch(Kind kind, void* behaviour, void* program) {
                // Copied under the lock: a callback may subscribe or unsubscribe.
                std::vector<Subscriber> snapshot;
                {
                    std::scoped_lock lock(g_hookMutex);
                    for (const auto& s : g_subscribers)
                        if (s.kind == kind) snapshot.push_back(s);
                }
                for (const auto& s : snapshot) s.callback(behaviour, program, s.user);
            }

            void __fastcall InitTrampoline(void*, void* behaviour, void* program, void*) {
                Dispatch(Kind::Init, behaviour, program);
            }

            void __fastcall SerializeTrampoline(void*, void* behaviour, void*) {
                Dispatch(Kind::RequestSerialization, behaviour, nullptr);
            }

            /// Combine a native delegate into one of UdonBehaviour's static hook properties.
            bool Install(const char* property, void* trampoline) {
                Class klass = Class::find("VRC.Udon.UdonBehaviour");
                if (!klass) return false;
                Module::Property prop = klass.get_property(property);
                if (!prop) return false;
                Method getter = prop.getter(), setter = prop.setter();
                if (!getter || !setter) return false;

                Class delegateClass = getter.return_type().get_class();
                if (!delegateClass) return false;

                Module::System::Delegate ours =
                    Module::System::Delegate::CreateNative(delegateClass, trampoline);
                if (!ours) return false;

                Module::System::Delegate existing{ MethodHandler::invoke<void*>(getter, nullptr) };
                Module::System::Delegate combined =
                    existing ? Module::System::Delegate::Combine(existing, ours) : ours;
                if (!combined) return false;

                void* params[1] = { combined.raw() };
                MethodHandler::invoke<void>(setter, nullptr, params);
                return true;
            }

        } // namespace

        uint32_t Subscribe(Kind kind, Callback callback, void* user) {
            if (!callback) return 0;
            const int slot = static_cast<int>(kind);
            if (slot < 0 || slot > 1) return 0;

            {
                std::scoped_lock lock(g_hookMutex);
                if (!g_installed[slot]) {
                    const bool ok = slot == 0
                        ? Install("OnInit", reinterpret_cast<void*>(&InitTrampoline))
                        : Install("RequestSerializationHook", reinterpret_cast<void*>(&SerializeTrampoline));
                    if (!ok) return 0;
                    g_installed[slot] = true;
                }
                const uint32_t token = g_nextToken++;
                g_subscribers.push_back({ token, kind, callback, user });
                return token;
            }
        }

        void Unsubscribe(uint32_t token) {
            std::scoped_lock lock(g_hookMutex);
            std::erase_if(g_subscribers, [token](const Subscriber& s) { return s.token == token; });
        }

    } // namespace UdonEvents

} // namespace IL2CPP::VRChat
