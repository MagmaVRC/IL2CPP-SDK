#pragma once

#include <Bootstrap.Common/include/sdk_version.hpp>

/// Internal helper to concatenate the packed string at compile time.
#define _UNIX_MODULE_INFO_CONCAT(name, version) \
    "UNIx\x01" name "\0" version "\0" UNIX_SDK_MAJOR_STR

/// Declare module metadata visible to UNIxLoader.
/// Must appear exactly once per module DLL, at file scope.
///   UNIX_MODULE_INFO("MyMod", "1.0.0");
#define UNIX_MODULE_INFO(name, version) \
    extern "C" __declspec(dllexport) \
    const char _CLUE_[] = _UNIX_MODULE_INFO_CONCAT(name, version)
