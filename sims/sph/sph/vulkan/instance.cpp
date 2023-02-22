#include <sph/vulkan/instance.hpp>

#include <sph/core.hpp>
#include <sph/vulkan/details/vulkan.hpp>

#include <libphyseng/physeng-info.hpp>

#include <tl/expected.hpp>

#include <spdlog/logger.h>

#include <algorithm>
#include <string_view>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE; // NOLINT

namespace
{
	using namespace std::literals;
	namespace stdr = std::ranges;

	auto get_validation_message_type(VkDebugUtilsMessageTypeFlagsEXT messageType) -> std::string
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

		return {};
	}

	static VKAPI_ATTR auto VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data) -> VkBool32
	{
		assert(p_user_data != nullptr); // NOLINT

		auto& logger = *static_cast<spdlog::logger*>(p_user_data);

		let type = get_validation_message_type(messageType);

		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			if (!type.empty())
			{
				logger.error("{0} - {1}", type, p_callback_data->pMessage);
			}
			else
			{
				logger.error("{0}", p_callback_data->pMessage);
			}
		}
		else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			if (!type.empty())
			{
				logger.warn("{0} - {1}", type, p_callback_data->pMessage);
			}
			else
			{
				logger.warn("{0}", p_callback_data->pMessage);
			}
		}

		return VK_FALSE;
	}

	auto load_vulkan_symbols() -> vk::DynamicLoader
	{
		auto loader = vk::DynamicLoader{};

		VULKAN_HPP_DEFAULT_DISPATCHER.init(
			loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

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

	auto create_vk_instance(std::string_view app_name) -> vk::UniqueInstance
	{
		auto enabled_layers = std::vector<char const*>{};
		auto enabled_exts = std::vector<char const*>{};

		if constexpr (vulkan::should_enable_validation_layers)
		{
			let layer_props = vk::enumerateInstanceLayerProperties();
			if (stdr::find_if(layer_props, is_khr_validation_layer) != stdr::end(layer_props))
			{
				enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
			}

			let ext_props = vk::enumerateInstanceExtensionProperties();
			if (stdr::find_if(ext_props, is_debug_utils_ext) != stdr::end(ext_props))
			{
				enabled_exts.push_back("VK_EXT_debug_utils");
			}
		}

		let app_info =
			vk::ApplicationInfo{}
				.setApiVersion(VK_API_VERSION_1_3)
				.setPApplicationName(app_name.data())
				.setApplicationVersion(vulkan::to_vulkan_version(get_version()))
				.setPEngineName(physeng::get_engine_name().data())
				.setEngineVersion(vulkan::to_vulkan_version(physeng::get_engine_version()));
		let create_info = vk::InstanceCreateInfo{}
							  .setPApplicationInfo(&app_info)
							  .setPEnabledLayerNames(enabled_layers)
							  .setPEnabledExtensionNames(enabled_exts);
		auto instance = vk::createInstanceUnique(create_info);

		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());

		return instance;
	}

	auto create_vk_debug_utils(vk::Instance instance, spdlog::logger& logger)
		-> vk::UniqueDebugUtilsMessengerEXT
	{
		let create_info =
			vk::DebugUtilsMessengerCreateInfoEXT()
				.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
									| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
									| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
				.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
								| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
								| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
				.setPfnUserCallback(debug_callback)
				.setPUserData(static_cast<void*>(&logger));

		if constexpr (vulkan::should_enable_validation_layers)
		{
			let ext_props = vk::enumerateInstanceExtensionProperties();
			if (stdr::find_if(ext_props, is_debug_utils_ext) != stdr::end(ext_props))
			{
				return instance.createDebugUtilsMessengerEXTUnique(create_info, nullptr,
																   VULKAN_HPP_DEFAULT_DISPATCHER);
			}
		}

		return {};
	}
} // namespace

namespace vulkan
{
	auto instance::make(std::string_view app_name, spdlog::logger& logger) -> tl::expected<instance, instance_error>
	{
		auto loader = load_vulkan_symbols();
		auto instance = create_vk_instance(app_name);
		auto debug_utils = create_vk_debug_utils(instance.get(), logger);

		return vulkan::instance{std::move(loader), std::move(instance), std::move(debug_utils)};
	}

	instance::instance(vk::DynamicLoader&& loader, vk::UniqueInstance&& instance,
					   vk::UniqueDebugUtilsMessengerEXT&& debug) :
		m_loader{std::move(loader)},
		m_instance{std::move(instance)}, m_debug_utils(std::move(debug))
	{}

	instance::operator vk::Instance() const
	{
		return m_instance.get();
	}
} // namespace vulkan
