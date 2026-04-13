#pragma once
#include <cstdint>

#define UNIX_SDK_MAJOR 6
#define UNIX_SDK_MINOR 0
#define UNIX_SDK_PATCH 0

#define _UNIX_STR(x) #x
#define _UNIX_XSTR(x) _UNIX_STR(x)
#define UNIX_SDK_MAJOR_STR _UNIX_XSTR(UNIX_SDK_MAJOR)

namespace Bootstrap {

    constexpr uint32_t sdk_major = UNIX_SDK_MAJOR;
    constexpr uint32_t sdk_minor = UNIX_SDK_MINOR;
    constexpr uint32_t sdk_patch = UNIX_SDK_PATCH;
    constexpr uint32_t sdk_version = sdk_major;

} // namespace Bootstrap
