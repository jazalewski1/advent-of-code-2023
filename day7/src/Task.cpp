#include "Task.hpp"
#include "utility/Dbg.hpp"
#include "utility/Stream.hpp"
#include <algorithm>
#include <unordered_map>
#include <regex>
#include <vector>
#include <ranges>

namespace 
{
using Figure = char;
using FigureCount = unsigned;
struct Game
{
    using Cards = std::string;
    Cards cards;
    using Bid = unsigned;
    Bid bid;
    using FigureCountMap = std::unordered_map<Figure, FigureCount>;
    FigureCountMap figure_count_map;
};
using Games = std::vector<Game>;

auto parse_game(const std::string& string)
{
    const std::regex game_regex{R"((.{5})( )(\d+))"};
    std::smatch match;
    std::regex_search(string, match, game_regex);
    Game game;
    game.cards = match.str(1);
    for (auto c : game.cards)
    {
        const auto [itr, inserted] = game.figure_count_map.emplace(c, FigureCount{1});
        if (not inserted)
        {
            itr->second += 1;
        }
    }
    game.bid = std::stoul(match.str(3));
    return game;
}

auto parse_games(utility::Stream& stream)
{
    Games games;
    for (const auto& line : stream)
    {
        auto game = parse_game(line);
        games.push_back(std::move(game));
    }
    return games;
}

enum class Rank
{
    high, one_pair, two_pair, three, full, four, five,
};

auto evaluate_rank(const Rank& rank, FigureCount count)
{
    if (count == 2)
    {
        switch (rank)
        {
            case Rank::one_pair: return Rank::two_pair;
            case Rank::three: return Rank::full;
            default: break;
        }
        return Rank::one_pair;
    }
    if (count == 3)
    {
        return (rank == Rank::one_pair) ? Rank::full : Rank::three; 
    }
    if (count == 4)
    {
        return Rank::four;
    }
    if (count == 5)
    {
        return Rank::five;
    }
    return Rank::high;
}
} // namespace

namespace
{
namespace part1
{
auto evaluate_rank(const Game::FigureCountMap& figure_count_map)
{
    auto rank = Rank::high;
    for (const auto [_, count] : figure_count_map)
    {
        const auto possible_rank = evaluate_rank(rank, count);
        rank = std::max(rank, possible_rank);
    }
    return rank;
}

using Strength = int;
Strength get_strength(const Figure c)
{
    switch (c)
    {
        case 'A': return 14;
        case 'K': return 13;
        case 'Q': return 12;
        case 'J': return 11;
        case 'T': return 10;
    }
    return static_cast<Strength>(c - '0');
}

const auto compare_games = [](const Game& lhs, const Game& rhs)
{
    const auto lhs_rank = evaluate_rank(lhs.figure_count_map);
    const auto rhs_rank = evaluate_rank(rhs.figure_count_map);
    if (lhs_rank != rhs_rank)
    {
        return lhs_rank < rhs_rank;
    }
    const auto [itr1, itr2] = std::ranges::mismatch(lhs.cards, rhs.cards);
    return get_strength(*itr1) < get_strength(*itr2);
};
} // namespace part1
} // namespace 

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    auto games = parse_games(stream);
    std::ranges::sort(games, part1::compare_games);
    auto sum = 0ul;
    for (auto index = 0u; index < games.size(); ++index)
    {
        const auto score = index + 1;
        sum += games[index].bid * score;
    }
    return sum;
}
} // namespace task

namespace
{
namespace part2
{
auto count_js(const Game::FigureCountMap& map)
{
    const auto j_itr = map.find('J');
    return j_itr != map.end() ? j_itr->second : FigureCount{0};
}

auto evaluate_rank(const Game::FigureCountMap& map)
{
    auto current_rank = Rank::high;
    const auto without_js = [](const auto& pair) { return pair.first != 'J'; };
    for (const auto [_, count] : map | std::views::filter(without_js))
    {
        const auto possible_rank = evaluate_rank(current_rank, count);
        current_rank = std::max(current_rank, possible_rank);
    }
    const auto num_of_js = count_js(map);

    if (num_of_js == 1)
    {
        switch (current_rank)
        {
            case Rank::four: return Rank::five;
            case Rank::three: return Rank::four;
            case Rank::two_pair: return Rank::full;
            case Rank::one_pair: return Rank::three;
            default: break;
        }
        return Rank::one_pair;
    }
    if (num_of_js == 2)
    {
        switch (current_rank)
        {
            case Rank::three: return Rank::five;
            case Rank::one_pair: return Rank::four;
            default: break;
        }
        return Rank::three;
    }
    if (num_of_js == 3)
    {
        return (current_rank == Rank::one_pair) ? Rank::five : Rank::four;
    }
    if (num_of_js >= 4)
    {
        return Rank::five;
    }
    return current_rank;
}

using Strength = int;
Strength get_strength(const Figure c)
{
    switch (c)
    {
        case 'J': return 1;
        case 'A': return 13;
        case 'K': return 12;
        case 'Q': return 11;
        case 'T': return 10;
    }
    return static_cast<Strength>(c - '0');
}

const auto compare_games = [](const Game& lhs, const Game& rhs)
{
    const auto lhs_rank = evaluate_rank(lhs.figure_count_map);
    const auto rhs_rank = evaluate_rank(rhs.figure_count_map);
    if (lhs_rank != rhs_rank)
    {
        return lhs_rank < rhs_rank;
    }
    const auto [itr1, itr2] = std::ranges::mismatch(lhs.cards, rhs.cards);
    return get_strength(*itr1) < get_strength(*itr2);
};
} // namespace part2
} // namespace 

namespace task
{
Answer solve_part2(utility::Stream& stream)
{
    auto games = parse_games(stream);
    std::ranges::sort(games, part2::compare_games);
    auto sum = 0ul;
    for (auto index = 0u; index < games.size(); ++index)
    {
        const auto score = index + 1;
        sum += games[index].bid * score;
    }
    return sum;
}
} // namespace task

