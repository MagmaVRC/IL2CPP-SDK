#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace Logger::ExCon {

inline constexpr std::uint32_t kMagic = 0x4E435845;
inline constexpr std::uint16_t kProtocolMajor = 2;
inline constexpr std::uint16_t kProtocolMinor = 0;
inline constexpr std::uint64_t kMaxPayloadSize = 1024 * 1024;
inline constexpr std::uint32_t kMaxStringSize = 64 * 1024;
inline constexpr std::uint32_t kMaxRepeatedFieldCount = 4096;
inline constexpr std::uint16_t kMaxHangingIndent = 256;
inline constexpr std::uint32_t kMaxHistorySize = 10'000;

static_assert(std::endian::native == std::endian::little);

enum class MessageType : std::uint16_t {
    ready = 0x0001,
    stdout_data = 0x0002,
    stderr_data = 0x0003,
    session_metadata = 0x0004,
    set_title = 0x0005,
    configure = 0x0006,
    shutdown = 0x0007,
    input_data = 0x0008,
    resize = 0x0009,
    cmd_complete_req = 0x000A,
    cmd_complete_resp = 0x000B,
    cmd_schema_req = 0x000C,
    cmd_schema_resp = 0x000D
};

enum class Capabilities : std::uint64_t {
    none = 0,
    stdin_input = 1ull << 0,
    stderr_stream = 1ull << 1,
    alternate_screen = 1ull << 2,
    resize_events = 1ull << 3,
    dynamic_metadata = 1ull << 4
};

[[nodiscard]] constexpr Capabilities operator|(Capabilities left, Capabilities right) noexcept {
    return static_cast<Capabilities>(static_cast<std::uint64_t>(left) |
                                     static_cast<std::uint64_t>(right));
}

[[nodiscard]] constexpr Capabilities operator&(Capabilities left, Capabilities right) noexcept {
    return static_cast<Capabilities>(static_cast<std::uint64_t>(left) &
                                     static_cast<std::uint64_t>(right));
}

constexpr Capabilities& operator|=(Capabilities& left, Capabilities right) noexcept {
    left = left | right;
    return left;
}

enum class TimestampPreference : std::uint8_t {
    none = 0,
    time = 1,
    time_milliseconds = 2,
    date_time = 3,
    date_time_milliseconds = 4,
    iso_8601 = 5
};

#pragma pack(push, 1)
struct FrameHeader {
    std::uint32_t magic;
    std::uint16_t protocol_major;
    std::uint16_t protocol_minor;
    MessageType message_type;
    std::uint16_t flags;
    std::uint32_t request_id;
    std::uint64_t payload_size;
};
#pragma pack(pop)

static_assert(sizeof(FrameHeader) == 24);
static_assert(std::is_trivially_copyable_v<FrameHeader>);
static_assert(offsetof(FrameHeader, magic) == 0);
static_assert(offsetof(FrameHeader, protocol_major) == 4);
static_assert(offsetof(FrameHeader, protocol_minor) == 6);
static_assert(offsetof(FrameHeader, message_type) == 8);
static_assert(offsetof(FrameHeader, flags) == 10);
static_assert(offsetof(FrameHeader, request_id) == 12);
static_assert(offsetof(FrameHeader, payload_size) == 16);

enum class HeaderDisposition {
    accept,
    drain_extension,
    reject
};

[[nodiscard]] constexpr bool IsCoreMessageType(MessageType type) noexcept {
    const auto value = static_cast<std::uint16_t>(type);
    return value >= static_cast<std::uint16_t>(MessageType::ready) &&
           value <= static_cast<std::uint16_t>(MessageType::cmd_schema_resp);
}

[[nodiscard]] constexpr bool IsBootstrapToConsole(MessageType type) noexcept {
    return type == MessageType::stdout_data || type == MessageType::stderr_data ||
           type == MessageType::session_metadata || type == MessageType::set_title ||
           type == MessageType::configure || type == MessageType::shutdown ||
           type == MessageType::cmd_complete_resp || type == MessageType::cmd_schema_resp;
}

