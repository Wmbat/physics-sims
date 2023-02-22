#include <sph/vulkan/instance.hpp>

#include <sph/core.hpp>
#include <sph/sph-info.hpp>
#include <sph/vulkan/details/vulkan.hpp>

#include <libphyseng/physeng-info.hpp>

#include <tl/expected.hpp>

#include <algorithm>
#include <string_view>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE; // NOLINT

namespace
{
	using namespace std::literals;
	namespace ranges = std::ranges;

	auto load_vulkan_symbols() -> vk::DynamicLoader
	{
		auto loader = vk::DynamicLoader{};

		VULKAN_HPP_DEFAULT_DISPATCHER.init(
			loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

		return loader;
	}

	auto is_validation_layer(vk::LayerProperties const& property) -> bool
	{
		return property.layerName == "VK_LAYER_KHRONOS_validation"sv;
	}

	auto create_vk_instance() -> vk::UniqueInstance
	{
		auto enabled_layers = std::vector<char const*>{};
		auto enabled_exts = std::vector<char const*>{};

		let layer_props = vk::enumerateInstanceLayerProperties();
		if (ranges::find_if(layer_props, is_validation_layer) != ranges::end(layer_props))
		{
			enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
		}

		let app_info =
			vk::ApplicationInfo{}
				.setApiVersion(VK_API_VERSION_1_3)
				.setPApplicationName(get_name().data())
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

	auto create_vk_debug_utils() -> vk::UniqueDebugUtilsMessengerEXT
	{
		return {};
	}
} // namespace

namespace vulkan
{
	auto instance::make(const instance_create_info& /*info*/)
		-> tl::expected<instance, instance_error>
	{
		return instance{load_vulkan_symbols(), create_vk_instance(), create_vk_debug_utils()};
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
