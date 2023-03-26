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
 * @file libphyseng/libphyseng/util/enum_helpers.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 12/03/2023
 * @brief
 */

#include <fmt/format.h>

#include <magic_enum.hpp>

/**
 * @brief 
 */
template<typename Enum>
	requires(std::is_enum_v<Enum> and std::is_same_v<char, std::string_view::value_type>)
struct fmt::formatter<Enum> : fmt::formatter<std::string_view, char>
{
	auto format(Enum val, auto& ctx) const
	{
		return fmt::format_to(ctx.out(), "{}", magic_enum::enum_name(val));
	}
};
