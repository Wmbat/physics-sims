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

#include <libphyseng/util/crtp.hpp>

namespace physeng::inline v0::strong // NOLINT
{
	template<typename Type>
	struct binary_addable : crtp<Type, binary_addable>
	{
		constexpr auto operator+(Type const& other) const -> Type
		{
			return Type(this->underlying().get() + other.get());
		}
		auto operator+=(Type const& other) -> Type&
		{
			this->underlying().get() += other.get();
			return this->underlying();
		}
	};

	template<typename Type>
	struct unary_addable : crtp<Type, unary_addable>
	{
		constexpr auto operator+() const -> Type { return Type(+this->underlying().get()); }
	};

	template<typename Type>
	struct addable : binary_addable<Type>, unary_addable<Type>
	{
		using binary_addable<Type>::operator+;
		using unary_addable<Type>::operator+;
	};
} // namespace physeng::inline v0::strong
