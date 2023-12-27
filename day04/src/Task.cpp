#include "Task.hpp"
#include "utility/Stream.hpp"
#include <algorithm>
#include <iterator>
#include <ranges>
#include <regex>
#include <set>
#include <vector>

namespace
{
using Number = unsigned long;
using Numbers = std::set<Number>;
struct Card
{
    using Id = unsigned long;
    Id id;
    Numbers own_numbers;
    Numbers winning_numbers;
};

auto parse_numbers(const std::string& string)
{
    Numbers numbers;
    const std::regex number_regex{R"(\b\d+?\b)"};
    const std::sregex_iterator numbers_begin{string.begin(), string.end(), number_regex};
    const std::sregex_iterator numbers_end{};
    for (auto number_itr = numbers_begin; number_itr != numbers_end; ++number_itr)
    {
        numbers.insert(std::stoul(number_itr->str()));
    }
    return numbers;
}

auto parse_card(const std::string& string)
{
    const std::regex regex{R"((Card +)(\d+)(: )(.*?)( \| )(.*))"};
    std::smatch result;
    std::regex_search(string, result, regex);
    Card card;
    card.id = std::stoul(result.str(2));
    card.own_numbers = parse_numbers(result.str(4));
    card.winning_numbers = parse_numbers(result.str(6));
    return card;
}

auto count_matching_numbers(const Card& card)
{
    std::vector<Number> matching_numbers;
    std::set_intersection(
        card.winning_numbers.begin(),
        card.winning_numbers.end(),
        card.own_numbers.begin(),
        card.own_numbers.end(),
        std::back_inserter(matching_numbers));
    return matching_numbers.size();
}
} // namespace 

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    auto sum = 0ull;
    for (const auto& line : stream)
    {
        const auto card = parse_card(line);
        const auto count_of_matching_numbers = count_matching_numbers(card);
        if (count_of_matching_numbers > 0)
        {
            const auto double_count = count_of_matching_numbers - 1;
            sum += 1 << double_count;
        }
    }
    return sum;
}

namespace 
{
using CopyCount = unsigned;
using CountMap = std::map<Card::Id, CopyCount>;
} // namespace 

Answer solve_part2(utility::Stream& stream)
{
    auto sum = 0ull;
    CountMap count_map;
    for (const auto& line : stream)
    {
        const auto card = parse_card(line);
        auto& current_count = count_map[card.id];
        ++current_count;

        const auto count_of_matching_numbers = count_matching_numbers(card);
        const auto following_begin = card.id + 1;
        const auto following_end = following_begin + count_of_matching_numbers;
        for (const auto& next_id : std::views::iota(following_begin, following_end))
        {
            auto& next_count = count_map[next_id];
            next_count += current_count;
        }
        sum += current_count;
    }
    return sum;
}
} // namespace task

