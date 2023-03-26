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

/**
 * @file sims/sph/sph/vulkan/physical_device.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 26/02/2023
 * @brief
 */

#pragma once

#include <sph/vulkan/details/vulkan.hpp>
#include <sph/vulkan/vendor.hpp>

#include <libphyseng/util/semantic_version.hpp>
#include <libphyseng/util/strong_ops/comparable.hpp>
#include <libphyseng/util/strong_type.hpp>

#include <cstdint>
#include <optional>
#include <span>

namespace vulkan
{
	/**
	 * @brief
	 */
	struct physical_device
	{
		std::string name;
		device_vendor vendor;
		physeng::semantic_version driver_version;

		vk::PhysicalDevice value;
	};

	/**
	 * @brief 
	 *
	 * @param[in] vendor The device vendor of the physical device.
	 * @param[in] packed_version The packed version given by the vulkan physical device.
	 *
	 * @return The extracted semantic version of the driver
	 */
	auto get_driver_version(device_vendor vendor, std::uint32_t packed_version)
		-> physeng::semantic_version;

	/**
	 * @brief
	 *
	 * @param[in] devices The list of available physical devices
	 *
	 * @return
	 */
	auto find_best_suited_physical_device(std::span<vk::PhysicalDevice const> devices)
		-> std::optional<physical_device>;
} // namespace vulkan
