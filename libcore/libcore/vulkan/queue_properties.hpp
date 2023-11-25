/**
 * @file libcore/libcore/vulkan/queue_properties.hpp
 * @brief Contains code to select a physical device queue
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

#include "libcore/vulkan/include.hpp"

namespace core::vk
{
    /**
     * @brief Properties of a physical device queue.
     */
    struct queue_properties
    {
        /**
         * @brief The operations that are supported by the queue.
         */
        ::vk::QueueFlags flags;

        /**
         * @brief The index of the queue family.
         */
        std::size_t family_index;

        /**
         * @brief The index of the queue within the family.
         */
        std::size_t queue_index;
    };
} // namespace core::vk
