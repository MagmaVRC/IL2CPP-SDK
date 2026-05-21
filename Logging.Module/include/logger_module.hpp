#pragma once
#include <Logging.Common/logger_shared.hpp>
#include <string_view>
#include <format>
#include <source_location>

namespace Logger::Module {

    class Submodule;

    class Logger {
    public:
        Logger() = default;
        explicit Logger(std::string_view name);
        ~Logger();

        Logger(Logger const&) = delete;
        Logger& operator=(Logger const&) = delete;
        Logger(Logger&& other) noexcept;
        Logger& operator=(Logger&& other) noexcept;

        [[nodiscard]] bool valid() const noexcept;
        [[nodiscard]] uint32_t id() const noexcept;
        [[nodiscard]] std::string_view name() const noexcept;

        [[nodiscard]] Submodule create_submodule(std::string_view name);

        void set_mask(LevelMask mask);
        void set_color(Color fg, Color bg = Color::default_color);

        [[nodiscard]] bool is_enabled(Level lvl) const noexcept;

        void log(Level lvl, std::string_view message, std::source_location loc = std::source_location::current()) const;

        template<typename... Args>
        void log(Level lvl, std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(lvl)) return;
            log(lvl, std::vformat(_fmt, std::make_format_args(args...)));
        }

        void trace(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void debug(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void info(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void warn(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void error(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void fatal(std::string_view msg, std::source_location loc = std::source_location::current()) const;

        template<typename... Args>
        void trace(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::trace)) return;
            log(Level::trace, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void debug(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::debug)) return;
            log(Level::debug, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void info(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::info)) return;
            log(Level::info, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void warn(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::warn)) return;
            log(Level::warn, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void error(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::error)) return;
            log(Level::error, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void fatal(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::fatal)) return;
            log(Level::fatal, std::vformat(_fmt, std::make_format_args(args...)));
        }

    private:
        uint32_t    m_id = invalid_id;
        std::string m_name;

        friend class Submodule;
    };

    class Submodule {
    public:
        Submodule() = default;
        Submodule(Logger const& parent, std::string_view name);
        ~Submodule();

        Submodule(Submodule const&) = delete;
        Submodule& operator=(Submodule const&) = delete;
        Submodule(Submodule&& other) noexcept;
        Submodule& operator=(Submodule&& other) noexcept;

        [[nodiscard]] bool valid() const noexcept;
        [[nodiscard]] ModuleId id() const noexcept;
        [[nodiscard]] std::string_view name() const noexcept;

        void set_mask(LevelMask mask);
        void set_color(Color fg, Color bg = Color::default_color);

        [[nodiscard]] bool is_enabled(Level lvl) const noexcept;

        void log(Level lvl, std::string_view message, std::source_location loc = std::source_location::current()) const;

        template<typename... Args>
        void log(Level lvl, std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(lvl)) return;
            log(lvl, std::vformat(_fmt, std::make_format_args(args...)));
        }

        void trace(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void debug(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void info(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void warn(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void error(std::string_view msg, std::source_location loc = std::source_location::current()) const;
        void fatal(std::string_view msg, std::source_location loc = std::source_location::current()) const;

        template<typename... Args>
        void trace(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::trace)) return;
            log(Level::trace, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void debug(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::debug)) return;
            log(Level::debug, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void info(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::info)) return;
            log(Level::info, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void warn(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::warn)) return;
            log(Level::warn, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void error(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::error)) return;
            log(Level::error, std::vformat(_fmt, std::make_format_args(args...)));
        }

        template<typename... Args>
        void fatal(std::string_view _fmt, Args&&... args) const {
            if (!is_enabled(Level::fatal)) return;
            log(Level::fatal, std::vformat(_fmt, std::make_format_args(args...)));
        }

    private:
        uint32_t    m_module_id = invalid_id;
        uint32_t    m_submodule_id = invalid_id;
        std::string m_name;
    };

    [[nodiscard]] bool Connect();
    void Disconnect();
    [[nodiscard]] bool IsConnected() noexcept;
    void flush();

}
