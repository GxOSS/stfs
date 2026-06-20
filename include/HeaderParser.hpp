#pragma once

#include <filesystem>
#include <span>
#include <stfs.hpp>

namespace stfs {

    [[nodiscard]] Header parseHeader(std::span<const std::byte> data);

    [[nodiscard]] Header readHeaderFromFile(const std::filesystem::path& path);

} // namespace stfs