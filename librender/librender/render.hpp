#pragma once

#include <iosfwd>
#include <string>

#include <librender/export.hpp>
#include <librender/system.hpp>

namespace render
{
  // Print a greeting for the specified name into the specified
  // stream. Throw std::invalid_argument if the name is empty.
  //
  LIBRENDER_SYMEXPORT void
  say_hello (std::ostream&, const std::string& name);
}
