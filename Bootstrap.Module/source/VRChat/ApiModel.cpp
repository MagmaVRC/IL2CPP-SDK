#include <VRChat/ApiModel.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

    std::string ApiModel::GetId() {
        static auto m = MethodHandler::resolve("VRC.Core.ApiModel", "get_id", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

} // namespace IL2CPP::VRChat
