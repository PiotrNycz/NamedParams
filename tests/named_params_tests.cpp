#include <params.hpp>
#include <gtest/gtest.h>
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

TEST_F(NamedParamTest, canUseReference)
{
    using namespace std::string_literals;

    std::string str = "First";
    // replaceValue(p_<ref> = str, p_<value> = "Second");

    const char* second = "Second";
    named::param<ref, std::string&>(p_ref = str).value = named::param<value, const std::string&>(p_value = "Second").value;
    ASSERT_EQ(str, "Second");
}
