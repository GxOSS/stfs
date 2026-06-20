#pragma once

#include <span>
#include <stfs.hpp>

namespace stfs {

    [[nodiscard]] Metadata parseMetadata(std::span<const std::byte> data);

} // namespace stfs