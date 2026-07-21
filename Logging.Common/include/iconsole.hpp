#pragma once
#include <string_view>
#include <string>
#include <cstddef>
#include <cstdint>
#include <atomic>
#include "excon_protocol.hpp"

namespace Logger {

    class IConsole {
    public:
        virtual ~IConsole() = default;

        virtual void Write(std::string_view text) = 0;
        virtual void SetTitle(std::wstring_view title) = 0;
        virtual size_t GetWidth() = 0;
        virtual bool IsConnected() = 0;
        virtual void Shutdown() = 0;
        virtual ExCon::Capabilities GetCapabilities() const { return ExCon::Capabilities::none; }
        virtual void SetSessionMetadata(const ExCon::SessionMetadata&) {}
        virtual void ConfigureTerminal(const ExCon::TerminalConfig&) {}
        virtual bool ReadInput(std::string&) { return false; }
    };

    inline std::atomic<IConsole*> g_console{ nullptr };

} // namespace Logger
