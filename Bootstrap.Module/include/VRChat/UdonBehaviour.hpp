#pragma once

#include "VRC_Interactable.hpp"
#include "Enums.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace IL2CPP::VRChat {

    struct UdonSymbol {
        std::string name;
        uint32_t    address = 0;
        std::string typeName;
    };

    enum class UdonValueKind : uint8_t {
        Unknown,
        Null,
        Bool,
        Integer,
        Unsigned,
        Float,
        String,
        Object,
    };

    struct UdonVariable {
        std::string   symbol;
        uint32_t      address = 0;
        std::string   typeName;
        std::string   display;
        UdonValueKind kind = UdonValueKind::Unknown;
        int64_t       integer = 0;
        double        number = 0.0;
        void*         boxed = nullptr;
        bool          initialized = false;
    };

    /// One element of a collection held in a heap slot.
    struct UdonElement {
        int           index = 0;
        std::string   key;                 // dictionaries only, the key rendered as text
        std::string   display;
        std::string   typeName;
        UdonValueKind kind = UdonValueKind::Unknown;
        bool          writable = false;    // WriteHeapElement accepts text for this element
    };

    enum class UdonShape : uint8_t { None, Array, List, Dictionary };

    /// A heap slot whose value is an array, a List&lt;T&gt; or a Dictionary&lt;K,V&gt;.
    struct UdonCollection {
        UdonShape   shape = UdonShape::None;
        std::string typeName;              // the collection's own type
        std::string elementType;           // element type, or the value type of a dictionary
        std::string keyType;               // dictionaries only
        int         count = 0;             // total elements, not just the window returned
        int         first = 0;             // index of items[0]
        bool        resizable = false;     // arrays only
        std::vector<UdonElement> items;
    };

    /// One parameter of a [NetworkCallable] entry point.
    struct UdonNetworkParam {
        std::string name;      // the parameter's declared name
        std::string symbol;    // heap symbol the receiver reads it from, when it has one
        std::string typeName;  // declared type of that symbol, when it resolved
    };

    /// An entry point VRChat will accept parameters for over the network.
    struct UdonNetworkEvent {
        std::string name;
        int         maxEventsPerSecond = 0;
        std::vector<UdonNetworkParam> parameters;
    };

    /// <summary>Box a literal as a managed value: as `typeName` when one is given, otherwise as
    /// the closest primitive the text itself names (true/false, an integer, a decimal, or a
    /// string).</summary>
    /// <param name="error">Receives why the text was refused.</param>
    /// <returns>The boxed object, or null.</returns>
    [[nodiscard]] void* BoxLiteral(std::string_view text, std::string_view typeName = {},
                                   std::string* error = nullptr);

    /// <summary>Build a System.Object[] from literals, for the parameter array Networking::RPC
    /// takes.</summary>
    /// <param name="typeNames">One managed type name per literal, or empty to let each value's
    /// text pick its type. A short list applies to the literals it covers.</param>
    /// <returns>The array, or null when a literal could not be boxed.</returns>
    [[nodiscard]] void* MakeObjectArray(const std::vector<std::string>& literals,
                                        const std::vector<std::string>& typeNames = {},
                                        std::string* error = nullptr);

    /// What VRChat's sync metadata says about one program variable.
    struct UdonSyncInfo {
        bool        synced = false;
        std::string symbol;
        std::string interpolation;  // "None", "Linear" or "Smooth"
    };

    struct UdonCachedExtern {
        uint32_t    address = 0;
        std::string signature;
        void*       delegateObject = nullptr;
        void*       methodPointer = nullptr;
        int32_t     parameterCount = 0;
    };

    class UdonBehaviour : public VRC_Interactable {
    public:
        using VRC_Interactable::VRC_Interactable;

        [[nodiscard]] bool GetSynchronizePosition();
        [[nodiscard]] bool GetSynchronizeAnimation();
        [[nodiscard]] bool GetReliable();
        [[nodiscard]] SyncType GetSyncMethod();
        [[nodiscard]] bool GetIsReady();
        [[nodiscard]] bool GetHasError();
        [[nodiscard]] bool GetInitialized();

        void SendCustomEvent(std::string_view eventName);
        void SendCustomNetworkEvent(NetworkEventTarget target, std::string_view eventName);

        /// <summary>Send a networked event carrying up to eight parameters, parsing each
        /// from text as the type the receiving entry point declares.</summary>
        /// <param name="parameterTexts">One literal per declared parameter, in order.</param>
        /// <param name="error">Receives why the send was refused.</param>
        /// <returns>False when the event takes no network parameters, the count does not
        /// match, or a value could not be parsed. Nothing is sent in that case.</returns>
        bool SendCustomNetworkEventWithParams(NetworkEventTarget target, std::string_view eventName,
                                              const std::vector<std::string>& parameterTexts,
                                              std::string* error = nullptr);

        /// <summary>List the entry points declared [NetworkCallable], with their parameters
        /// and per-second rate limit.</summary>
        [[nodiscard]] std::vector<UdonNetworkEvent> GetNetworkCallableEvents() const;

        /// <summary>Look up one entry point's network-calling metadata.</summary>
        /// <returns>False when the entry point declares none.</returns>
        [[nodiscard]] bool GetNetworkCallableEvent(std::string_view eventName, UdonNetworkEvent& out) const;

        /// <summary>Whether VRChat will deliver a networked send of this event, i.e. the
        /// program exports it or declares it [NetworkCallable]. Reserved events such as
        /// _interact are local-only however they are sent.</summary>
        [[nodiscard]] bool CanSendNetworkEvent(std::string_view eventName) const;

        /// <summary>The heap symbol an event parameter is delivered into.</summary>
        [[nodiscard]] std::string GetEventParameterName(std::string_view eventName,
                                                        std::string_view symbolName) const;

        void RequestSerialization();
        void SendCustomEventDelayedSeconds(std::string_view eventName, float delaySeconds, EventTiming eventTiming = EventTiming::Update);
        void SendCustomEventDelayedFrames(std::string_view eventName, int delayFrames, EventTiming eventTiming = EventTiming::Update);

        [[nodiscard]] std::string GetInteractionText();
        void SetInteractionText(std::string_view text);
        void Interact();
        [[nodiscard]] bool GetDisableInteractive();
        void SetDisableInteractive(bool value);

        /// <summary>Whether VRChat currently offers this behaviour for interaction, which
        /// accounts for the program declaring _interact at all, not just DisableInteractive.</summary>
        [[nodiscard]] bool GetIsInteractive();

        /// <summary>The behaviour's master switch: with event processing off it runs nothing,
        /// local or networked, and stays loaded.</summary>
        [[nodiscard]] bool GetDisableEventProcessing();
        void SetDisableEventProcessing(bool value);

        [[nodiscard]] bool GetIsNetworkingSupported();
        void SetIsNetworkingSupported(bool value);

        void SetSyncMethod(SyncType value);

        /// <summary>Run an entry point by name, bypassing the event table SendCustomEvent walks.</summary>
        void RunProgram(std::string_view eventName);

        /// <summary>Run whatever starts at this code address. No name lookup, no validation:
        /// an address that is not an entry point runs the program from the middle.</summary>
        void RunProgramAt(uint32_t entryPoint);

        /// <summary>Run an entry point locally and report whether it actually ran, which
        /// SendCustomEvent cannot: it returns nothing whether the event exists or not.</summary>
        /// <param name="canRunBeforeStart">Run it even though the behaviour has not had Start yet.</param>
        bool RunEvent(std::string_view eventName, bool canRunBeforeStart = false);

        /// <summary>The hash VRChat puts on the wire for this entry point.</summary>
        /// <returns>False when the program declares no such entry point.</returns>
        [[nodiscard]] bool TryGetEntrypointHash(std::string_view eventName, uint32_t& hash) const;

        /// <summary>Resolve an entry-point hash seen on the wire back to its name.</summary>
        /// <returns>An empty string when this program declares no entry point with that hash.</returns>
        [[nodiscard]] std::string GetEntrypointName(uint32_t hash) const;

        /// <summary>The declared type of a program variable, asked of the behaviour by symbol
        /// name rather than read off the heap slot.</summary>
        [[nodiscard]] std::string GetProgramVariableType(std::string_view symbolName) const;

        /// <summary>Run an entry point locally with named arguments, which VRChat writes into
        /// the program's own parameter symbols for the duration of the call. Unlike writing the
        /// heap first, nothing else can observe or clobber the values in between.</summary>
        /// <param name="parameters">Argument name to literal text, parsed as the receiving
        /// symbol's declared type.</param>
        /// <param name="mangleParameterNames">Map each name through GetEventParameterName, which
        /// is what an UdonSharp event parameter expects. Off passes the symbol names verbatim.</param>
        /// <param name="canRunBeforeStart">Run it even though the behaviour has not had Start yet.</param>
        /// <param name="error">Receives why the call was refused.</param>
        /// <returns>False when a name did not resolve, a value did not parse, or the event did
        /// not run. Nothing is written in the first two cases.</returns>
        bool RunEventWithParams(std::string_view eventName,
                                const std::vector<std::pair<std::string, std::string>>& parameters,
                                bool mangleParameterNames = true, bool canRunBeforeStart = false,
                                std::string* error = nullptr);

        /// <summary>Deliver an Udon input event (_inputJump, _inputUse, _inputGrab, ...) with the
        /// UdonInputEventArgs the program reads for its value and hand.</summary>
        /// <param name="handType">0 right, 1 left; VRChat's HandType.</param>
        bool RunInputEvent(std::string_view eventName, bool value, int handType = 0);
        bool RunInputEvent(std::string_view eventName, float value, int handType = 0);

        /// <summary>What the program's sync metadata says about one variable.</summary>
        [[nodiscard]] UdonSyncInfo GetSyncMetadata(std::string_view symbolName) const;

        /// <summary>Every program symbol the metadata marks as synced.</summary>
        [[nodiscard]] std::vector<UdonSyncInfo> GetSyncedVariables() const;

        void SetProgramVariable(std::string_view symbolName, void* value);
        [[nodiscard]] void* GetProgramVariable(std::string_view symbolName);
        [[nodiscard]] bool TryGetVariableValue(std::string_view symbolName, void** value);
        [[nodiscard]] bool TrySetVariableValue(std::string_view symbolName, void* value);

        /// <summary>Get the IUdonProgram this behaviour runs, or null when no program is loaded.</summary>
        [[nodiscard]] void* GetProgram() const;

        /// <summary>Get the live IUdonHeap, preferring the VM's heap over the program's.</summary>
        [[nodiscard]] void* GetHeap() const;

        /// <summary>Get the VRC.Udon.VM.UdonVM this behaviour runs its program on.</summary>
        [[nodiscard]] void* GetVM() const;

        /// <summary>The authored name of the script this behaviour runs, e.g. the UdonSharp
        /// class name, rather than the content-hashed name of the serialized asset.</summary>
        /// <returns>An empty string when neither reflection symbols nor a named program
        /// asset are available.</returns>
        [[nodiscard]] std::string GetScriptName() const;

        /// <summary>Copy the VM's processed bytecode, one entry per instruction word.</summary>
        [[nodiscard]] std::vector<uint32_t> GetByteCode() const;

        /// <summary>The number of bytecode words, without copying the array.</summary>
        [[nodiscard]] uint32_t GetByteCodeLength() const;

        /// <summary>Replace the VM's processed bytecode, reallocating when the length differs.</summary>
        /// <returns>False when the VM or its bytecode array could not be resolved.</returns>
        bool SetByteCode(const std::vector<uint32_t>& words);

        /// <summary>Get the program's instruction set identifier, e.g. "UdonVM".</summary>
        [[nodiscard]] std::string GetInstructionSetIdentifier() const;

        /// <summary>List every entry point the program declares, including internal ones.</summary>
        [[nodiscard]] std::vector<std::string> GetEventNames() const;

        /// <summary>List the entry points that are exported, i.e. callable over the network.</summary>
        [[nodiscard]] std::vector<std::string> GetNetworkedEventNames() const;

        /// <summary>Whether the program declares an entry point with this name.</summary>
        [[nodiscard]] bool HasEvent(std::string_view eventName) const;

        /// <summary>List every entry point with the code address its event starts at.</summary>
        [[nodiscard]] std::vector<UdonSymbol> GetEntryPoints() const;

        /// <summary>List every program symbol with its heap address and declared type.</summary>
        [[nodiscard]] std::vector<UdonSymbol> GetSymbols() const;

        /// <summary>List every program symbol with its heap address, without resolving a
        /// declared type per symbol. The type field of each entry is left empty.</summary>
        [[nodiscard]] std::vector<UdonSymbol> GetSymbolAddresses() const;

        /// <summary>List the exported symbols, i.e. the program's public variables.</summary>
        [[nodiscard]] std::vector<std::string> GetPublicVariableNames() const;

        /// <summary>Resolve a symbol name to its heap address.</summary>
        /// <returns>False when the program has no symbol with that name.</returns>
        [[nodiscard]] bool TryGetSymbolAddress(std::string_view symbolName, uint32_t& address) const;

        /// <summary>Get the number of addressable heap slots.</summary>
        [[nodiscard]] uint32_t GetHeapCapacity() const;

        /// <summary>Read one heap slot by address, decoding the stored value to a primitive where possible.</summary>
        [[nodiscard]] UdonVariable ReadHeap(uint32_t address) const;

        /// <summary>Read the heap slot a symbol points at.</summary>
        [[nodiscard]] UdonVariable ReadVariable(std::string_view symbolName) const;

        /// <summary>Read a chosen set of heap slots, one entry per address, decoded.</summary>
        /// <param name="addresses">Heap addresses to read; out-of-range ones come back uninitialized.</param>
        /// <param name="count">How many addresses to read.</param>
        /// <param name="stringify">Call ToString() on reference values. Off by default: it runs
        /// game code per slot and throws on a destroyed UnityEngine.Object.</param>
        [[nodiscard]] std::vector<UdonVariable> ReadHeapSlots(const uint32_t* addresses, size_t count,
                                                             bool stringify = false) const;

        /// <summary>Read the whole heap into a flat list, one entry per slot, decoded.</summary>
        /// <param name="namedOnly">Only return slots the symbol table has a name for.</param>
        [[nodiscard]] std::vector<UdonVariable> DumpHeap(bool namedOnly = false) const;

        /// <summary>Call ToString() on one heap slot's value. Runs game code on that object,
        /// so it is never safe in a loop over the heap.</summary>
        /// <returns>The text, capped in length, or a note when the slot holds nothing or a
        /// destroyed UnityEngine.Object.</returns>
        [[nodiscard]] std::string StringifyHeapValue(uint32_t address) const;

        /// <summary>List every heap slot holding an already-resolved extern delegate.</summary>
        [[nodiscard]] std::vector<UdonCachedExtern> GetCachedExterns() const;

        /// <summary>Write a boxed value into a heap slot, keeping the slot's declared type.</summary>
        /// <returns>False when the heap, the address, or the slot type could not be resolved.</returns>
        bool WriteHeap(uint32_t address, void* boxedValue);

        /// <summary>Parse text as the slot's declared type and write it into the heap.</summary>
        /// <param name="address">Heap address of the slot.</param>
        /// <param name="text">Literal text: a number, true/false, or string contents.</param>
        /// <param name="error">Receives why the write was refused.</param>
        /// <returns>False when the slot type is not a supported primitive or the text does not parse.</returns>
        bool WriteHeapText(uint32_t address, std::string_view text, std::string* error = nullptr);

        /// <summary>Resolve a symbol and write its slot from text. See WriteHeapText.</summary>
        bool WriteVariableText(std::string_view symbolName, std::string_view text, std::string* error = nullptr);

        /// <summary>The declared type of a heap slot, e.g. "System.Single".</summary>
        [[nodiscard]] std::string GetHeapType(uint32_t address) const;

        /// <summary>Decode the elements of a slot holding an array, a List or a Dictionary,
        /// so the value reads as data instead of a pointer.</summary>
        /// <param name="address">Heap address of the slot.</param>
        /// <param name="first">Index of the first element to decode.</param>
        /// <param name="max">How many elements to decode, capping the cost on a large collection.</param>
        /// <returns>A collection with shape None when the slot holds something else.</returns>
        [[nodiscard]] UdonCollection ReadHeapCollection(uint32_t address, int first = 0, int max = 256) const;

        /// <summary>Parse text as the element type and store it at one index of the collection
        /// in a heap slot. Dictionaries write the value of the entry at that index.</summary>
        /// <param name="error">Receives why the write was refused.</param>
        /// <returns>False when the slot is not a writable collection, the index is out of
        /// range, the element type is not a supported primitive, or the text does not parse.</returns>
        bool WriteHeapElement(uint32_t address, int index, std::string_view text, std::string* error = nullptr);

        /// <summary>Replace the array in a heap slot with one of a new length, keeping the
        /// elements that still fit.</summary>
        /// <returns>False when the slot does not hold an array or the array could not be made.</returns>
        bool ResizeHeapArray(uint32_t address, int length, std::string* error = nullptr);

        /// <summary>Set the entry with this key in a Dictionary in a heap slot, adding it when
        /// the key is new. Both texts are parsed as the dictionary's own key and value types.</summary>
        /// <param name="error">Receives why the write was refused.</param>
        /// <returns>False when the slot holds no Dictionary or either text does not parse.</returns>
        bool SetHeapEntry(uint32_t address, std::string_view keyText, std::string_view valueText,
                          std::string* error = nullptr);

        /// <summary>Remove the entry with this key from a Dictionary in a heap slot.</summary>
        /// <returns>False when the slot holds no Dictionary, the key does not parse, or no
        /// entry had that key.</returns>
        bool RemoveHeapEntry(uint32_t address, std::string_view keyText, std::string* error = nullptr);

        /// <summary>Append a parsed value to a List in a heap slot.</summary>
        /// <returns>False when the slot holds no List or the text does not parse.</returns>
        bool AppendHeapElement(uint32_t address, std::string_view text, std::string* error = nullptr);

        /// <summary>Remove one element of a List in a heap slot by index.</summary>
        /// <returns>False when the slot holds no List or the index is out of range.</returns>
        bool RemoveHeapElement(uint32_t address, int index, std::string* error = nullptr);
    };

    /// <summary>VRChat's networked-event plumbing, VRC.SDK3.UdonNetworkCalling.NetworkCalling.
    /// Every call here must run on the Unity main thread: the game throws otherwise.</summary>
    namespace UdonNetworkCalling {

        /// <summary>Whether the code running right now was entered from a networked event.</summary>
        [[nodiscard]] bool InNetworkCall();

        /// <summary>The VRCPlayerApi that sent the networked event being handled, or null
        /// outside one.</summary>
        [[nodiscard]] void* CallingPlayer();

        /// <summary>Sends of this event on this behaviour still waiting on the rate limiter.</summary>
        [[nodiscard]] int GetQueuedEvents(const UdonBehaviour& behaviour, std::string_view eventName);

        /// <summary>Every networked send this client has queued and not yet sent. Non-zero means
        /// the rate limiter is holding sends back, so sending harder achieves nothing.</summary>
        [[nodiscard]] int GetAllQueuedEvents();

        /// <summary>Run fn with VRChat's calling-player context set to this player, so an
        /// entry point that reads NetworkCalling.CallingPlayer sees it. Synchronous.</summary>
        /// <param name="vrcPlayerApi">The VRCPlayerApi to present as the caller.</param>
        /// <returns>False when the context could not be established; fn did not run.</returns>
        bool WithCallingPlayer(void* vrcPlayerApi, void (*fn)(void* user), void* user);

    } // namespace UdonNetworkCalling

    /// <summary>VRChat's own per-behaviour lifecycle hooks, the supported alternative to
    /// polling FindObjectsOfType for behaviours that have appeared.</summary>
    namespace UdonEvents {

        enum class Kind : int {
            Init = 0,                 // a behaviour finished loading its program
            RequestSerialization = 1, // a behaviour asked for its synced variables to be sent
        };

        /// <param name="program">The IUdonProgram, for Kind::Init only; null otherwise.</param>
        using Callback = void (*)(void* behaviour, void* program, void* user);

        /// <summary>Subscribe to one hook. The first subscription of a kind installs a delegate
        /// on VRChat's static hook; later ones cost nothing.</summary>
        /// <returns>A token for Unsubscribe, or 0 when the hook could not be installed.</returns>
        [[nodiscard]] uint32_t Subscribe(Kind kind, Callback callback, void* user);

        /// <summary>Stop delivering to one subscription. The installed delegate stays.</summary>
        void Unsubscribe(uint32_t token);

    } // namespace UdonEvents

} // namespace IL2CPP::VRChat
