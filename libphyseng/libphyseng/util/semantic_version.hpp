#pragma once

#include <libphyseng/version.hpp>

#include <cstdint>

namespace physeng
{
	struct semantic_version
	{
		std::uint32_t major;
		std::uint32_t minor;
		std::uint32_t patch;
	};
} // namespace physeng
