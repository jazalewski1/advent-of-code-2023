#include "Task.hpp"
#include "utility/Dbg.hpp"
#include "utility/Stream.hpp"
#include <functional>
#include <string>
#include <optional>
#include <vector>

namespace
{
using DigitExtractor = std::function<std::optional<int> (const std::string_view&)>;

int get_first_digit(const std::string& string, const DigitExtractor& extractor)
{
    for (auto itr = string.begin(); itr != string.end(); ++itr)
    {
        const auto maybe_digit = extractor(std::string_view{itr, string.end()});
        if (maybe_digit.has_value())
        {
            return maybe_digit.value();
        }
    }
    throw std::logic_error{"String does not contain a digit"};
}

int get_last_digit(const std::string& string, const DigitExtractor& extractor)
{
    for (auto length = std::string::size_type{1}; length <= string.length(); ++length)
    {
        const auto maybe_digit = extractor(std::string_view{string.end() - length, string.end()});
        if (maybe_digit.has_value())
        {
            return maybe_digit.value();
        }
    }
    throw std::logic_error{"String does not contain a digit"};
}

auto get_digits(const std::string& string, const DigitExtractor& extractor)
{
    const auto first_digit = get_first_digit(string, extractor);
    const auto last_digit = get_last_digit(string, extractor);
    return std::pair(first_digit, last_digit);
}

auto join_into_number(int decimal, int unit)
{
    return 10 * decimal + unit;
}

auto to_digit(char in)
{
    return static_cast<int>(in - '0');
}

const std::vector<std::pair<std::string, int>> digit_word_map
{
    {"one", 1},
    {"two", 2},
    {"three", 3},
    {"four", 4},
    {"five", 5},
    {"six", 6},
    {"seven", 7},
    {"eight", 8},
    {"nine", 9}
};

auto accumulate(utility::Stream& stream, DigitExtractor&& digit_extractor)
{
    auto sum = 0ul;
    for (const auto& line : stream)
    {
        const auto digits = get_digits(line, std::move(digit_extractor));
        const auto value = join_into_number(digits.first, digits.second);
        sum += value;
    }
    return sum;
}
} // namespace 

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    const auto digit_extractor = [](const std::string_view& string) -> std::optional<int> {
        if (std::isdigit(string.front()))
        {
            return to_digit(string.front());
        }
        return std::nullopt;
    };
    return accumulate(stream, digit_extractor);
}

Answer solve_part2(utility::Stream& stream)
{
    const auto digit_extractor = [](const std::string_view& string) -> std::optional<int> {
        if (std::isdigit(string.front()))
        {
            return to_digit(string.front());
        }
        for (const auto& [word, digit] : digit_word_map)
        {
            if (string.starts_with(word))
            {
                return digit;
            }
        }
        return std::nullopt;
    };
    return accumulate(stream, digit_extractor);
}
} // namespace task

