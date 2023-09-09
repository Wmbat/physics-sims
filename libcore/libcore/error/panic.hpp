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
 * @file libcore/libcore/error/panic.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 09/08/2023
 * @brief Implementation of a panic function in care of critical errors
 */


#pragma once

#include <fmt/format.h>
#include <fmt/os.h>

#include <source_location>
#include <string_view>
#include <type_traits>

namespace core
{
	namespace detail
	{
		[[noreturn]] void panic_impl(std::string_view message) noexcept;
	}

	struct panic_dynamic_string_view
	{
		template<class Type>
			requires std::constructible_from<std::string_view, Type>
		panic_dynamic_string_view(
			Type const& str, std::source_location loc = std::source_location::current()) noexcept :
			str{str},
			loc{loc}
		{}

		std::string_view str;
		std::source_location loc;
	};

	template<class... Args>
	struct panic_format
	{
		template<class Type>
		consteval panic_format(Type const& fmt,
							   std::source_location loc = std::source_location::current()) :
			format{fmt},
			loc{loc}
		{}

		fmt::format_string<Args...> format;
		std::source_location loc;
	};

	[[noreturn]] inline void panic(panic_dynamic_string_view str) noexcept
	{
		auto const message =
			fmt::format("{}:{} panic: {}\n", str.loc.file_name(), str.loc.line(), str.str);
		detail::panic_impl(message);
	}

	template<class... Args>
	[[noreturn]] void panic(panic_format<std::type_identity_t<Args>...> fmt,
							Args&&... args) noexcept
		requires(sizeof...(Args) > 0)
	{
		auto const message = fmt::format("{}:{} panic: {}\n", fmt.loc.file_name(), fmt.loc.line(),
										 fmt::format(fmt.format, std::forward<Args>(args)...));
		detail::panic_impl(message);
	}
} // namespace core
