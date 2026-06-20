#include <array>
#include <bit>
#include <cstring>
#include <endian.hpp>
#include <fstream>
#include <stfs.hpp>

namespace stfs {

    namespace {

        std::u8string readLocaleString(const std::byte* ptr, std::size_t max_bytes) {
            const auto* char_ptr = reinterpret_cast<const char8_t*>(ptr);
            std::size_t len = 0;
            while (len < max_bytes && char_ptr[len] != u8'\0') {
                ++len;
            }
            return std::u8string(char_ptr, len);
        }

        StfsVolumeDescriptor parseStfsVolumeDescriptor(const std::byte* ptr) {
            StfsVolumeDescriptor vd;
            vd.size = static_cast<std::uint8_t>(ptr[0x00]);
            vd.block_separation = static_cast<std::uint8_t>(ptr[0x02]);
            vd.file_table_block_count =
                static_cast<std::int16_t>((static_cast<std::uint16_t>(ptr[0x03])) |
                                          (static_cast<std::uint16_t>(ptr[0x04]) << 8));
            vd.file_table_block_number =
                static_cast<std::int32_t>(static_cast<std::uint32_t>(ptr[0x05]) |
                                          (static_cast<std::uint32_t>(ptr[0x06]) << 8) |
                                          (static_cast<std::uint32_t>(ptr[0x07]) << 16));
            std::memcpy(vd.top_hash_table_hash.data(), ptr + 0x08, 0x14);
            vd.total_allocated_block_count = static_cast<std::int32_t>(readBE32(ptr + 0x1C));
            vd.total_unallocated_block_count = static_cast<std::int32_t>(readBE32(ptr + 0x20));
            return vd;
        }

        SvodVolumeDescriptor parseSvodVolumeDescriptor(const std::byte* ptr) {
            SvodVolumeDescriptor vd;
            vd.size = static_cast<std::uint8_t>(ptr[0x00]);
            vd.block_cache_element_count = static_cast<std::uint8_t>(ptr[0x01]);
            vd.worker_thread_processor = static_cast<std::uint8_t>(ptr[0x02]);
            vd.worker_thread_priority = static_cast<std::uint8_t>(ptr[0x03]);
            std::memcpy(vd.hash.data(), ptr + 0x04, 0x14);
            vd.device_features = static_cast<std::uint8_t>(ptr[0x18]);
            vd.data_block_count = readUInt24BE(ptr + 0x19);
            vd.data_block_offset = readUInt24BE(ptr + 0x1C);
            return vd;
        }

        std::vector<LicenseEntry> parseLicenseEntries(const std::byte* ptr) {
            std::vector<LicenseEntry> entries;
            constexpr std::size_t entry_size = 0x10;
            constexpr std::size_t entry_count = 0x100 / entry_size;

            for (std::size_t i = 0; i < entry_count; ++i) {
                const auto* entry_ptr = ptr + i * entry_size;
                std::uint64_t license_id = readBE64(entry_ptr + 0x0);

                if (license_id == 0) {
                    continue;
                }

                LicenseEntry entry;
                entry.license_id = static_cast<std::int64_t>(license_id);
                entry.license_bits = static_cast<std::int32_t>(readBE32(entry_ptr + 0x8));
                entry.license_flags = static_cast<std::int32_t>(readBE32(entry_ptr + 0xC));
                entries.push_back(entry);
            }

            return entries;
        }

    } // namespace

    class HeaderParser {
      public:
        static Header parse(std::span<const std::byte> data) {
            if (data.size() < 0x1AC) {
                throw std::runtime_error("Insufficient data for header parsing");
            }

            Header header;
            header.magic = parseMagic(data);

            switch (header.magic) {
                case Magic::CON:
                    header.signature = parseConSignature(data);
                    break;
                case Magic::PIRS:
                case Magic::LIVE:
                    header.signature = parseLiveSignature(data);
                    break;
            }

            return header;
        }

