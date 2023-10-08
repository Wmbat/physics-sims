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

#include <librender/vulkan/physical_device.hpp>

#include <magic_enum.hpp>

#include <system_error>

namespace
{
    struct device_rating_pair
    {
        render::vk::physical_device device;
        int rating = 0;
    };

    auto to_device_rating_pair(std::tuple<render::vk::physical_device, int> input) -> device_rating_pair
    {
        return device_rating_pair{.device = std::get<0>(input), .rating = std::get<1>(input)};
    }

    auto is_device_supported(device_rating_pair pair) -> bool
    {
        return pair.rating >= 0;
    }

    auto populate_physical_device(::vk::PhysicalDevice device) -> render::vk::physical_device
    {
        return {.handle = device, .properties = device.getProperties2(), .memory_properties = {}};
    }
} // namespace

namespace render::vk
{
    struct physical_device_selection_error_category : public std::error_category
    {
        [[nodiscard]] auto name() const noexcept -> char const* override { return "physical device selection error"; }

        [[nodiscard]] auto message(int error_code) const noexcept -> std::string override
        {
            auto const enum_name = magic_enum::enum_name(static_cast<physical_device_selection_error>(error_code));
            return std::string{enum_name};
        }
    };

    auto make_error_code(physical_device_selection_error error_code) -> std::error_code
    {
        static auto error_category = physical_device_selection_error_category{};
        return {static_cast<int>(error_code), error_category};
    }

    auto rate_physical_device_type(::vk::PhysicalDeviceProperties2 properties) -> int
    {
        using enum ::vk::PhysicalDeviceType;

        static constexpr int discrete_gpu_grade = 100;
        static constexpr int integrated_gpu_grade = 70;
        static constexpr int virtual_gpu_grade = 30;

        auto type = properties.properties.deviceType;
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
            return -1;
        }
    }

    auto rate_physical_device(physical_device device) -> int
    {
        auto const device_type_grade = rate_physical_device_type(device.properties);

        return device_type_grade;
    }

    auto physical_device::get_name() const noexcept -> std::string_view
    {
        return {properties.properties.deviceName};
    }

    auto select_physical_device(instance const& instance) -> tl::expected<physical_device, core::error>
    {
        using enum physical_device_selection_error;

        auto const [result, devices] = instance->enumeratePhysicalDevices();
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
        auto const ratings = populated_devices | ranges::views::transform(rate_physical_device);
        auto const device_rating_pairs = ranges::views::zip(populated_devices, ratings)
                                       | ranges::views::transform(to_device_rating_pair)
                                       | ranges::views::filter(is_device_supported) 
                                       | ranges::to<std::vector>();

        // clang-format on

        auto const it = std::ranges::max_element(device_rating_pairs, {}, &device_rating_pair::rating);

        if (it == device_rating_pairs.end())
        {
            return tl::unexpected{core::error{.error_code = make_error_code(no_suitable_physical_devices_found)}};
        }

        return it->device;
    }
} // namespace render::vk
