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

#include <librender/vulkan/include.hpp>

#include <libcore/application_info.hpp>
#include <libcore/semantic_version.hpp>

#include <spdlog/logger.h>

#include <tl/expected.hpp>

namespace render::vk
{
	struct instance
	{
	public:
		static auto make(core::application_info const& app_info, spdlog::logger& logger)
			-> tl::expected<instance, std::error_code>;

	private:
		instance(::vk::DynamicLoader&& loader, ::vk::UniqueInstance instance,
				 ::vk::UniqueDebugUtilsMessengerEXT debug_utils);

	private:
		::vk::DynamicLoader m_loader = {};
		::vk::UniqueInstance m_instance = {};
		::vk::UniqueDebugUtilsMessengerEXT m_debug_utils = {};
	};
} // namespace render::vk