      private:
        static Magic parseMagic(std::span<const std::byte> data) {
            std::array<char, 4> magic_bytes;
            std::memcpy(magic_bytes.data(), data.data(), 4);

            if (magic_bytes[0] == 'C' && magic_bytes[1] == 'O' && magic_bytes[2] == 'N' &&
                magic_bytes[3] == ' ') {
                return Magic::CON;
            } else if (magic_bytes[0] == 'P' && magic_bytes[1] == 'I' && magic_bytes[2] == 'R' &&
                       magic_bytes[3] == 'S') {
                return Magic::PIRS;
            } else if (magic_bytes[0] == 'L' && magic_bytes[1] == 'I' && magic_bytes[2] == 'V' &&
                       magic_bytes[3] == 'E') {
                return Magic::LIVE;
            }

            throw std::runtime_error("Invalid magic bytes");
        }

        static ConSignature parseConSignature(std::span<const std::byte> data) {
            ConSignature sig;

            const auto* ptr = data.data();

            sig.public_key_certificate_size = *reinterpret_cast<const std::uint16_t*>(ptr + 0x004);

            std::memcpy(sig.certificate_owner_console_id.data(), ptr + 0x006, 5);

            std::memcpy(sig.certificate_owner_console_part_number.data(), ptr + 0x00B, 0x14);

            sig.certificate_owner_console_type =
                *reinterpret_cast<const std::uint8_t*>(ptr + 0x01F);

            std::memcpy(sig.certificate_date_of_generation.data(), ptr + 0x020, 8);

            std::memcpy(sig.public_exponent.data(), ptr + 0x028, 4);

            std::memcpy(sig.public_modulus.data(), ptr + 0x02C, 0x80);

            std::memcpy(sig.certificate_signature.data(), ptr + 0x0AC, 0x100);

            std::memcpy(sig.signature.data(), ptr + 0x1AC, 0x80);

            return sig;
        }

        static LiveSignature parseLiveSignature(std::span<const std::byte> data) {
            LiveSignature sig;

            const auto* ptr = data.data();

            std::memcpy(sig.package_signature.data(), ptr + 0x004, 0x100);

            std::memcpy(sig.padding.data(), ptr + 0x104, 0x128);

            return sig;
        }
    };

    Header parseHeader(std::span<const std::byte> data) {
        return HeaderParser::parse(data);
    }

