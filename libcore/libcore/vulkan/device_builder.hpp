/**
 * @file libcore/libcore/vulkan/device_builder.hpp
 * @brief Utility to simplify the creation of a vulkan logical device.
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

#include "libcore/error/error.hpp"
#include "libcore/vulkan/device.hpp"
#include "libcore/vulkan/include.hpp"
#include "libcore/vulkan/instance.hpp"
#include "libcore/vulkan/physical_device.hpp"

#include "tl/expected.hpp"

#include <vulkan/vulkan_handles.hpp>

namespace core::vk
{
    struct device_builder
    {
    public:
        device_builder(physical_device const& physical_device);

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
        auto with_compute_queues(int desired_queue_count = 1) -> device_builder&;
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
        auto with_graphics_queues(int desired_queue_count = 1) -> device_builder&;
        /**
         * @brief Specity whether the selection should try to create transfer queues, and set the amount of compute
         * queues to create.
         * @warning There may not be the correct amount of queue if no device supports the amount requested.
         * @todo Implement precondition checking with jeremy-rifkin's libassert
         * @since 0.1.0
         *
         * @pre @p desired_queue_count > 0
         *
         * @param[in] desired_queue_count The number of transfer queues that should be created.
         */
        auto with_transfer_queues(int desired_queue_count = 1) -> device_builder&;

        auto build() -> tl::expected<device, core::error>;

    private:
        physical_device const* m_physical_device;

        int m_compute_queue_count = 0;
        int m_graphics_queue_count = 1;
        int m_transfer_queue_count = 1;
    };
} // namespace core::vk
