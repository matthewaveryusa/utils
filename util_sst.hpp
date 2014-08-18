//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
//SST = SIZE,SPEED,TIME
//constants and conversions that pertain to the above

#include <cstdint>
#include <chrono>
#include <ctime>
#include <chrono>

namespace util {
namespace sst {
namespace k {
constexpr char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
constexpr char digits_size = sizeof(digits);
constexpr int kilo = 1024;
constexpr int mega = 1024 * 1024;
constexpr uint64_t giga = 1024 * 1024 * 1024;
constexpr int bit = 1;
constexpr int byte = 8;
constexpr int kiloBit = 1024;
constexpr int megaBit = 1024 * 1024;
constexpr int gigaBit = 1024 * 1024 * 1024;
constexpr int kiloByte = byte * 1024;
constexpr int megaByte = byte * 1024 * 1024;
constexpr uint64_t gigaByte = 8ull * 1024ull * 1024ull * 1024ull;
constexpr int kb = kiloBit;
constexpr int Mb = megaBit;
constexpr int Gb = gigaBit;
constexpr int KB = kiloByte;
constexpr int MB = megaByte;
constexpr uint64_t GB = gigaByte;

namespace s {
constexpr float micro = 1 / 1000000;
constexpr float millisec = 1 / 1000;
constexpr int second = 1;
constexpr int minute = 60;
constexpr int hour = 60 * minute;
constexpr int day = 24 * hour;
}

namespace ms {
constexpr float micro = 1 / 1000;
constexpr int millisec = 1;
constexpr int second = 1000;
constexpr int minute = 60 * second;
constexpr uint64_t hour = 60ull * minute *second;
constexpr uint64_t day = 24ull * hour *second;
}

namespace micro {
constexpr int micro = 1;
constexpr int millisec = 1000;
constexpr int second = 1000000;
constexpr int minute = 60 * second;
constexpr uint64_t hour = 60ull * minute *second;
//overflow
//const uint64_t day = 24ull * hour * second;
}

namespace m {
constexpr float millisec = 1 / 6000;
constexpr float second = 1 / 60;
constexpr int minute = 1;
constexpr int hour = 60 * minute ;
constexpr int day = 24 * hour;
}

namespace h {
constexpr float millisec = 1 / 36000;
constexpr float second = 1 / 360;
constexpr float minute = 1 / 60;
constexpr int hour = 1;
constexpr int day = 24 * hour;
}

namespace d {
constexpr float millisecond = 1 / (36000 * 24);
constexpr float second = 1 / (360 * 24);
constexpr float minute = 1 / (60 * 24);
constexpr float hour = 1 / 24;
constexpr int day = 1;
constexpr int week = 7;
}
}

inline size_t string2bytes(const std::string &s) {
  size_t split = s.find_last_of(k::digits, k::digits_size) + 1;
  size_t string_left = s.size() - split;

  if (split == std::string::npos || string_left < 1) {
    throw std::runtime_error("Error parsing invalid size");
  }

  size_t size = strtoull(s.c_str(), nullptr, 10);


  const char *last_char = &(s[split]);
  if (*(last_char) == 'b') {
    return (size + 7) / k::byte;
  }

  switch (*(last_char)) {
  case 'k':
  case 'K':
    size *= k::kilo;
    break;
  case 'm':
  case 'M':
    size *= k::mega;
    break;
  case 'g':
  case 'G':
    size *= k::giga;
    break;
  }
  
  if (string_left > 1 && *(last_char+1) == 'b') {
    size = (size + 7) / k::byte;
  }
  return size;
}

inline size_t string2kBps(const std::string &s) {
  size_t rate = string2bytes(s);
  return rate / k::kilo;

}

template<class T>
inline T string2duration(const std::string &s) {

  using std::chrono::microseconds;
  using std::chrono::milliseconds;
  using std::chrono::seconds;
  using std::chrono::minutes;
  using std::chrono::hours;

  static const int two_character_unit_offset = 1;
  if (s.empty()) {
    throw std::runtime_error("Error parsing invalid duration");
  }

  int64_t time = strtoll(s.c_str(),nullptr,10);

  const char *last_char = &s.back();
  const char *second_last_char = last_char - two_character_unit_offset;

  if (std::isdigit(*second_last_char)) {
    return seconds(static_cast<long>(time));
  }

  switch (*last_char) {
  case 'h':
    return std::chrono::duration_cast<T>(hours(static_cast<long>(time)));
  case 'm':
    return std::chrono::duration_cast<T>(minutes(static_cast<long>(time)));
  case 's':
    switch (*second_last_char) {
    case 'm':
      return std::chrono::duration_cast<T>(milliseconds(time));
    case 'u':
      return std::chrono::duration_cast<T>(microseconds(time));
    default:
      return std::chrono::duration_cast<T>(seconds(static_cast<long>(time)));
    }
  default:
    throw std::runtime_error("unable to convert in string2duration");
  }
}

//the joys of platforms
inline void gmtime(const time_t* time, struct tm* a_tm){
#ifdef _WIN32
	gmtime_s(a_tm, time);
#else
	gmtime_r(time, a_tm);
#endif
}

inline std::string to_timestamp(const std::time_t &time) {
 const auto format = "%F %T";
 const auto max_size = sizeof("9999-12-31 23:59:59");
 char buffer[max_size];
 struct tm a_tm;
 gmtime(&time,&a_tm);
 std::strftime(buffer, max_size, format, &a_tm);
 return buffer;
}


//sugar
inline std::string to_timestamp(const std::chrono::time_point<std::chrono::system_clock> &time) {
  return to_timestamp(std::chrono::system_clock::to_time_t(time));
}



}
}
