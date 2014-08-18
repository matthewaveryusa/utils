//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <vector>
#include <cstdint>
#include <limits>
#include <random>
#include <openssl/rand.h>
#include <util_bytes.hpp>
#include <util_swiss.hpp>

#include <stdexcept>

namespace util {
namespace random {

inline byte_t byte_pseudo() {
  thread_local std::random_device rd;
  thread_local std::mt19937 gen(rd());
  thread_local std::uniform_int_distribution<> dis(0, std::numeric_limits<byte_t>::max());
  return dis(gen);
}

inline bool bytes_pseudo(bytes::View data) {
  std::for_each(data.begin(),data.end(),[](byte_t& byte){ byte = byte_pseudo(); });
  return true;
}


inline bool bytes_safe(bytes::View data) {
  byte_t* output_iterator = data.data();
  return swiss::numeric_limit_split<int>([&](size_t size) {
      if (RAND_bytes(output_iterator, size) != 1) { return false; }
      output_iterator += size; return true; }
    , data.size());
}

inline bool bytes_safe_attempt(bytes::View data) {
  if(bytes_safe(data) == false) {
    return bytes_pseudo(data);
  } 
  return true;
}

}
}
