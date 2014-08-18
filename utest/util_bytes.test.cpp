#include <gtest/gtest.h> 
#include <util_bytes.hpp>
#include <iterator>
#include <algorithm>


TEST(util_bytes, basic) {
  using namespace util::bytes;
  using namespace util::bytes::k;
  using util::byte_t;
  EXPECT_EQ(0, shift(0));
  EXPECT_EQ(8, shift(1));
  EXPECT_EQ(16, shift(2));
  EXPECT_EQ(24, shift(3));
  EXPECT_EQ(32, shift(4));
  EXPECT_EQ(40, shift(5));
  EXPECT_EQ(48, shift(6));
  EXPECT_EQ(56, shift(7));
  EXPECT_EQ(64, shift(8));

  EXPECT_EQ(0x00000000000000FFul, mask(0));
  EXPECT_EQ(0x000000000000FF00ul, mask(1));
  EXPECT_EQ(0x0000000000FF0000ul, mask(2));
  EXPECT_EQ(0x00000000FF000000ul, mask(3));
  EXPECT_EQ(0x000000FF00000000ul, mask(4));
  EXPECT_EQ(0x0000FF0000000000ul, mask(5));
  EXPECT_EQ(0x00FF000000000000ul, mask(6));
  EXPECT_EQ(0xFF00000000000000ul, mask(7));

  byte_t val[] = {'\0','\0','\0','\0','\0','\0','\0','\0'};
  EXPECT_EQ(true, is_32_bit_in_64_bit(&val[0]));
  val[7] = 1;
  EXPECT_EQ(true, is_32_bit_in_64_bit(&val[0]));
  val[0] = 1;
  EXPECT_EQ(false, is_32_bit_in_64_bit(&val[0]));
}


TEST(util_bytes, set) {
  using namespace util::bytes;
  using namespace util::bytes::k;
  using util::byte_t;

  EXPECT_EQ(true, is_set(none, none,none));
  EXPECT_EQ(true, is_set(none, all,none));
  EXPECT_EQ(true, is_set(all, none,none));
  EXPECT_EQ(true, is_set(all, all, all));
  EXPECT_EQ(false, is_set(all, all, none));
  EXPECT_EQ(true, is_set(bit_1, bit_1,bit_1));
  EXPECT_EQ(false, is_set(bit_2, bit_1,bit_1));
  EXPECT_EQ(false, is_set(bottom_2_bits, bottom_2_bits,bit_1));
  EXPECT_EQ(true, is_set(bottom_2_bits, bottom_2_bits,bottom_2_bits));
  EXPECT_EQ(false, is_set(bottom_2_bits, bit_1,bottom_2_bits));
  EXPECT_EQ(true, is_set(bottom_2_bits, bit_1,bit_1));
  EXPECT_EQ(true, is_set(top_7_bits, bottom_7_bits,0x7E));

  EXPECT_EQ(true, is_set(none, none));
  EXPECT_EQ(false, is_set(none, all));
  EXPECT_EQ(true, is_set(all, none));
  EXPECT_EQ(true, is_set(all, all));
  EXPECT_EQ(true, is_set(bit_1, bit_1));
  EXPECT_EQ(false, is_set(bit_2, bit_1));
  EXPECT_EQ(true, is_set(bottom_2_bits, bottom_2_bits));
  EXPECT_EQ(true, is_set(bottom_2_bits, bit_1));
  EXPECT_EQ(true, is_set(bottom_2_bits, bit_2));
  EXPECT_EQ(false, is_set(top_7_bits, bottom_7_bits));
}


