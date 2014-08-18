#include <gtest/gtest.h>
#include <algorithm>
#include <util_hash.hpp>
#include <util_bytes.hpp>
#include <vector>
#include <string>

using util::bytes_t;
using util::byte_t;

TEST(util_hash, SHA512_base64) {
  {
  std::string expected_response_str = "MJ7MSJwS1utMxA9QyQLytNDtd+5RGnx6m808qG1M2G+YndNbxf9JlnDaNCVbRbDP2DDoH2Bdz33FVC6TrpzXbw==";
  std::string input_str = "hello world";
  std::string salt_str  = "";

  bytes_t expected_response(expected_response_str.begin(),expected_response_str.end());
  bytes_t input(input_str.begin(),input_str.end());
  bytes_t salt(salt_str.begin(),salt_str.end());
  bytes_t output;

  ASSERT_EQ(true,util::hash::SHA512(input,salt,output,util::bytes::resize_callback(output)));
  input.swap(output);
  ASSERT_EQ(0,util::base64::encode(input,output, util::bytes::resize_callback(output)));
  ASSERT_EQ(true,std::equal(output.begin(),output.end(),expected_response.begin(),expected_response.end()));
  }
  
  {
  std::string expected_response_str = "MJ7MSJwS1utMxA9QyQLytNDtd+5RGnx6m808qG1M2G+YndNbxf9JlnDaNCVbRbDP2DDoH2Bdz33FVC6TrpzXbw==";
  std::string input_str = " world";
  std::string salt_str  = "hello";
  
  bytes_t expected_response(expected_response_str.begin(),expected_response_str.end());
  bytes_t input(input_str.begin(),input_str.end());
  bytes_t salt(salt_str.begin(),salt_str.end());
  bytes_t output;

  ASSERT_EQ(true,util::hash::SHA512(input,salt,output, util::bytes::resize_callback(output)));
  input.swap(output);
  ASSERT_EQ(0,util::base64::encode(input,output, util::bytes::resize_callback(output)));
  ASSERT_EQ(true,std::equal(output.begin(),output.end(),expected_response.begin(),expected_response.end()));
  }
}


