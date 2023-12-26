#include "Task.hpp"
#include "utility/Dbg.hpp"
#include "utility/Stream.hpp"
#include <regex>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

namespace
{
using Number = long;
using Numbers = std::vector<Number>;
auto parse_numbers(const std::string& string)
{
    const std::regex regex{R"([\d\-]+)"};
    const std::sregex_iterator numbers_begin{string.cbegin(), string.cend(), regex};
    const std::sregex_iterator numbers_end{};
    Numbers numbers;
    const auto get_number = [](const auto& m) { return std::stol(m.str()); };
    std::transform(numbers_begin, numbers_end, std::back_inserter(numbers), get_number);
    return numbers;
}

auto contains_only_zeros(const Numbers& numbers)
{
    return std::ranges::all_of(numbers, [](auto n){ return n == 0; });
}

auto get_differences(const Numbers& numbers)
{
    Numbers result;
    if (numbers.empty())
    {
        return result;
    }
    auto first_itr = numbers.begin();
    auto second_itr = std::next(first_itr);
    while (second_itr != numbers.end())
    {
        result.push_back(*second_itr - *first_itr);
        ++first_itr;
        ++second_itr;
    }
    return result;
}
} // namespace 

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    Number sum{};
    for (const auto& line : stream)
    {
        auto numbers = parse_numbers(line);
        sum += numbers.back();
        while (not contains_only_zeros(numbers))
        {
            numbers = get_differences(numbers);
            sum += numbers.back();
        }
    }
    return sum;
}

Answer solve_part2(utility::Stream& stream)
{
    Number sum{0};
    for (const auto& line : stream)
    {
        auto numbers = parse_numbers(line);
        Numbers front_numbers{numbers.front()};
        while (not contains_only_zeros(numbers))
        {
            numbers = get_differences(numbers);
            front_numbers.push_back(numbers.front());
        }
        Number extrapolated{};
        for (auto itr = front_numbers.rbegin(); itr != front_numbers.rend(); ++itr)
        {
            extrapolated = *itr - extrapolated;
        }
        sum += extrapolated;
    }
    return sum;
}
} // namespace task

