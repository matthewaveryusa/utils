#include <gtest/gtest.h>
#include <util_swiss.hpp>
#include <iostream>

namespace {
  struct TestClass {
    int &count;
    bool has_count;
    TestClass(int &a_count): count(a_count),has_count(true){
      ++count;
    }
    TestClass(TestClass &&other):count(other.count),has_count(other.has_count) {
      other.has_count = false;
    }
    ~TestClass() {
      if(has_count) { --count; }
    }
  };

}

TEST(util_swiss, meta) {
  int count = 0;
  TestClass *test_ptr = nullptr;
  {
  TestClass test(count);
  test_ptr = &test;
  ASSERT_EQ(test_ptr,&test);
  ASSERT_EQ(1,test.count);
  ASSERT_EQ(count,test.count);
  }
  ASSERT_EQ(0,count);
}


TEST(util_swiss, make_delete) {
  int count = 0;
  TestClass test(count);
  TestClass *test_ptr = &test;
  auto delfunc = util::swiss::make_delete<std::nullptr_t>(std::move(test));
  ASSERT_EQ(1,test_ptr->count);
  ASSERT_EQ(1,count);
  delfunc(nullptr);
  ASSERT_EQ(0,count);
}

TEST(util_swiss, make_delete_scopes) {
  int count = 0;
  TestClass *test_ptr;
  std::function<void(std::nullptr_t)> delfunc;
  {
    TestClass test(count);
    test_ptr = &test;
    delfunc = util::swiss::make_delete<std::nullptr_t>(std::move(test));
    ASSERT_EQ(1,test_ptr->count);
  }
  ASSERT_EQ(1,test_ptr->count);
  ASSERT_EQ(1,count);
  delfunc(nullptr);
  ASSERT_EQ(0,count);
}
