#pragma once

#include <utility>
#include <array>


namespace named
{

template <typename name, typename T>
struct arg;

template <typename name, typename T>
struct param
{
    template <typename U>
    param(arg<name, U> const& a_);

    operator T() const { return value; }
    T value;
};

template <typename name, typename T>
struct arg
{
    T value;
};



template <typename name>
struct evaluator
{
    template <typename T>
    constexpr auto operator = (T&& value) const &
    {
        return arg<name, T&&>{std::forward<T>(value) };
    }
};

template <typename name>
constexpr evaluator<name> p_;



template <typename name, typename T>
template <typename U>
param<name, T>::param(arg<name, U> const& a_)
    : value(a_.value)
{}


}

