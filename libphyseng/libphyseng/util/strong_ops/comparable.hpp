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
	struct equatable : crtp<Type, equatable>
	{
		constexpr auto operator==(equatable<Type> const& other) const -> bool
		{
			return this->underlying().get() == other.underlying().get();
		}
	};

	template<typename Type>
	struct comparable : crtp<Type, comparable>
	{
		constexpr auto operator<=>(equatable<Type> const& other) const
		{
			return this->underlying().get() <=> other.underlying().get();
		}
	};
} // namespace physeng::inline v0::strong
