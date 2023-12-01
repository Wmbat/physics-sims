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

#include "libcore/vulkan/queue_selector.hpp"

#include "libcore/vulkan/flag_helpers.hpp"
#include "libcore/vulkan/queue_properties.hpp"

#include "range/v3/range/conversion.hpp"
#include "range/v3/view/enumerate.hpp"
#include "range/v3/view/filter.hpp"
#include "range/v3/view/iota.hpp"
#include "range/v3/view/remove_if.hpp"
#include "range/v3/view/take.hpp"
#include "range/v3/view/transform.hpp"

#include "spdlog/logger.h"

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace rv = ranges::views;
namespace stdr = std::ranges;

namespace
{
    auto get_queues_with_flag(std::span<vk::QueueFamilyProperties const> queue_families, vk::QueueFlags flags)
    {
        using enum ::vk::QueueFlagBits;

        // clang-format off
        
        return queue_families 
            | rv::enumerate 
            | rv::filter([=](auto&& pair) {
                return core::vk::has_bits(pair.second.queueFlags, flags);
            });

        // clang-format on
    }

    auto get_specialized_compute_queues(std::span<vk::QueueFamilyProperties const> queue_families)
    {
        using enum ::vk::QueueFlagBits;

        // clang-format off
        
        return queue_families 
            | rv::enumerate 
            | rv::remove_if([](auto&& pair) {
                return core::vk::has_bit(pair.second.queueFlags, eTransfer) 
                    or core::vk::has_bit(pair.second.queueFlags, eGraphics);
            });

        // clang-format on
    }

    auto get_specialized_transfer_queues(std::span<const ::vk::QueueFamilyProperties> queue_families)
    {
        using enum ::vk::QueueFlagBits;

        // clang-format off
        
        return queue_families 
            | rv::enumerate 
            | rv::remove_if([](auto&& pair) {
                return core::vk::has_bit(pair.second.queueFlags, eCompute) 
                    or core::vk::has_bit(pair.second.queueFlags, eGraphics);
            });

        // clang-format on
    }

    auto get_queue_count_at_index(core::vk::queue_family_map const& map, std::size_t index)
        -> std::optional<std::int64_t>
    {
        if (map.contains(index))
        {
            return stdr::ssize(map.at(index));
        }
        else
        {
            return std::nullopt;
        }
    }

    auto get_queue_allocations(std::optional<std::int64_t> already_selected_count, std::int64_t queue_count, // NOLINT
                               std::int64_t count_to_select, ::vk::QueueFlagBits purpose)
    {
        if (already_selected_count)
        {
            auto queue_count_diff = queue_count - already_selected_count.value();
            auto number_to_select = std::min(count_to_select, queue_count_diff);

            // clang-format off

            return  rv::iota(queue_count_diff) 
                | rv::take(number_to_select)
                | rv::transform([=](int64_t) -> ::vk::QueueFlags { return purpose; })
                | ranges::to<std::vector>();

            // clang-format on
        }
        else
        {
            auto number_to_select = std::min(count_to_select, queue_count);

            // clang-format off

            return rv::iota(0) 
                | rv::take(number_to_select)
                | rv::transform([=](int) -> ::vk::QueueFlags { return purpose; })
                | ranges::to<std::vector>();

            // clang-format on
        }
    }
} // namespace

namespace core::vk
{
    auto queue_selector::with_logger(spdlog::logger& logger) -> queue_selector&
    {
        m_logger = &logger;
        return *this;
    }

    auto queue_selector::with_compute_queues(int desired_queue_count) -> queue_selector&
    {
        m_compute_queue_count = desired_queue_count;
        return *this;
    }

    auto queue_selector::with_graphics_queues(int desired_queue_count) -> queue_selector&
    {
        m_graphics_queue_count = desired_queue_count;
        return *this;
    }

