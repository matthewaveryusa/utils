//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <random>
#include <functional>
#include <cstdint>
#include <algorithm>

namespace util {
namespace retry {
namespace k {
constexpr float delta_variance = 0.2;
}
//no boundry checking on backoff increment
std::function<uint64_t(bool &)> exponential_backoff(int num_retries,
                                                    uint64_t min_backoff, uint64_t max_backoff, uint64_t delta,
                                                    float delta_variance = k::delta_variance) {
  thread_local std::random_device rd;
  thread_local std::mt19937 gen(rd());
  return [delta,delta_variance,min_backoff,max_backoff,num_retries, num_retries_count = -1](bool & retry) mutable -> uint64_t {
    ++num_retries_count;
    retry = (num_retries_count <= num_retries);
    if (!retry) { return max_backoff; }
    uint64_t min = 0ul;
    float min_f = delta * (1 - delta_variance) * (2<<num_retries_count);
    if(min_f > 0 ) { min = min_f; }
    uint64_t max = std::numeric_limits<uint64_t>::max();
    float max_f = delta * (1 + delta_variance) * (2<<num_retries_count);
    if(max_f < max ) { max = max_f; }
    return std::min(min_backoff + std::uniform_int_distribution<uint64_t>(min, max)(gen),max_backoff);
  };
}

}
}