[[nodiscard]] constexpr bool IsConsoleToBootstrap(MessageType type) noexcept {
    return type == MessageType::ready || type == MessageType::input_data ||
           type == MessageType::resize || type == MessageType::cmd_complete_req ||
           type == MessageType::cmd_schema_req;
}

[[nodiscard]] constexpr HeaderDisposition IsValidHeader(const FrameHeader& header) noexcept {
    if (header.magic != kMagic || header.protocol_major != kProtocolMajor ||
        header.payload_size > kMaxPayloadSize) {
        return HeaderDisposition::reject;
    }
    if (IsCoreMessageType(header.message_type)) {
        return HeaderDisposition::accept;
    }
    const auto value = static_cast<std::uint16_t>(header.message_type);
    return value >= 0x8000 && value <= 0xFFFE
               ? HeaderDisposition::drain_extension
               : HeaderDisposition::reject;
}

[[nodiscard]] constexpr bool CheckedAdd(std::size_t left, std::size_t right,
                                        std::size_t& result) noexcept {
    if (right > (std::numeric_limits<std::size_t>::max)() - left) {
        return false;
    }
    result = left + right;
    return true;
}

class PayloadWriter {
public:
    [[nodiscard]] bool WriteU8(std::uint8_t value) { return WriteScalar(value); }
    [[nodiscard]] bool WriteU16(std::uint16_t value) { return WriteScalar(value); }
    [[nodiscard]] bool WriteU32(std::uint32_t value) { return WriteScalar(value); }
    [[nodiscard]] bool WriteU64(std::uint64_t value) { return WriteScalar(value); }

    [[nodiscard]] bool WriteString(std::string_view value) {
        if (value.size() > kMaxStringSize) {
            return false;
        }
        std::size_t required{};
        if (!CheckedAdd(sizeof(std::uint32_t), value.size(), required) || !CanAppend(required)) {
            return false;
        }
        const auto length = static_cast<std::uint32_t>(value.size());
        AppendUnchecked(&length, sizeof(length));
        AppendUnchecked(value.data(), value.size());
        return true;
    }

    [[nodiscard]] bool WriteCount(std::uint32_t count) {
        return count <= kMaxRepeatedFieldCount && WriteU32(count);
    }

    [[nodiscard]] const std::vector<std::byte>& Data() const noexcept { return data_; }
    [[nodiscard]] std::vector<std::byte> Take() noexcept { return std::move(data_); }

private:
    template <typename T>
    [[nodiscard]] bool WriteScalar(T value) {
        static_assert(std::is_integral_v<T>);
        if (!CanAppend(sizeof(value))) {
            return false;
        }
        AppendUnchecked(&value, sizeof(value));
        return true;
    }

    [[nodiscard]] bool CanAppend(std::size_t size) const noexcept {
        return size <= kMaxPayloadSize && data_.size() <= kMaxPayloadSize - size;
    }

    void AppendUnchecked(const void* source, std::size_t size) {
        if (size == 0) {
            return;
        }
        const auto* first = static_cast<const std::byte*>(source);
        data_.insert(data_.end(), first, first + size);
    }

    std::vector<std::byte> data_;
};

class PayloadReader {
public:
    explicit PayloadReader(std::span<const std::byte> data) noexcept : data_(data) {}

    [[nodiscard]] bool ReadU8(std::uint8_t& value) { return ReadScalar(value); }
    [[nodiscard]] bool ReadU16(std::uint16_t& value) { return ReadScalar(value); }
    [[nodiscard]] bool ReadU32(std::uint32_t& value) { return ReadScalar(value); }
    [[nodiscard]] bool ReadU64(std::uint64_t& value) { return ReadScalar(value); }

    [[nodiscard]] bool ReadString(std::string& value) {
        std::uint32_t length{};
        if (!ReadU32(length) || length > kMaxStringSize || length > Remaining()) {
            return false;
        }
        value.assign(reinterpret_cast<const char*>(data_.data() + offset_), length);
        offset_ += length;
        return true;
    }

