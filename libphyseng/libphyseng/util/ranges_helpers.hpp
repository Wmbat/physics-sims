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
 * @file libphyseng/libphyseng/util/ranges_helpers.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 11/03/2023
 * @brief
 */

#include <ranges>

namespace physeng::inline v0
{
	namespace detail
	{
		// clang-format off
		
		template<typename Type>
		concept can_invoke_empty = requires(Type&& t) { bool(t.empty()); } 
			or requires(Type&& t) { bool(t.is_empty()); };

		template<typename Type>
		concept can_invoke_size = !can_invoke_empty<Type>
			&& requires(Type&& t) { std::ranges::size(t); };

		template <typename Type>
		concept can_compare_begin_end = not can_invoke_empty<Type> 
			&& not can_invoke_size<Type>
			&& requires(Type&& t) { 
				bool(std::ranges::begin(t) == std::ranges::end(t));
				{ std::ranges::begin(t) } -> std::forward_iterator;
			};

		// clang-format on

		struct is_empty_invocable
		{
			[[nodiscard]] constexpr auto operator()(can_invoke_empty auto&& t) const -> bool
			{
				return t.empty();
			}

			[[nodiscard]] constexpr auto operator()(can_invoke_size auto&& t) const -> bool
			{
				return std::ranges::size(t) == 0;
			}

			[[nodiscard]] constexpr auto operator()(can_compare_begin_end auto&& t) const -> bool
			{
				return std::ranges::begin(t) == std::ranges::end(t);
			}
		};
	} // namespace detail

	inline constexpr auto is_empty = detail::is_empty_invocable{};

} // namespace physeng::inline v0
