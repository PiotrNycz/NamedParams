#pragma once

#include <utility>
#include <array>
#include <type_traits>
#include <typeinfo>


#include <algorithm>
#include <tuple>
#include <functional>
#include <iostream>

#include <cxxabi.h>

namespace DEBUG
{

#define DEBUG_IS_ON 1

#if DEBUG_IS_ON
#define DEBUG_ONLY(...) __VA_ARGS__
#else
#define DEBUG_ONLY(...)
#endif

#define PRINT(a) DEBUG_ONLY(std::cerr << a << std::endl;)

DEBUG_ONLY(

template <typename T>
struct incomplete_name_
{};

template <typename T>
std::string get_name()
{
    using TT = DEBUG::incomplete_name_<T>;
    std::string prefix = "DEBUG::incomplete_name_<";
    std::string suffix = ">";


    const char* mangled_name = typeid(TT).name();
    std::string ret_val = abi::__cxa_demangle(mangled_name, 0, 0, NULL);
    const std::string string_name = "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >";
    for (size_t start_pos = ret_val.find(string_name); start_pos != std::string::npos; start_pos = ret_val.find(string_name))
          ret_val.replace(start_pos, string_name.length(), "std::string");
    ret_val = ret_val.substr(prefix.length());
    ret_val.erase(std::prev(ret_val.end()));
    return ret_val;
}


struct Trace
{
    std::function<void(std::ostream&)> data{};

    Trace() = default;
    template <typename ...Arg>
    Trace(const char* name, Arg const& ...arg)
    {
        setTrace(name, arg...);
    }

    template <typename ...Arg>
    void setTrace(const char* name, Arg const& ...arg)
    {
        data = [name, args = std::tuple{arg...}](std::ostream& os) -> void
        {
            os << name;
            std::apply([&](auto& ...elem) { ((os << ' ' << elem), ...); }, args);
        };
        PRINT("+++" << *this);
    }
    ~Trace()
    {
        PRINT("---" << *this);
    }
    friend std::ostream& operator << (std::ostream& os, Trace const& obj)
    {
        obj.data(os);
        return os;
    }
};

)

#if DEBUG_IS_ON

#define TRACE_PASTE(x, y) x ## y
#define TRACE_PASTE_2(x, y) TRACE_PASTE(x, y)
#define TRACE_NAME TRACE_PASTE_2(trace, __LINE__)

#define LOCAL_TRACE DEBUG::Trace  TRACE_NAME
#define MEMBER_TRACE DEBUG::Trace trace__local
#define SET_TRACE  trace__local.setTrace

#else

#define LOCAL_TRACE(...)
#define MEMBER_TRACE(...)
#define SET_TRACE(...)


#endif

}

namespace named
{

template <typename name, typename T>
struct arg;

template <typename name, typename A>
struct is_arg : std::false_type {};

template <typename name, typename T>
struct is_arg<name, arg<name, T>> : std::true_type {};

template <typename name, typename T>
struct param
{
    template <typename U>
    param(U&&  a_, std::enable_if_t<is_arg<name, U>::value>* = nullptr)
        : value(std::forward<U>(a_).value)
    {
        SET_TRACE("param<", DEBUG::get_name<T>(), ",", DEBUG::get_name<U>(), ">(this->value = *", (void*)&value, ", value = *", (void*)&a_.value, "'", a_.value , "')");
    }

    operator T() const { return value; }
    T value;

    DEBUG_ONLY(MEMBER_TRACE;)
};

template <typename name, typename T>
struct arg
{
    T value;
    DEBUG_ONLY(MEMBER_TRACE{"arg<", DEBUG::get_name<name>(), ",", DEBUG::get_name<T>(), "> = &", (void*)&value};)
};

template <typename name>
struct evaluator
{
    template <typename T>
    constexpr auto operator = (T* value) const
    {
        LOCAL_TRACE(__PRETTY_FUNCTION__, " = &", (void*)&value);
        return arg<name, T*>{value};
    }

    template <typename T>
    constexpr auto operator = (T&& value) const
    {
        LOCAL_TRACE(__PRETTY_FUNCTION__, " = &", (void*)&value);
        return arg<name, T&&>{ std::forward<T>(value) };
    }
};

template <typename name>
constexpr evaluator<name> p_;

}

