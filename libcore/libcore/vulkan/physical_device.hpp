/**
 * @file libcore/libcore/vulkan/physical_device.hpp
 * @brief Use for selection the best physical device to use.
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

#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <string_view>
#include <vector>

namespace core::vk
{
    /**
     * @brief Represents a physical device with Vulkan support.
     */
    struct physical_device
    {
        /**
         * @brief The Vulkan handle to the physical device
         */
        ::vk::PhysicalDevice handle;

        /**
         * @brief The general properties of the physical device
         */
        ::vk::PhysicalDeviceProperties2 properties;

        /**
         * @brief The memory properties of the physical device.
         */
        ::vk::PhysicalDeviceMemoryProperties memory_properties;

        /**
         * @brief The queue family properties of the physical device.
         */
        std::vector<::vk::QueueFamilyProperties> queue_family_properties;

        /**
         * @brief The features supported by the physical device
         */
        ::vk::PhysicalDeviceFeatures features;

        /**
         * @brief Get the name of the physical device.
         */
        [[nodiscard]]
        auto get_name() const noexcept -> std::string_view;
    };
} // namespace core::vk
