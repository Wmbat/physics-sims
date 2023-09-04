#pragma once

#include <iosfwd>
#include <string>

#include <libphysics/export.hpp>

namespace physics
{
  // Print a greeting for the specified name into the specified
  // stream. Throw std::invalid_argument if the name is empty.
  //
  LIBPHYSICS_SYMEXPORT void
  say_hello (std::ostream&, const std::string& name);
}
