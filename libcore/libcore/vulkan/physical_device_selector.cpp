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

#include <libcore/vulkan/physical_device_selector.hpp>

#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/transform.hpp>

#include <magic_enum.hpp>

#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <utility>

template<>
struct std::hash<vk::QueueFlags>
{
    auto operator()(vk::QueueFlags const& flags) const -> std::size_t
    {
        using mask_type = vk::QueueFlags::MaskType;
        return hash<mask_type>{}(static_cast<mask_type>(flags));
    };
};

namespace
{
    struct physical_device_selection_error_category : public std::error_category
    {
        [[nodiscard]] auto name() const noexcept -> char const* override { return "physical device selection error"; }

        [[nodiscard]] auto message(int error_code) const noexcept -> std::string override
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

    physical_device_selector::physical_device_selector(instance const& instance) : m_instance(*instance) {}

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
            | ranges::views::transform(populate_physical_device) 
            | ranges::to<std::vector>();
        auto const rated_devices = populated_devices 
            | ranges::views::transform([&](physical_device const& device) {
                return rate_device(device);
              });
        auto const device_ratings = ranges::views::zip(populated_devices, rated_devices)
            | ranges::to<std::vector>();

        // clang-format on

        auto const it = std::ranges::max_element(device_ratings, {}, &device_rating::second);
        if (it == std::ranges::end(device_ratings))
        {
            return tl::unexpected{core::error{.error_code = make_error_code(no_suitable_physical_devices_found)}};
        }

        return it->first;
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

    auto physical_device_selector::rate_device_queues(std::span<::vk::QueueFamilyProperties const> queue_families)
        -> int
    {
        auto map = std::unordered_map<::vk::QueueFlags, int>();

        // Pass for dedicated queues
        for (auto const& family : queue_families)
        {
            auto const available_count = static_cast<int>(family.queueCount);

            if (family.queueFlags == ::vk::QueueFlagBits::eGraphics && m_graphics_queue_count.has_value())
            {
                map[family.queueFlags] = std::max(m_graphics_queue_count.value(), available_count);
                continue;
            }

            if (family.queueFlags == ::vk::QueueFlagBits::eCompute && m_compute_queue_count.has_value())
            {
                map[family.queueFlags] = std::max(m_compute_queue_count.value(), available_count);
                continue;
            }

            if (family.queueFlags == ::vk::QueueFlagBits::eTransfer && m_transfer_queue_count.has_value())
            {
                continue;
            }
        }

        for (auto const& family : queue_families)
        {
            [[maybe_unused]] auto const available_count = static_cast<int>(family.queueCount);
        }

        return 0;
    }
} // namespace core::vk
