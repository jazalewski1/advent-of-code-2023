#include "utility/Stream.hpp"
#include <fstream>
#include <string>
#include <iterator>
#include <istream>

namespace utility
{
StreamIterator::StreamIterator(std::istream& s) : stream{&s}
{
    read();
}

StreamIterator::StreamIterator() : stream{nullptr}, is_end_of_stream{true} {}

const StreamIterator::value_type& StreamIterator::operator*() const { return current_line; }

const StreamIterator::value_type* StreamIterator::operator->() const { return &current_line; }

StreamIterator& StreamIterator::operator++()
{
    read();
    return *this;
}

StreamIterator StreamIterator::operator++(int) { return StreamIterator{*stream}; }

bool operator==(const StreamSentinel&, const StreamIterator& itr)
{
    return itr.is_end_of_stream;
}

bool operator!=(const StreamSentinel& sen, const StreamIterator& itr)
{
    return not (sen == itr);
}

void StreamIterator::read()
{
    is_end_of_stream = stream->eof();
    if (not is_end_of_stream)
    {
        std::getline(*stream, current_line);
    }
}


Stream::Stream(std::istream& stream) : stream{stream}
{
}

bool Stream::is_active() const { return not stream.eof(); }

std::string Stream::next_line() { return *StreamIterator{stream}; }

void Stream::skip(unsigned count)
{
    for (auto i = 0u; i < count; ++i)
    {
        StreamIterator{stream};
    }
}

StreamIterator Stream::begin() { return StreamIterator{stream}; }

StreamSentinel Stream::end() { return StreamSentinel{}; }
} // namespace utility

