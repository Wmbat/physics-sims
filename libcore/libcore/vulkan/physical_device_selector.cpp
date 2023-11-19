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

#include "libcore/vulkan/physical_device_selector.hpp"

#include "libcore/error/error.hpp"
#include "libcore/semantic_version.hpp"
#include "libcore/vulkan/include.hpp"
#include "libcore/vulkan/instance.hpp"
#include "libcore/vulkan/physical_device.hpp"

#include "range/v3/algorithm/fold.hpp"
#include "range/v3/algorithm/fold_left.hpp"
#include "range/v3/algorithm/max_element.hpp"
#include "range/v3/functional/bind_back.hpp"
#include "range/v3/functional/comparisons.hpp"
#include "range/v3/functional/invoke.hpp"
#include "range/v3/iterator/basic_iterator.hpp"
#include "range/v3/iterator/unreachable_sentinel.hpp"
#include "range/v3/range/access.hpp"
#include "range/v3/range/conversion.hpp"
#include "range/v3/range/dangling.hpp"
#include "range/v3/range_fwd.hpp"
#include "range/v3/utility/common_type.hpp"
#include "range/v3/view/all.hpp"
#include "range/v3/view/enumerate.hpp"
#include "range/v3/view/facade.hpp"
#include "range/v3/view/filter.hpp"
#include "range/v3/view/transform.hpp"
#include "range/v3/view/view.hpp"
#include "range/v3/view/zip.hpp"

#include "magic_enum.hpp"

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_funcs.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace rv = ranges::views;

template<>
struct std::hash<vk::QueueFlags>
{
    auto operator()(vk::QueueFlags const& flags) const -> std::size_t
    {
        using mask_type = vk::QueueFlags::MaskType;
        return hash<mask_type>{}(static_cast<mask_type>(flags));
    }
};

namespace
{
    struct physical_device_selection_error_category : public std::error_category
    {
        [[nodiscard]]
        auto name() const noexcept -> char const* override
        {
            return "physical device selection error";
        }

        [[nodiscard]]
        auto message(int error_code) const noexcept -> std::string override
        {
            auto const enum_name =
                magic_enum::enum_name(static_cast<core::vk::physical_device_selection_error>(error_code));
            return std::string{enum_name};
        }
    };

    auto populate_physical_device(vk::PhysicalDevice device) -> core::vk::physical_device
    {
        return {.handle = device,
                .properties = device.getProperties2(),
                .memory_properties = device.getMemoryProperties(),
                .queue_family_properties = device.getQueueFamilyProperties()};
    }
} // namespace

namespace core::vk
{
    auto make_error_code(physical_device_selection_error error_code) -> std::error_code
    {
        static auto error_category = physical_device_selection_error_category{};
        return {static_cast<int>(error_code), error_category};
    }

    physical_device_selector::physical_device_selector(instance const& instance, spdlog::logger* logger) :
        m_instance{*instance}, m_logger{logger}
    {}

    auto physical_device_selector::with_minimum_vulkan_version(semantic_version const& version)
        -> physical_device_selector&
    {
        m_minimum_vulkan_version = version;
        return *this;
    }

    auto physical_device_selector::with_desired_vulkan_version(semantic_version const& version)
        -> physical_device_selector&
    {
        m_desired_vulkan_version = version;
        return *this;
    }

    auto physical_device_selector::with_prefered_device_type(::vk::PhysicalDeviceType type) -> physical_device_selector&
    {
        m_prefered_device_type = type;
        return *this;
    }

    auto physical_device_selector::allow_any_device_type(bool value) -> physical_device_selector&
    {
        m_should_allow_any_device_type = value;
        return *this;
    }

    auto physical_device_selector::with_compute_queues(int desired_queue_count) -> physical_device_selector&
    {
        m_compute_queue_count = desired_queue_count;
        return *this;
    }

    auto physical_device_selector::with_graphics_queues(int desired_queue_count) -> physical_device_selector&
    {
        m_graphics_queue_count = desired_queue_count;
        return *this;
    }

    auto physical_device_selector::with_transfer_queues(int desired_queue_count) -> physical_device_selector&
    {
        m_transfer_queue_count = desired_queue_count;
        return *this;
    }

    auto physical_device_selector::select() -> tl::expected<physical_device, core::error>
    {
        using device_rating = std::pair<physical_device, int>;
        using enum physical_device_selection_error;

        auto const [result, devices] = m_instance.enumeratePhysicalDevices();
        if (result != ::vk::Result::eSuccess)
        {
            return tl::unexpected{core::error{.error_code = make_error_code(result)}};
        }

        if (devices.empty())
        {
            return tl::unexpected{core::error{.error_code = make_error_code(no_physical_devices_found)}};
        }

        // clang-format off

        auto const populated_devices = devices 
            | rv::transform(populate_physical_device) 
            | ranges::to<std::vector>();
        auto const rated_devices = populated_devices 
            | rv::transform([&](physical_device const& device) { return rate_device(device); });
        auto const device_ratings = rv::zip(populated_devices, rated_devices)
            | ranges::to<std::vector>();

        // clang-format on

        if (m_logger)
        {
            for (auto const& [device, rating] : device_ratings)
            {
                m_logger->debug("The physical device \"{}\" was given a rating of {}", device.get_name(), rating);
            }
        }

        if (auto const it = ranges::max_element(device_ratings, {}, &device_rating::second);
            it != ranges::end(device_ratings))
        {
            return it->first;
        }

        return tl::unexpected{core::error{.error_code = make_error_code(no_suitable_physical_devices_found)}};
    }

