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
#include "libcore/vulkan/flag_helpers.hpp"
#include "libcore/vulkan/include.hpp"
#include "libcore/vulkan/physical_device.hpp"
#include "libcore/vulkan/queue.hpp"
#include "libcore/vulkan/queue_family.hpp"
#include "libcore/vulkan/queue_properties.hpp"
#include "libcore/vulkan/queue_selector.hpp"

#include "libcore/error/error.hpp"

#include "tl/expected.hpp"

#include "range/v3/range/conversion.hpp"
#include "range/v3/range/primitives.hpp"
#include "range/v3/view/chunk_by.hpp"
#include "range/v3/view/enumerate.hpp"
#include "range/v3/view/filter.hpp"
#include "range/v3/view/join.hpp"
#include "range/v3/view/remove_if.hpp"
#include "range/v3/view/subrange.hpp"
#include "range/v3/view/transform.hpp"

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace rv = ranges::views;

namespace
{
    auto to_queue_family_priorities(std::pair<std::size_t, std::vector<vk::QueueFlags>> const& pair)
        -> std::pair<std::uint32_t, std::vector<float>>
    {
        return {static_cast<std::uint32_t>(pair.first), std::vector<float>(std::size(pair.second), 1.0f)};
    }

    auto get_queues_per_family(::vk::Device device, std::pair<std::size_t, std::vector<vk::QueueFlags>> const& pair)
        -> core::vk::queue_family
    {
        // clang-format off
        auto const queues = pair.second 
            | rv::enumerate
            | rv::transform([&](std::pair<std::size_t, vk::QueueFlags> queue) -> core::vk::queue {
                return {.handle = device.getQueue(static_cast<std::uint32_t>(pair.first),
                                                  static_cast<std::uint32_t>(queue.first)),
                        .purpose = queue.second};
                });
        // clang-format on

        return {.index = static_cast<std::uint32_t>(pair.first), .queues = queues | ranges::to<std::vector>()};
    }

    template<vk::QueueFlagBits type>
    auto get_queues_of_type(std::span<core::vk::queue_family const> queue_families)
    {
        auto const get_queues_from_family = [](core::vk::queue_family const& family) {
            return family.queues | rv::filter([](core::vk::queue const& q) {
                       return core::vk::has_bit(q.purpose, type);
                   });
        };

        return queue_families | rv::transform(get_queues_from_family) | rv::join | ranges::to<std::vector>();
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
        // clang-format off

        auto const selected_queues = queue_selector{}
            .with_graphics_queues(m_graphics_queue_count)
            .with_compute_queues(m_compute_queue_count)
            .with_transfer_queues(m_transfer_queue_count)
            .with_logger(*m_logger)
            .select_from(m_physical_device->queue_family_properties);

        auto const queue_family_priority_map = selected_queues 
            | rv::transform(to_queue_family_priorities) 
            | ranges::to<std::unordered_map>();

        auto const queue_create_infos = queue_family_priority_map 
            | rv::transform([](auto pair) {
                return ::vk::DeviceQueueCreateInfo{}
                    .setFlags({})
                    .setQueueFamilyIndex(static_cast<std::uint32_t>(pair.first))
                    .setQueueCount(static_cast<std::uint32_t>(std::size(pair.second)))
                    .setQueuePriorities(pair.second);
                })
            | ranges::to<std::vector>();

        // clang-format on

        auto const enabled_layers = get_desired_validation_layers();
        auto const create_info = ::vk::DeviceCreateInfo{}
                                     .setQueueCreateInfos(queue_create_infos)
                                     .setPEnabledLayerNames(enabled_layers)
                                     .setPEnabledFeatures(&(m_physical_device->features));

        auto [result, device] = m_physical_device->handle.createDeviceUnique(create_info);
        if (result != ::vk::Result::eSuccess)
        {
            // ERROR
        }

        // clang-format off

        std::vector const queue_families = selected_queues 
            | rv::transform([&](auto pair) { return get_queues_per_family(device.get(), pair); }) 
            | ranges::to<std::vector>();

        // clang-format on

        if (m_logger)
        {
            using enum ::vk::QueueFlagBits;

            m_logger->info("Vulkan device created !");
            m_logger->info("we have {} graphics queues", std::size(get_queues_of_type<eGraphics>(queue_families)));
            m_logger->info("we have {} compute queues", std::size(get_queues_of_type<eCompute>(queue_families)));
            m_logger->info("we have {} transfer queues", std::size(get_queues_of_type<eTransfer>(queue_families)));
        }

        return vk::device{.handle = std::move(device), .queue_families = queue_families};
    }
} // namespace core::vk