    Header readHeaderFromFile(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + path.string());
        }

        file.seekg(0, std::ios::end);
        std::size_t file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (file_size < 0x1AC) {
            throw std::runtime_error("File too small for header");
        }

        std::vector<std::byte> buffer(0x1AC);
        file.read(reinterpret_cast<char*>(buffer.data()), 0x1AC);

        return HeaderParser::parse(buffer);
    }

    Metadata parseMetadata(std::span<const std::byte> data) {
        if (data.size() < 0x571A + 0x4000) {
            throw std::runtime_error("Insufficient data for metadata parsing (v1 assumed)");
        }

        const auto* base = data.data();
        Metadata meta;

        meta.license_entries = parseLicenseEntries(base + 0x022C);

        std::memcpy(meta.header_sha1.data(), base + 0x032C, 0x14);
        meta.header_size = readBE32(base + 0x0340);
        meta.content_type = static_cast<ContentType>(readBE32(base + 0x0344));
        meta.metadata_version = static_cast<std::int32_t>(readBE32(base + 0x0348));
        meta.content_size = static_cast<std::int64_t>(readBE64(base + 0x034C));
        meta.media_id = readBE32(base + 0x0354);
        meta.version = static_cast<std::int32_t>(readBE32(base + 0x0358));
        meta.base_version = static_cast<std::int32_t>(readBE32(base + 0x035C));
        meta.title_id = readBE32(base + 0x0360);
        meta.platform = static_cast<Platform>(static_cast<std::uint8_t>(base[0x0364]));
        meta.executable_type = static_cast<std::uint8_t>(base[0x0365]);
        meta.disc_number = static_cast<std::uint8_t>(base[0x0366]);
        meta.disc_in_set = static_cast<std::uint8_t>(base[0x0367]);
        meta.save_game_id = readBE32(base + 0x0368);

        std::memcpy(meta.console_id.data(), base + 0x036C, 5);
        std::memcpy(meta.profile_id.data(), base + 0x0371, 8);

        auto descriptor_type_raw = readBE32(base + 0x03A9);
        meta.descriptor_type = static_cast<DescriptorType>(descriptor_type_raw);

        if (meta.descriptor_type == DescriptorType::Svod) {
            meta.volume_descriptor = parseSvodVolumeDescriptor(base + 0x0379);
        } else {
            meta.volume_descriptor = parseStfsVolumeDescriptor(base + 0x0379);
        }

        meta.data_file_count = static_cast<std::int32_t>(readBE32(base + 0x039D));
        meta.data_file_combined_size = static_cast<std::int64_t>(readBE64(base + 0x03A1));

        if (meta.metadata_version == 2) {
            MetadataV2Extra extra;
            std::memcpy(extra.series_id.data(), base + 0x03B1, 0x10);
            std::memcpy(extra.season_id.data(), base + 0x03C1, 0x10);
            extra.season_number = static_cast<std::int16_t>(readBE16(base + 0x03D1));
            extra.episode_number = static_cast<std::int16_t>(readBE16(base + 0x03D3));
            meta.v2_extra = extra;
        }

        std::memcpy(meta.device_id.data(), base + 0x03FD, 0x14);

        meta.display_name = readLocaleString(base + 0x0411, 0x900);
        meta.display_description = readLocaleString(base + 0x0D11, 0x900);
        meta.publisher_name = readLocaleString(base + 0x1611, 0x80);
        meta.title_name = readLocaleString(base + 0x1691, 0x80);

        meta.transfer_flags = static_cast<std::uint8_t>(base[0x1711]);
        meta.thumbnail_image_size = static_cast<std::int32_t>(readBE32(base + 0x1712));
        meta.title_thumbnail_image_size = static_cast<std::int32_t>(readBE32(base + 0x1716));

        auto thumb_size = static_cast<std::size_t>(meta.thumbnail_image_size);
        if (thumb_size > 0x4000)
            thumb_size = 0x4000;
        meta.thumbnail_image.assign(base + 0x171A, base + 0x171A + thumb_size);

        auto title_thumb_size = static_cast<std::size_t>(meta.title_thumbnail_image_size);
        if (title_thumb_size > 0x4000)
            title_thumb_size = 0x4000;
        meta.title_thumbnail_image.assign(base + 0x571A, base + 0x571A + title_thumb_size);

        return meta;
    }

    std::vector<FileEntry> parseFileListing(std::span<const std::byte> data) {
        std::vector<FileEntry> entries;
        constexpr std::size_t entry_size = 0x40;

        if (data.size() % entry_size != 0) {
            throw std::runtime_error("File listing size not aligned to entry size");
        }

        std::size_t entry_count = data.size() / entry_size;
        const auto* base = data.data();

        for (std::size_t i = 0; i < entry_count; ++i) {
            const auto* ptr = base + i * entry_size;

            bool all_zero = true;
            for (std::size_t b = 0; b < entry_size; ++b) {
                if (ptr[b] != std::byte{0}) {
                    all_zero = false;
                    break;
                }
            }
            if (all_zero) {
                break;
            }

            FileEntry entry;

            std::uint8_t flags = static_cast<std::uint8_t>(ptr[0x28]);
            std::uint8_t name_length = flags & 0x3F;

            entry.name.assign(reinterpret_cast<const char*>(ptr), name_length);
            entry.flags = flags;

            entry.blocks_allocated = readUInt24LE(ptr + 0x29);
            entry.blocks_allocated_copy = readUInt24LE(ptr + 0x2C);
            entry.starting_block = readUInt24LE(ptr + 0x2F);
            entry.path_indicator = static_cast<std::int16_t>(readBE16(ptr + 0x32));
            entry.file_size = readBE32(ptr + 0x34);
            entry.update_timestamp = readBE32(ptr + 0x38);
            entry.access_timestamp = readBE32(ptr + 0x3C);

            entries.push_back(std::move(entry));
        }

        return entries;
    }

} // namespace stfs