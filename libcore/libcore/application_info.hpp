/**
 * @file libcore/libcore/application_info.hpp
 * @author wmbat wmbat-dev@protonmail.com
 * @date 09/22/2023
 * @brief Contains the information about the user's application
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

#include <libcore/semantic_version.hpp>

#include <string>

namespace core
{
    /**
     * @brief Information about a specific information.
     */
    struct application_info
    {
        std::string_view name;    ///< The name of the application
        semantic_version version; ///< The version of the application
    };
} // namespace core
