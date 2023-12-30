#include "Task.hpp"
#include "utility/Stream.hpp"
#include <numeric>
#include <string>
#include <vector>
#include <optional>

namespace
{
using Map = std::vector<std::string>;
auto parse_map(utility::Stream& stream)
{
    Map map;
    for (auto line : stream)
    {
        map.push_back(std::move(line));
    }
    return map;
}

struct Vector
{
    int x, y;
};
auto get_size(const Map& map)
{
    if (map.empty())
    {
        return Vector{0, 0};
    }
    const auto y = static_cast<int>(map.size());
    const auto x = static_cast<int>(map.front().length());
    return Vector{x, y};
}

struct Number
{
    std::string value;
    int y, x_begin, x_end;
};
using Numbers = std::vector<Number>;

class NumberCreator
{
public:
    void add_digit(char digit, int x, int y)
    {
        if (not number.has_value())
        {
            number = Number{};
            number->y = y;
            number->x_begin = x;
            number->x_end = x;
        }
        number->value += digit;
        number->x_end += 1;
    }

    std::optional<Number> finish()
    {
        std::optional<Number> result;
        number.swap(result);
        return result;
    }

private:
    std::optional<Number> number;
};

auto parse_numbers(const Map& map)
{
    Numbers numbers;
    const auto add_number = [&](const auto& opt_number) {
        if (opt_number.has_value())
        {
            numbers.push_back(*opt_number);
        }
    };
    NumberCreator number_creator{};
    const auto map_size = get_size(map);
    for (auto y = 0; y < map_size.y; ++y)
    {
        for (auto x = 0; x < map_size.x; ++x)
        {
            const auto c = map[y][x];
            if (std::isdigit(c))
            {
                number_creator.add_digit(c, x, y);
            }
            else
            {
                auto current_number = number_creator.finish();
                add_number(current_number);
            }
        }
        auto current_number = number_creator.finish();
        add_number(current_number);
    }
    return numbers;
}

auto is_adjacent(const Vector origin, const Vector query)
{
    const auto diff_x = origin.x - query.x;
    const auto diff_y = origin.y - query.y;
    return std::abs(diff_x) <= 1 and std::abs(diff_y) <= 1;
}

auto is_adjacent(const Number& number, const Vector query)
{
    for (auto x = number.x_begin; x < number.x_end; ++x)
    {
        const Vector position{x, number.y};
        if (is_adjacent(position, query))
        {
            return true;
        }
    }
    return false;
}

using Integers = std::vector<unsigned long>;
Integers find_adjacent_integers(const Numbers& numbers, const Vector position)
{
    Integers result;
    for (const auto& number : numbers)
    {
        if (is_adjacent(number, position))
        {
            result.push_back(std::stoul(number.value));
        }
    }
    return result;
}

auto is_part_symbol(char c)
{
    return not std::isdigit(c) and c != '.';
}
} // namespace

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    const auto map = parse_map(stream);
    const auto numbers = parse_numbers(map);

    auto sum = 0ull;
    const auto map_size = get_size(map);
    for (auto y = 0; y < map_size.y; ++y)
    {
        for (auto x = 0; x < map_size.x; ++x)
        {
            const auto c = map[y][x];
            if (is_part_symbol(c))
            {
                const auto adjacent_numbers = find_adjacent_integers(numbers, Vector{x, y});
                sum += std::accumulate(adjacent_numbers.begin(), adjacent_numbers.end(), 0);
            }
        }
    }

    return sum;
}

Answer solve_part2(utility::Stream& stream)
{
    const auto map = parse_map(stream);
    const auto numbers = parse_numbers(map);

    auto sum = 0ull;
    const auto map_size = get_size(map);
    for (auto y = 0; y < map_size.y; ++y)
    {
        for (auto x = 0; x < map_size.x; ++x)
        {
            const auto c = map[y][x];
            if (c == '*')
            {
                const auto adjacent_numbers = find_adjacent_integers(numbers, Vector{x, y});
                if (adjacent_numbers.size() != 2u)
                {
                    continue;
                }
                sum += adjacent_numbers[0] * adjacent_numbers[1];
            }
        }
    }

    return sum;
}
} // namespace task
