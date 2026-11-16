#pragma once

#include <IL2CPP.Module/include/il2cpp_module.hpp>
#include <string>

namespace IL2CPP::VRChat {

    class ApiModel : public IL2CPP::Module::ManagedObject {
    public:
        using ManagedObject::ManagedObject;

        [[nodiscard]] std::string GetId();

        /// <summary>True once the record holds real data. VRChat caches a model
        /// before filling it in, so an unpopulated object carries only an id.</summary>
        [[nodiscard]] bool GetPopulated();

        /// <summary>SupportedPlatforms flags VRChat derived from every unity package the API
        /// returned, before it narrowed the model to the running platform.</summary>
        /// <returns>The raw flag word; 0 when unpopulated.</returns>
        [[nodiscard]] int32_t GetSupportedPlatforms();
    };

} // namespace IL2CPP::VRChat
