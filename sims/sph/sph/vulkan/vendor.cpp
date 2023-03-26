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

#include <sph/vulkan/vendor.hpp>

namespace vulkan
{
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
} // namespace vulkan
