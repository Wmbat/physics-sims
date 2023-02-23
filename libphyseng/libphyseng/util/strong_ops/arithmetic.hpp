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

#include <libphyseng/util/strong_ops/addable.hpp>
#include <libphyseng/util/strong_ops/comparable.hpp>
#include <libphyseng/util/strong_ops/decrementable.hpp>
#include <libphyseng/util/strong_ops/divisible.hpp>
#include <libphyseng/util/strong_ops/incrementable.hpp>
#include <libphyseng/util/strong_ops/multiplicable.hpp>
#include <libphyseng/util/strong_ops/subtractable.hpp>

namespace physeng::inline v0::strong // NOLINT
{
	template<typename Type>
	struct arithmetic :
		incrementable<Type>,
		decrementable<Type>,
		addable<Type>,
		subtractable<Type>,
		multiplicable<Type>,
		divisible<Type>,
		comparable<Type>
	{
	};

} // namespace physeng::inline v0::strong
