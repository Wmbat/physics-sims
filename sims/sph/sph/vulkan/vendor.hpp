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
 * @file sims/sph/sph/vulkan/vendor.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 12/03/2023
 * @brief
 */

#pragma once

#include <libphyseng/util/strong_ops/comparable.hpp>
#include <libphyseng/util/strong_type.hpp>

#include <cstdint>
#include <optional>

namespace vulkan
{
	/**
	 * @brief Represents an vendor id stored as a u32 by vulkan
	 */
	using vendor_id =
		physeng::strong_type<std::uint32_t, struct vender_id_tag, physeng::strong::equatable>;

	/**
	 * @brief Enum representing the supported GPU vendors
	 */
	enum struct device_vendor
	{
		amd = 0x1002,	  ///<
		arm = 0x13b5,	  ///<
		intel = 0x8086,	  ///<
		nvidia = 0x10de,  ///<
		qualcomm = 0x5143 ///<
	};

	/**
	 * @brief Takes the vendor id number of the vulkan physical device and converts it into a more
	 * usable `device_vendor` enum value
	 *
	 * @param[in] id The `vendor_id` retrieved from the vulkan physical device
	 *
	 * @return The `device_vendor` associated with the `vendor_id` or an empty std::optional if the
	 * `vendor_id` does not match any of the supported vendors
	 */
	auto get_device_vendor_from_id(vendor_id id) -> std::optional<device_vendor>;
} // namespace vulkan
