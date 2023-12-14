#include "utility/Dbg.hpp"
#include <ostream>

std::string get_filename(std::string s)
{
    const auto last_slash_pos = s.find_last_of('/');
    if (last_slash_pos != std::string::npos)
    {
        s.erase(0, last_slash_pos + 1);
    }
    return s;
}

template <>
void print(std::ostream& stream, const char& c)
{
    stream << '\'' << c << '\'';
}

std::ostream& operator<<(std::ostream& stream, const Stringified& in)
{
    return stream << in.string;
}
