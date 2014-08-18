#include <gtest/gtest.h>
#include <util_base64.hpp>
#include <util_bytes.hpp>
using util::bytes_t;
using util::byte_t;

class util_base64 : public ::testing::Test {
protected:
	virtual void SetUp() {	
    typedef std::tuple<bytes_t,bytes_t> t;
    valid = {
      t({}                               ,{}),
      t({'Z','g','=','='}                ,{'f'}),
      t({'Z','m','8','='}                ,{'f','o'}),
      t({'Z','m','9','v'}                ,{'f','o','o'}),
      t({'Z','m','9','v','Y','g','=','='},{'f','o','o','b'}),
      t({'Z','m','9','v','Y','m','E','='},{'f','o','o','b','a'}),
      t({'Z','m','9','v','Y','m','F','y'},{'f','o','o','b','a','r'})
    };

  }

	virtual void TearDown() { }
  std::vector<std::tuple<bytes_t,bytes_t>> valid;
  std::vector<bytes_t> invalid;
};

TEST_F(util_base64, valid_to_base64) {
  for (const auto & test : valid) {
    bytes_t out;
    ASSERT_EQ(0,util::base64::encode(std::get<1>(test), out, util::bytes::resize_callback(out)));
    ASSERT_EQ(std::get<0>(test),out);
  }
}

TEST_F(util_base64, valid_from_base64) {
  for (const auto & test : valid) {
    bytes_t out;
    ASSERT_EQ(0,util::base64::decode(std::get<0>(test), out, util::bytes::resize_callback(out)));
    ASSERT_EQ(std::get<1>(test),out);
  }
}
