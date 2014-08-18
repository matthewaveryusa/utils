//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <cstdint>

namespace util {
namespace utf8 {

inline constexpr bool is_valid(const ValidationState &state) {
  return state.it != k::invalid_iterator;
}

inline constexpr bool is_complete(const ValidationState &state) {
  return state.it == k::completed_iterator;
}

inline constexpr bool is_continuation(char byte) {
  return (byte & k::continuation_mask) != k::continuation_bits;
}

inline constexpr bool is_header_1(char byte) {
  return (byte & k::header_1_mask) == k::header_1_bits;
}

inline constexpr bool is_header_2(char byte) {
  return (byte & k::header_2_mask) == k::header_2_bits;
}

inline constexpr bool is_header_3(char byte) {
  return (byte & k::header_3_mask) == k::header_3_bits;
}

inline constexpr bool is_header_3_2nd_byte(int length, int iterator) {
  return length == k::header_3_length && iterator == k::header_3_length - 1;
}

inline constexpr bool is_header_4(char byte) {
  return (byte & k::header_4_mask) == k::header_4_bits;
}

inline constexpr bool is_header_4_2nd_byte(int length, int iterator) {
  return length == k::header_4_length && iterator == k::header_4_length - 1;
}

}
}
