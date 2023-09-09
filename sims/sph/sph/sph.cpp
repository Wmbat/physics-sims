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
#include <sph/version.hpp>

#include <libcore/application_info.hpp>
#include <libcore/core.hpp>
#include <libcore/error/panic.hpp>
#include <libcore/semantic_version.hpp>
#include <librender/system.hpp>

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

using namespace std::literals::chrono_literals;

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

void core_main(std::span<const std::string_view> args)
{
	auto const app_info = core::application_info{
		.name = args[0],
		.version = core::semantic_version{
			.major = SPH_VERSION_MAJOR, .minor = SPH_VERSION_MINOR, .patch = SPH_VERSION_MINOR}};

	auto app_logger = create_logger(app_info.name);

	if (auto res = render::system::make(app_info, app_logger))
	{
		auto render_system = res.value();
		render_system.update(16ms);
	}
	else
	{
		core::panic("{}", res.error());
	}
}
