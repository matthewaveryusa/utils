//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <cstdint>

#include <util_utf8_constants.ipp>

namespace util {
namespace utf8 {

struct ValidationState {
  uint8_t length;
  int8_t  it;
  char previous_byte;
  ValidationState():length(0),it(0),previous_byte(0){}
};

}
}

#include <util_utf8_getters.ipp>

namespace util {
namespace utf8 {

inline bool set_length(const char byte, ValidationState& state) {
  if (is_header_1(byte)) {
    state.length = k::header_1_length;
    state.it = k::header_1_begin_iterator;
    return true;
  }

  if (is_header_2(byte)) {
    if (byte <= k::header_2_overly_long_min) {
      state.it = k::invalid_iterator;
      return false;
    }
    state.length = k::header_2_length;
    state.it = k::header_2_begin_iterator;
    return true;
  }

  if (is_header_3(byte)) {
    state.length = k::header_3_length;
    state.it = k::header_3_begin_iterator;
    state.previous_byte = byte;
    return true;
  }

  if (is_header_4(byte)) {
    if (byte >= k::header_4_max_part1) {
      state.it = k::invalid_iterator;
      return false;
    }
    state.length = k::header_4_length;
    state.it = k::header_4_begin_iterator;
    state.previous_byte = byte;
    return true;
  }
  //could test to see if it's a continuation byte
  //but it's invalid in this context anways
  state.it = k::invalid_iterator;
  return false;
}

inline bool validate(const char byte, ValidationState& state) {
  if (state.it == k::completed_iterator) {
    return set_length(byte,state);
  }

  if (is_continuation(byte)) {
    state.it = k::invalid_iterator;
    return false;
  }

  if (is_header_4_2nd_byte(state.length, state.it) && (
        (state.previous_byte == k::header_4_overly_long_min_part1
         && byte <= k::header_4_overly_long_min_part2)
        || (state.previous_byte == k::header_4_max_part2
            && byte >= k::header_4_max_part3))) {
    state.it = k::invalid_iterator;
    return false;
  }

  if (is_header_3_2nd_byte(state.length, state.it) && (
        (state.previous_byte == k::header_3_illegal_sequence_part1
         && byte >= k::header_3_illegal_sequence_part2)
        || (state.previous_byte == k::header_3_overly_long_min_part1
            && byte <= k::header_3_overly_long_min_part2))) {
    state.it = k::invalid_iterator;
    return false;
  }

  --state.it;
  return true;
}


}
}