    auto queue_selector::with_transfer_queues(int desired_queue_count) -> queue_selector&
    {
        m_transfer_queue_count = desired_queue_count;
        return *this;
    }

    auto is_queue_family_exclusive(::vk::QueueFlags flag, ::vk::QueueFlags target,
                                   std::same_as<::vk::QueueFlagBits> auto... excludes)
    {
        return has_bits(flag, target) and not(has_bits(flag, excludes) or ...);
    }

    template<typename... Args>
    void safe_debug(spdlog::logger* logger, spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        if (logger)
        {
            logger->debug(fmt, args...);
        }
    }

    auto queue_selector::select_from(std::span<::vk::QueueFamilyProperties const> queue_families) -> queue_family_map
    {
        using enum ::vk::QueueFlagBits;

        auto queue_map = queue_family_map{};

        long graphics_queue_counter = m_graphics_queue_count;
        long compute_queue_counter = m_compute_queue_count;
        long transfer_queue_counter = m_transfer_queue_count;

        safe_debug(m_logger, "We are trying to find {} graphics queues, {} compute queues, and {} transfer queues",
                   graphics_queue_counter, compute_queue_counter, transfer_queue_counter);

        safe_debug(m_logger, "Trying to find specialized compute queues");
        for (auto const& [index, properties] : get_specialized_compute_queues(queue_families))
        {
            if (compute_queue_counter <= 0)
            {
                break;
            }

            auto const queue_count = properties.queueCount;
            auto const allocated_count = get_queue_count_at_index(queue_map, index);
            queue_map[index] = get_queue_allocations(allocated_count, queue_count, compute_queue_counter, eCompute);
            compute_queue_counter -= stdr::ssize(queue_map[index]);
        }

        safe_debug(m_logger, "Trying to find specialized transfer queues");
        for (auto const& [index, properties] : get_specialized_transfer_queues(queue_families))
        {
            if (transfer_queue_counter <= 0)
            {
                break;
            }

            auto const queue_count = properties.queueCount;
            auto const allocated_count = get_queue_count_at_index(queue_map, index);
            queue_map[index] = get_queue_allocations(allocated_count, queue_count, transfer_queue_counter, eTransfer);
            transfer_queue_counter -= stdr::ssize(queue_map[index]);
        }

        safe_debug(m_logger, "Trying to find graphics queues");
        for (auto const& [index, properties] : get_queues_with_flag(queue_families, eGraphics))
        {
            if (graphics_queue_counter <= 0)
            {
                break;
            }

            auto const queue_count = properties.queueCount;
            auto const allocated_count = get_queue_count_at_index(queue_map, index);
            queue_map[index] = get_queue_allocations(allocated_count, queue_count, graphics_queue_counter, eGraphics);
            graphics_queue_counter -= stdr::ssize(queue_map[index]);
        }

        safe_debug(m_logger, "Trying to find compute queues");
        for (auto const& [index, properties] : get_queues_with_flag(queue_families, eCompute))
        {
            if (compute_queue_counter <= 0)
            {
                break;
            }

            auto const queue_count = properties.queueCount;
            auto const allocated_count = get_queue_count_at_index(queue_map, index);
            queue_map[index] = get_queue_allocations(allocated_count, queue_count, compute_queue_counter, eCompute);
            compute_queue_counter -= stdr::ssize(queue_map[index]);
        }

        safe_debug(m_logger, "Trying to find transfer queues");
        for (auto const& [index, properties] : get_queues_with_flag(queue_families, eTransfer))
        {
            if (transfer_queue_counter <= 0)
            {
                break;
            }

            auto const queue_count = properties.queueCount;
            auto const allocated_count = get_queue_count_at_index(queue_map, index);
            queue_map[index] = get_queue_allocations(allocated_count, queue_count, transfer_queue_counter, eTransfer);
            transfer_queue_counter -= stdr::ssize(queue_map[index]);
        }

        return queue_map;
    }
} // namespace core::vk
