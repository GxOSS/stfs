#pragma once

#include <span>
#include <stfs.hpp>
#include <vector>

namespace stfs {

    [[nodiscard]] std::vector<FileEntry> parseFileListing(std::span<const std::byte> data);

} // namespace stfs