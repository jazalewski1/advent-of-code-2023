#include "Task.hpp"
#include "utility/Dbg.hpp"
#include "utility/Stream.hpp"
#include <numeric>
#include <vector>
#include <string>
#include <regex>

namespace
{
using Length = unsigned;
using Lengths = std::vector<Length>;

struct Record
{
    using Pattern = std::string;
    Pattern pattern;
    Lengths sections;
};
using Records = std::vector<Record>;

auto parse_sections(const std::string& string)
{
    const std::regex number_regex{R"(\d+)"};
    const std::sregex_iterator numbers_begin{string.cbegin(), string.cend(), number_regex};
    const std::sregex_iterator numbers_end{};
    Lengths sections;
    for (auto numbers_itr = numbers_begin; numbers_itr != numbers_end; ++numbers_itr)
    {
        sections.push_back(std::stoul(numbers_itr->str()));
    }
    return sections;
}

auto parse_records(utility::Stream& stream)
{
    const std::regex regex{R"((.*?)( )(.*))"};
    Records records;
    for (const auto& line : stream)
    {
        std::smatch match;
        std::regex_search(line, match, regex);
        auto pattern = match.str(1);
        auto sections = parse_sections(match.str(3));
        records.push_back(Record{std::move(pattern), std::move(sections)});
    }
    return records;
}

using Offset = unsigned;
struct Range
{
    Offset begin, end;
};

struct Group
{
    Length length;
    Range offset_range;
};
using Groups = std::vector<Group>;

auto build_groups(const Record& record)
{
    const auto& [pattern, sections] = record;
    const auto length_of_gaps = static_cast<Length>(sections.size()) - 1u;
    const auto sum_of_sections = std::accumulate(sections.begin(), sections.end(), Length{0});
    const auto total_length = length_of_gaps + sum_of_sections;
    const auto base_offset = static_cast<Length>(pattern.length()) - total_length;
    Groups groups;
    Offset offset{0u};
    for (auto length : sections)
    {
        const auto max_offset = offset + base_offset;
        Group group{length, Range{offset, max_offset}};
        groups.push_back(std::move(group));
        offset += length + 1;
    }
    return groups;
}

auto contains(const Record::Pattern& pattern, const Range& range, char symbol)
{
    const auto row_begin = pattern.begin() + range.begin;
    const auto row_end = pattern.begin() + range.end;
    const auto is_symbol = [symbol](const auto c) { return c == symbol; };
    return std::any_of(row_begin, row_end, is_symbol);
}

auto contains_dot(const Record::Pattern& pattern, const Range& range)
{
    return contains(pattern, range, '.');
}

auto contains_hash(const Record::Pattern& pattern, const Range& range)
{
    return contains(pattern, range, '#');
}

using Count = unsigned long long;

class CombinationCounter
{
public:
    CombinationCounter(const Record& record) :
        record{record}, groups{build_groups(this->record)}, count_cache(this->groups.size(), CountMap{})
    {
    }

    Count count_combinations()
    {
        if (groups.empty())
        {
            return 0;
        }
        const auto& first_group = groups.front();
        Count current_count{0};
        for (Offset offset{0}; offset <= first_group.offset_range.end; ++offset)
        {
            const Range gap{Offset{0}, offset};
            if (contains_hash(record.pattern, gap))
            {
                continue;
            }
            const Range range{offset, offset + first_group.length};
            if (contains_dot(record.pattern, range))
            {
                continue;
            }
            const auto next_count = count_combinations(Index{1}, range.end);
            current_count += next_count;
        }
        return current_count;
    }

private:
    const Record& record;
    const Groups groups;
    using CountMap = std::map<Offset, Count>;
    using CountCache = std::vector<CountMap>;
    CountCache count_cache;
    using Index = std::size_t;

    std::optional<Count> get_cached_count(const Index index, const Offset offset)
    {
        const auto& map = count_cache.at(index);
        if (const auto map_itr = map.find(offset); map_itr != map.end())
        {
            return map_itr->second;
        }
        return std::nullopt;
    }

    void add_count_to_cache(const Index index, const Offset offset, Count count)
    {
        auto& map = count_cache.at(index);
        map[offset] = count;
    }

    Count count_combinations(const Index index, const Offset previous_end)
    {
        if (index == groups.size())
        {
            const Range last_gap{previous_end, static_cast<Offset>(record.pattern.size())};
            if (contains_hash(record.pattern, last_gap))
            {
                return 0;
            }
            return 1;
        }

        const auto& group = groups.at(index);
        Count current_count{0};
        for (auto offset = previous_end + 1; offset <= group.offset_range.end; ++offset)
        {
            if (const auto cached_count = get_cached_count(index, offset); cached_count.has_value())
            {
                current_count += cached_count.value();
                continue;
            }
            const Range gap{previous_end, offset};
            if (contains_hash(record.pattern, gap))
            {
                continue;
            }
            const Range range{offset, offset + group.length};
            if (contains_dot(record.pattern, range))
            {
                continue;
            }

            const auto next_count = count_combinations(index + 1, range.end);
            add_count_to_cache(index, offset, next_count);
            current_count += next_count;
        }
        return current_count;
    }
};
} // namespace

namespace task
{
Answer solve_part1(utility::Stream& stream)
{
    const auto records = parse_records(stream);
    Count sum{0};
    for (const auto& record : records)
    {
        CombinationCounter counter{record};
        sum += counter.count_combinations();
    }
    return sum;
}
} // namespace task

namespace
{
auto parse_quintuple_records(utility::Stream& stream)
{
    constexpr auto num_of_copies = 5u;
    constexpr auto num_of_gaps = 4u;
    auto records = parse_records(stream);
    for (auto& record : records)
    {
        auto pattern = record.pattern;
        pattern.reserve((pattern.length() * num_of_copies) + num_of_gaps);
        auto sections = record.sections;
        sections.reserve(sections.size() * num_of_copies);
        for (auto i = 1u; i < num_of_copies; ++i)
        {
            pattern += '?';
            pattern.append(record.pattern);
            sections.insert(sections.end(), record.sections.begin(), record.sections.end());
        }
        record.pattern = pattern;
        record.sections = sections;
    }
    return records;
}
} // namespace

namespace task
{
Answer solve_part2(utility::Stream& stream)
{
    const auto records = parse_quintuple_records(stream);
    Count sum{0};
    for (const auto& record : records)
    {
        CombinationCounter counter{record};
        sum += counter.count_combinations();
    }
    return sum;
}
} // namespace task

