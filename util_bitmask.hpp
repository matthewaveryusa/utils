//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <type_traits>

namespace util {
namespace bitmask {

  //http://stackoverflow.com/questions/14589417/can-an-enum-class-be-converted-to-the-underlying-type 
  template<typename E>
  constexpr typename std::underlying_type<E>::type  enum_type(E e)
  {
     return static_cast<typename std::underlying_type<E>::type>(e);
  }
 

  template<class T, class U>
  bool is_set(T the_bit,U the_bitmask) {
    return ((enum_type(the_bit) & enum_type(the_bitmask)) == enum_type(the_bitmask));
  }
  
  template<class T>
  void set(T &byte,T bit, bool set) {
    if (set) {
      byte =  T(enum_type(byte) | enum_type(bit));
    } else {
      byte = T(enum_type(byte) & ~enum_type(bit));
    }
    return;
  }

}
}
