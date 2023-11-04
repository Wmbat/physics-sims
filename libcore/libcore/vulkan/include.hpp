/**
 * @file libcore/libcore/vulkan/include.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 09/22/2023
 * @brief Contains helper functions for vulkan and is the central inclusion point for the vulkan library
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

#include <libcore/semantic_version.hpp>

#if !defined(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC)
#    define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdeprecated-copy"
#    pragma clang diagnostic ignored "-Wdefaulted-function-deleted"
#elif defined(__GNUC__) || defined(__GNUG__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wconversion"
#endif

#if !defined(VULKAN_HPP_NO_EXCEPTIONS)
#    define VULKAN_HPP_NO_EXCEPTIONS
#endif
#include <vulkan/vulkan.hpp>

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#    pragma GCC diagnostic pop
#endif

#include <system_error>

namespace core::vk
{
#if defined(NDEBUG)
    static constexpr bool should_enable_validation_layers = false;
#else
    static constexpr bool should_enable_validation_layers = true;
#endif

    /**
     * @brief Takes a core::semantic_version and packs it into a single 32 bit integer used by Vulkan to store
     * version numbers.
     * @since 0.1.0
     *
     * @param[in] version The version to pack for Vulkan
     */
    constexpr auto to_vulkan_version(core::semantic_version const& version) -> std::uint32_t
    {
        return VK_MAKE_API_VERSION(0, version.major, version.minor, version.patch);
    }

    /**
     * @brief Takes a packed version used by Vulkan and extract the Major, Minor and Patch numbers that are stored
     * within.
     * @since 0.1.0
     *
     * @param[in] version The version to unpack for Vulkan
     */
    constexpr auto from_vulkan_version(std::uint32_t version) -> core::semantic_version
    {
        return {.major = VK_API_VERSION_MAJOR(version),
                .minor = VK_API_VERSION_MINOR(version),
                .patch = VK_API_VERSION_PATCH(version)};
    }

    /**
     * @brief Makes a std::error_code from a ::vk::Result.
     * @since 0.1.0
     */
    auto make_error_code(::vk::Result error_code) -> std::error_code;
} // namespace core::vk
