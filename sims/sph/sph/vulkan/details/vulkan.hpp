#pragma once

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

#include <libphyseng/util/semantic_version.hpp>

#include <cstdint>

namespace vulkan
{
#if defined(NDEBUG)
	static constexpr bool should_enable_validation_layers = false;
#else
	static constexpr bool should_enable_validation_layers = true;
#endif

	constexpr auto to_vulkan_version(const physeng::semantic_version& version) -> std::uint32_t
	{
		return VK_MAKE_API_VERSION(0, version.major, version.minor, version.patch);
	}

	constexpr auto from_vulkan_version(std::uint32_t version) -> physeng::semantic_version
	{
		return {.major = VK_API_VERSION_MAJOR(version),
				.minor = VK_API_VERSION_MINOR(version),
				.patch = VK_API_VERSION_PATCH(version)};
	}
} // namespace vulkan
