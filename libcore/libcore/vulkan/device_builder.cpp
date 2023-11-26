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
#include "libcore/vulkan/include.hpp"
#include "libcore/vulkan/physical_device.hpp"
#include "libcore/vulkan/queue_properties.hpp"
#include "libcore/vulkan/queue_selector.hpp"

#include "libcore/error/error.hpp"

#include "tl/expected.hpp"

#include "range/v3/range/conversion.hpp"
#include "range/v3/range/primitives.hpp"
#include "range/v3/view/chunk_by.hpp"
#include "range/v3/view/filter.hpp"
#include "range/v3/view/remove_if.hpp"
#include "range/v3/view/subrange.hpp"
#include "range/v3/view/transform.hpp"

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace rv = ranges::views;

namespace
{
    struct queue_family_properties
    {
        ::vk::DeviceQueueCreateFlags flags;

        std::uint32_t queue_family_index;

        std::uint32_t queue_count;

        std::vector<float> queue_priorities;
    };

    [[nodiscard]]
    auto make_queue_family_properties(auto queue_families) -> queue_family_properties
    {
        auto beg = ranges::begin(queue_families);
        auto queue_count = ranges::size(queue_families);

        return {.flags = {},
                .queue_family_index = beg->family_index,
                .queue_count = static_cast<std::uint32_t>(queue_count),
                .queue_priorities = std::vector<float>(queue_count, 1.0f)};
    }

    auto to_device_queue_properties(queue_family_properties const& properties)
    {
        return vk::DeviceQueueCreateInfo{}
            .setFlags(properties.flags)
            .setQueueFamilyIndex(properties.queue_family_index)
            .setQueueCount(properties.queue_count)
            .setQueuePriorities(properties.queue_priorities);
    }

    auto get_queues(::vk::Device device, std::span<core::vk::queue_properties const> queues, vk::QueueFlags flags)
        -> std::vector<::vk::Queue>
    {
        return queues | rv::filter([=](core::vk::queue_properties const& property) {
                   return property.flags == flags;
               })
             | rv::transform([=](core::vk::queue_properties const& property) {
                   return device.getQueue(property.family_index, property.queue_index);
               })
             | ranges::to<std::vector>();
    }
} // namespace

namespace core::vk
{
    device_builder::device_builder(physical_device const& physical_device) : m_physical_device(&physical_device) {}

    auto device_builder::with_logger(spdlog::logger& logger) -> device_builder&
    {
        m_logger = &logger;
        return *this;
    }

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

        auto compare_family_indices = [](queue_properties const& lhs, queue_properties const& rhs) {
            return lhs.family_index == rhs.family_index;
        };

        // clang-format off

        std::vector const queues_by_family = rv::chunk_by(queues, compare_family_indices)
            | rv::remove_if([](auto subrange) { return ranges::empty(subrange); })
            | rv::transform([](auto subrange) { return make_queue_family_properties(subrange); })
            | ranges::to<std::vector>();

        std::vector const queue_family_properties = queues_by_family 
            | rv::transform(to_device_queue_properties) 
            | ranges::to<std::vector>();

        // clang-format on

        auto enabled_layers = get_desired_validation_layers();

        auto create_info = ::vk::DeviceCreateInfo{}
                               .setQueueCreateInfos(queue_family_properties)
                               .setPEnabledLayerNames(enabled_layers)
                               .setPEnabledFeatures(&(m_physical_device->features));

        auto [result, device] = m_physical_device->handle.createDeviceUnique(create_info);
        if (result != ::vk::Result::eSuccess)
        {
            // ERROR
        }

        auto graphics_queues = get_queues(device.get(), queues, ::vk::QueueFlagBits::eGraphics);
        auto compute_queues = get_queues(device.get(), queues, ::vk::QueueFlagBits::eCompute);
        auto transfer_queues = get_queues(device.get(), queues, ::vk::QueueFlagBits::eTransfer);

        if (m_logger)
        {
            m_logger->info("Vulkan device has been created !");
            m_logger->info("There are {0} graphics queues available", std::ssize(graphics_queues));
            m_logger->info("There are {0} compute queues available", std::ssize(compute_queues));
            m_logger->info("There are {0} transfer queues available", std::ssize(transfer_queues));
        }

        return vk::device{.handle = std::move(device),
                          .graphics_queues = graphics_queues,
                          .compute_queues = compute_queues,
                          .transfer_queues = transfer_queues};
    }
} // namespace core::vk
