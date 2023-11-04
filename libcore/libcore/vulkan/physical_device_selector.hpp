/**
 * @file libcore/libcore/vulkan/physical_device_selector.hpp
 * @brief Helps with the selection of the best suited physicald device
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
#include <libcore/vulkan/physical_device.hpp>

#include <libcore/semantic_version.hpp>

#include <functional>
#include <optional>

namespace core::vk
{
    /**
     * @brief Represents an error when selecting a render::vk::physical_device.
     * @since 0.1.0
     */
    enum struct physical_device_selection_error
    {
        no_physical_devices_found,
        no_suitable_physical_devices_found
    };

    /**
     * @brief Wraps a render::vk::physical_device_selection_error in a std::error_code for portable error handling.
     * @since 0.1.0
     *
     * @param[in] error_code The error code to wrap.
     */
    auto make_error_code(physical_device_selection_error error_code) -> std::error_code;

    /**
     * @brief Aid in the selection of a physical device.
     * @details Allows the user to set various settings to customize the selection process for the device. Setting some
     * options versus other can modify which physical device will be selected.
     * @note By default, the selector searches for a discrete physical device with 1 graphics queue and 1 transfer
     * queue.
     *
     * Example usage:
     * @code{.cpp}
     * auto device_result = core::vk::physical_device_selector{}
     *     .set_prefered_device_type(::vk::PhysicalDeviceType::eDiscreteGpu)
     *     .should_create_graphics_queue(true)
     *     .should_create_transfer_queue(true)
     *     .should_create_compute_queue(true)
     *     .select(instance);
     *
     * if (not device_result.has_value())
     * {
     *     core::panic("{}", device_result.error());
     * }
     *
     * auto const& device = device_result.value();
     * // Do something with the physical device.
     * @endcode
     * @since 0.1.0
     *
     * @todo Implement robust device rejection messages detailing exactly what went wrong.
     */
    struct physical_device_selector
    {
    public:
        physical_device_selector(instance const& instance);

        /**
         * @brief Set the minimum vulkan version that the physical device should support.
         * @since 0.1.0
         *
         * @param[in] version The minimum version to set.
         */
        auto with_minimum_vulkan_version(semantic_version const& version) -> physical_device_selector&;
        /**
         * @brief Set the desired vulkan version that the physical device should support.
         * @note If the desired vulkan version is less than the minimum version (set through
         * physical_device_selector::set_minimum_vulkan_version), the minimum version will be used.
         * @since 0.1.0
         *
         * @param[in] version The desired version to set.
         */
        auto with_desired_vulkan_version(semantic_version const& version) -> physical_device_selector&;

        /**
         * @brief Set the prefered physical device type.
         * @note The default prefered device type targets discrete devices (eDiscreteGpu).
         * @since 0.1.0
         *
         * @param[in] type The prefered physical device type to set.
         */
        auto with_prefered_device_type(::vk::PhysicalDeviceType type) -> physical_device_selector&;

        /**
         * @brief Allow or restrict the selection to the prefered device type
         * @since 0.1.0
         *
         * @param[in] Whether we should restrict the selection to the prefered device type.
         */
        auto allow_any_device_type(bool value) -> physical_device_selector&;

        /**
         * @brief Specify the amount of compute queues to create.
         * @warning There may not be the correct amount of queue if no device supports the amount requested.
         * @todo Implement precondition checking with jeremy-rifkin's libassert
         * @since 0.1.0
         *
         * @pre @p desired_queue_count > 0
         *
         * @param[in] desired_queue_count The number of compute queues that should be created.
         */
        auto with_compute_queues(int desired_queue_count = 1) -> physical_device_selector&;
        /**
         * @brief Specify the amount of compute queues to create.
         * @warning There may not be the correct amount of queue if no device supports the amount requested.
         * @todo Implement precondition checking with jeremy-rifkin's libassert
         * @since 0.1.0
         *
         * @pre @p desired_queue_count > 0
         *
         * @param[in] desired_queue_count The number of graphics queues that should be created.
         */
        auto with_graphics_queues(int desired_queue_count = 1) -> physical_device_selector&;
        /**
         * @brief Specity whether the selection should try to create transfer queues, and set the amount of compute queues
         * to create.
         * @warning There may not be the correct amount of queue if no device supports the amount requested.
         * @todo Implement precondition checking with jeremy-rifkin's libassert
         * @since 0.1.0
         *
         * @pre @p desired_queue_count > 0
         *
         * @param[in] desired_queue_count The number of transfer queues that should be created.
         */
        auto with_transfer_queues(int desired_queue_count = 1) -> physical_device_selector&;

        /**
         * @brief Selects the physical device that best suits the settings.
         * @since 0.1.0
         *
         * @param[in] instance
         *
         * @returns A valid physical device that can be used for various render or compute operations or an error code
         * detailing what could have gone wrong during the selection.
         */
        auto select() -> tl::expected<physical_device, core::error>;

    private:
        auto rate_device(physical_device const& device) -> int;
        auto rate_device_type(::vk::PhysicalDeviceType type) -> int;
        auto rate_device_queues(std::span<::vk::QueueFamilyProperties const> queue_families) -> int;

    private:
        ::vk::Instance m_instance;

        semantic_version m_minimum_vulkan_version = from_vulkan_version(VK_API_VERSION_1_3);
        semantic_version m_desired_vulkan_version = from_vulkan_version(VK_API_VERSION_1_3);

        ::vk::PhysicalDeviceType m_prefered_device_type = ::vk::PhysicalDeviceType::eDiscreteGpu;
        bool m_should_allow_any_device_type = false;

        std::optional<int> m_compute_queue_count = std::nullopt;
        std::optional<int> m_graphics_queue_count = 1;
        std::optional<int> m_transfer_queue_count = 1;
    };
} // namespace core::vk
