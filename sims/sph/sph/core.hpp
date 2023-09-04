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

#include <sph/version.hpp>

#include <libcore/semantic_version.hpp>

#include <string_view>

#define let auto const

inline constexpr auto get_version() -> core::semantic_version
{
	return {.major = SPH_VERSION_MAJOR, .minor = SPH_VERSION_MINOR, .patch = SPH_VERSION_PATCH};
}
