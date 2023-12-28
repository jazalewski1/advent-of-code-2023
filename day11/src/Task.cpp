#include "Task.hpp"
#include "utility/Dbg.hpp"
#include "utility/Stream.hpp"
#include <vector>
#include <string>

namespace
{
struct Vector
{
    using Coord = int;
    Coord x{}, y{};
};

using SymbolMap = std::vector<std::string>;

auto get_size(const SymbolMap& map)
{
    if (map.empty())
    {
        return Vector{0, 0};
    }
    const auto& row = map.front();
    return Vector{static_cast<int>(row.length()), static_cast<int>(map.size())};
}

auto parse_symbol_map(utility::Stream& stream)
{
    SymbolMap map;
    for (auto line : stream)
    {
        map.push_back(std::move(line));
    }
    return map;
}

using Positions = std::vector<Vector>;

auto collect_galaxy_positions(const SymbolMap& map)
{
    Positions galaxy_positions;
    for (auto y = 0; const auto& line : map)
    {
        for (auto x = 0; const auto c : line)
        {
            if (c == '#')
            {
                galaxy_positions.push_back(Vector{x, y});
            }
            ++x;
        }
        ++y;
    }
    return galaxy_positions;
}

using Coords = std::vector<Vector::Coord>;

auto get_coords_of_empty_rows(const SymbolMap& map)
{
    Coords coords;
    for (std::size_t y{}; const auto& row : map)
    {
        if (row.find_first_not_of('.') == std::string::npos)
        {
            coords.push_back(y);
        }
        ++y;
    }
    return coords;
}

auto get_coords_of_empty_columns(const SymbolMap& map)
{
    Coords coords;
    const auto map_size = get_size(map);
    for (auto x = 0; x < map_size.x; ++x)
    {
        auto is_empty = true;
        for (auto y = 0; y < map_size.y; ++y)
        {
            if (map[y][x] != '.')
            {
                is_empty = false;
                break;
            }
        }
        if (is_empty)
        {
            coords.push_back(x);
        }
    }
    return coords;
}

auto calculate_real_distance(const Vector& v1, const Vector& v2)
{
    return std::abs(v2.x - v1.x) + std::abs(v2.y - v1.y);
}

auto count_steps_between(const Coords& coords, int first, int last)
{
    const auto is_between = [first, last](const auto value) {
        const auto& lower = std::min(first, last);
        const auto& greater = std::max(first, last);
        return value > lower and value < greater;
    };
    return std::ranges::count_if(coords, is_between);
}

using ExpansionMultiplier = unsigned;
auto count_all_steps(
    const Positions& galaxy_positions,
    const Coords& coords_of_empty_rows,
    const Coords& coords_of_empty_columns,
    ExpansionMultiplier multiplier)
{
    auto sum = 0ull;
    for (auto first_itr = galaxy_positions.cbegin(); first_itr != galaxy_positions.cend(); ++first_itr)
    {
        const auto& first_pos = *first_itr;
        for (auto second_itr = std::next(first_itr); second_itr != galaxy_positions.cend(); ++second_itr)
        {
            const auto& second_pos = *second_itr;
            const auto real_distance = calculate_real_distance(first_pos, second_pos);
            const auto num_of_empty_rows = count_steps_between(coords_of_empty_rows, first_pos.y, second_pos.y);
            const auto num_of_empty_columns = count_steps_between(coords_of_empty_columns, first_pos.x, second_pos.x);
            const auto num_of_steps_without_empty_spaces = real_distance - num_of_empty_rows - num_of_empty_columns;
            const auto num_of_steps_through_empty_spaces = (num_of_empty_rows + num_of_empty_columns) * multiplier;
            const auto full_distance = num_of_steps_without_empty_spaces + num_of_steps_through_empty_spaces;
            sum += full_distance;
        }
    }
    return sum;
}
} // namespace

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    const auto symbol_map = parse_symbol_map(stream);
    const auto galaxy_positions = collect_galaxy_positions(symbol_map);
    const auto coords_of_empty_rows = get_coords_of_empty_rows(symbol_map);
    const auto coords_of_empty_columns = get_coords_of_empty_columns(symbol_map);
    constexpr ExpansionMultiplier multiplier{2};
    return count_all_steps(galaxy_positions, coords_of_empty_rows, coords_of_empty_columns, multiplier);
}

Answer solve_part2(utility::Stream& stream)
{
    const auto symbol_map = parse_symbol_map(stream);
    const auto galaxy_positions = collect_galaxy_positions(symbol_map);
    const auto coords_of_empty_rows = get_coords_of_empty_rows(symbol_map);
    const auto coords_of_empty_columns = get_coords_of_empty_columns(symbol_map);
    constexpr ExpansionMultiplier multiplier{1'000'000};
    return count_all_steps(galaxy_positions, coords_of_empty_rows, coords_of_empty_columns, multiplier);
}
} // namespace task

