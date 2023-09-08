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

#include <librender/vulkan/instance.hpp>

#include <tl/expected.hpp>

#include <system_error>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE; // NOLINT

namespace render::vk
{
	auto instance::make(std::string_view app_name, spdlog::logger& logger)
		-> tl::expected<instance, std::error_code>
	{
	}

	instance::instance(::vk::DynamicLoader&& loader, ::vk::UniqueInstance instance,
					   ::vk::UniqueDebugUtilsMessengerEXT debug_utils) :
		m_loader(std::move(loader)),
		m_instance(std::move(instance)), m_debug_utils(std::move(debug_utils))
	{}
} // namespace render::vk
