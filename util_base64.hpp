//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <string>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <util_base64_assertions.ipp>
#include <util_bytes.hpp>
#include <type_traits>

namespace util {
namespace base64 {

constexpr size_t encode_size(size_t size) {
 return 4*( (size+2) / 3);
}

inline size_t decode_size(const bytes::ConstView &input) {
 size_t ret = (input.size() +1) * 6 / 8;
 if(input.size() > 0 && *(input.rbegin()) == '=') { --ret; }
 if(input.size() > 1 && *(input.rbegin()+1) == '=') { --ret; }
 return ret;
}

inline byte_t offset_to_char(const byte_t b64) {
    if(b64 < 26) { return 'A' + b64; }
    else if (b64 < 52) { return 'a' + b64 - 26; }
    else if (b64 < 62) { return '0' + b64 - 52; }
    else if (b64 == 62) { return '+'; } 
    else { return '/'; }
}

inline byte_t char_to_6_bits(const byte_t b64) {
    if(isupper(b64)) { return b64 - 'A'; } 
    else if(islower(b64)) { return b64 - 'a' + 26; }
    else if(isdigit(b64)) { return b64 - '0' + 52; }
    else if(b64 == '+') { return 62; }
    else if(b64 == '/') { return 63; }
    else { return 0;}
}

inline size_t encode(const bytes::ConstView input, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  if(input.size() == 0) { return 0; }
  if(!resize(encode_size(input.size()),'=',output)) { return encode_size(input.size()); };
  std::fill(output.rbegin(),output.rbegin()+2,'=');
  byte_t excess_bits = 0;
  size_t excess_offset = 0;

  auto output_byte = output.begin();
  for(auto const input_byte : input) {
    if(excess_offset == 0) {
      *output_byte = offset_to_char( (input_byte & bytes::k::top_6_bits) >> 2);
      excess_bits = (input_byte & bytes::k::bottom_2_bits) << 4;
    } else if(excess_offset == 1) {
      *output_byte = offset_to_char(excess_bits | ((input_byte & bytes::k::top_4_bits) >> 4));
      excess_bits = (input_byte & bytes::k::bottom_4_bits) << 2;
    } else if(excess_offset == 2) {
      *output_byte = offset_to_char(excess_bits | ((input_byte & bytes::k::top_2_bits) >> 6));
      *(++output_byte) = offset_to_char(input_byte & bytes::k::bottom_6_bits);
      excess_bits = 0;
    }
    excess_offset = (excess_offset + 1) % 3;
    ++output_byte;
  }
  if ( excess_offset != 0 ) { *output_byte = offset_to_char(excess_bits); }
  return 0;
}

inline size_t decode(const bytes::ConstView input, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  if(input.size() == 0) { return 0; }
  if(!resize(decode_size(input),'\0',output)) { return decode_size(input); }
  
  char excess_bits = 0;
  size_t excess_offset = 0;

  auto output_byte = output.begin();
  for(const auto input_byte : input) {
    if(input_byte == '=') {
      *output_byte++ = excess_bits; 
      return 0;
    }
    byte_t bits = char_to_6_bits(input_byte);
    if(excess_offset == 0) {
      excess_bits = bits << 2;
    } else if(excess_offset == 1) {
      //really 2 bits from 2 bits of the top bit, but since the top
      //2 bits will always be 0, binary-anding the top 4 is fine
      *output_byte++ = excess_bits | ((bits & bytes::k::top_4_bits) >> 4);
      excess_bits = (bits & bytes::k::bottom_4_bits) << 4;
    } else if(excess_offset == 2) {
      *output_byte++ = excess_bits | (bits >> 2);
      excess_bits = (bits & bytes::k::bottom_2_bits) << 6;
    } else if(excess_offset == 3) {
      *output_byte++ = excess_bits | bits;
      excess_bits = 0;
    }
    excess_offset = (excess_offset + 1) % 4;
  }
  return 0;
}

}
}
