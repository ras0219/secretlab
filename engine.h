#pragma once

#include "environment.h"

struct Commands
{
    size_t size;
    const Command* begin;
};

extern "C" Commands __cdecl get_commands();

using get_commands_t = decltype(&get_commands);
