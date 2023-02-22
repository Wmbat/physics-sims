#include <libphyseng/main.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/tail.hpp>

#include <ranges>

auto main(int argc, char *argv[]) -> int
{
	namespace stdr = std::ranges;

	auto const args = ranges::span{argv, argc} | ranges::to<std::vector<std::string_view>>;

	// If we have no arguments, there is something wrong
	if (stdr::empty(args))
	{
		return -1;
	}

	physeng_main(args);

	return 0;
}
