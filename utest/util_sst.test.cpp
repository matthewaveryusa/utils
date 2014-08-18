#include <gtest/gtest.h>
#include <util_sst.hpp>

TEST(util_sst,string2bytes){
  ASSERT_EQ(1024,util::sst::string2bytes("1kB"));
  ASSERT_EQ(1024,util::sst::string2bytes("8kb"));
  ASSERT_EQ(12884901888,util::sst::string2bytes("12GB"));
  ASSERT_EQ(12884901888/8,util::sst::string2bytes("12Gb"));
  ASSERT_EQ(12582912,util::sst::string2bytes("12MB"));
  ASSERT_EQ(12582912/8,util::sst::string2bytes("12mb"));
  ASSERT_EQ(1,util::sst::string2bytes("8b"));
  ASSERT_EQ(8,util::sst::string2bytes("8B"));
}

TEST(util_sst,string2kBps){
  ASSERT_EQ(1,util::sst::string2kBps("1kBps"));
  ASSERT_EQ(1,util::sst::string2kBps("8kbps"));
  ASSERT_EQ(12582912,util::sst::string2kBps("12GBps"));
  ASSERT_EQ(1572864,util::sst::string2kBps("12Gbps"));
  ASSERT_EQ(12288,util::sst::string2kBps("12MBps"));
  ASSERT_EQ(1536,util::sst::string2kBps("12mbps"));
  ASSERT_EQ(0,util::sst::string2kBps("8bps"));
  ASSERT_EQ(0,util::sst::string2kBps("8Bps"));
}

TEST(util_sst,string2duration){ }

TEST(util_sst,to_timestamp){
 time_t t= 1426325213;
 ASSERT_EQ("2015-03-14 09:26:53",util::sst::to_timestamp(t));
 t = 0;
 ASSERT_EQ("1970-01-01 00:00:00",util::sst::to_timestamp(t));
}

