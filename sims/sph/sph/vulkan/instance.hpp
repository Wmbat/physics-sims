#pragma once

#include <sph/vulkan/details/vulkan.hpp>

#include <tl/expected.hpp>

namespace vulkan
{
	struct instance_create_info
	{
	};

	enum struct instance_error
	{

	};

	class instance
	{
	public:
		static auto make(const instance_create_info& info)
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
