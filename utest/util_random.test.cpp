#include <gtest/gtest.h>
#include <util_random.hpp>
#include <util_bytes.hpp>
#include <algorithm>

using util::bytes_t;
using util::byte_t;

TEST(util_random,byte_pseudo){
  //hard to test. The only thing I can do
  //is see that the distribution is good
  //on various bucket sizes
  std::vector<char> vec;
  size_t sample_size = 10000000;
  for(size_t i = 0; i < sample_size;++i) {
    vec.push_back(util::random::byte_pseudo());
  }

  SCOPED_TRACE(sample_size);
  for(int num_buckets = 2; num_buckets <=256; num_buckets*=2) {
    std::vector<size_t> buckets(num_buckets,0);
    for(const auto& byte:vec) {
      buckets[byte%num_buckets]++;
    }
    auto minmax = std::minmax_element(buckets.begin(),buckets.end());
    float max_percent_diff = 100.0*((1.0*(*minmax.second - *minmax.first)) / ((1.0*sample_size)/(1.0*num_buckets)));
    SCOPED_TRACE(num_buckets);
    SCOPED_TRACE(*minmax.first);
    SCOPED_TRACE(*minmax.second);
    ASSERT_GT(5,max_percent_diff);
  }
}

TEST(util_random,bytes_safe){
  size_t sample_size = 10000000;
  bytes_t vec(sample_size);
  ASSERT_TRUE(util::random::bytes_safe(vec));

  SCOPED_TRACE(sample_size);
  for(int num_buckets = 2; num_buckets <=256; num_buckets*=2) {
    std::vector<size_t> buckets(num_buckets,0);
    for(const auto& byte:vec) {
      buckets[byte%num_buckets]++;
    }
    auto minmax = std::minmax_element(buckets.begin(),buckets.end());
    float max_percent_diff = 100.0*((1.0*(*minmax.second - *minmax.first)) / ((1.0*sample_size)/(1.0*num_buckets)));
    SCOPED_TRACE(num_buckets);
    SCOPED_TRACE(*minmax.first);
    SCOPED_TRACE(*minmax.second);
    ASSERT_GT(5,max_percent_diff);
  }
}

TEST(util_random,bytes_pseudo){
  size_t sample_size = 100;
  bytes_t vec(sample_size);
  ASSERT_TRUE(util::random::bytes_pseudo(vec));
}


TEST(util_random,bytes_safe_attempt){ 
  size_t sample_size = 100;
  bytes_t vec(sample_size);
  ASSERT_TRUE(util::random::bytes_safe_attempt(vec));
}
