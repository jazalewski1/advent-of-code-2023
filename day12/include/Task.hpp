#pragma once

namespace utility
{
class Stream;
} // namespace utility

namespace task
{
using Answer = unsigned long long;
Answer solve_part1(utility::Stream&);

Answer solve_part2(utility::Stream&);
} // namespace task
