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

#include <libcore/semantic_version.hpp>

#if !defined(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC)
#	define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif

#if defined(__clang__)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wdeprecated-copy"
#	pragma clang diagnostic ignored "-Wdefaulted-function-deleted"
#elif defined(__GNUC__) || defined(__GNUG__)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <vulkan/vulkan.hpp>

#if defined(__clang__)
#	pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#	pragma GCC diagnostic pop
#endif

namespace render::vk
{
#if defined(NDEBUG)
	static constexpr bool should_enable_validation_layers = false;
#else
	static constexpr bool should_enable_validation_layers = true;
#endif

	constexpr auto to_vulkan_version(core::semantic_version const& version) -> std::uint32_t
	{
		return VK_MAKE_API_VERSION(0, version.major, version.minor, version.patch);
	}

	constexpr auto from_vulkan_version(std::uint32_t version) -> core::semantic_version
	{
		return {.major = VK_API_VERSION_MAJOR(version),
				.minor = VK_API_VERSION_MINOR(version),
				.patch = VK_API_VERSION_PATCH(version)};
	}
} // namespace render::vk
