#include <gtest/gtest.h>
#include <util_bitmask.hpp>

TEST(util_bitmask, basic) {
    using namespace util::bitmask;
    enum class Eight : char {A = 0x1, B = 0x2, C = 0x04};
    Eight a = Eight::A;
    Eight b = Eight::B;
    char stuff = enum_type(a) + enum_type(b);
    ASSERT_EQ(3,stuff);
    ASSERT_EQ(true,is_set(a,Eight::A));
    ASSERT_EQ(false,is_set(b,Eight::A));
    set(b,Eight::A,true);
    ASSERT_EQ(true,is_set(b,Eight::A));
    set(b,Eight::A,false);
    ASSERT_EQ(false,is_set(b,Eight::A));
}
