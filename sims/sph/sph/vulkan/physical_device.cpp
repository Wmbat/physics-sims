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

#include <sph/core.hpp>
#include <sph/vulkan/details/vulkan.hpp>
#include <sph/vulkan/physical_device.hpp>

#include <libphyseng/util/strong_ops/arithmetic.hpp>
#include <libphyseng/util/strong_type.hpp>

#undef let

#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/functional/arithmetic.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#define let auto const

#include <cstdint>
#include <optional>
#include <utility>
#include <ranges>

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

	auto rate_device_features(physical_device_info const& /*info*/) -> int
	{
		return 0;
	}

	auto rate_device_queue_support(physical_device_info const&) -> int {}

	auto combine_ratings(std::tuple<int, int> ratings) -> int
	{
		return std::get<0>(ratings) + std::get<1>(ratings);
	}

	/**
	 * @brief
	 *
	 * @param[in] lhs
	 * @param[in] rhs
	 *
	 * @return True if the lhs' rating is greater than the rhs'
	 */
	auto greater_rating(std::tuple<physical_device_info, int> lhs,
						std::tuple<physical_device_info, int> rhs) -> bool
	{
		return std::get<1>(lhs) > std::get<1>(rhs);
	}
} // namespace

namespace vulkan
{
	auto get_driver_version(device_vendor vendor, std::uint32_t version)
		-> physeng::semantic_version const
	{
		if (vendor == device_vendor::nvidia)
		{
			return {.major = (version >> 22u) & 0x3ffu, // NOLINT
					.minor = (version >> 14u) & 0x0ffu, // NOLINT
					.patch = (version >> 6u) & 0x0ffu}; // NOLINT
		}

		return vulkan::from_vulkan_version(version);
	}

	auto get_device_vendor_from_id(vendor_id id) -> std::optional<device_vendor>
	{
		if (id.get() == std::to_underlying(device_vendor::amd))
		{
			return device_vendor::amd;
		}
		else if (id.get() == std::to_underlying(device_vendor::arm))
		{
			return device_vendor::arm;
		}
		else if (id.get() == std::to_underlying(device_vendor::intel))
		{
			return device_vendor::intel;
		}
		else if (id.get() == std::to_underlying(device_vendor::nvidia))
		{
			return device_vendor::nvidia;
		}
		else if (id.get() == std::to_underlying(device_vendor::qualcomm))
		{
			return device_vendor::qualcomm;
		}
		else
		{
			return std::nullopt;
		}
	}

	auto find_best_suited_physical_device(std::span<vk::PhysicalDevice const> devices)
		-> physical_device
	{
		let supported_vendor_devices = devices 
			| rv::transform(populate_physical_device_info)
			| rv::filter(is_vendor_supported) 
			| ranges::to<std::vector>;

		let combined_ratings =
			rv::zip(supported_vendor_devices | rv::transform(rate_device_features),
					supported_vendor_devices | rv::transform(rate_device_queue_support))
			| rv::transform(combine_ratings);
		let device_ratings = rv::zip(supported_vendor_devices, combined_ratings);

		let best_device_it = ranges::max_element(device_ratings, greater_rating);
		if (best_device_it != ranges::end(device_ratings))
		{
			auto best_device = std::get<0>(*best_device_it);
		}

		return {};
	}
} // namespace vulkan
