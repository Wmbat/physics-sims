
#include <libcore/core.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/view/tail.hpp>

auto main(int argc, char *argv[]) -> int
{
	auto const args = ranges::span{argv, argc} | ranges::to<std::vector<std::string_view>>;

	// If we have no arguments, there is something wrong
	if (ranges::empty(args))
	{
		return -1;
	}

	core_main(args);

	return 0;
}
