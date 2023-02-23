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


#pragma once

#include <sph/vulkan/details/vulkan.hpp>

#include <libphyseng/util/semantic_version.hpp>
#include <libphyseng/util/strong_ops/comparable.hpp>
#include <libphyseng/util/strong_type.hpp>

#include <cstdint>

namespace vulkan
{
	using vendor_id =
		physeng::strong_type<std::uint32_t, struct vender_id_tag, physeng::strong::equatable>;
	using driver_version = physeng::strong_type<std::uint32_t, struct driver_version_tag>;

	static constexpr auto const nvidia_vendor_id = vendor_id{4318};

	auto get_driver_version(vendor_id id, driver_version version) -> physeng::semantic_version const;
} // namespace vulkan
