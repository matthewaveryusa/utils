#include <gtest/gtest.h>
#include <util_exception.hpp>


TEST(util_exception, ) {
  {
  char * ptr =  nullptr;
  ASSERT_THROW(util::exception::throw_null(ptr),std::runtime_error);
  }

  {
  char ptr = 12;
  ASSERT_NO_THROW(util::exception::throw_null(&ptr));
  }
}

