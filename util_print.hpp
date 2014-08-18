//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ostream>
#include <util_bytes.hpp>

namespace util {
namespace print {
    
inline std::ostream &hex(std::ostream &os, const byte_t *byte) {
  os << std::setw(2) << std::setfill('0') << std::hex
    << static_cast<unsigned int>(*reinterpret_cast<const char*>(byte)) << std::dec;
  return os;
}

inline std::ostream &hex_pretty(std::ostream &os, const byte_t *byte) {
  os << "0x" << std::setw(2) << std::setfill('0') << std::hex
    << static_cast<unsigned int>(*reinterpret_cast<const char*>(byte)) << std::dec;
  return os;
}

inline std::ostream &ascii(std::ostream &os, const byte_t *byte) {
  os << *reinterpret_cast<const char *>(byte);
  return os;
}


inline std::ostream &hex(std::ostream &os, const byte_t *begin, uint64_t size) {
  size_t i = 0;
  while(i < size) {
    hex(os,begin+i);
    while(++i < size) {
      os << " ";
      hex(os,begin+i);
    }
  }
  return os;
}

inline std::ostream &ascii_hex(std::ostream &os, const byte_t *begin, uint64_t size) {
  size_t i = 0;
  auto print = [&os,byte = begin](size_t &i, size_t size) -> void {
    if(isprint(byte[i])) {
      while(i < size && isprint(byte[i])) {
        ascii(os,byte+i);
        while(++i < size && isprint(byte[i])) {
          ascii(os,byte+i);
        }
      }
    } else {
      while(i < size && !isprint(byte[i])) {
        hex_pretty(os,byte+i);
        while(++i < size && !isprint(byte[i])) {
          os << " ";
          hex_pretty(os,byte+i);
        }
      }
    }
  };
  
  while(i < size) {
    print(i,size);
    while(i < size) {
      os << " ";
      print(i,size);
    }
  }
  return os;
}

inline std::ostream &ascii_hex_clump(std::ostream &os, const byte_t *begin,
                                     uint64_t size) {
  
  size_t i = 0;
  auto print = [&os,byte = begin](size_t &i, size_t size) -> void {
    size_t count = 0;
    char previous = byte[i] + 1;
    if(isprint(byte[i])) {
      while(i < size && isprint(byte[i])) {
        ascii(os,byte+i);
        previous = byte[i];
        while(++i < size && isprint(byte[i])) {
          if (byte[i] == previous) {
            ++count;
            continue;
          }
          previous = byte[i];
          if (count != 0) {
            os << "(x" << std::dec << count+1 << ")";
            count = 0;
          }
          ascii(os,byte+i);
        }
      }
    } else {
      while(i < size && !isprint(byte[i])) {
        hex_pretty(os,byte+i);
        previous = byte[i];
        while(++i < size && !isprint(byte[i])) {
          if (byte[i] == previous) {
            ++count;
            continue;
          }
          previous = byte[i];
          if (count != 0) {
            os << "(x" << std::dec << count+1 << ")";
            count = 0;
          }
          os << " ";
          hex_pretty(os,byte+i);
        }
      }
    }
    if (count != 0) {
      os << "(x" << std::dec << count+1 << ")";
      count = 0;
    }
  };
  
  if (size <= 0) {
    return os;
  }
  
  while(i < size) {
    print(i,size);
    while(i < size) {
      os << " ";
      print(i,size);
    }
  }
  os << std::dec;
  return os;
}

template<class T>
constexpr int max_bytes_required() {
  //+1 for the sign, +1 for the extra digit
  //since digits10 will return 9 for an int
  //because it can't represent the full range
  //of 10 digit numbers, even though some of 
  //the numbers are 10 digits long (like 2^30)
  //
  //if the number is a float or double, we calculate
  //the number of bytes differently (stolen from std::string<T>::to_string)
  return std::numeric_limits<T>::is_integer ? std::numeric_limits<T>::digits10 + 2 :     
    std::numeric_limits<T>::max_exponent10 + 20;
}

template<class T>  constexpr const char * format(){ 
  typedef typename std::remove_extent<T>::type no_extent_t;
  typedef typename std::remove_cv<no_extent_t>::type no_cv_extent_t;
  static_assert(std::is_array<T>::value && std::is_same<char,no_cv_extent_t>::value, "Invalid print format"); return "%s"; }
template<> constexpr const char *format<char[]>() { return "%s"; }
template<> constexpr const char *format<char *>() { return "%s"; }
template<> constexpr const char *format<const char *>() { return "%s"; }
template<> constexpr const char *format<int>() { return "%d"; }
template<> constexpr const char *format<unsigned int>() { return "%u"; }
template<> constexpr const char *format<long>() { return "%ld"; }
template<> constexpr const char *format<unsigned long>() { return "%lu"; }
template<> constexpr const char *format<long long>() { return "%lld"; }
template<> constexpr const char *format<unsigned long long>() { return "%llu"; }
template<> constexpr const char *format<float>() { return "%.16g"; }
template<> constexpr const char *format<double>() { return "%.16g"; }
template<> constexpr const char *format<char>() { return "%c"; }
template<> constexpr const char *format<unsigned char>() { return "%d"; }
template<> constexpr const char *format<std::vector<char>>() { return "%d"; }


template<class T, class U, class S>
inline bool number_to_buffer(T * begin, size_t size, U val, S &num_bytes) {
    auto it = begin;
    bool is_negative = (val < 0);
    if(is_negative) { val = -val; }
    num_bytes = 0;
    T *end = begin+size;
    do{
        if(it != end) { *it = "0123456789"[val % 10]; ++it; }
        ++num_bytes;
    } while((val /= 10) > 0);
    
    if (is_negative){
      ++num_bytes;
      if(it != end) {
        *it = '-'; 
        ++it;
      }
    }

    if(num_bytes <= static_cast<size_t>(std::distance(begin,it))) {
      std::reverse(begin,it);
      return true;
    } else {
      return false;
    }
}

//returns whether to_buffer succeeded or not. 
//if it did not succeed, num_bytes contains the minimum number of bytes needed for
//a successful write.
//If it did succeed, it contains the number of bytes that was written 
//if negative, the number is not parsable

template<class T, class U, class S>
inline bool to_buffer(T * begin, size_t size, const U &val, S &num_bytes) {
  typedef typename std::remove_cv<U>::type no_cv_t;
  num_bytes = snprintf(reinterpret_cast<char *>(begin), size, print::format<no_cv_t>(), val);
  if(static_cast<size_t>(num_bytes) < size) {
    return true;
  } else {
    //add the extra byte needed for the null terminator
    ++num_bytes;
    return false;
  }
}


template<class T, class U, class S>
inline bool to_buffer(T &buffer, const U &val, S &num_bytes) {
  return to_buffer(buffer.data(),buffer.size(),val,num_bytes);
}

template<class T, class S>
inline bool to_buffer(T * begin, size_t size, const std::nullptr_t &nptr, S &num_bytes) {
  num_bytes = 4;
  if(size >= 4) {
    begin[0] = 'n'; begin[1] = 'u'; begin[2] = 'l'; begin[3] = 'l';
    return true;
  }
  return false;
}

template<class T, class S>
inline bool to_buffer(T * begin, size_t size, const bool &boolean, S &num_bytes) {
  num_bytes = boolean ? 4 : 5;
  if(static_cast<size_t>(num_bytes) < size) {
    if(boolean) {
      begin[0] = 't'; begin[1] = 'r'; begin[2] = 'u'; begin[3] = 'e';
    } else {
      begin[0] = 'f'; begin[1] = 'a'; begin[2] = 'l'; begin[3] = 's'; begin[4] = 'e';
    }
    return true;
  }
  return false;
}



}
}
