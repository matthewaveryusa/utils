#include <gtest/gtest.h>
#include <util_print.hpp>
using util::byte_t;

TEST(util_print, hex) {
  {
    std::ostringstream oss;
    byte_t buffer[2] = {255,255};
    util::print::hex(oss,buffer,sizeof(buffer));
    ASSERT_EQ("ff ff",oss.str());
  }

  {
    std::ostringstream oss;
    byte_t buffer[2] = {0,0};
    util::print::hex(oss,buffer,sizeof(buffer));
    ASSERT_EQ("00 00",oss.str());
  }
}

TEST(util_print, hex_ascii) {
  {
    std::ostringstream oss;
    byte_t buffer[] = {15,0,'a','0','A',1};
    util::print::ascii_hex(oss,buffer,sizeof(buffer));
    ASSERT_EQ("0x0f 0x00 a0A 0x01",oss.str());
  }
  
  {
    std::ostringstream oss;
    byte_t buffer[] = {15,0,'a','0','A',1,'1','2'};
    util::print::ascii_hex(oss,buffer,sizeof(buffer));
    ASSERT_EQ("0x0f 0x00 a0A 0x01 12",oss.str());
  }
}

TEST(util_print, hex_ascii_clump) {
  {
    std::ostringstream oss;
    byte_t buffer[] = {15,0,'a','0','A',1};
    util::print::ascii_hex_clump(oss,buffer,sizeof(buffer));
    ASSERT_EQ("0x0f 0x00 a0A 0x01",oss.str());
  }
  
  {
    std::ostringstream oss;
    byte_t buffer[] = {15,0,'a','0','A',1,'1','2'};
    util::print::ascii_hex_clump(oss,buffer,sizeof(buffer));
    ASSERT_EQ("0x0f 0x00 a0A 0x01 12",oss.str());
  }
  
  {
    std::ostringstream oss;
    byte_t buffer[] = {15,15,0,0,'a','a','0','A','A',1,'1','2'};
    util::print::ascii_hex_clump(oss,buffer,sizeof(buffer));
    ASSERT_EQ("0x0f(x2) 0x00(x2) a(x2)0A(x2) 0x01 12",oss.str());
  }
}


TEST(util_print, max_bytes_required) {
  ASSERT_EQ(4,util::print::max_bytes_required<char>());
  ASSERT_EQ(11,util::print::max_bytes_required<int>());
  ASSERT_EQ(20,util::print::max_bytes_required<int64_t>());
  ASSERT_EQ(58,util::print::max_bytes_required<float>());
  ASSERT_EQ(328,util::print::max_bytes_required<double>());
}

TEST(util_print, format) {
  ASSERT_EQ("%s",util::print::format<char *>());
  ASSERT_EQ("%s",util::print::format<char[4]>());
  ASSERT_EQ("%d",util::print::format<int>());
  ASSERT_EQ("%.16g",util::print::format<float>());
  ASSERT_EQ("%c",util::print::format<char>());
  ASSERT_EQ("%d",util::print::format<unsigned char>());
}

TEST(util_print, to_buffer) {
  byte_t buffer[500];

  auto test = [&buffer,buffer_size = sizeof(buffer)](auto val, const std::string &result)->void {
    SCOPED_TRACE(val);
    SCOPED_TRACE(result);
    std::fill(std::begin(buffer),std::end(buffer),'\0');
    int num_bytes = 0;
    ASSERT_TRUE(util::print::to_buffer(buffer,buffer_size,val,num_bytes));
    ASSERT_EQ(result.size(),num_bytes);
    ASSERT_EQ(true,std::equal(buffer,buffer+num_bytes,result.begin(),result.end()));
  };

  test(12,"12");
  test(-12,"-12");
  test(12.5,"12.5");
  test(-12.5,"-12.5");
  test("hi world","hi world");
  test((uint8_t) 4,"4");
  test('a',"a");
  test((unsigned char) 'a',"97");
  test((char) 97,"a");
  test((char) 97,"a");
  test(1.0/3.0,"0.3333333333333333");
  test(0.001,"0.001");
  test(0.1,"0.1");
  test(1.0000000000000002,"1");
  test(1.000000000000002,"1.000000000000002");
  test(150000000000000000,"150000000000000000");
  test(true,"true");
  test(false,"false");
  {
    std::string result = "null";
    size_t buffer_size = sizeof(buffer);
    SCOPED_TRACE(result);
    std::fill(std::begin(buffer),std::end(buffer),'\0');
    int num_bytes = 0;
    ASSERT_TRUE(util::print::to_buffer(buffer,buffer_size,nullptr,num_bytes));
    ASSERT_EQ(result.size(),num_bytes);
    ASSERT_EQ(true,std::equal(buffer,buffer+num_bytes,result.begin(),result.end()));
  }

}

TEST(util_print, number_to_buffer) {
  byte_t buffer[500];

  auto test = [&buffer,buffer_size = sizeof(buffer)](auto val, const std::string &result)->void {
    SCOPED_TRACE(val);
    SCOPED_TRACE(result); 
    std::fill(std::begin(buffer),std::end(buffer),'\0');
    size_t num_bytes;
    ASSERT_FALSE(util::print::number_to_buffer(buffer,0,val,num_bytes));
    ASSERT_EQ(result.size(),num_bytes);
    ASSERT_TRUE(util::print::number_to_buffer(buffer,buffer_size,val,num_bytes));
    ASSERT_EQ(result.size(),num_bytes);
    SCOPED_TRACE(std::string(buffer,buffer+num_bytes));
    ASSERT_EQ(true,std::equal(buffer,buffer+num_bytes,result.begin(),result.end()));
  };

  test(12,"12");
  test(-12,"-12");
  test((char) 4,"4");
  test(150000000000000000,"150000000000000000");
  test((unsigned char) 'a',"97");
}
