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
	struct pre_decrementable : crtp<Type, pre_decrementable>
	{
		constexpr auto operator--() -> Type&
		{
			--this->underlying().get();
			return this->underlying();
		}
	};

	template<typename Type>
	struct post_decrementable : crtp<Type, post_decrementable>
	{
		constexpr auto operator--(int) -> const Type { return Type(this->underlying().get()--); }
	};

	template<typename Type>
	struct decrementable : pre_decrementable<Type>, post_decrementable<Type>
	{
		using post_decrementable<Type>::operator--;
		using pre_decrementable<Type>::operator--;
	};
} // namespace physeng::inline v0::strong
