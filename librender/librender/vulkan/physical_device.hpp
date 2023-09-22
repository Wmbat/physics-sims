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

#include <librender/vulkan/include.hpp>
#include <librender/vulkan/instance.hpp>

#include <libcore/error/error.hpp>

#include <tl/expected.hpp>

#include <range/v3/action/sort.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/range/operations.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <concepts>
#include <ranges>

namespace render::vk
{
    enum struct physical_device_selection_error
    {
        no_physical_devices_found,
        no_suitable_physical_devices_found
    };

    auto make_error_code(physical_device_selection_error error_code) -> std::error_code;

    auto select_physical_device(instance const& instance) -> tl::expected<::vk::PhysicalDevice, core::error>;
} // namespace render::vk
