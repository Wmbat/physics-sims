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

#include <libcore/vulkan/include.hpp>

#include <magic_enum.hpp>

namespace
{
    struct vulkan_error_category : public std::error_category
    {
        [[nodiscard]] auto name() const noexcept -> char const* override { return "vulkan error"; }
        [[nodiscard]] auto message(int error_code) const noexcept -> std::string override
        {
            auto const enum_name = magic_enum::enum_name(static_cast<::vk::Result>(error_code));
            return std::string{enum_name};
        }
    };
} // namespace

namespace core::vk
{
    auto make_error_code(::vk::Result error_code) -> std::error_code
    {
        static auto error_category = vulkan_error_category{};
        return std::error_code{static_cast<int>(error_code), error_category};
    }
} // namespace core::vk
