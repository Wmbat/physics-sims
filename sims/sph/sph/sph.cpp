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

#include <sph/core.hpp>
#include <sph/vulkan/details/vulkan.hpp>
#include <sph/vulkan/instance.hpp>
#include <sph/vulkan/physical_device.hpp>
#include <sph/vulkan/vendor.hpp>

#include <libphyseng/main.hpp>
#include <libphyseng/util/enum_helpers.hpp>
#include <libphyseng/util/semantic_version.hpp>

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace
{
	auto create_logger(std::string_view name) -> spdlog::logger
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::trace);
		console_sink->set_pattern("[%n] [%^%l%$] %v");

		auto file_sink =
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::string{name} + ".logs", true);
		file_sink->set_pattern("[%H:%M:%S.%f] [%n] [%^%l%$] %v");
		file_sink->set_level(spdlog::level::trace);

		auto logger = spdlog::logger(std::string(name), {console_sink, file_sink});
		logger.set_level(spdlog::level::trace);

		return logger;
	}
} // namespace

void physeng_main(std::span<const std::string_view> args)
{
	let app_name = args[0];

	auto app_logger = create_logger(app_name);

	let instance = vulkan::instance::make(app_name, app_logger).value();
	let physical_devices = instance->enumeratePhysicalDevices();
	let best_device = vulkan::find_best_suited_physical_device(physical_devices);

	app_logger.info("Selected device: {{name: {}, vendor: {}, driver version: {}}}\n",
					best_device->name, best_device->vendor, best_device->driver_version);
}
