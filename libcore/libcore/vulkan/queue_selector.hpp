/**
 * @file libcore/libcore/vulkan/queue_selector.hpp
 * @brief Utilities to simplify and generalize the selection of vulkan queues.
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

#pragma once

#include "libcore/vulkan/queue_properties.hpp"

#include <vulkan/vulkan_structs.hpp>

#include <span>
#include <unordered_map>

namespace spdlog
{
    class logger;
}

namespace core::vk
{
    using queue_family_map = std::unordered_map<std::size_t, std::vector<::vk::QueueFlags>>;

    /**
     * @brief Tries to select the best queues based on their purpose from a list of available queues.
     */
    struct queue_selector
    {
    public:
        /**
         * @brief Enable logging for the selection process, this will give extra information about what is happening.
         * @since 0.1.0
         *
         * @param[in] logger The logger to use.
         */
        auto with_logger(spdlog::logger& logger) -> queue_selector&;

        /**
         * @brief Specify the amount of compute queues to find.
         * @warning There may not be the correct amount of queue if no device supports the amount requested.
         * @todo Implement precondition checking with jeremy-rifkin's libassert
         * @since 0.1.0
         *
         * @pre @p desired_queue_count > 0
         *
         * @param[in] desired_queue_count The number of compute queues that should be created.
         */
        auto with_compute_queues(int desired_queue_count = 1) -> queue_selector&;

        /**
         * @brief Specify the amount of compute queues to find.
         * @warning There may not be the correct amount of queue if no device supports the amount requested.
         * @todo Implement precondition checking with jeremy-rifkin's libassert
         * @since 0.1.0
         *
         * @pre @p desired_queue_count > 0
         *
         * @param[in] desired_queue_count The number of graphics queues that should be created.
         */
        auto with_graphics_queues(int desired_queue_count = 1) -> queue_selector&;

        /**
         * @brief Specity whether the selection should try to find transfer queues, and set the amount of compute
         * queues to create.
         * @warning There may not be the correct amount of queue if no device supports the amount requested.
         * @todo Implement precondition checking with jeremy-rifkin's libassert
         * @since 0.1.0
         *
         * @pre @p desired_queue_count > 0
         *
         * @param[in] desired_queue_count The number of transfer queues that should be created.
         */
        auto with_transfer_queues(int desired_queue_count = 1) -> queue_selector&;

        /**
         * @brief Selects the queues.
         * @since 0.1.0
         */
        auto select_from(std::span<const ::vk::QueueFamilyProperties> queue_families) -> queue_family_map;

    private:
        spdlog::logger* m_logger = nullptr;

        int m_compute_queue_count = 0;
        int m_graphics_queue_count = 1;
        int m_transfer_queue_count = 1;
    };
} // namespace core::vk
