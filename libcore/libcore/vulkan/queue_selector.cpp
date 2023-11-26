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

#include "range/v3/algorithm/fold_left.hpp"
#include "range/v3/algorithm/count_if.hpp"
#include "range/v3/range/conversion.hpp"
#include "range/v3/view/enumerate.hpp"
#include "range/v3/view/filter.hpp"
#include "range/v3/view/iota.hpp"
#include "range/v3/view/take.hpp"
#include "range/v3/view/transform.hpp"

#include <vulkan/vulkan_enums.hpp>

namespace rv = ranges::views;

namespace core::vk
{
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

    auto queue_selector::select_from(std::span<const ::vk::QueueFamilyProperties> queue_families)
        -> std::vector<queue_properties>
    {
        auto queues = std::vector<queue_properties>{};

        int graphics_queue_counter = m_graphics_queue_count;
        int compute_queue_counter = m_compute_queue_count;
        int transfer_queue_counter = m_transfer_queue_count;

        for (auto const& [family_index, family] : queue_families | rv::enumerate)
        {
            if ((family.queueFlags & ::vk::QueueFlagBits::eGraphics) and graphics_queue_counter > 0)
            {
                auto const used_count = ranges::count_if(queues, [family_index](queue_properties const& prop) {
                    return std::cmp_equal(prop.family_index, family_index);
                });
                auto const available_queue_count = static_cast<int>(family.queueCount - used_count);
                auto const queue_count = std::min(graphics_queue_counter, available_queue_count);

                for (auto i : rv::iota(used_count) | rv::take(queue_count))
                {
                    queues.push_back({.flags = ::vk::QueueFlagBits::eGraphics,
                                      .family_index = static_cast<std::uint32_t>(family_index),
                                      .queue_index = static_cast<std::uint32_t>(i)});
                }

                graphics_queue_counter -= queue_count;
            }

            if ((family.queueFlags & ::vk::QueueFlagBits::eCompute) and compute_queue_counter > 0)
            {
                auto const used_count = ranges::count_if(queues, [family_index](queue_properties const& prop) {
                    return std::cmp_equal(prop.family_index, family_index);
                });
                auto const available_queue_count = static_cast<int>(family.queueCount - used_count);
                auto const queue_count = std::min(compute_queue_counter, available_queue_count);

                for (auto i : rv::iota(used_count) | rv::take(queue_count))
                {
                    queues.push_back({.flags = ::vk::QueueFlagBits::eCompute,
                                      .family_index = static_cast<std::uint32_t>(family_index),
                                      .queue_index = static_cast<std::uint32_t>(i)});
                }

                compute_queue_counter -= queue_count;
            }

            if ((family.queueFlags & ::vk::QueueFlagBits::eTransfer) and transfer_queue_counter > 0)
            {
                auto const used_count = ranges::count_if(queues, [family_index](queue_properties const& prop) {
                    return std::cmp_equal(prop.family_index, family_index);
                });
                auto const available_queue_count = static_cast<int>(family.queueCount - used_count);
                auto const queue_count = std::min(transfer_queue_counter, available_queue_count);

                for (auto i : rv::iota(used_count) | rv::take(queue_count))
                {
                    queues.push_back({.flags = ::vk::QueueFlagBits::eTransfer,
                                      .family_index = static_cast<std::uint32_t>(family_index),
                                      .queue_index = static_cast<std::uint32_t>(i)});
                }

                transfer_queue_counter -= queue_count;
            }
        }

        return queues;
    }
} // namespace core::vk
