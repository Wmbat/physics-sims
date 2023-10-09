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

#include <libcore/vulkan/instance.hpp>
#include <libcore/vulkan/loader.hpp>
#include <libcore/vulkan/physical_device.hpp>

#include <spdlog/logger.h>

#include <chrono>
#include <vulkan/vulkan.hpp>

namespace render
{
    auto system::make(core::application_info const& app_info, spdlog::logger& logger)
        -> tl::expected<system, core::error>
    {
        return core::vk::instance::make(app_info, logger)
            .and_then([&](core::vk::instance&& instance) -> tl::expected<system, core::error> {
                auto device = core::vk::select_physical_device(instance);
                if (not device.has_value())
                {
                    return tl::unexpected{std::move(device).error()};
                }

                logger.info("Using gpu \"{}\"", device->get_name());

                return system{std::move(instance)};
            });
    }

    system::system(core::vk::instance&& instance) : m_instance{std::move(instance)} {}

    void system::update(std::chrono::milliseconds) {}
} // namespace render