    auto physical_device_selector::rate_device(physical_device const& device) -> int
    {
        auto const& properties = device.properties.properties;

        return rate_device_type(properties.deviceType) + rate_device_queues(device.queue_family_properties);
    }

    auto physical_device_selector::rate_device_type(::vk::PhysicalDeviceType type) -> int
    {
        using enum ::vk::PhysicalDeviceType;

        static constexpr int prefered_type_grade = 100;
        static constexpr int discrete_gpu_grade = 80;
        static constexpr int integrated_gpu_grade = 50;
        static constexpr int virtual_gpu_grade = 25;
        static constexpr int rejection_grade = 0;

        bool const is_prefered_type = type == m_prefered_device_type;
        if (is_prefered_type)
        {
            return prefered_type_grade;
        }

        if (not is_prefered_type and not m_should_allow_any_device_type)
        {
            return rejection_grade;
        }
        else
        {
            if (type == eDiscreteGpu)
            {
                return discrete_gpu_grade;
            }
            else if (type == eIntegratedGpu)
            {
                return integrated_gpu_grade;
            }
            else if (type == eVirtualGpu)
            {
                return virtual_gpu_grade;
            }
            else
            {
                return rejection_grade;
            }
        }
    }

    struct queue_allocation
    {
        int index;
        int count;
        ::vk::QueueFlags purpose;
    };

    auto get_used_queue_count(std::span<queue_allocation const> queue_allocations, std::size_t index) -> int
    {
        // clang-format off

        auto const allocations_at_index = queue_allocations 
            | rv::filter([index](queue_allocation const& data) { return std::cmp_equal(data.index, index); })
            | rv::transform([](queue_allocation const& data) { return data.count; })
            | ranges::to<std::vector>();

        return ranges::fold_left(allocations_at_index, 0, std::plus());

        // clang-format on
    }

    auto physical_device_selector::rate_device_queues(std::span<::vk::QueueFamilyProperties const> queue_families)
        -> int
    {
        auto queue_allocations = std::vector<queue_allocation>();

        int graphics_queue_counter = m_graphics_queue_count;
        int compute_queue_counter = m_compute_queue_count;
        int transfer_queue_counter = m_transfer_queue_count;

        // Pass for dedicated queues
        for (auto const& [index, family] : queue_families | rv::enumerate)
        {
            auto const available_count = static_cast<int>(family.queueCount);

            if (family.queueFlags == ::vk::QueueFlagBits::eGraphics)
            {
                auto const queue_count = std::min(graphics_queue_counter, available_count);
                graphics_queue_counter -= queue_count;

                queue_allocations.push_back({.index = static_cast<int>(index),
                                             .count = queue_count,
                                             .purpose = ::vk::QueueFlagBits::eGraphics});
                continue;
            }

            if (family.queueFlags == ::vk::QueueFlagBits::eCompute)
            {
                auto const queue_count = std::min(compute_queue_counter, available_count);
                compute_queue_counter -= queue_count;

                queue_allocations.push_back(
                    {.index = static_cast<int>(index), .count = queue_count, .purpose = ::vk::QueueFlagBits::eCompute});

                continue;
            }

            if (family.queueFlags == ::vk::QueueFlagBits::eTransfer)
            {
                auto const queue_count = std::min(transfer_queue_counter, available_count);
                transfer_queue_counter -= queue_count;

                queue_allocations.push_back({.index = static_cast<int>(index),
                                             .count = transfer_queue_counter,
                                             .purpose = ::vk::QueueFlagBits::eTransfer});
                continue;
            }
        }

        for (auto const& [index, family] : queue_families | rv::enumerate)
        {
            auto const used_count = get_used_queue_count(queue_allocations, index);
            auto const available_queue_count = std::max(0, static_cast<int>(family.queueCount) - used_count);

            if ((family.queueFlags & ::vk::QueueFlagBits::eGraphics) && graphics_queue_counter > 0)
            {
                auto const queue_count = std::min(graphics_queue_counter, available_queue_count);
                graphics_queue_counter -= queue_count;

                queue_allocations.push_back({.index = static_cast<int>(index),
                                             .count = queue_count,
                                             .purpose = ::vk::QueueFlagBits::eGraphics});
            }

            if ((family.queueFlags & ::vk::QueueFlagBits::eCompute) && compute_queue_counter > 0)
            {
                auto const queue_count = std::min(compute_queue_counter, available_queue_count);
                compute_queue_counter -= queue_count;

                queue_allocations.push_back(
                    {.index = static_cast<int>(index), .count = queue_count, .purpose = ::vk::QueueFlagBits::eCompute});
            }

            if ((family.queueFlags & ::vk::QueueFlagBits::eTransfer) && transfer_queue_counter > 0)
            {
                auto const queue_count = std::min(transfer_queue_counter, available_queue_count);
                transfer_queue_counter -= queue_count;

                queue_allocations.push_back({.index = static_cast<int>(index),
                                             .count = queue_count,
                                             .purpose = ::vk::QueueFlagBits::eTransfer});
            }
        }

        auto const rating = queue_allocations | rv::transform([](queue_allocation const& data) {
                                return data.count;
                            });

        return ranges::fold_left(rating, 0, std::plus());
    }
} // namespace core::vk
