/**
 * @file libcore/libcore/error/error.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 09/22/2023
 * @brief Implement utilities and classes for error handling.
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

#include <string>
#include <system_error>

#include <fmt/core.h>

namespace core
{
    /**
     * @brief A class that holds information about an error that occured in the code.
     */
    struct error
    {
        std::error_code error_code; ///< The actual error code that was emitted by the code.
        std::string context;        ///< Context message to provide additional information in logs.
    };
} // namespace core

/**
 * @brief Custom formatter to convert an \ref core::error_code to string.
 */
template<>
struct fmt::formatter<core::error>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<class FormatContext>
    auto format(core::error const& error, FormatContext& ctx)
    {
        auto const& error_code = error.error_code;
        return fmt::format_to(ctx.out(), "{0} ({1}: {2})", error.context, error_code.category().name(),
                              error_code.message());
    };
};
