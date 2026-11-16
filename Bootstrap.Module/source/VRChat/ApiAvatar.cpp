#include <VRChat/ApiAvatar.hpp>
#include <IL2CPP.Module/include/MethodHandler.hpp>
#include <IL2CPP.Module/include/System/String.hpp>

#include <mutex>

namespace IL2CPP::VRChat {

    using IL2CPP::Module::Class;
    using IL2CPP::Module::Field;
    using IL2CPP::Module::MethodHandler;

    namespace {

        struct PlatformFlag {
            std::string name;
            int32_t     value = 0;
        };

        // Single-bit members of SupportedPlatforms, read from metadata so a renumbering
        // or a new platform never needs a code change. None and All are not single bits.
        const std::vector<PlatformFlag>& AvatarPlatformFlags() {
            static std::vector<PlatformFlag> flags;
            static std::mutex mutex;
            std::lock_guard lock(mutex);
            if (!flags.empty()) return flags;
            const Field supported = Class::find(IL2CPP_STR("VRC.Core.ApiModel"))
                                        .get_field(IL2CPP_STR("supportedPlatforms"));
            if (!supported) return flags;
            const Class enumClass = supported.type().get_class();
            if (!enumClass) return flags;
            for (const auto& f : enumClass.get_fields()) {
                if (!f.is_static() || !f.name()) continue;
                int32_t value = 0;
                f.get_static_value(&value);
                if (value <= 0 || (value & (value - 1)) != 0) continue;
                flags.push_back({ f.name(), value });
            }
            return flags;
        }

        int64_t AvatarDateUnix(const IL2CPP::Module::ManagedObject& obj, std::string_view field) {
            const uint64_t raw = obj.get_field<uint64_t>(field);
            const int64_t ticks = static_cast<int64_t>(raw & 0x3FFFFFFFFFFFFFFFull);
            constexpr int64_t kUnixEpochTicks = 621355968000000000ll;
            if (ticks <= kUnixEpochTicks) return 0;
            return (ticks - kUnixEpochTicks) / 10000000ll;
        }

    } // namespace

    std::vector<std::pair<std::string, std::string>> ApiAvatar::GetPlatformRatings() {
        std::vector<std::pair<std::string, std::string>> out;
        if (!valid()) return out;
        const int32_t mask = GetSupportedPlatforms();
        if (mask == 0) return out;
        const auto rating = get_class().get_method(IL2CPP_STR("GetPerformanceRatingForPlatform"), 1);
        for (const auto& flag : AvatarPlatformFlags()) {
            if ((mask & flag.value) == 0) continue;
            std::string value;
            if (rating) {
                int32_t arg = flag.value;
                void* params[] = { &arg };
                void* str = MethodHandler::invoke<void*>(rating, raw(), params);
                if (str) value = IL2CPP::Module::System::String(str).to_string();
            }
            out.emplace_back(flag.name, std::move(value));
        }
        return out;
    }

    int64_t ApiAvatar::GetCreatedAtUnix() {
        if (!valid()) return 0;
        return AvatarDateUnix(*this, IL2CPP_STR("<created_at>k__BackingField"));
    }

    int64_t ApiAvatar::GetUpdatedAtUnix() {
        if (!valid()) return 0;
        return AvatarDateUnix(*this, IL2CPP_STR("<updated_at>k__BackingField"));
    }

} // namespace IL2CPP::VRChat
