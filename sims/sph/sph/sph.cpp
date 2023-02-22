#include <sph/core.hpp>
#include <sph/vulkan/instance.hpp>

#include <libphyseng/main.hpp>
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
	auto instance = vulkan::instance::make(app_name, app_logger);
}
