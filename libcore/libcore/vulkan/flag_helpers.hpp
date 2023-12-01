/**
 * @file libcore/libcore/vulkan/flag_helpers.hpp
 * @brief Utility function to improve the ergonomics of working the Vulkan api flags.
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

#include "libcore/vulkan/include.hpp"

#include <type_traits>
#include <vulkan/vulkan_enums.hpp>

namespace core::vk
{
    /**
     * @brief Check if a flag contains the bit pattern of another flag.
     *
     * @param[in] value The value to check.
     * @param[in] target_pattern The target bit pattern to search for in value.
     *
     * @returns True if value contains the target_pattern bits, otherwise false.
     */
    template<class BitType>
        requires(std::is_scoped_enum_v<BitType>)
    auto has_bits(::vk::Flags<BitType> value, ::vk::Flags<BitType> target_pattern) -> bool
    {
        return (value & target_pattern) == target_pattern;
    }

    /**
     * @brief Check if a flag contains a specific bit.
     *
     * @param[in] value The value to check.
     * @param[in] target The target bit to search for in value.
     *
     * @returns True if the value contains the target bit, otherwise false.
     */
    template<class BitType>
        requires(std::is_scoped_enum_v<BitType>)
    auto has_bit(::vk::Flags<BitType> value, BitType target) -> bool
    {
        return (value & target) == target;
    }
} // namespace core::vk
