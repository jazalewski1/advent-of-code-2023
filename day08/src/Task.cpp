#include "Task.hpp"
#include "utility/Dbg.hpp"
#include "utility/Stream.hpp"
#include <algorithm>
#include <unordered_map>
#include <ranges>
#include <regex>
#include <numeric>

namespace 
{
using Direction = char;
using Directions = std::string;
constexpr auto left = 'L';

using NodeId = std::string;
struct Node
{
    NodeId left, right;
};
using NodeMap = std::unordered_map<NodeId, Node>;

auto parse_map(utility::Stream& stream)
{
    const std::regex regex{R"(([A-Z]{3})(.*?)([A-Z]{3})(.*?)([A-Z]{3})(.*))"};
    NodeMap map;
    for (const auto& line : stream)
    {
        std::smatch match;
        std::regex_search(line, match, regex);
        auto node_id = match.str(1);
        auto left_id = match.str(3);
        auto right_id = match.str(5);
        map.emplace(std::move(node_id), Node{std::move(left_id), std::move(right_id)});
    }
    return map;
}

using StepCount = unsigned long long;

using StopPredicate = std::function<bool (NodeId)>;
auto count_steps(const NodeMap& map, const Directions& directions, NodeId current_id, StopPredicate predicate)
{
    auto direction_itr = directions.cbegin();
    StepCount step_count{0};
    while (not predicate(current_id))
    {
        const auto direction = *direction_itr;

        const auto& node = map.at(current_id);
        current_id = (direction == left) ? node.left : node.right;

        std::advance(direction_itr, 1);
        if (direction_itr == directions.cend())
        {
            direction_itr = directions.cbegin();
        }
        ++step_count;
    }
    return step_count;
}
} // namespace 

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    const auto directions = stream.next_line();
    stream.skip();
    const auto map = parse_map(stream);

    const NodeId start_id{"AAA"};
    const NodeId end_id{"ZZZ"};
    const auto is_end_node = [&end_id](const NodeId& id) { return id == end_id; };
    const auto step_count = count_steps(map, directions, start_id, is_end_node);
    return step_count;
}

Answer solve_part2(utility::Stream& stream)
{
    const auto directions = stream.next_line();
    stream.skip();
    const auto map = parse_map(stream);

    const auto is_start_node = [](const NodeId& id) { return id[2] == 'A'; };
    const auto is_end_node = [](const NodeId& id) { return id[2] == 'Z'; };
    StepCount lcm = count_steps(map, directions, map.begin()->first, is_end_node);
    for (const auto& current_id : std::views::keys(map) | std::views::drop(1) | std::views::filter(is_start_node))
    {
        const auto step_count = count_steps(map, directions, current_id, is_end_node);
        lcm = std::lcm(lcm, step_count);
    }
    return lcm;
}
} // namespace task

