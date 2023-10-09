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

#include <libcore/vulkan/instance.hpp>

#include <libcore/vulkan/include.hpp>
#include <libcore/vulkan/loader.hpp>

#include <libcore/application_info.hpp>
#include <libcore/semantic_version.hpp>

#include <spdlog/logger.h>

#include <tl/expected.hpp>

#include <magic_enum.hpp>

#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/view/transform.hpp>

#include <cstdint>
#include <optional>
#include <system_error>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE; // NOLINT

namespace
{
    auto get_validation_message_type(VkDebugUtilsMessageTypeFlagsEXT messageType) -> std::optional<std::string>
    {
        if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
        {
            return "GENERAL";
        }

        if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
        {
            return "VALIDATION";
        }

        if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        {
            return "PERFORMANCE";
        }

        return std::nullopt;
    }

    static VKAPI_ATTR auto VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     VkDebugUtilsMessengerCallbackDataEXT const* p_callback_data,
                                                     void* p_user_data) -> VkBool32
    {
        assert(p_user_data != nullptr); // NOLINT

        if (not(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                or messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT))
        {
            return VK_FALSE;
        }

        auto& logger = *static_cast<spdlog::logger*>(p_user_data);
        if (auto const type = get_validation_message_type(messageType))
        {
            logger.error("{0} - {1}", type.value(), p_callback_data->pMessage);
        }
        else
        {
            logger.error("{0}", p_callback_data->pMessage);
        }

        return VK_FALSE;
    }
} // namespace

namespace core::vk::detail
{
    using namespace std::literals;

    auto is_khr_validation_layer(::vk::LayerProperties const& property) -> bool
    {
        return std::string_view{property.layerName} == "VK_LAYER_KHRONOS_validation"sv;
    }

    auto is_debug_utils_ext(::vk::ExtensionProperties const& property) -> bool
    {
        return std::string_view{property.extensionName} == "VK_EXT_debug_utils"sv;
    }

    /**
     * @brief Get the list of validation layers to enable for the vulkan instance.
     */
    [[nodiscard]] auto get_desired_validation_layers() -> std::vector<char const*>
    {
        auto enabled_layers = std::vector<char const*>{};
        if constexpr (should_enable_validation_layers)
        {
            auto const [result, properties] = ::vk::enumerateInstanceLayerProperties();
            if (result == ::vk::Result::eSuccess)
            {
                if (ranges::find_if(properties, is_khr_validation_layer) != ranges::end(properties))
                {
                    enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
                }
            }
        }

        return enabled_layers;
    }

    /**
     * @brief Get the list of extension to enable for the vulkan instance.
     */
    [[nodiscard]] auto get_desired_extensions() -> std::vector<char const*>
    {
        auto enabled_exts = std::vector<char const*>{};

        if constexpr (should_enable_validation_layers)
        {
            auto const [result, properties] = ::vk::enumerateInstanceExtensionProperties();
            if (result == ::vk::Result::eSuccess)
            {
                if (ranges::find_if(properties, is_debug_utils_ext) != ranges::end(properties))
                {
                    enabled_exts.push_back("VK_EXT_debug_utils");
                }
            }
        }

        return enabled_exts;
    }

    /**
     * @brief Get the version of the local vulkan API implementation and checks if the version is supported.
     */
    auto get_vulkan_version() -> tl::expected<core::semantic_version, core::error>
    {
        auto const [result, version] = ::vk::enumerateInstanceVersion();
        if (result != ::vk::Result::eSuccess)
        {
            return tl::unexpected(core::error{.error_code = vk::make_error_code(result)});
        }

        auto const unpacked_version = from_vulkan_version(version);
        auto const unpacked_min_version = from_vulkan_version(VK_API_VERSION_1_3);
        if (unpacked_version < unpacked_min_version)
        {
            return tl::unexpected(core::error{
                .error_code = make_error_code(instance_error::vulkan_version_too_low),
                .context = fmt::format("The most recent version found is {}, but the minimum required version is {}. "
                                       "Please upgrade Vulkan to a newer version.",
                                       unpacked_version, unpacked_min_version)});
        }

        return unpacked_version;
    }

