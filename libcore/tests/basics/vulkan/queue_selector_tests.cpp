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

#include "catch2/catch_test_macros.hpp"

#include "libcore/vulkan/queue_selector.hpp"

SCENARIO("Selecting queues with default parameters", "[libcore][vulkan]") // NOLINT
{
    GIVEN("An empty list of queue families")
    {
        auto queue_families = std::vector<vk::QueueFamilyProperties>{};

        WHEN("The selecting queues")
        {
            auto const result = core::vk::queue_selector{}.select_from(queue_families);

            THEN("should return an empty list")
            {
                REQUIRE(result.empty()); // NOLINT
            }
        }
    }
}
