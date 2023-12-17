#pragma once

#include <string>
#include <iterator>
#include <istream>

namespace utility
{
struct StreamSentinel {};

class StreamIterator
{
public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::string;

    StreamIterator(std::istream&);

    StreamIterator();

    const value_type& operator*() const;

    const value_type* operator->() const;

    StreamIterator& operator++();

    StreamIterator operator++(int);

    friend bool operator==(const StreamSentinel&, const StreamIterator&);

    friend bool operator!=(const StreamSentinel&, const StreamIterator&);

private:
    std::istream* stream;
    std::string current_line;
    bool is_end_of_stream;

    void read();
};

class Stream
{
public:
    Stream(std::istream&);

    bool is_active() const;

    std::string next_line();

    void skip(unsigned = 1);

    StreamIterator begin();

    StreamSentinel end();

private:
    std::istream& stream;
};
} // namespace utility