    [[nodiscard]] bool ReadCount(std::uint32_t& count) {
        return ReadU32(count) && count <= kMaxRepeatedFieldCount;
    }

    [[nodiscard]] std::size_t Remaining() const noexcept { return data_.size() - offset_; }
    [[nodiscard]] bool AtEnd() const noexcept { return offset_ == data_.size(); }

private:
    template <typename T>
    [[nodiscard]] bool ReadScalar(T& value) {
        static_assert(std::is_integral_v<T>);
        if (sizeof(value) > Remaining()) {
            return false;
        }
        std::memcpy(&value, data_.data() + offset_, sizeof(value));
        offset_ += sizeof(value);
        return true;
    }

    std::span<const std::byte> data_;
    std::size_t offset_{};
};

struct SessionMetadata {
    std::string product_name;
    std::string unity_version;
    std::string bootstrap_version;
    std::string build_number;
    std::uint32_t process_id{};

    bool operator==(const SessionMetadata&) const = default;
};

struct TerminalConfig {
    bool alternate_screen{true};
    bool wrap{true};
    std::uint16_t hanging_indent{4};
    std::uint32_t history_size{256};
    TimestampPreference timestamp{TimestampPreference::time};

    bool operator==(const TerminalConfig&) const = default;
};

struct Ready {
    Capabilities capabilities{Capabilities::none};
    std::uint64_t max_payload_size{kMaxPayloadSize};
    std::uint32_t width{120};
    std::uint32_t height{30};

    bool operator==(const Ready&) const = default;
};

struct ResizeEvent {
    std::uint32_t width{120};
    std::uint32_t height{30};

    bool operator==(const ResizeEvent&) const = default;
};

[[nodiscard]] constexpr bool IsValid(TimestampPreference preference) noexcept {
    return static_cast<std::uint8_t>(preference) <=
           static_cast<std::uint8_t>(TimestampPreference::iso_8601);
}

[[nodiscard]] inline bool Encode(const SessionMetadata& value, std::vector<std::byte>& output) {
    PayloadWriter writer;
    if (!writer.WriteString(value.product_name) || !writer.WriteString(value.unity_version) ||
        !writer.WriteString(value.bootstrap_version) || !writer.WriteString(value.build_number) ||
        !writer.WriteU32(value.process_id)) {
        return false;
    }
    output = writer.Take();
    return true;
}

[[nodiscard]] inline bool Decode(std::span<const std::byte> input, SessionMetadata& value) {
    PayloadReader reader(input);
    SessionMetadata decoded;
    if (!reader.ReadString(decoded.product_name) || !reader.ReadString(decoded.unity_version) ||
        !reader.ReadString(decoded.bootstrap_version) || !reader.ReadString(decoded.build_number) ||
        !reader.ReadU32(decoded.process_id) || !reader.AtEnd()) {
        return false;
    }
    value = std::move(decoded);
    return true;
}

[[nodiscard]] inline bool Encode(const TerminalConfig& value, std::vector<std::byte>& output) {
    if (!IsValid(value.timestamp) || value.hanging_indent > kMaxHangingIndent ||
        value.history_size > kMaxHistorySize) {
        return false;
    }
    PayloadWriter writer;
    if (!writer.WriteU8(value.alternate_screen ? 1 : 0) || !writer.WriteU8(value.wrap ? 1 : 0) ||
        !writer.WriteU16(value.hanging_indent) || !writer.WriteU32(value.history_size) ||
        !writer.WriteU8(static_cast<std::uint8_t>(value.timestamp))) {
        return false;
    }
    output = writer.Take();
    return true;
}

[[nodiscard]] inline bool Decode(std::span<const std::byte> input, TerminalConfig& value) {
    PayloadReader reader(input);
    std::uint8_t alternate{};
    std::uint8_t wrap{};
    std::uint8_t timestamp{};
    TerminalConfig decoded;
    if (!reader.ReadU8(alternate) || alternate > 1 || !reader.ReadU8(wrap) || wrap > 1 ||
        !reader.ReadU16(decoded.hanging_indent) || !reader.ReadU32(decoded.history_size) ||
        !reader.ReadU8(timestamp) || !reader.AtEnd()) {
        return false;
    }
    decoded.alternate_screen = alternate != 0;
    decoded.wrap = wrap != 0;
    decoded.timestamp = static_cast<TimestampPreference>(timestamp);
    if (!IsValid(decoded.timestamp) || decoded.hanging_indent > kMaxHangingIndent ||
        decoded.history_size > kMaxHistorySize) {
        return false;
    }
    value = decoded;
    return true;
}

