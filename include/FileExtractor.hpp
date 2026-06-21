#pragma once

#include <Commons.hpp>
#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>


namespace stfs {

    [[nodiscard]] std::vector<std::uint32_t> followBlockChain(std::span<const std::byte> package,
                                                              std::uint32_t starting_block,
                                                              std::uint32_t header_size);

    [[nodiscard]] std::vector<std::byte> extractFile(std::span<const std::byte> package,
                                                     const FileEntry& entry,
                                                     std::uint32_t header_size);

    void extractFileToDisk(std::span<const std::byte> package, const FileEntry& entry,
                           std::uint32_t header_size, const std::filesystem::path& output_path);

} // namespace stfs