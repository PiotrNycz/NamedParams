#include <params.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>


struct x;
constexpr named::evaluator<x> p_x;
struct y;
constexpr named::evaluator<y> p_y;
struct n;
constexpr named::evaluator<n> p_n;
struct str;
constexpr named::evaluator<str> p_str;
struct ref;
constexpr named::evaluator<ref> p_ref;
struct value;
constexpr named::evaluator<value> p_value;

static int passThrough(named::param<x, int> x)
{
    return x;
}
static int add(named::param<x, int> x, named::param<y, int> y)
{
    return x + y;
}
static std::string repeatStr(named::param<n, int> n, named::param<str, std::string> str)
{
    LOCAL_TRACE("repeatStr");
    std::ostringstream res;
    std::fill_n(std::ostream_iterator<std::string>(res, ""), n, str);
    return res.str();
}

static void replaceValue(named::param<ref, std::string&> ref, named::param<value, std::string const&> value)
{
    LOCAL_TRACE("replaceValue(*", (void*)&ref.value, ", *", (void*)&value.value, ")");
    ref.value = value;
}

using namespace ::testing;
struct NamedParamTest : public Test
{};

using named::p_;

TEST_F(NamedParamTest, DISABLED_canUseOne)
{
    ASSERT_EQ(7, passThrough(p_x = 7));
    ASSERT_EQ(13, passThrough(p_x = 13));
}

TEST_F(NamedParamTest, DISABLED_canUseMoreThanOne)
{
    ASSERT_EQ(20, add(p_x = 7, p_y = 13));
    ASSERT_EQ(0, add(p_x = 7, p_y = -7));
}

TEST_F(NamedParamTest, DISABLED_canUseMoreThanOneType)
{
    using namespace std::string_literals;
    ASSERT_EQ("abc abc abc "s, repeatStr(p_n = 3, p_str = "abc "s));
}

TEST_F(NamedParamTest, DISABLED_canUseReference)
{
    using namespace std::string_literals;

    std::string str = "First";
    // replaceValue(p_<ref> = str, p_<value> = "Second");

    const char* second = "Second";
    // named::param<ref, std::string&>(p_ref = str).value = named::param<value, const std::string&>(p_value = "Second").value;
    named::param<ref, std::string&>(named::arg<ref, std::string&>{str}).value = named::param<value, const std::string&>(named::arg<value, const char (&)[7]>{"Second"}).value;
    // named::param<ref, std::string&>(named::arg<ref, std::string&>{str}).value = "Second";
    // named::param<ref, std::string&>(named::arg<ref, std::string&>{str}).value = named::arg<value, const char (&)[7]>{"Second"}.value;
    ASSERT_EQ(str, "Second");
}

template <auto Function>
struct param_call;
template <auto Function>
param_call<Function> with;

template <auto Function, typename ...T>
struct param_arg;

template <auto Function, typename ...T>
struct param_middle_arg
{
    std::tuple<T...> args;
    template <typename U>
    auto add_arg(U&& arg) const
    {
        return std::tuple_cat(args, std::make_tuple(std::forward<U>(arg)));
    }
};
template <auto Function, typename ...T>
struct param_final_arg
{
    std::tuple<T...> args;
    auto call() const
    {
        return std::apply(Function, args);
    }
};


int distance(int from, int to) { return to - from; }
using distance_param_call = param_call<&distance>;
using distance_param_from_call = param_arg<&distance, int>;
using distance_param_from_to_call = param_arg<&distance, int, int>;

template <>
struct param_arg<&distance, int, int> : param_final_arg<&distance, int, int>
{};

template <>
struct param_arg<&distance, int> : param_middle_arg<&distance, int>
{
    distance_param_from_to_call to(int value) const
    {
        return { this->add_arg(value) };
    }
};

template <>
struct param_call<&distance>
{
    distance_param_from_call from(int value) const { return {{value}}; }
};

constexpr auto distance_p = with<&distance>;
// with<distance>.from(1)
//               .to(2)
//               .call();
//




struct ConstCharStar
{
    const char* str;
    // const std::string& str;
};
struct StringCRef
{
    template <typename ...T>
    StringCRef(T&& ...arg) : str(*new std::string(arg...))  {}

    StringCRef(const std::string& str) : str(str) {}

    const std::string& str;
    // std::string str;
};

TEST_F(NamedParamTest, canPassConstCharAsConstStringRef)
{
    auto passConstCharStarAsStringCRef = [](const ConstCharStar& arg)
    {
        return StringCRef{arg.str};
    };
    ConstCharStar input{"Ala ma kota"};
    ASSERT_EQ(std::string("Ala ma kota"), passConstCharStarAsStringCRef(input).str);
}


using namespace testing;
class MockMM
{
public:
    MOCK_METHOD4(GetSegment, void(int refrenceId, int, int, int* a));
};

TEST(A, A)
{
    MockMM mock;
    EXPECT_CALL(mock, GetSegment(1, _, _, _))
            .WillOnce(SetArgPointee<3>(0))
            .WillOnce(SetArgPointee<3>(1));

    int a;
    int b;
    mock.GetSegment(1, 1, 0, &a);
    mock.GetSegment(1, 0, 1, &b);
    ASSERT_EQ(0, a);
    ASSERT_EQ(1, b);

}
