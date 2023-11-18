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

#include <optional>
#include <string>
#include <system_error>

#include <fmt/core.h>

namespace core
{
    /**
     * @brief Represents the general idea of an error.
     * @details It contains optional information about the error code that caused the error as well as a string that
     * contains contextual information about the error.
     * @warning This struct should **NOT** be used for exception handling !
     *
     * Example usage:
     * @code{.cpp}
     * auto divide_int_by(int divident, int divisor) -> tl::expected<int, core::error>
     * {
     *     if (divider == 0)
     *     {
     *         // We create the error, giving it a message of what went wrong.
     *         auto error = core::error
     *         {
     *             .error_code = {},
     *             .context = "cannot divide by 0"
     *         };
     *
     *         return tl::unexpected(error);
     *     }
     *
     *     return dividend / divisor;
     * }
     * @endcode
     */
    struct error
    {
        std::optional<std::error_code> error_code; ///< The actual error code that was emitted by the code.
        std::string context;                       ///< Context message to provide additional information in logs.
    };
} // namespace core

/**
 * @brief Custom formatter to convert an core::error to string.
 *
 * Example usage:
 * @code{.cpp}
 * auto division_result = divide_int_by(10, 0);
 * if (!division_result)
 * {
 *     fmt::print("{}", division_result.error());
 *     // This should print "cannot divide by 0"
 * }
 * @endcode
 */
template<>
struct fmt::formatter<core::error>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<class FormatContext>
    auto format(core::error const& error, FormatContext& ctx)
    {
        auto const& error_code = error.error_code;
        if (!error_code)
        {
            return fmt::format_to(ctx.out(), "{0}", error.context);
        }

        if (error.context.empty())
        {
            return fmt::format_to(ctx.out(), "{0}: {1} (with error value {2})", error_code->category().name(),
                                  error_code->message(), error_code->value());
        }

        return fmt::format_to(ctx.out(), "{1}:\n\t- {2} (with error value {3})\n\t- {0}", error.context,
                              error_code->category().name(), error_code->message(), error_code->value());
    }
};
