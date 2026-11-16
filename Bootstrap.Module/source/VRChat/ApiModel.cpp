#include <VRChat/ApiModel.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

    std::string ApiModel::GetId() {
        if (!valid()) return "";
        static auto m = MethodHandler::resolve("VRC.Core.ApiModel", "get_id", 0);
        void* str = MethodHandler::invoke<void*>(m, raw());
        return str ? IL2CPP::Module::System::String(str).to_string() : "";
    }

    bool ApiModel::GetPopulated() {
        if (!valid()) return false;
        static auto m = MethodHandler::resolve(IL2CPP_STR("VRC.Core.ApiModel"),
                                               IL2CPP_STR("get_Populated"), 0);
        return MethodHandler::invoke<bool>(m, raw());
    }

    int32_t ApiModel::GetSupportedPlatforms() {
        if (!valid()) return 0;
        static IL2CPP::Module::Field field;
        if (!field)
            field = IL2CPP::Module::Class::find(IL2CPP_STR("VRC.Core.ApiModel"))
                        .get_field(IL2CPP_STR("supportedPlatforms"));
        return get_field<int32_t>(field);
    }

} // namespace IL2CPP::VRChat
