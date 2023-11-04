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

#include <libcore/application_info.hpp>
#include <libcore/core.hpp>
#include <libcore/error/panic.hpp>
#include <libcore/error/error.hpp>

#include <librender/system.hpp>

#include <fmt/core.h>

#include <spdlog/logger.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/details/console_globals.h>
#include <spdlog/details/file_helper-inl.h>
#include <spdlog/sinks/ansicolor_sink-inl.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/base_sink-inl.h>
#include <spdlog/sinks/basic_file_sink-inl.h>
#include <spdlog/sinks/sink-inl.h>

#include <range/v3/range/conversion.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/view/span.hpp>

#include <tl/expected.hpp>

#include <string>
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <string_view>
#include <vector>

using namespace std::literals::chrono_literals;

namespace
{
    auto create_logger(std::string_view name) -> spdlog::logger
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[%n] [%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::string{name} + ".logs", true);
        file_sink->set_pattern("[%H:%M:%S.%f] [%n] [%^%l%$] %v");
        file_sink->set_level(spdlog::level::trace);

        auto logger = spdlog::logger(std::string(name), {console_sink, file_sink});
        logger.set_level(spdlog::level::trace);

        return logger;
    }
} // namespace

auto main(int argc, char* argv[]) -> int
{
    auto const args = ranges::span{argv, argc} | ranges::to<std::vector<std::string_view>>;

    // If we have no arguments, there is something wrong
    if (ranges::empty(args))
    {
        core::panic("No input parameters given !");
    }

    auto const app_info = core::application_info{.name = args[0], .version = get_version()};
    auto app_logger = create_logger(app_info.name);

    if (auto res = render::system::make(app_info, app_logger))
    {
        auto& render_system = res.value();
        render_system.update(16ms);
    }
    else
    {
        app_logger.error("Failed to create the rendering system.");
        app_logger.error("Failure Cause: {}", res.error());

        return EXIT_FAILURE;
    }

    return 0;
}
