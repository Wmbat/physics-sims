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

#include <type_traits>
#include <utility>

namespace physeng
{
	template<typename Type, typename Parameter, template<typename> class... Skills>
	class strong_type : public Skills<strong_type<Type, Parameter, Skills...>>...
	{
	public:
		using underlying_type = Type;
		using reference_type = strong_type<Type&, Parameter, Skills...>;

	public:
		strong_type() = default;
		explicit constexpr strong_type(Type const& value) : value_(value) {}

		template<typename T_ = Type>
			requires(!std::is_reference_v<Type>)
		explicit constexpr strong_type(Type&& value) : value_(std::move(value))
		{}

		[[nodiscard]] constexpr auto get() noexcept -> Type& { return value_; }
		[[nodiscard]] constexpr auto get() const noexcept -> std::remove_reference_t<Type> const&
		{
			return value_;
		}

		operator reference_type() { return reference_type(value_); }

		struct argument
		{
			auto operator=(Type&& value) const -> strong_type
			{
				return strong_type(std::forward<Type>(value));
			}

			template<typename U>
			auto operator=(U&& value) const -> strong_type
			{
				return strong_type(std::forward<U>(value));
			}

			argument() = default;
			argument(argument const&) = delete;
			argument(argument&&) = delete;
			~argument() = default;
			auto operator=(argument const&) -> argument& = delete;
			auto operator=(argument&&) -> argument& = delete;
		};

	private:
		Type value_;
	};

	template<template<typename Type> class StrongType, typename T>
	constexpr auto make_strong(T const& value) -> StrongType<T>
	{
		return StrongType<T>(value);
	}
} // namespace physeng
