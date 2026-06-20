#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <span>
#include <variant>

namespace stfs {

    enum class Magic {
        CON,
        PIRS,
        LIVE
    };

    struct ConSignature {
        std::uint16_t public_key_certificate_size;
        std::array<std::byte, 5> certificate_owner_console_id;
        std::array<char, 0x14> certificate_owner_console_part_number;
        std::uint8_t certificate_owner_console_type;
        std::array<char, 8> certificate_date_of_generation;
        std::array<std::byte, 4> public_exponent;
        std::array<std::byte, 0x80> public_modulus;
        std::array<std::byte, 0x100> certificate_signature;
        std::array<std::byte, 0x80> signature;
    };

    struct LiveSignature {
        std::array<std::byte, 0x100> package_signature;
        std::array<std::byte, 0x128> padding;
    };

    using Signature = std::variant<ConSignature, LiveSignature>;

    struct Header {
        Magic magic;
        Signature signature;
    };

    Header parseHeader(std::span<const std::byte> data);
    Header readHeaderFromFile(const std::filesystem::path& path);

} // namespace stfs