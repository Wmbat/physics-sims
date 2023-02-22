#include <libphyseng/util/semantic_version.hpp>
#include <libphyseng/version.hpp>

#include <string_view>

namespace physeng
{
	inline constexpr auto get_engine_version() -> semantic_version
	{
		return {.major = LIBPHYSENG_VERSION_MAJOR,
				.minor = LIBPHYSENG_VERSION_MINOR,
				.patch = LIBPHYSENG_VERSION_PATCH};
	}

	inline constexpr auto get_engine_name() -> std::string_view
	{
		return std::string_view{"physeng"};
	}
} // namespace physeng
