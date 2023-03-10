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

#include <libphyseng/version.hpp>

#include <fmt/format.h>

#include <cstdint>

namespace physeng
{
	/**
	 * @brief Represent a format convention for determining a version encoded using a major, minor
	 * and patch
	 */
	struct semantic_version
	{
		std::uint32_t major; //< The major number of the version
		std::uint32_t minor; //< The minor version of the number
		std::uint32_t patch; //< The patch version of the number
	};
} // namespace physeng

template<>
struct fmt::formatter<physeng::semantic_version> : fmt::formatter<std::string_view>
{
	template<typename FormatContext>
	auto format(physeng::semantic_version const& version, FormatContext& ctx) const
	{
		return fmt::format_to(ctx.out(), "{}.{}.{}", version.major, version.minor, version.patch);
	}
};
