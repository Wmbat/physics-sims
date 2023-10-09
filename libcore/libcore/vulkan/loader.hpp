/**
 * @file libcore/libcore/vulkan/loader.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 09/22/2023
 * @brief Implements functionality to load vulkan handles at runtime.
 * @copyright Copyright 2023 wmbat
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

#include <libcore/vulkan/include.hpp>

#include <spdlog/logger.h>

namespace core::vk
{
    /**
     * @brief Loads the vulkan symbols using a \ref ::vk::DynamicLoader
     *
     * @param[in] logger
     */
    auto load_vulkan_symbols(spdlog::logger& logger) -> ::vk::DynamicLoader;
} // namespace render::vk
