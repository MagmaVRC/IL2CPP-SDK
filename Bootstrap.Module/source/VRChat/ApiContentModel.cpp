#include <VRChat/ApiContentModel.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>
#include <IL2CPP.Module/include/System/List.hpp>
#include <string_view>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::MethodHandler;

    namespace {
        // These properties belong to VRC.Core.ApiContentModel`1, but that name
        // resolves to the generic type *definition*, whose methods are not callable
        // -- invoking one throws "Invalid call to method". The callable methods live
        // on the inflated instance, which is the concrete subclass's parent, so
        // resolve against the object's own class and let il2cpp walk up to it.
        IL2CPP::Module::Method getter(const IL2CPP::Module::ManagedObject& obj, std::string_view name) {
            if (!obj) return IL2CPP::Module::Method{};
            return obj.get_class().get_method(name, 0);
        }

        std::string get_string(const IL2CPP::Module::ManagedObject& obj, std::string_view name) {
            if (!obj) return "";
            void* str = MethodHandler::invoke<void*>(getter(obj, name), obj.raw());
            return str ? IL2CPP::Module::System::String(str).to_string() : "";
        }

        std::vector<std::string> get_string_list(const IL2CPP::Module::ManagedObject& obj,
                                                 std::string_view name) {
            std::vector<std::string> result;
            if (!obj) return result;
            auto* listPtr = MethodHandler::invoke<void*>(getter(obj, name), obj.raw());
            if (!listPtr) return result;
            IL2CPP::Module::System::List<void*> list(listPtr);
            int cnt = list.count();
            if (cnt > 0) result.reserve(static_cast<size_t>(cnt));
            for (int i = 0; i < cnt; ++i) {
                auto* elemPtr = list.try_at(static_cast<uintptr_t>(i));
                if (elemPtr && *elemPtr) {
                    result.push_back(IL2CPP::Module::System::String(*elemPtr).to_string());
                }
            }
            return result;
        }
    }

    std::string ApiContentModel::GetName() { return get_string(*this, IL2CPP_STR("get_name")); }

    std::string ApiContentModel::GetDescription() { return get_string(*this, IL2CPP_STR("get_description")); }

    std::string ApiContentModel::GetAuthorId() { return get_string(*this, IL2CPP_STR("get_authorId")); }

    std::string ApiContentModel::GetAuthorName() { return get_string(*this, IL2CPP_STR("get_authorName")); }

    std::string ApiContentModel::GetImageUrl() { return get_string(*this, IL2CPP_STR("get_imageUrl")); }

    std::string ApiContentModel::GetThumbnailImageUrl() {
        return get_string(*this, IL2CPP_STR("get_thumbnailImageUrl"));
    }

    std::string ApiContentModel::GetReleaseStatus() {
        return get_string(*this, IL2CPP_STR("get_releaseStatus"));
    }

    int ApiContentModel::GetVersion() {
        if (!valid()) return 0;
        return MethodHandler::invoke<int>(getter(*this, IL2CPP_STR("get_version")), raw());
    }

    std::vector<std::string> ApiContentModel::GetTags() {
        return get_string_list(*this, IL2CPP_STR("get_tags"));
    }

    std::string ApiContentModel::GetAssetUrl() { return get_string(*this, IL2CPP_STR("get_assetUrl")); }

    std::string ApiContentModel::GetUnityVersion() {
        return get_string(*this, IL2CPP_STR("get_unityVersion"));
    }

} // namespace IL2CPP::VRChat
