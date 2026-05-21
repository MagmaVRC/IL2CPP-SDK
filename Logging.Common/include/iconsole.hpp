#pragma once
#include <string_view>
#include <cstdint>
#include <atomic>

namespace Logger {

    struct ConsolePacket {
        enum Op : uint8_t {
            op_write    = 0,
            op_title    = 1,
            op_width    = 2,
            op_shutdown = 0xFF
        };

        Op       opcode;
        uint32_t length;
    };

    class IConsole {
    public:
        virtual ~IConsole() = default;

        virtual void Write(std::string_view text) = 0;
        virtual void SetTitle(std::wstring_view title) = 0;
        virtual size_t GetWidth() = 0;
        virtual bool IsConnected() = 0;
        virtual void Shutdown() = 0;
    };

    inline std::atomic<IConsole*> g_console{ nullptr };

} // namespace Logger
