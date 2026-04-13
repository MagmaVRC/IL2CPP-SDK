#include <VRChat/UdonBehaviour.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>
#include <IL2CPP.Module/include/ManagedObject.hpp>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

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
        auto str = IL2CPP::Module::System::String::create(eventName);
        void* params[1] = { str.raw() };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::SendCustomNetworkEvent(NetworkEventTarget target, std::string_view eventName) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "SendCustomNetworkEvent", 2);
        auto str = IL2CPP::Module::System::String::create(eventName);
        int iTarget = static_cast<int>(target);
        void* params[2] = { &iTarget, str.raw() };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::RequestSerialization() {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "RequestSerialization", 0);
        MethodHandler::invoke<void>(m, raw());
    }

    void UdonBehaviour::SendCustomEventDelayedSeconds(std::string_view eventName, float delaySeconds, EventTiming eventTiming) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "SendCustomEventDelayedSeconds", 3);
        auto str = IL2CPP::Module::System::String::create(eventName);
        int iTiming = static_cast<int>(eventTiming);
        void* params[3] = { str.raw(), &delaySeconds, &iTiming };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::SendCustomEventDelayedFrames(std::string_view eventName, int delayFrames, EventTiming eventTiming) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "SendCustomEventDelayedFrames", 3);
        auto str = IL2CPP::Module::System::String::create(eventName);
        int iTiming = static_cast<int>(eventTiming);
        void* params[3] = { str.raw(), &delayFrames, &iTiming };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    std::string UdonBehaviour::GetInteractionText() {
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "get_InteractionText", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    void UdonBehaviour::SetInteractionText(std::string_view text) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "set_InteractionText", 1);
        auto str = IL2CPP::Module::System::String::create(text);
        void* params[1] = { str.raw() };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void UdonBehaviour::SetProgramVariable(std::string_view symbolName, void* value) {
        if (!valid()) return;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "SetProgramVariable", 2);
        auto str = IL2CPP::Module::System::String::create(symbolName);
        void* params[2] = { str.raw(), value };
        MethodHandler::invoke<void>(m, raw(), params);
    }

    void* UdonBehaviour::GetProgramVariable(std::string_view symbolName) {
        if (!valid()) return nullptr;
        static auto m = MethodHandler::resolve("VRC.Udon.UdonBehaviour", "GetProgramVariable", 1);
        auto str = IL2CPP::Module::System::String::create(symbolName);
        void* params[1] = { str.raw() };
        return MethodHandler::invoke<void*>(m, raw(), params);
    }

    bool UdonBehaviour::TryGetVariableValue(std::string_view symbolName, void** value) {
        if (!valid() || !value) return false;
        auto* table = get_field<void*>("publicVariables");
        if (!table) return false;
        IL2CPP::Module::ManagedObject tableObj(table);
        auto str = IL2CPP::Module::System::String::create(symbolName);
        void* params[2] = { str.raw(), value };
        return tableObj.call_method<bool>("TryGetVariableValue", params, 2);
    }

    bool UdonBehaviour::TrySetVariableValue(std::string_view symbolName, void* value) {
        if (!valid()) return false;
        auto* table = get_field<void*>("publicVariables");
        if (!table) return false;
        IL2CPP::Module::ManagedObject tableObj(table);
        auto str = IL2CPP::Module::System::String::create(symbolName);
        void* params[2] = { str.raw(), value };
        return tableObj.call_method<bool>("TrySetVariableValue", params, 2);
    }

} // namespace IL2CPP::VRChat