[[nodiscard]] inline bool Encode(const Ready& value, std::vector<std::byte>& output) {
    if (value.max_payload_size == 0 || value.max_payload_size > kMaxPayloadSize ||
        value.width == 0 || value.height == 0) {
        return false;
    }
    PayloadWriter writer;
    if (!writer.WriteU64(static_cast<std::uint64_t>(value.capabilities)) ||
        !writer.WriteU64(value.max_payload_size) || !writer.WriteU32(value.width) ||
        !writer.WriteU32(value.height)) {
        return false;
    }
    output = writer.Take();
    return true;
}

[[nodiscard]] inline bool Decode(std::span<const std::byte> input, Ready& value) {
    PayloadReader reader(input);
    std::uint64_t capabilities{};
    Ready decoded;
    if (!reader.ReadU64(capabilities) || !reader.ReadU64(decoded.max_payload_size) ||
        !reader.ReadU32(decoded.width) || !reader.ReadU32(decoded.height) ||
        !reader.AtEnd() || decoded.max_payload_size == 0 ||
        decoded.max_payload_size > kMaxPayloadSize || decoded.width == 0 || decoded.height == 0) {
        return false;
    }
    decoded.capabilities = static_cast<Capabilities>(capabilities);
    value = decoded;
    return true;
}

[[nodiscard]] inline bool Encode(const ResizeEvent& value, std::vector<std::byte>& output) {
    if (value.width == 0 || value.height == 0) {
        return false;
    }
    PayloadWriter writer;
    if (!writer.WriteU32(value.width) || !writer.WriteU32(value.height)) {
        return false;
    }
    output = writer.Take();
    return true;
}

[[nodiscard]] inline bool Decode(std::span<const std::byte> input, ResizeEvent& value) {
    PayloadReader reader(input);
    ResizeEvent decoded;
    if (!reader.ReadU32(decoded.width) || !reader.ReadU32(decoded.height) ||
        !reader.AtEnd() || decoded.width == 0 || decoded.height == 0) {
        return false;
    }
    value = decoded;
    return true;
}

struct CompletionRequest {
    std::string line;
    std::uint32_t cursor{};

    bool operator==(const CompletionRequest&) const = default;
};

struct CompletionItem {
    std::string text;
    std::uint8_t kind{};

    bool operator==(const CompletionItem&) const = default;
};

struct CompletionResponse {
    std::uint32_t replace_start{};
    std::uint32_t replace_end{};
    std::vector<CompletionItem> items;

    bool operator==(const CompletionResponse&) const = default;
};

[[nodiscard]] inline bool Encode(const CompletionRequest& value, std::vector<std::byte>& output) {
    PayloadWriter writer;
    if (!writer.WriteString(value.line) || !writer.WriteU32(value.cursor)) return false;
    output = writer.Take();
    return true;
}

[[nodiscard]] inline bool Decode(std::span<const std::byte> input, CompletionRequest& value) {
    PayloadReader reader(input);
    CompletionRequest decoded;
    if (!reader.ReadString(decoded.line) || !reader.ReadU32(decoded.cursor) || !reader.AtEnd()) {
        return false;
    }
    value = std::move(decoded);
    return true;
}

[[nodiscard]] inline bool Encode(const CompletionResponse& value, std::vector<std::byte>& output) {
    if (value.items.size() > kMaxRepeatedFieldCount) return false;
    PayloadWriter writer;
    if (!writer.WriteU32(value.replace_start) || !writer.WriteU32(value.replace_end) ||
        !writer.WriteCount(static_cast<std::uint32_t>(value.items.size()))) {
        return false;
    }
    for (const auto& item : value.items) {
        if (!writer.WriteString(item.text) || !writer.WriteU8(item.kind)) return false;
    }
    output = writer.Take();
    return true;
}

