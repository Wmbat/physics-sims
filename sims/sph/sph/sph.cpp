#include <libphyseng/main.hpp>
#include <libphyseng/util/semantic_version.hpp>

void physeng_main(std::span<const std::string_view>)
{
	[[maybe_unused]] auto test = physeng::semantic_version{};
}
