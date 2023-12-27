#include "Task.hpp"
#include "utility/Dbg.hpp"
#include "utility/Stream.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <ranges>

namespace
{
using Row = std::string;
using Map = std::vector<Row>;

struct Vector
{
    int x{}, y{};
};
struct VectorHash
{
    std::size_t operator()(const Vector& v) const
    {
        const auto hash_x = std::hash<int>{}(v.x);
        const auto hash_y = std::hash<int>{}(v.y);
        return hash_x ^ (hash_y << 1);
    }
};
bool operator==(const Vector& lhs, const Vector& rhs)
{
    return lhs.x == rhs.x and lhs.y == rhs.y;
}

auto get_size(const Map& map)
{
    if (map.empty())
    {
        return Vector{0, 0};
    }
    const auto& row = map.front();
    return Vector{static_cast<int>(row.length()), static_cast<int>(map.size())};
}

bool is_within_bounds(const Map& map, const Vector& position)
{
    const auto map_size = get_size(map);
    const auto within_x = position.x >= 0 and position.x < map_size.x; 
    const auto within_y = position.y >= 0 and position.y < map_size.y; 
    return within_x and within_y;
}

auto get_symbol(const Map& map, const Vector& position)
{
    return map[position.y][position.x];
}

auto parse_map(utility::Stream& stream)
{
    Map map;
    for (const auto& line : stream)
    {
        map.push_back(line);
    }
    return map;
}

using Symbol = char;
using Symbols = std::vector<Symbol>;
struct Tile
{
    Symbol symbol;
    Vector position;
};

enum class Direction
{
    up, down, left, right
};

auto get_adjacent(Vector position, const Direction direction)
{
    switch (direction)
    {
        case Direction::up: --position.y; break;
        case Direction::down: ++position.y; break;
        case Direction::left: --position.x; break;
        case Direction::right: ++position.x; break;
    }
    return position;
}

auto find_start_position(const Map& map)
{
    for (auto y = 0u; y < map.size(); ++y)
    {
        const auto& row = map[y];
        if (const auto x = row.find('S'); x != std::string::npos)
        {
            return Vector{static_cast<int>(x), static_cast<int>(y)};
        }
    }
    throw std::logic_error{"Start not found"};
}

auto find_start_symbol(const Map& map, const Vector& position)
{
    auto connected_to_top = false;
    auto connected_to_bottom = false;
    auto connected_to_right = false;
    if (const auto above = get_adjacent(position, Direction::up); is_within_bounds(map, above))
    {
        const auto symbol = get_symbol(map, above);
        connected_to_top = (symbol == '|' or symbol == '7' or symbol == 'F');
    }
    if (const auto below = get_adjacent(position, Direction::down); is_within_bounds(map, below))
    {
        const auto symbol = get_symbol(map, below);
        connected_to_bottom = (symbol == '|' or symbol == 'J' or symbol == 'L');
    }
    if (const auto right = get_adjacent(position, Direction::right); is_within_bounds(map, right))
    {
        const auto symbol = get_symbol(map, right);
        connected_to_right = (symbol == '-' or symbol == '7' or symbol == 'J');
    }
    if (connected_to_top)
    {
        if (connected_to_bottom)
        {
            return '|';
        }
        return connected_to_right ? 'L' : 'J';
    }
    if (connected_to_bottom)
    {
        return connected_to_right ? 'F' : '7';
    }
    return '-';
}

auto find_start_tile(const Map& map)
{
    auto position = find_start_position(map);
    auto symbol = find_start_symbol(map, position);
    return Tile{symbol, position};
}

auto pick_first_direction(const Symbol& symbol)
{
    switch (symbol)
    {
        case '|': return Direction::up;
        case '-': return Direction::right;
        case 'F': return Direction::right;
        case '7': return Direction::down;
        case 'J': return Direction::up;
        case 'L': return Direction::left;
    }
    throw std::logic_error{"Unknown symbol - cannot pick direction"};
}

auto pick_next_direction(const Symbol& symbol, const Direction previous_direction)
{
    if (symbol == 'F')
    {
        return previous_direction == Direction::up ? Direction::right : Direction::down;
    }
    if (symbol == '7')
    {
        return previous_direction == Direction::up ? Direction::left : Direction::down;
    }
    if (symbol == 'L')
    {
        return previous_direction == Direction::down ? Direction::right : Direction::up;
    }
    if (symbol == 'J')
    {
        return previous_direction == Direction::down ? Direction::left : Direction::up;
    }
    return previous_direction;
}

using TileMap = std::unordered_map<Vector, Symbol, VectorHash>;

auto create_loop(const Map& map)
{
    TileMap tile_map;
    const auto start = find_start_tile(map);
    tile_map.emplace(start.position, start.symbol);
    auto direction = pick_first_direction(start.symbol);
    auto position = get_adjacent(start.position, direction);
    auto symbol = get_symbol(map, position);
    tile_map.emplace(position, symbol);
    while (symbol != 'S')
    {
        direction = pick_next_direction(symbol, direction);
        position = get_adjacent(position, direction);
        symbol = get_symbol(map, position);
        tile_map.emplace(position, symbol);
    }
    return tile_map;
}
} // namespace

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    const auto map = parse_map(stream);
    const auto loop = create_loop(map);
    return loop.size() / 2;
}
} // namespace task

namespace
{
bool is_corner(const Symbol symbol)
{
    return symbol == 'F' or symbol == 'L' or symbol == '7' or symbol == 'J';
}
bool are_mutually_exclusive(const Symbol s1, const Symbol s2)
{
    switch (s1)
    {
        case 'F': return s2 == '7';
        case 'L': return s2 == 'J';
        case '7': return s2 == 'F';
        case 'J': return s2 == 'L';
    }
    return false;
}
bool is_inside(const Map& map, const TileMap& loop, const Vector& position)
{
    const auto row_size = get_size(map).x;
    auto num_of_crossings = 0u;
    std::optional<Symbol> prev_corner;
    for (auto x = position.x + 1; x < row_size; ++x)
    {
        const Vector next_position{x, position.y};
        if (not loop.contains(next_position))
        {
            continue;
        }
        const auto next_symbol = loop.at(next_position);
        if (next_symbol == '|')
        {
            ++num_of_crossings;
            continue;
        }
        if (is_corner(next_symbol))
        {
            if (prev_corner.has_value())
            {
                if (not are_mutually_exclusive(next_symbol, *prev_corner))
                {
                    ++num_of_crossings;
                }
                prev_corner.reset();
            }
            else
            {
                prev_corner = next_symbol;
            }
        }
    }
    return (num_of_crossings % 2) == 1;
}
} // namespace

namespace task
{
Answer solve_part2(utility::Stream& stream)
{
    const auto map = parse_map(stream);
    const auto loop = create_loop(map);
    const auto map_size = get_size(map);
    auto num_of_tiles_inside = 0u;
    for (auto y = 0; y < map_size.y; ++y)
    {
        for (auto x = 0; x < map_size.x; ++x)
        {
            const Vector position{x, y};
            if (loop.contains(position))
            {
                continue;
            }
            if (is_inside(map, loop, position))
            {
                ++num_of_tiles_inside;
            }
        }
    }
    return num_of_tiles_inside;
}
} // namespace task

