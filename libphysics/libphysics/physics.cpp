#include <libphysics/physics.hpp>

#include <ostream>
#include <stdexcept>

using namespace std;

namespace physics
{
    void say_hello(ostream& o, string const& n)
    {
        if (n.empty())
        {
            throw invalid_argument("empty name");
        }

        o << "Hello, " << n << '!' << endl;
    }
} // namespace physics
