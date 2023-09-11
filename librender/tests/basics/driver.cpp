#include <sstream>
#include <stdexcept>

#include <librender/render.hpp>
#include <librender/version.hpp>

#undef NDEBUG
#include <cassert>

int main()
{
    using namespace std;
    using namespace render;

    // Basics.
    //
    {
        ostringstream o;
        say_hello(o, "World");
        assert(o.str() == "Hello, World!\n");
    }

    // Empty name.
    //
    try
    {
        ostringstream o;
        say_hello(o, "");
        assert(false);
    }
    catch (invalid_argument const& e)
    {
        assert(e.what() == string("empty name"));
    }
}
