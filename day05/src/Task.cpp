#include "Task.hpp"
#include "utility/Stream.hpp"
#include <limits>
#include <map>
#include <regex>
#include <set>
#include <vector>

namespace
{
using Value = long long;

struct Range
{
    Value first, last;
};
bool operator<(const Range& lhs, const Range& rhs)
{
    return lhs.first < rhs.first;
}
using Ranges = std::set<Range>;

auto parse_seeds(const std::string& string)
{
    const std::regex regex{R"(seeds: (.*))"};
    std::smatch result;
    std::regex_search(string, result, regex);
    return result.str();
}

auto parse_seed_values(const std::string& string)
{
    const auto seeds_str = parse_seeds(string);

    const std::regex value_regex{R"(\d+)"};
    const std::sregex_iterator values_begin{seeds_str.begin(), seeds_str.end(), value_regex};
    const std::sregex_iterator values_end{};
    Ranges seeds;
    for (auto value_itr = values_begin; value_itr != values_end; ++value_itr)
    {
        const auto value = std::stoll(value_itr->str());
        const auto length = Value{1};
        seeds.insert(Range{value, length});
    }
    return seeds;
}

auto parse_seed_ranges(const std::string& string)
{
    const auto seeds_str = parse_seeds(string);

    const std::regex pair_regex{R"((\d+) (\d+))"};
    const std::sregex_iterator pairs_begin{seeds_str.begin(), seeds_str.end(), pair_regex};
    const std::sregex_iterator pairs_end{};
    Ranges ranges;
    for (auto pair_itr = pairs_begin; pair_itr != pairs_end; ++pair_itr)
    {
        const auto range_first = std::stoll(pair_itr->str(1));
        const auto range_length = std::stoll(pair_itr->str(2));
        const auto range_last = range_first + (range_length - 1);
        ranges.insert(Range{range_first, range_last});
    }
    return ranges;
}

using Start = Value;
using Shift = Value;
using Map = std::map<Start, Shift>;
using Maps = std::vector<Map>;

auto parse_map(utility::Stream& stream)
{
    Map map{{Start{0}, Shift{0}}};
    stream.skip();
    const std::regex mapping_regex{R"((\d+) (\d+) (\d+))"};
    while (stream.is_active())
    {
        const auto line = stream.next_line();
        if (line.empty())
        {
            break;
        }
        std::smatch result;
        std::regex_search(line, result, mapping_regex);

        const auto start = std::stoll(result.str(2));
        const auto destination_start = std::stoll(result.str(1));
        const auto shift = destination_start - start;
        map.insert_or_assign(start, shift);

        const auto length = std::stoll(result.str(3));
        const auto next_start = start + length;
        map.emplace(next_start, Shift{0});
    }

    return map;
}

auto parse_maps(utility::Stream& stream)
{
    Maps maps;
    while (stream.is_active())
    {
        auto map = parse_map(stream);
        maps.push_back(std::move(map));
    }
    return maps;
}

bool overlapping(const Range& r1, const Range& r2)
{
    if (r1.first < r2.first)
    {
        return r1.last >= r2.first;
    }
    if (r1.first > r2.first)
    {
        return r1.first <= r2.last;
    }
    return true;
}

auto get_intersection(const Range& r1, const Range& r2)
{
    auto first = std::max(r1.first, r2.first);
    auto last = std::min(r1.last, r2.last);
    return Range{first, last};
}

Range translate(const Range& range, const Shift& shift)
{
    return Range{range.first + shift, range.last + shift};
}

constexpr auto infinity = std::numeric_limits<Value>::max();

Ranges translate(const Ranges& ranges, const Map& map)
{
    Ranges result;
    for (const auto& range : ranges)
    {
        for (auto mapping_itr = map.cbegin(); mapping_itr != map.cend(); ++mapping_itr)
        {
            const auto [first, shift] = *mapping_itr;
            const auto next_mapping_itr = std::next(mapping_itr);
            const auto last = (next_mapping_itr != map.cend()) ? next_mapping_itr->first - 1 : infinity;
            const Range mapping_range{first, last};
            if (not overlapping(range, mapping_range))
            {
                continue;
            }
            const auto intersection = get_intersection(range, mapping_range);
            const auto new_range = translate(intersection, shift);
            result.insert(new_range);
        }
    }
    return result;
}

auto find_lowest_location(Ranges ranges, const Maps& maps)
{
    for (const auto& map : maps)
    {
        ranges = translate(ranges, map);
    }

    const auto& lowest_range = *ranges.begin();
    return lowest_range.first;
}
} // namespace 

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    const auto seed_ranges = parse_seed_values(stream.next_line());
    stream.skip();
    const auto maps = parse_maps(stream);

    return find_lowest_location(seed_ranges, maps);
}

Answer solve_part2(utility::Stream& stream)
{
    const auto seed_ranges = parse_seed_ranges(stream.next_line());
    stream.skip();
    auto maps = parse_maps(stream);

    return find_lowest_location(seed_ranges, maps);
}
} // namespace task

