#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>
#include <sstream>
#include <stack>
#include <string>

std::string get_filename(std::string);

#define LOCATION std::string{"["}.append(get_filename(__FILE__)).append(":").append(std::to_string(__LINE__)).append("]")

#define LOG_MACRO(arg) std::cout << LOCATION << ' ' << arg << std::endl;

template <typename T>
void print(std::ostream& stream, const T& data)
{
    stream << data;
}

template <typename T, typename U>
void print(std::ostream&, const std::pair<T, U>&);

template <typename T>
concept Stringable = requires (T data)
{
    {data} -> std::convertible_to<std::string>;
    typename T::iterator;
};

template <Stringable String>
void print(std::ostream&, const String&);

template <typename T>
concept Iterable = requires
{
    typename T::iterator;
};

template <Iterable Container> requires (not Stringable<Container>)
void print(std::ostream&, const Container&);

template <typename T>
void print(std::ostream&, const std::optional<T>&);

template <>
void print(std::ostream&, const char&);

template <typename T, typename U>
void print(std::ostream& stream, const std::pair<T, U>& pair)
{
    stream << '{';
    print(stream, pair.first);
    stream << ", ";
    print(stream, pair.second);
    stream << '}';
}

template <Stringable String>
void print(std::ostream& stream, const String& string)
{
    stream << std::quoted(string);
}

template <Iterable Container> requires (not Stringable<Container>)
void print(std::ostream& stream, const Container& container)
{
    stream << '[';
    if (not container.empty())
    {
        const auto first_element_iter = container.begin();
        const auto size = container.size();
        const auto last_element_iter = std::next(first_element_iter, size - 1);
        std::for_each(first_element_iter, last_element_iter, [&stream](const auto& e) {
            print(stream, e);
            stream << ", ";
        });
        print(stream, *last_element_iter);
    }
    stream << ']';
}

template <typename T>
void print(std::ostream& stream, const std::optional<T>& opt)
{
    if (opt.has_value())
    {
        return print(stream, opt.value());
    }
    stream << "nullopt";
}

#define DBG(expr) \
    [&](auto&& e) -> decltype(auto) { \
        using Expr = decltype(e); \
        std::stringstream stream; \
        stream << std::boolalpha; \
        stream << #expr << ':'; \
        print(stream, e); \
        stream << std::noboolalpha; \
        LOG_MACRO(stream.str()); \
        return std::forward<Expr>(e); \
    } (expr)

struct Stringified
{
    std::string string;
};

std::ostream& operator<<(std::ostream&, const Stringified&);

#define STR(expr) \
    [&](auto&& e) -> decltype(auto) { \
        std::stringstream stream; \
        stream << #expr << ':'; \
        stream << std::boolalpha; \
        print(stream, e); \
        stream << std::noboolalpha; \
        return Stringified{stream.str()}; \
    } (expr)

template <typename ... Ts>
void log(const std::string& location, const Ts& ... data)
{
    std::cout << location << ' ';
    ((print(std::cout, data)), ...);
    std::cout << std::endl;
}

#define LOG(...) log(LOCATION, __VA_ARGS__)

template <typename ... Ts>
std::string concatenate(const Ts& ... data)
{
    std::stringstream stream;
    ((print(stream, data)), ...);
    return stream.str();
}

#define CONCAT(...) concatenate(__VA_ARGS__)

template <typename T>
void log_params_rest(const T& data)
{
    print(std::cout, data);
    std::cout << std::endl;
}

template <typename T, typename ... Ts>
void log_params_rest(const T& data, const Ts& ... tail)
{
    print(std::cout, data);
    std::cout << ", ";
    log_params_rest(tail...);
}

template <typename ... Ts>
void log_params(const std::string& location, const Ts& ... data)
{
    std::cout << location << ' ';
    log_params_rest(data...);
}

#define LOG_PARAM(...) log_params(LOCATION, __VA_ARGS__)

#define LOG_SEPARATOR() log(LOCATION, "----------------------------------------------------------------------")

#define WAIT() std::cin.get();
