/**
 * Copyright 2023 wmbat
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

#include "libcore/vulkan/device_builder.hpp"
#include "libcore/vulkan/device.hpp"
#include "libcore/vulkan/physical_device.hpp"
#include "libcore/vulkan/queue_properties.hpp"
#include "libcore/vulkan/queue_selector.hpp"

#include "libcore/error/error.hpp"

#include "range/v3/view/subrange.hpp"
#include "tl/expected.hpp"

#include "range/v3/range/conversion.hpp"
#include "range/v3/view/chunk_by.hpp"
#include "range/v3/view/transform.hpp"

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace rv = ranges::views;

namespace
{
    struct queue_family_properties
    {
    };
} // namespace

namespace core::vk
{
    device_builder::device_builder(physical_device const& physical_device) : m_physical_device(&physical_device) {}

    auto device_builder::with_compute_queues(int desired_queue_count) -> device_builder&
    {
        m_compute_queue_count = desired_queue_count;
        return *this;
    }

    auto device_builder::with_graphics_queues(int desired_queue_count) -> device_builder&
    {
        m_graphics_queue_count = desired_queue_count;
        return *this;
    }

    auto device_builder::with_transfer_queues(int desired_queue_count) -> device_builder&
    {
        m_transfer_queue_count = desired_queue_count;
        return *this;
    }

    auto device_builder::build() -> tl::expected<device, core::error>
    {
        auto const queues = queue_selector{}
                                .with_graphics_queues(m_graphics_queue_count)
                                .with_compute_queues(m_compute_queue_count)
                                .with_transfer_queues(m_transfer_queue_count)
                                .select_from(m_physical_device->queue_family_properties);

        auto const queues_by_family =
            rv::chunk_by(queues,
                         [](queue_properties const& lhs, queue_properties const& rhs) {
                             return lhs.family_index == rhs.family_index and lhs.flags == rhs.flags;
                         })
            | rv::transform([](auto subrange) { return subrange | ranges::to<std::vector>(); })
            | ranges::to<std::vector>();

        auto test = queues_by_family | rv::transform([](auto subrange) {
                return ::vk::DeviceQueueCreateInfo{};
                    });

        std::vector<::vk::DeviceQueueCreateInfo> queue_create_infos;

        auto create_info = ::vk::DeviceCreateInfo{}.setQueueCreateInfos(queue_create_infos);

        auto [result, device] = m_physical_device->handle.createDeviceUnique(create_info);
        if (result != ::vk::Result::eSuccess)
        {
            // ERROR
        }

        return vk::device{};
    }
} // namespace core::vk
