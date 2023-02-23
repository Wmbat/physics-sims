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


#include <sph/vulkan/physical_device.hpp>

#include <sph/core.hpp>
#include <sph/vulkan/details/vulkan.hpp>

namespace vulkan
{
	auto get_driver_version(vendor_id id, driver_version version)
		-> physeng::semantic_version const
	{
		if (id == nvidia_vendor_id)
		{
			return {.major = (version.get() >> 22u) & 0x3ffu,
					.minor = (version.get() >> 14u) & 0x0ffu,
					.patch = (version.get() >> 6u) & 0x0ffu};
		}

		return vulkan::from_vulkan_version(version.get());
	}
} // namespace vulkan
