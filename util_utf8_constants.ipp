//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <util_bytes.hpp>
#include <cstdint>

namespace util {
namespace utf8 {
namespace k {
constexpr char continuation_bits  = bytes::k::bit_8;
constexpr char continuation_mask  = bytes::k::top_2_bits;
         
constexpr int  invalid_iterator   = -1;
constexpr int  completed_iterator = 0;
         
constexpr char header_1_bits  = bytes::k::none;
constexpr char header_1_mask  = bytes::k::bit_8;
constexpr int     header_1_length = 1;
constexpr int     header_1_begin_iterator = completed_iterator;
         
         
constexpr char header_2_bits  = bytes::k::top_2_bits;
constexpr char header_2_mask  = bytes::k::top_3_bits;
constexpr char header_2_overly_long_min = header_2_bits | bytes::k::bit_1;
constexpr int     header_2_length = 2;
constexpr int     header_2_begin_iterator = 1;
         
constexpr char header_3_bits  = bytes::k::top_3_bits;
constexpr char header_3_mask  = bytes::k::top_4_bits;
constexpr char header_3_illegal_sequence_part1 = header_3_bits | 0x0D;
constexpr char header_3_illegal_sequence_part2 = continuation_bits | bytes::k::bit_6;
constexpr char header_3_overly_long_min_part1 = header_3_bits;
constexpr char header_3_overly_long_min_part2 = continuation_bits | bytes::k::bottom_5_bits;
constexpr int     header_3_length = 3;
constexpr int     header_3_begin_iterator = 2;
         
constexpr char header_4_bits  = bytes::k::top_4_bits;
constexpr char header_4_mask  = bytes::k::top_5_bits;
constexpr char header_4_max_part1  = header_4_bits | bytes::k::bit_3 | bytes::k::bit_1;
constexpr char header_4_max_part2  = header_4_bits | bytes::k::bit_3;
constexpr char header_4_max_part3  = continuation_bits | bytes::k::bit_5;
constexpr char header_4_overly_long_min_part1 = header_4_bits;
constexpr char header_4_overly_long_min_part2 = continuation_bits | bytes::k::bottom_4_bits;
constexpr int     header_4_length = 4;
constexpr int     header_4_begin_iterator = 3;
}

}
}
