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

#include <librender/system.hpp>
#include <librender/vulkan/instance.hpp>

#include <spdlog/logger.h>

#include <chrono>

namespace render
{
	auto system::make(core::application_info const& app_info, spdlog::logger& logger)
		-> tl::expected<system, std::error_code>
	{
		[[maybe_unused]] auto instance = vk::instance::make(app_info, logger);

		return {};
	}

	void system::update(std::chrono::milliseconds) {}
} // namespace render