[[nodiscard]] inline bool Decode(std::span<const std::byte> input, CompletionResponse& value) {
    PayloadReader reader(input);
    CompletionResponse decoded;
    std::uint32_t count{};
    if (!reader.ReadU32(decoded.replace_start) || !reader.ReadU32(decoded.replace_end) ||
        !reader.ReadCount(count)) {
        return false;
    }
    decoded.items.resize(count);
    for (auto& item : decoded.items) {
        if (!reader.ReadString(item.text) || !reader.ReadU8(item.kind)) return false;
    }
    if (!reader.AtEnd()) return false;
    value = std::move(decoded);
    return true;
}

// Canonical argument type. Kept here so ExCon (out-of-process) can decode the
// schema payload without pulling in Bootstrap headers.
enum class ArgType : std::uint8_t {
    String = 0,
    Int    = 1,
    Float  = 2,
    Bool   = 3,
    Enum   = 4,
    Module = 5,
    Hook   = 6,
    Path   = 7
};

struct ArgSchema {
    std::string name;
    std::uint8_t type{};
    std::uint8_t required{};
    std::string help;
    std::vector<std::string> enum_values;

    bool operator==(const ArgSchema&) const = default;
};

struct CommandSchema {
    std::string name;
    std::string help;
    std::vector<ArgSchema> args;

    bool operator==(const CommandSchema&) const = default;
};

struct CommandSchemaResponse {
    std::vector<CommandSchema> commands;

    bool operator==(const CommandSchemaResponse&) const = default;
};

[[nodiscard]] inline bool Encode(const CommandSchemaResponse& value, std::vector<std::byte>& output) {
    if (value.commands.size() > kMaxRepeatedFieldCount) return false;
    PayloadWriter writer;
    if (!writer.WriteCount(static_cast<std::uint32_t>(value.commands.size()))) return false;
    for (const auto& cmd : value.commands) {
        if (cmd.args.size() > kMaxRepeatedFieldCount) return false;
        if (!writer.WriteString(cmd.name) || !writer.WriteString(cmd.help) ||
            !writer.WriteCount(static_cast<std::uint32_t>(cmd.args.size()))) return false;
        for (const auto& a : cmd.args) {
            if (a.enum_values.size() > kMaxRepeatedFieldCount) return false;
            if (!writer.WriteString(a.name) || !writer.WriteU8(a.type) ||
                !writer.WriteU8(a.required) || !writer.WriteString(a.help) ||
                !writer.WriteCount(static_cast<std::uint32_t>(a.enum_values.size()))) return false;
            for (const auto& v : a.enum_values) if (!writer.WriteString(v)) return false;
        }
    }
    output = writer.Take();
    return true;
}

[[nodiscard]] inline bool Decode(std::span<const std::byte> input, CommandSchemaResponse& value) {
    PayloadReader reader(input);
    CommandSchemaResponse decoded;
    std::uint32_t cmd_count{};
    if (!reader.ReadCount(cmd_count)) return false;
    decoded.commands.resize(cmd_count);
    for (auto& cmd : decoded.commands) {
        std::uint32_t arg_count{};
        if (!reader.ReadString(cmd.name) || !reader.ReadString(cmd.help) ||
            !reader.ReadCount(arg_count)) return false;
        cmd.args.resize(arg_count);
        for (auto& a : cmd.args) {
            std::uint32_t enum_count{};
            if (!reader.ReadString(a.name) || !reader.ReadU8(a.type) ||
                !reader.ReadU8(a.required) || !reader.ReadString(a.help) ||
                !reader.ReadCount(enum_count)) return false;
            a.enum_values.resize(enum_count);
            for (auto& v : a.enum_values) if (!reader.ReadString(v)) return false;
        }
    }
    if (!reader.AtEnd()) return false;
    value = std::move(decoded);
    return true;
}

}