TEST(util_bytes, get) {
  using namespace util::bytes;
  using namespace util::bytes::k;
  using util::byte_t;
  {
  uint64_t data = 0xFEDCBA9876543210ul;
  EXPECT_EQ(0x10, get(0,data));
  EXPECT_EQ(0x32, get(1,data));
  EXPECT_EQ(0x54, get(2,data));
  EXPECT_EQ(0x76, get(3,data));
  EXPECT_EQ(0x98, get(4,data));
  EXPECT_EQ(0xBA, get(5,data));
  EXPECT_EQ(0xDC, get(6,data));
  EXPECT_EQ(0xFE, get(7,data));
  }
  
  {
  uint32_t data = 0x76543210u;
  EXPECT_EQ(0x10, get(0,data));
  EXPECT_EQ(0x32, get(1,data));
  EXPECT_EQ(0x54, get(2,data));
  EXPECT_EQ(0x76, get(3,data));
  }
 
  {
  uint32_t data = 0x3210u;
  EXPECT_EQ(0x10, get(0,data));
  EXPECT_EQ(0x32, get(1,data));
  }
}

TEST(util_bytes, reads) {
  using namespace util::bytes;
  using namespace util::bytes::k;
  using util::byte_t;

  const byte_t data[] =  { 0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10 };
  EXPECT_EQ(0xFEDCBA9876543210ul,read_64_bits(&data[0]));
  EXPECT_EQ(        0x76543210u ,read_32_bits(&data[4]));
  EXPECT_EQ(            0x3210u ,read_16_bits(&data[6]));
}

TEST(util_bytes, writes) {
  using namespace util::bytes;
  using namespace util::bytes::k;
  using util::byte_t;
  const byte_t data[] =  { 0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10 };
  byte_t out[sizeof(data)];
  uint64_t in64 = 0xFEDCBA9876543210ul;
  uint32_t in32 = 0x76543210u;
  uint16_t in16 = 0x3210u;
  write_64_bits(in64,  std::begin(out));
  EXPECT_EQ(true, std::equal(std::begin(data),std::end(data),std::begin(out),std::end(out)));
  write_32_bits(in32,  std::begin(out));
  EXPECT_EQ(true, std::equal(std::begin(data)+4,std::end(data),std::begin(out),std::end(out)-4));
  //sanity test
  EXPECT_EQ(false, std::equal(std::begin(data),std::end(data),std::begin(out),std::end(out)));
  write_16_bits(in16,  std::begin(out));
  EXPECT_EQ(true, std::equal(std::begin(data)+6,std::end(data),std::begin(out),std::end(out)-6));
  //sanity test
  EXPECT_EQ(false, std::equal(std::begin(data),std::end(data),std::begin(out),std::end(out)));
}

TEST(util_bytes, search) {
  using namespace util::bytes;
  using namespace util::bytes::k;
  using util::byte_t;
  byte_t data[] =  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  };
  EXPECT_EQ(nullptr,search(std::begin(data),std::end(data),0x01));
  EXPECT_EQ(std::begin(data),search(std::begin(data),std::end(data),*std::begin(data)));
  data[0] = 0x01;
  EXPECT_EQ(&data[0],search(std::begin(data),std::end(data),0x01));
  data[1] = 0x01;
  EXPECT_EQ(&data[0],search(std::begin(data),std::end(data),0x01));
  data[0] = 0x00;
  EXPECT_EQ(&data[1],search(std::begin(data),std::end(data),0x01));
  data[1] = 0x00;
  *(std::end(data)-1) = 0x01;
  EXPECT_EQ(nullptr,search(std::begin(data),std::end(data)-1,0x01));
  EXPECT_EQ(std::end(data)-1,search(std::begin(data),std::end(data),0x01));

  byte_t s[] =  { 0x00,0x00  };
  EXPECT_EQ(std::begin(data),search(std::begin(data),std::end(data),std::begin(s),std::end(s)));
  byte_t data2[] =  { 0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01  };
  EXPECT_EQ(nullptr,search(std::begin(data2),std::end(data2),std::begin(s),std::end(s)));
  byte_t data3[] =  { 0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x00  };
  EXPECT_EQ(std::end(data3)-2,search(std::begin(data3),std::end(data3),std::begin(s),std::end(s)));
  EXPECT_EQ(nullptr,search(std::begin(data3),std::end(data3)-1,std::begin(s),std::end(s)));
}


