#pragma once

#include <libcore/core.hpp>
#include <libcore/semantic_version.hpp>
#include <libcore/export.hpp>

#include <span>
#include <string_view>

extern void core_main(std::span<std::string_view const> args);
