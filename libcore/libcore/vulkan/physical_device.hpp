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

#include <libcore/semantic_version.hpp>
#include <libcore/vulkan/include.hpp>
#include <libcore/vulkan/instance.hpp>

#include <libcore/error/error.hpp>

#include <tl/expected.hpp>

#include <range/v3/action/sort.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <concepts>
#include <ranges>

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
         * @brief Get the name of the physical device.
         */
        [[nodiscard]] auto get_name() const noexcept -> std::string_view;
    };
} // namespace core::vk
