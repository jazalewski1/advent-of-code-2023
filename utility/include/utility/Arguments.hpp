#pragma once

#include "utility/Dbg.hpp"
#include <fstream>
#include <string>
#include <vector>

namespace utility
{
using Arguments = std::vector<std::string>;
Arguments parse_arguments(int, char**);

std::string get_input_filename(const Arguments&);

std::ifstream open_file(const std::string&);
} // namespace utility
