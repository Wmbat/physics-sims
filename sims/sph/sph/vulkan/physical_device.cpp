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

#include "sph/vulkan/vendor.hpp"
#include <sph/core.hpp>
#include <sph/vulkan/details/vulkan.hpp>
#include <sph/vulkan/physical_device.hpp>

#include <libphyseng/util/strong_ops/arithmetic.hpp>
#include <libphyseng/util/strong_type.hpp>

#undef let

#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/functional/arithmetic.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#define let auto const

#include <cstdint>
#include <optional>
#include <ranges>
#include <utility>

namespace rv = ranges::views;

namespace
{
	struct physical_device_info
	{
		vk::PhysicalDevice device;
		vk::PhysicalDeviceProperties properties;
		vk::PhysicalDeviceFeatures features;
		std::vector<vk::QueueFamilyProperties> queue_families;
	};

	auto populate_physical_device_info(vk::PhysicalDevice const& device) -> physical_device_info
	{
		return physical_device_info{.device = device,
									.properties = device.getProperties(),
									.features = device.getFeatures(),
									.queue_families = device.getQueueFamilyProperties()};
	}

	auto is_vendor_supported(physical_device_info const& info) -> bool
	{
		let vendor_id = vulkan::vendor_id{info.properties.vendorID};

		return !vulkan::get_device_vendor_from_id(vendor_id);
	}

	auto rate_device_properties(physical_device_info const& /*info*/) -> int
	{
		return 0;
	}

	auto rate_device_features(physical_device_info const& /*info*/) -> int
	{
		return 0;
	}

	auto rate_device_queue_support(physical_device_info const& info) -> int
	{
		std::span queue_families = info.queue_families;

		return 0;
	}

	auto get_rating_value(std::tuple<physical_device_info, int> value)
	{
		return std::get<1>(value);
	}

	auto rate_physical_device(physical_device_info const& info)
	{
		return rate_device_properties(info) + rate_device_features(info)
			 + rate_device_queue_support(info);
	}
} // namespace

namespace vulkan
{
	auto get_driver_version(device_vendor vendor, std::uint32_t version)
		-> physeng::semantic_version
	{
		if (vendor == device_vendor::nvidia)
		{
			return {.major = (version >> 22u) & 0x3ffu, // NOLINT
					.minor = (version >> 14u) & 0x0ffu, // NOLINT
					.patch = (version >> 6u) & 0x0ffu}; // NOLINT
		}

		return vulkan::from_vulkan_version(version);
	}

	auto find_best_suited_physical_device(std::span<vk::PhysicalDevice const> devices)
		-> std::optional<physical_device>
	{
		// clang-format off

		let supported_devices = devices 
			| rv::transform(populate_physical_device_info)
			| rv::filter(is_vendor_supported) 
			| ranges::to<std::vector>;

		// clang-format on

		let device_ratings = supported_devices | rv::transform(rate_physical_device);
		let rated_devices = rv::zip(supported_devices, device_ratings);

		if (let it = ranges::max_element(rated_devices, ranges::greater{}, get_rating_value);
			it != ranges::end(rated_devices))
		{
			let& device_info = std::get<0>(*it);
			let device_vendor_id = vendor_id{device_info.properties.vendorID};

			let vendor = get_device_vendor_from_id(device_vendor_id).value(); // NOLINT

			return physical_device{
				.name = device_info.properties.deviceName,
				.vendor = vendor,
				.driver_version = get_driver_version(vendor, device_info.properties.driverVersion),
				.value = device_info.device};
		}

		return std::nullopt;
	}
} // namespace vulkan
