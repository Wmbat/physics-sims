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
     * @brief Represents an error when selecting a render::vk::physical_device.
     */
    enum struct physical_device_selection_error
    {
        no_physical_devices_found,
        no_suitable_physical_devices_found
    };

    /**
     * @brief Wraps a render::vk::physical_device_selection_error in a std::error_code for portable error handling.
     *
     * @param[in] error_code The error code to wrap.
     */
    auto make_error_code(physical_device_selection_error error_code) -> std::error_code;

    struct physical_device
    {
        ::vk::PhysicalDevice handle;                            ///< The Vulkan handle to the physical device.
        ::vk::PhysicalDeviceProperties2 properties;             ///< The general properties of the physical device.
        ::vk::PhysicalDeviceMemoryProperties memory_properties; ///< The memory properties of the physical device.

        /**
         * @brief Get the name of the physical device.
         */
        [[nodiscard]] auto get_name() const noexcept -> std::string_view;
    };

    /**
     * @brief Select the best suited physical device found on the client system.
     *
     * @param[in] instance The instance that was used to initialize vulkan.
     *
     * @returns A valid handle to a physical device or an error detailing what went wrong.
     */
    auto select_physical_device(instance const& instance) -> tl::expected<physical_device, core::error>;
} // namespace render::vk
