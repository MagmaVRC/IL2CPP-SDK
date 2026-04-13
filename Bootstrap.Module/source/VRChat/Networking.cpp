#include <VRChat/Networking.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

    IL2CPP::Module::Class Networking::klass() {
        static auto c = IL2CPP::Module::Class::find("VRC.SDKBase.Networking");
        return c;
    }

    bool Networking::IsNetworkSettled() {
        auto k = klass();
        if (!k) return false;
        auto f = k.get_field("IsNetworkSettled");
        if (!f) return false;
        bool val = false;
        f.get_static_value(&val);
        return val;
    }

    bool Networking::IsMaster() {
        auto k = klass();
        if (!k) return false;
        auto f = k.get_field("IsMaster");
        if (!f) return false;
        bool val = false;
        f.get_static_value(&val);
        return val;
    }

    bool Networking::IsClogged() {
        auto k = klass();
        if (!k) return false;
        auto f = k.get_field("IsClogged");
        if (!f) return false;
        bool val = false;
        f.get_static_value(&val);
        return val;
    }

    bool Networking::IsInstanceOwner() {
        auto k = klass();
        if (!k) return false;
        auto f = k.get_field("IsInstanceOwner");
        if (!f) return false;
        bool val = false;
        f.get_static_value(&val);
        return val;
    }

    VRCPlayerApi Networking::GetLocalPlayer() {
        auto k = klass();
        if (!k) return {};
        auto f = k.get_field("LocalPlayer");
        if (!f) return {};
        void* val = nullptr;
        f.get_static_value(&val);
        return VRCPlayerApi(val);
    }

    VRCPlayerApi Networking::GetMaster() {
        auto k = klass();
        if (!k) return {};
        auto f = k.get_field("Master");
        if (!f) return {};
        void* val = nullptr;
        f.get_static_value(&val);
        return VRCPlayerApi(val);
    }

    double Networking::GetServerTimeInSeconds() {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetServerTimeInSeconds", 0);
        return MethodHandler::invoke<double>(m, nullptr);
    }

    int Networking::GetServerTimeInMilliseconds() {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetServerTimeInMilliseconds", 0);
        return MethodHandler::invoke<int>(m, nullptr);
    }

    bool Networking::GoToRoom(const std::string& roomID) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GoToRoom", 1);
        auto str = Module::System::String::create(roomID.c_str());
        if (!str) return false;
        void* params[1] = { str.raw() };
        return MethodHandler::invoke<bool>(m, nullptr, params);
    }

    bool Networking::IsOwner(VRCPlayerApi player, IL2CPP::Module::Unity::GameObject obj) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "IsOwner", 2);
        void* params[2] = { player.raw(), obj.raw() };
        return MethodHandler::invoke<bool>(m, nullptr, params);
    }

    bool Networking::IsOwner(IL2CPP::Module::Unity::GameObject obj) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "IsOwner", 1);
        void* params[1] = { obj.raw() };
        return MethodHandler::invoke<bool>(m, nullptr, params);
    }

    VRCPlayerApi Networking::GetOwner(IL2CPP::Module::Unity::GameObject obj) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetOwner", 1);
        void* params[1] = { obj.raw() };
        void* result = MethodHandler::invoke<void*>(m, nullptr, params);
        return result ? VRCPlayerApi(result) : VRCPlayerApi();
    }

    void Networking::SetOwner(VRCPlayerApi player, IL2CPP::Module::Unity::GameObject obj) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "SetOwner", 2);
        void* params[2] = { player.raw(), obj.raw() };
        MethodHandler::invoke<void>(m, nullptr, params);
    }

    VRCPlayerApi Networking::GetInstanceOwnerPlayer() {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "get_InstanceOwner", 0);
        void* result = MethodHandler::invoke<void*>(m, nullptr);
        return result ? VRCPlayerApi(result) : VRCPlayerApi();
    }

    int64_t Networking::GetNetworkDateTime() {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetNetworkDateTime", 0);
        return MethodHandler::invoke<int64_t>(m, nullptr);
    }

    void Networking::RPC(RpcDestination targetClients, IL2CPP::Module::Unity::GameObject targetObject,
                         std::string_view methodName, void* parameters) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "RPC", 4);
        auto str = IL2CPP::Module::System::String::create(methodName);
        int dest = static_cast<int>(targetClients);
        void* params[4] = { &dest, targetObject.raw(), str.raw(), parameters };
        MethodHandler::invoke<void>(m, nullptr, params);
    }

    void Networking::RPC(VRCPlayerApi targetPlayer, IL2CPP::Module::Unity::GameObject targetObject,
                         std::string_view methodName, void* parameters) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "RPC", 4);
        auto str = IL2CPP::Module::System::String::create(methodName);
        void* params[4] = { targetPlayer.raw(), targetObject.raw(), str.raw(), parameters };
        MethodHandler::invoke<void>(m, nullptr, params);
    }

    IL2CPP::Module::ManagedObject Networking::GetEventDispatcher() {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetEventDispatcher", 0);
        void* result = MethodHandler::invoke<void*>(m, nullptr);
        return IL2CPP::Module::ManagedObject(result);
    }

    double Networking::CalculateServerDeltaTime(double timeInSeconds, double previousTimeInSeconds) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "CalculateServerDeltaTime", 2);
        void* params[2] = { &timeInSeconds, &previousTimeInSeconds };
        return MethodHandler::invoke<double>(m, nullptr, params);
    }

    bool Networking::IsObjectReady(IL2CPP::Module::Unity::GameObject obj) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "IsObjectReady", 1);
        void* params[1] = { obj.raw() };
        return MethodHandler::invoke<bool>(m, nullptr, params);
    }

    std::string Networking::GetUniqueName(IL2CPP::Module::Unity::GameObject obj) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetUniqueName", 1);
        void* params[1] = { obj.raw() };
        void* str = MethodHandler::invoke<void*>(m, nullptr, params);
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    float Networking::SimulationTime(IL2CPP::Module::Unity::GameObject target) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "SimulationTime", 1);
        void* params[1] = { target.raw() };
        return MethodHandler::invoke<float>(m, nullptr, params);
    }

    float Networking::SimulationTime(VRCPlayerApi target) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "SimulationTime", 1);
        void* params[1] = { target.raw() };
        return MethodHandler::invoke<float>(m, nullptr, params);
    }

    void Networking::Destroy(IL2CPP::Module::Unity::GameObject obj) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "Destroy", 1);
        void* params[1] = { obj.raw() };
        MethodHandler::invoke<void>(m, nullptr, params);
    }

    void Networking::RequestStorageUsageUpdate() {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "RequestStorageUsageUpdate", 0);
        MethodHandler::invoke<void>(m, nullptr);
    }

    int Networking::GetPlayerDataStorageLimit() {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetPlayerDataStorageLimit", 0);
        return MethodHandler::invoke<int>(m, nullptr);
    }

    int Networking::GetPlayerObjectStorageLimit() {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetPlayerObjectStorageLimit", 0);
        return MethodHandler::invoke<int>(m, nullptr);
    }

    int Networking::GetPlayerDataStorageUsage(VRCPlayerApi target) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetPlayerDataStorageUsage", 1);
        void* params[1] = { target.raw() };
        return MethodHandler::invoke<int>(m, nullptr, params);
    }

    int Networking::GetPlayerObjectStorageUsage(VRCPlayerApi target) {
        static auto m = MethodHandler::resolve("VRC.SDKBase.Networking", "GetPlayerObjectStorageUsage", 1);
        void* params[1] = { target.raw() };
        return MethodHandler::invoke<int>(m, nullptr, params);
    }

} // namespace IL2CPP::VRChat
