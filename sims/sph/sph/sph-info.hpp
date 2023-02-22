#pragma once

#include <sph/version.hpp>

#include <libphyseng/util/semantic_version.hpp>

#include <string_view>

inline constexpr auto get_version() -> physeng::semantic_version
{
	return {.major = SPH_VERSION_MAJOR, .minor = SPH_VERSION_MINOR, .patch = SPH_VERSION_PATCH};
}

inline constexpr auto get_name() -> std::string_view
{
	return "sph";
}
