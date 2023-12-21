#include "Task.hpp"
#include "utility/Dbg.hpp"
#include "utility/Stream.hpp"
#include <vector>
#include <regex>

namespace
{
using Time = unsigned long long;
using Distance = unsigned long long;
struct Race
{
    Time time;
    Distance top_distance;
};
using Races = std::vector<Race>;

auto parse_races(utility::Stream& stream)
{
    const std::regex numbers_regex{R"(( *)(\d+))"};
    const auto time_line = stream.next_line();
    std::sregex_iterator time_matches_itr{time_line.begin(), time_line.end(), numbers_regex};
    const auto distance_line = stream.next_line();
    std::sregex_iterator distance_matches_itr{distance_line.begin(), distance_line.end(), numbers_regex};
    const std::sregex_iterator matches_end{};
    
    Races races;
    while (time_matches_itr != matches_end)
    {
        const auto time = std::stoull(time_matches_itr->str(2));
        const auto distance = std::stoull(distance_matches_itr->str(2));
        races.push_back(Race{time, distance});
        ++time_matches_itr;
        ++distance_matches_itr;
    }
    return races;
}

auto count_options(const Race& race)
{
    auto num_of_options = 0u;
    for (Time hold_time{0}; hold_time < race.time; ++hold_time)
    {
        const auto speed = hold_time;
        const auto remaining_time = race.time - hold_time;
        const auto travelled_distance = speed * remaining_time;
        if (travelled_distance > race.top_distance)
        {
            ++num_of_options;
        }
    }
    return num_of_options;
}
} // namespace 

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    const auto races = parse_races(stream);
    auto result = 1;
    for (const auto& race : races)
    {
        const auto num_of_options = count_options(race);
        result *= num_of_options;
    }
    return result;
}

namespace
{
auto parse_single_race(utility::Stream& stream)
{
    const std::regex numbers_regex{R"(( *)(\d+))"};
    const auto time_line = stream.next_line();
    std::sregex_iterator time_matches_itr{time_line.begin(), time_line.end(), numbers_regex};
    const auto distance_line = stream.next_line();
    std::sregex_iterator distance_matches_itr{distance_line.begin(), distance_line.end(), numbers_regex};
    const std::sregex_iterator matches_end{};
    
    std::string time_string;
    std::string distance_string;
    while (time_matches_itr != matches_end)
    {
        time_string += time_matches_itr->str(2);
        distance_string += distance_matches_itr->str(2);
        ++time_matches_itr;
        ++distance_matches_itr;
    }
    const auto time = std::stoull(time_string);
    const auto distance = std::stoull(distance_string);
    return Race{time, distance};
}
} // namespace 

Answer solve_part2(utility::Stream& stream)
{
    const auto race = parse_single_race(stream);
    const auto num_of_options = count_options(race);
    return num_of_options;
}
} // namespace task

