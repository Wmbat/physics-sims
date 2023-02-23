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

#include <sph/vulkan/details/vulkan.hpp>

#include <spdlog/logger.h>

#include <tl/expected.hpp>

namespace vulkan
{
	enum struct instance_error
	{

	};

	class instance
	{
	public:
		static auto make(std::string_view app_name, spdlog::logger& logger)
			-> tl::expected<instance, instance_error>;

		/**
		 * Allow implicit conversion to a vulkan vk::Instance
		 */
		operator vk::Instance() const;

		[[nodiscard]] auto get() const -> vk::Instance;

	private:
		instance(vk::DynamicLoader&& loader, vk::UniqueInstance&& instance,
				 vk::UniqueDebugUtilsMessengerEXT&& debug);

	private:
		vk::DynamicLoader m_loader = {};
		vk::UniqueInstance m_instance = {};
		vk::UniqueDebugUtilsMessengerEXT m_debug_utils = {};
	};
} // namespace vulkan
