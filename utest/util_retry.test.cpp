#include <gtest/gtest.h>
#include <util_retry.hpp>

TEST(util_retry, exponential_backoff) { 
 auto func = util::retry::exponential_backoff(10,0,std::numeric_limits<uint64_t>::max(),1,0); 
 bool retry = true;
 ASSERT_EQ(2,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(4,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(8,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(16,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(32,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(64,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(128,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(256,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(512,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(1024,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(2048,func(retry));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(std::numeric_limits<uint64_t>::max(),func(retry));
 ASSERT_EQ(false,retry);
 ASSERT_EQ(std::numeric_limits<uint64_t>::max(),func(retry));
 ASSERT_EQ(false,retry);

 float delta = 1.0;
 float variance = 0.2;
 auto func2 = util::retry::exponential_backoff(10,0,std::numeric_limits<uint64_t>::max(),delta,variance); 
 retry = true;

 auto between = [=](uint64_t val,uint64_t expected)->bool {
   uint64_t min = 0ul;
   float min_f = delta * (1 - variance) * expected;
   if(min_f > 0 ) { min = min_f; }

   uint64_t max = std::numeric_limits<uint64_t>::max();
   float max_f = delta * (1 + variance) * expected;
   if(max_f < max ) { max = max_f; }
   return (max >= val) && (min <= val);
 };

 ASSERT_EQ(true,between(func2(retry),2));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),4));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),8));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),16));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),32));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),64));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),128));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),256));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),512));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),1024));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),2048));
 ASSERT_EQ(true,retry);
 ASSERT_EQ(true,between(func2(retry),std::numeric_limits<uint64_t>::max()));
 ASSERT_EQ(false,retry);
 ASSERT_EQ(true,between(func2(retry),std::numeric_limits<uint64_t>::max()));
 ASSERT_EQ(false,retry);
}
