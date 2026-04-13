#include <VRChat/ApiBadge.hpp>

namespace IL2CPP::VRChat {

    std::string ApiBadge::GetBadgeId() {
        return get_string_field("badgeId");
    }

    bool ApiBadge::GetShowcased() {
        if (!valid()) return false;
        return get_field<bool>("showcased");
    }

    std::string ApiBadge::GetBadgeName() {
        return get_string_field("badgeName");
    }

    std::string ApiBadge::GetBadgeDescription() {
        return get_string_field("badgeDescription");
    }

    std::string ApiBadge::GetBadgeImageUrl() {
        return get_string_field("badgeImageUrl");
    }

} // namespace IL2CPP::VRChat
