#include <params.hpp>
#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>


struct x;
struct y;
struct n;
struct str;
struct ref;
struct value;

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
    std::ostringstream res;
    std::fill_n(std::ostream_iterator<std::string>(res, ""), n, str);
    return res.str();
}

static void replaceValue(named::param<ref, std::string&> ref, named::param<value, std::string const&> value)
{
    ref = value;
}

using namespace ::testing;
struct NamedParamTest : public Test
{};

using named::p_;

TEST_F(NamedParamTest, canUseOne)
{
    ASSERT_EQ(7, passThrough(p_<x> = 7));
    ASSERT_EQ(13, passThrough(p_<x> = 13));
}

TEST_F(NamedParamTest, canUseMoreThanOne)
{
    ASSERT_EQ(20, add(p_<x> = 7, p_<y> = 13));
    ASSERT_EQ(0, add(p_<x> = 7, p_<y> = -7));
}

TEST_F(NamedParamTest, canUseMoreThanOneType)
{
    using namespace std::string_literals;
    ASSERT_EQ("abc abc abc "s, repeatStr(p_<n> = 3, p_<str> = "abc "s));
}

TEST_F(NamedParamTest, canUseReference)
{
    std::string str = "First";
    replaceValue(p_<ref> = str, p_<value> = "Second");
    ASSERT_EQ(str, "Second");
}
