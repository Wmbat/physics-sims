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
 * @file libphyseng/libphyseng/physeng-info.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 26/02/2023
 * @brief
 */

#include <libphyseng/util/semantic_version.hpp>
#include <libphyseng/version.hpp>

#include <string_view>

namespace physeng
{
	/**
	 * @brief Gives access to the current semantic version of the engine
	 */
	inline constexpr auto get_engine_version() -> semantic_version
	{
		return {.major = LIBPHYSENG_VERSION_MAJOR,
				.minor = LIBPHYSENG_VERSION_MINOR,
				.patch = LIBPHYSENG_VERSION_PATCH};
	}

	/**
	 * @brief Gives access to the name of the physics engine
	 */
	inline constexpr auto get_engine_name() -> std::string_view
	{
		return std::string_view{"physeng"};
	}
} // namespace physeng
