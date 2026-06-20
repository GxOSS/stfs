#pragma once

#include <cstddef>
#include <cstdint>

namespace stfs {

    [[nodiscard]] inline std::uint16_t readBE16(const std::byte* ptr) noexcept {
        return (static_cast<std::uint16_t>(ptr[0]) << 8) | static_cast<std::uint16_t>(ptr[1]);
    }

    [[nodiscard]] inline std::uint32_t readBE32(const std::byte* ptr) noexcept {
        return (static_cast<std::uint32_t>(ptr[0]) << 24) |
               (static_cast<std::uint32_t>(ptr[1]) << 16) |
               (static_cast<std::uint32_t>(ptr[2]) << 8) | static_cast<std::uint32_t>(ptr[3]);
    }

    [[nodiscard]] inline std::uint64_t readBE64(const std::byte* ptr) noexcept {
        std::uint64_t result = 0;
        for (int i = 0; i < 8; ++i) {
            result = (result << 8) | static_cast<std::uint64_t>(ptr[i]);
        }
        return result;
    }

    [[nodiscard]] inline std::uint32_t readUInt24BE(const std::byte* ptr) noexcept {
        return (static_cast<std::uint32_t>(ptr[0]) << 16) |
               (static_cast<std::uint32_t>(ptr[1]) << 8) | static_cast<std::uint32_t>(ptr[2]);
    }

    [[nodiscard]] inline std::uint32_t readUInt24LE(const std::byte* ptr) noexcept {
        return static_cast<std::uint32_t>(ptr[0]) | (static_cast<std::uint32_t>(ptr[1]) << 8) |
               (static_cast<std::uint32_t>(ptr[2]) << 16);
    }

} // namespace stfs