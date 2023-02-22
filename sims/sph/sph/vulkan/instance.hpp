#pragma once

#include <sph/vulkan/details/vulkan.hpp>

#include <spdlog/logger.h>

#include <tl/expected.hpp>

namespace vulkan
{
	enum struct instance_error
	{

	};

	class instance
	{
	public:
		static auto make(std::string_view app_name, spdlog::logger& logger)
			-> tl::expected<instance, instance_error>;

		/**
		 * Allow implicit conversion to a vulkan vk::Instance
		 */
		operator vk::Instance() const;

	private:
		instance(vk::DynamicLoader&& loader, vk::UniqueInstance&& instance,
				 vk::UniqueDebugUtilsMessengerEXT&& debug);

	private:
		vk::DynamicLoader m_loader = {};
		vk::UniqueInstance m_instance = {};
		vk::UniqueDebugUtilsMessengerEXT m_debug_utils = {};
	};
} // namespace vulkan
