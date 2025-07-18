/*
 * Copyright (C) 2018-present ScyllaDB
 */

/*
 * SPDX-License-Identifier: LicenseRef-ScyllaDB-Source-Available-1.0
 */

#include "version.hh"
#include "sstable_version.hh"
#include "sstable_version_k_l.hh"
#include "sstable_version_m.hh"

namespace sstables {

template <size_t S1, size_t S2>
constexpr bool check_sstable_versions(const std::array<sstable_version_types, S1>& all_sstable_versions,
        const std::array<sstable_version_types, S2>& writable_sstable_versions, sstable_version_types oldest_writable_sstable_format) {
    for (auto v : writable_sstable_versions) {
        if (v < oldest_writable_sstable_format) {
            return false;
        }
    }
    size_t expected = 0;
    for (auto v : all_sstable_versions) {
        if (v >= oldest_writable_sstable_format) {
            ++expected;
        }
    }
    return expected == S2;
}

static_assert(check_sstable_versions(all_sstable_versions, writable_sstable_versions, oldest_writable_sstable_format));

const sstring sstable_version_constants::TOC_SUFFIX = "TOC.txt";
const sstring sstable_version_constants::TEMPORARY_TOC_SUFFIX = "TOC.txt.tmp";

sstable_version_constants::component_map_t sstable_version_constants::create_component_map() {
    return {
        { component_type::Index, "Index.db"},
        { component_type::CompressionInfo, "CompressionInfo.db" },
        { component_type::Data, "Data.db" },
        { component_type::TOC, TOC_SUFFIX },
        { component_type::Summary, "Summary.db" },
        { component_type::CRC, "CRC.db" },
        { component_type::Filter, "Filter.db" },
        { component_type::Statistics, "Statistics.db" },
        { component_type::Scylla, "Scylla.db" },
        { component_type::TemporaryTOC, TEMPORARY_TOC_SUFFIX },
        { component_type::TemporaryStatistics, "Statistics.db.tmp" },
    };
}

const sstable_version_constants::component_map_t&
sstable_version_constants::get_component_map(sstable_version_types version) {
    switch (version) {
        case sstable_version_types::ka:
        case sstable_version_types::la:
            return sstable_version_constants_k_l::_component_map;
        case sstable_version_types::mc:
        case sstable_version_types::md:
        case sstable_version_types::me:
            return sstable_version_constants_m::_component_map;
    }
    // Should never reach this.
    // Compiler should complain if the switch above does no cover all sstable_version_types values.
    throw std::invalid_argument("Invalid sstable format version");
}

const sstable_version_constants::component_map_t sstable_version_constants_k_l::create_component_map() {
    auto result = sstable_version_constants::create_component_map();
    result.emplace(component_type::Digest, "Digest.sha1");
    return result;
}

const sstable_version_constants::component_map_t sstable_version_constants_k_l::_component_map =
        sstable_version_constants_k_l::create_component_map();

const sstable_version_constants::component_map_t sstable_version_constants_m::create_component_map() {
    auto result = sstable_version_constants::create_component_map();
    result.emplace(component_type::Digest, "Digest.crc32");
    return result;
}

const sstable_version_constants::component_map_t sstable_version_constants_m::_component_map =
        sstable_version_constants_m::create_component_map();

}
