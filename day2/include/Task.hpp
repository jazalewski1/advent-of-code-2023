#pragma once

#include "utility/Stream.hpp"
#include "utility/Dbg.hpp"
#include <algorithm>
#include <ranges>
#include <regex>

namespace
{
auto parse_game_id(const std::string& string)
{
    const std::regex regex{R"((Game )(\d+)(: )(.*))"};
    std::smatch result;
    std::regex_search(string, result, regex);
    return std::stoul(result.str(2));
}

auto find_max_quantity(const std::string& string, std::string_view color)
{
    const std::regex regex{std::string{R"((\d+) )"}.append(color)};
    std::sregex_iterator results_begin{string.begin(), string.end(), regex};
    std::sregex_iterator results_end{};
    auto max = 0ul;
    std::for_each(results_begin, results_end, [&max](const auto& result) {
        max = std::max(max, std::stoul(result.str(1)));
    });
    return max;
}

struct Cubes
{
    unsigned long red, green, blue;
};
auto find_max_quantities(const std::string& string)
{
    Cubes cubes;
    cubes.red = find_max_quantity(string, "red");
    cubes.green = find_max_quantity(string, "green");
    cubes.blue = find_max_quantity(string, "blue");
    return cubes;
}
} // namespace 

namespace task
{
auto solve_part1(utility::Stream& stream)
{
    constexpr auto max_red = 12ul;
    constexpr auto max_green = 13ul;
    constexpr auto max_blue = 14ul;
    auto sum = 0ul;
    for (const auto& line : stream)
    {
        auto game_id = parse_game_id(line);
        const auto largest = find_max_quantities(line);
        if (largest.red <= max_red and largest.green <= max_green and largest.blue <= max_blue)
        {
            sum += game_id;
        }
    }
    return sum;
}

auto solve_part2(utility::Stream& stream)
{
    auto sum = 0ul;
    for (const auto& line : stream)
    {
        const auto largest = find_max_quantities(line);
        const auto power = largest.red * largest.green * largest.blue;
        sum += power;
    }
    return sum;
}
} // namespace task
