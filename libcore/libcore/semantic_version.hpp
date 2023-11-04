/**
 * @file libcore/libcore/semantic_version.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 09/22/2023
 * @brief Implements the concept of semantic versioning
 * @copyright Copyright 2023 wmbat
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#pragma once

#include <fmt/format.h>

#include <cstdint>

namespace core
{
    /**
     * @brief Represent a format convention for determining a version encoded using a major, minor
     * and patch.
     */
    struct semantic_version
    {
        /**
         * @brief The major number of the version.
         */
        std::uint32_t major;
        /**
         * @brief The minor number of the version.
         */
        std::uint32_t minor;
        /**
         * @brief The patch number of the version.
         */
        std::uint32_t patch;

        constexpr auto operator==(semantic_version const& rhs) const -> bool = default;
        constexpr auto operator<=>(semantic_version const& rhs) const = default;
    };
} // namespace core

/**
 * @brief Custom formatter for \ref core::semantic_version
 */
template<>
struct fmt::formatter<core::semantic_version> : fmt::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(core::semantic_version const& version, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}.{}.{}", version.major, version.minor, version.patch);
    }
};
