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

#include <librender/vulkan/instance.hpp>

#include <libcore/application_info.hpp>
#include <libcore/semantic_version.hpp>

#include <spdlog/logger.h>

#include <tl/expected.hpp>

#include <optional>
#include <system_error>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE; // NOLINT

namespace
{
	using namespace std::literals;
	namespace stdr = std::ranges;

	auto get_validation_message_type(VkDebugUtilsMessageTypeFlagsEXT messageType)
		-> std::optional<std::string>
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

	static VKAPI_ATTR auto VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		VkDebugUtilsMessengerCallbackDataEXT const* p_callback_data, void* p_user_data) -> VkBool32
	{
		assert(p_user_data != nullptr); // NOLINT

		if (!(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
			  || messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT))
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

	auto load_vulkan_symbols(spdlog::logger& logger) -> vk::DynamicLoader
	{
		auto loader = vk::DynamicLoader{};

		VULKAN_HPP_DEFAULT_DISPATCHER.init(
			loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

		logger.debug("Vulkan symbols have been loaded.");

		return loader;
	}

	auto is_khr_validation_layer(vk::LayerProperties const& property) -> bool
	{
		return std::string_view{property.layerName} == "VK_LAYER_KHRONOS_validation"sv;
	}

	auto is_debug_utils_ext(vk::ExtensionProperties const& property) -> bool
	{
		return std::string_view{property.extensionName} == "VK_EXT_debug_utils"sv;
	}

	[[nodiscard]] auto get_desired_validation_layers() -> std::vector<char const*>
	{
		auto enabled_layers = std::vector<char const*>{};

		if constexpr (render::vk::should_enable_validation_layers)
		{
			auto const layer_props = vk::enumerateInstanceLayerProperties();
			if (stdr::find_if(layer_props, is_khr_validation_layer) != stdr::end(layer_props))
			{
				enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
			}
		}

		return enabled_layers;
	}

	[[nodiscard]] auto get_desired_extensions() -> std::vector<char const*>
	{
		auto enabled_exts = std::vector<char const*>{};

		if constexpr (render::vk::should_enable_validation_layers)
		{
			auto const ext_props = vk::enumerateInstanceExtensionProperties();
			if (stdr::find_if(ext_props, is_debug_utils_ext) != stdr::end(ext_props))
			{
				enabled_exts.push_back("VK_EXT_debug_utils");
			}
		}

		return enabled_exts;
	}

	auto create_vk_instance(core::application_info const& app_info, spdlog::logger& logger) -> vk::UniqueInstance
	{
		auto enabled_layers = get_desired_validation_layers();
		auto enabled_exts = get_desired_extensions();

		if constexpr (render::vk::should_enable_validation_layers)
		{
			auto const ext_props = vk::enumerateInstanceExtensionProperties();
			if (stdr::find_if(ext_props, is_debug_utils_ext) != stdr::end(ext_props))
			{
				enabled_exts.push_back("VK_EXT_debug_utils");
			}
		}

		auto const vk_info =
			vk::ApplicationInfo{}
				.setApiVersion(VK_API_VERSION_1_3)
				.setPApplicationName(app_info.name.data())
				.setApplicationVersion(render::vk::to_vulkan_version(app_info.version));
		auto const create_info = vk::InstanceCreateInfo{}
									 .setPApplicationInfo(&vk_info)
									 .setPEnabledLayerNames(enabled_layers)
									 .setPEnabledExtensionNames(enabled_exts);
		auto instance = vk::createInstanceUnique(create_info);

		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

		logger.debug("Vulkan instance has been created.");

		return instance;
	}

	auto create_vk_debug_utils(vk::Instance instance, spdlog::logger& logger)
		-> vk::UniqueDebugUtilsMessengerEXT
	{
		auto const create_info =
			vk::DebugUtilsMessengerCreateInfoEXT()
				.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
									| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
									| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
				.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
								| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
								| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
				.setPfnUserCallback(debug_callback)
				.setPUserData(static_cast<void*>(&logger));

		if constexpr (render::vk::should_enable_validation_layers)
		{
			auto const ext_props = vk::enumerateInstanceExtensionProperties();
			if (stdr::find_if(ext_props, is_debug_utils_ext) != stdr::end(ext_props))
			{
				logger.debug("Vulkan debug utilities have been created.");

				return instance.createDebugUtilsMessengerEXTUnique(create_info, nullptr,
																   VULKAN_HPP_DEFAULT_DISPATCHER);
			}
		}

		return {};
	}
} // namespace

namespace render::vk
{
	auto instance::make(core::application_info const& app_info, spdlog::logger& logger)
		-> tl::expected<instance, std::error_code>
	{
		auto loader = load_vulkan_symbols(logger);
		auto instance = create_vk_instance(app_info, logger);
		auto debug_utils = create_vk_debug_utils(instance.get(), logger);


		return render::vk::instance{std::move(loader), std::move(instance), std::move(debug_utils)};
	}

	instance::instance(::vk::DynamicLoader&& loader, ::vk::UniqueInstance instance,
					   ::vk::UniqueDebugUtilsMessengerEXT debug_utils) :
		m_loader(std::move(loader)),
		m_instance(std::move(instance)), m_debug_utils(std::move(debug_utils))
	{}
} // namespace render::vk