TEST(util_bytes, multi_copy) {
  using namespace util::bytes;
  using namespace util::bytes::k;
  using util::byte_t;
  byte_t out  []  =  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  byte_t data1[]  =  { 0x01,0x23  };
  byte_t data2[]  =  { 0x45,0x67  };
  
  {
  byte_t result[] =   { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  };
  multi_copy(std::begin(out),std::begin(data1),std::begin(data1));
  EXPECT_EQ(true,std::equal(std::begin(out),std::end(out), std::begin(result), std::end(result)));
  }
 
  {
  byte_t result[] =   { 0x01,0x23,0x00,0x00,0x00,0x00,0x00,0x00  };
  multi_copy(std::begin(out),std::begin(data1),std::end(data1));
  EXPECT_EQ(true,std::equal(std::begin(out),std::end(out), std::begin(result), std::end(result)));
  }
  
  {
  byte_t result[] =   { 0x01,0x23,0x45,0x67,0x00,0x00,0x00,0x00  };
  multi_copy(std::begin(out),std::begin(data1),std::end(data1),
      std::begin(data2),std::end(data2));
  EXPECT_EQ(true,std::equal(std::begin(out),std::end(out), std::begin(result), std::end(result)));
  }
  
  {
  byte_t result[] =   { 0x01,0x23,0x45,0x67,0x00,0x00,0x00,0x00  };
  multi_copy(std::begin(out),std::begin(data1),std::begin(data1));
  EXPECT_EQ(true,std::equal(std::begin(out),std::end(out), std::begin(result), std::end(result)));
  }
  
  {
  byte_t result[] =   { 0x45,0x67,0x45,0x67,0x00,0x00,0x00,0x00  };
  multi_copy(std::begin(out),std::begin(data2),std::end(data2));
  EXPECT_EQ(true,std::equal(std::begin(out),std::end(out), std::begin(result), std::end(result)));
  }
  
}

TEST(util_bytes, view) {
  using namespace util::bytes;
  using namespace util::bytes::k;
  using util::byte_t;

  std::string str = "0123456789";
  std::vector<char> vchar = {'0','1','2','3','4','5','6','7','8','9'};
  std::vector<char> vuchar = {'0','1','2','3','4','5','6','7','8','9'};
  std::array<char,10> achar = {{'0','1','2','3','4','5','6','7','8','9'}};
  std::array<unsigned char,10> auchar = {{'0','1','2','3','4','5','6','7','8','9'}};
  char cachar[] = {'0','1','2','3','4','5','6','7','8','9'};
  unsigned char cauchar[] = {'0','1','2','3','4','5','6','7','8','9'};
  std::vector<View> views = { str,vchar,vuchar,achar,auchar,cachar,{&cachar[0],10},{&cauchar[0],10} };
  auto first_view = views[0];
  for( auto &view:views) {
    EXPECT_TRUE(std::equal(first_view.begin(),first_view.end(),view.begin(),view.end()));
  }
  
  std::vector<ConstView> cviews = { str,vchar,vuchar,achar,auchar,cachar,{&cachar[0],10},{&cauchar[0],10} };
  auto first_cview = cviews[0];
  for( auto &cview:cviews) {
    EXPECT_TRUE(std::equal(first_cview.begin(),first_cview.end(),cview.begin(),cview.end()));
  }
  
  EXPECT_TRUE(resize_callback(str)(20,'\0',views[0]));
  EXPECT_TRUE(resize_callback(vuchar)(20,'\0',views[1]));
  EXPECT_FALSE(resize_callback(achar)(20,'\0',views[2]));
  EXPECT_FALSE(resize_callback(auchar)(20,'\0',views[3]));
  EXPECT_FALSE(resize_callback(achar)(5,'\0',views[2]));
  EXPECT_FALSE(resize_callback(auchar)(5,'\0',views[3]));
  EXPECT_TRUE(resize_callback(achar)(10,'\0',views[2]));
  EXPECT_TRUE(resize_callback(auchar)(10,'\0',views[3]));
  size_t output_size;
  EXPECT_TRUE(resize_callback(achar,output_size)(5,'\0',views[2]));
  EXPECT_EQ(5, output_size);
  output_size = 0;
  EXPECT_TRUE(resize_callback(auchar,output_size)(5,'\0',views[3]));
  EXPECT_EQ(5, output_size);
  
}
