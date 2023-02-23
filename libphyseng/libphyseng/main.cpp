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


#include <libphyseng/main.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/tail.hpp>

#include <ranges>

auto main(int argc, char *argv[]) -> int
{
	namespace stdr = std::ranges;

	auto const args = ranges::span{argv, argc} | ranges::to<std::vector<std::string_view>>;

	// If we have no arguments, there is something wrong
	if (stdr::empty(args))
	{
		return -1;
	}

	physeng_main(args);

	return 0;
}
