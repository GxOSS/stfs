#pragma once

#include <cstddef>
#include <cstdint>

/**
 * @file Endian.hpp
 * @brief Fixed-size endian reading utilities.
 */
namespace stfs {

    /**
     * @brief Reads a 16-bit big-endian integer.
     * @param ptr Pointer to the data.
     * @return Host-endian uint16_t value.
     */
    [[nodiscard]] inline std::uint16_t readBE16(const std::byte* ptr) noexcept {
        return (static_cast<std::uint16_t>(ptr[0]) << 8) | static_cast<std::uint16_t>(ptr[1]);
    }

    /**
     * @brief Reads a 32-bit big-endian integer.
     * @param ptr Pointer to the data.
     * @return Host-endian uint32_t value.
     */
    [[nodiscard]] inline std::uint32_t readBE32(const std::byte* ptr) noexcept {
        return (static_cast<std::uint32_t>(ptr[0]) << 24) |
               (static_cast<std::uint32_t>(ptr[1]) << 16) |
               (static_cast<std::uint32_t>(ptr[2]) << 8) | static_cast<std::uint32_t>(ptr[3]);
    }

    /**
     * @brief Reads a 64-bit big-endian integer.
     * @param ptr Pointer to the data.
     * @return Host-endian uint64_t value.
     */
    [[nodiscard]] inline std::uint64_t readBE64(const std::byte* ptr) noexcept {
        std::uint64_t result = 0;
        for (int i = 0; i < 8; ++i) {
            result = (result << 8) | static_cast<std::uint64_t>(ptr[i]);
        }
        return result;
    }

    /**
     * @brief Reads a 16-bit little-endian integer.
     * @param ptr Pointer to the data.
     * @return Host-endian uint16_t value.
     */
    [[nodiscard]] inline std::uint16_t readLE16(const std::byte* ptr) noexcept {
        return static_cast<std::uint16_t>(ptr[0]) | (static_cast<std::uint16_t>(ptr[1]) << 8);
    }

    /**
     * @brief Reads a 24-bit big-endian integer.
     * @param ptr Pointer to the data.
     * @return Host-endian uint32_t value.
     */
    [[nodiscard]] inline std::uint32_t readUInt24BE(const std::byte* ptr) noexcept {
        return (static_cast<std::uint32_t>(ptr[0]) << 16) |
               (static_cast<std::uint32_t>(ptr[1]) << 8) | static_cast<std::uint32_t>(ptr[2]);
    }

    /**
     * @brief Reads a 24-bit little-endian integer.
     * @param ptr Pointer to the data.
     * @return Host-endian uint32_t value.
     */
    [[nodiscard]] inline std::uint32_t readUInt24LE(const std::byte* ptr) noexcept {
        return static_cast<std::uint32_t>(ptr[0]) | (static_cast<std::uint32_t>(ptr[1]) << 8) |
               (static_cast<std::uint32_t>(ptr[2]) << 16);
    }

} // namespace stfs