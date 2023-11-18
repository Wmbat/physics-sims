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

#include "libcore/error/error.hpp"

#include "catch2/catch_test_macros.hpp"

#include "fmt/core.h"

#include <format>
#include <optional>
#include <string>
#include <system_error>

SCENARIO("converting an core::error to string", "[libcore]") // NOLINT
{
    GIVEN("An error without code or context")
    {
        auto const err = core::error{};

        WHEN("Converting to string")
        {
            auto const err_str = fmt::format("{0}", err);

            THEN("string should be empty")
            {
                REQUIRE(err_str.empty()); // NOLINT
            }
        }
    }

    GIVEN("An error without code, but with context")
    {
        auto const str = std::string{"This is a test context message"};
        auto const err = core::error{.error_code = std::nullopt, .context = str};

        WHEN("Converting to string")
        {
            auto const err_str = fmt::format("{0}", err);

            THEN("error string should equal to context string")
            {
                REQUIRE(err_str == str); // NOLINT
            }
        }
    }

    GIVEN("An error with code, but without context")
    {
        auto const err_code = std::error_code{};
        auto const err = core::error{.error_code = err_code};

        WHEN("Converting to string")
        {
            auto const err_str = fmt::format("{0}", err);

            THEN("error string should contain the category name, error code name & value")
            {
                auto const err_code_str = std::format("{}: {} (with error value {})", err_code.category().name(),
                                                      err_code.message(), err_code.value());
                REQUIRE(err_str == err_code_str); // NOLINT
            }
        }
    }

    GIVEN("An error with code and context")
    {
        auto const err_code = std::error_code{};
        auto const err_ctx = std::string{"Hello, World !"};
        auto const err = core::error{.error_code = err_code, .context = err_ctx};

        WHEN("Converting to string")
        {
            auto const err_str = fmt::format("{0}", err);

            THEN("error string should contain the category name, error code name, value & context message")
            {
                auto const err_code_str = std::format("{1}:\n\t- {2} (with error value {3})\n\t- {0}", err_ctx,
                                                      err_code.category().name(), err_code.message(), err_code.value());
                REQUIRE(err_str == err_code_str); // NOLINT
            }
        }
    }
}