    auto create_vk_instance(core::application_info const& app_info, core::semantic_version const& vulkan_version,
                            spdlog::logger& logger) -> tl::expected<::vk::UniqueInstance, core::error>
    {
        auto enabled_layers = get_desired_validation_layers();
        auto enabled_exts = get_desired_extensions();

        auto const vk_info = ::vk::ApplicationInfo{}
                                 .setApiVersion(to_vulkan_version(vulkan_version))
                                 .setPApplicationName(app_info.name.data())
                                 .setApplicationVersion(core::vk::to_vulkan_version(app_info.version));
        auto const create_info = ::vk::InstanceCreateInfo{}
                                     .setPApplicationInfo(&vk_info)
                                     .setPEnabledLayerNames(enabled_layers)
                                     .setPEnabledExtensionNames(enabled_exts);
        auto [result, instance] = ::vk::createInstanceUnique(create_info);
        if (result != ::vk::Result::eSuccess)
        {
            return tl::unexpected(core::error{.error_code = vk::make_error_code(result)});
        }

        VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

        logger.debug("Vulkan instance has been created.");

        return std::move(instance);
    }

    auto create_vk_debug_utils(::vk::Instance instance, spdlog::logger& logger)
        -> tl::expected<::vk::UniqueDebugUtilsMessengerEXT, core::error>
    {
        if constexpr (not core::vk::should_enable_validation_layers)
        {
            return {};
        }

        auto const extensions = get_desired_extensions();
        auto const is_debug_utils = [](char const* name) {
            return std::string_view{name} == "VK_EXT_debug_utils"sv;
        };
        if (ranges::find_if(extensions, is_debug_utils) == ranges::end(extensions))
        {
            return {};
        }

        auto const create_info = ::vk::DebugUtilsMessengerCreateInfoEXT()
                                     .setMessageSeverity(::vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                                                         | ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                                         | ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
                                     .setMessageType(::vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                                                     | ::vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                                                     | ::vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
                                     .setPfnUserCallback(debug_callback)
                                     .setPUserData(static_cast<void*>(&logger));

        auto [result, debug_utils] =
            instance.createDebugUtilsMessengerEXTUnique(create_info, nullptr, VULKAN_HPP_DEFAULT_DISPATCHER);
        if (result == ::vk::Result::eSuccess)
        {
            logger.debug("Vulkan debug utilities have been created.");

            return std::move(debug_utils);
        }
        else
        {
            return tl::unexpected{core::error{.error_code = make_error_code(result),
                                              .context = "Failed to create the debug utils. There will be no "
                                                         "Vulkan API debug reporting from now on"}};
        }
    }
} // namespace core::vk::detail

namespace core::vk
{
    struct instance_error_category : public std::error_category
    {
        [[nodiscard]] auto name() const noexcept -> char const* override { return "instance error"; }

        [[nodiscard]] auto message(int error_code) const noexcept -> std::string override
        {
            auto const enum_name = magic_enum::enum_name(static_cast<instance_error>(error_code));
            return std::string{enum_name};
        }
    };

    auto make_error_code(instance_error error_code) -> std::error_code
    {
        static auto error_category = instance_error_category{};
        return {static_cast<int>(error_code), error_category};
    }

    auto instance::make(core::application_info const& app_info, spdlog::logger& logger)
        -> tl::expected<instance, core::error>
    {
        auto loader = load_vulkan_symbols(logger);
        return detail::get_vulkan_version()
            .and_then([&](semantic_version version) {
                logger.info("Using Vulkan version {}", version);
                return detail::create_vk_instance(app_info, version, logger);
            })
            .transform([&](::vk::UniqueInstance instance) {
                auto result = detail::create_vk_debug_utils(instance.get(), logger);
                if (not result.has_value())
                {
                    logger.warn("{}", result.error());
                }

                return core::vk::instance{std::move(loader), std::move(instance), std::move(result).value()};
            });
    }

    instance::instance(::vk::DynamicLoader&& loader, ::vk::UniqueInstance&& instance,
                       ::vk::UniqueDebugUtilsMessengerEXT&& debug_utils) :
        m_loader(std::move(loader)),
        m_instance(std::move(instance)), m_debug_utils(std::move(debug_utils))
    {}

    auto instance::operator*() noexcept -> ::vk::Instance&
    {
        return m_instance.get();
    }

    auto instance::operator*() const noexcept -> ::vk::Instance const&
    {
        return m_instance.get();
    }

    auto instance::operator->() noexcept -> ::vk::Instance*
    {
        return &m_instance.get();
    }

    auto instance::operator->() const noexcept -> ::vk::Instance const*
    {
        return &m_instance.get();
    }
} // namespace core::vk
