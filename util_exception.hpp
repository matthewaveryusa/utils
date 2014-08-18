//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <exception>

namespace util {
namespace exception {

template <class T>
T* throw_null(T* ptr) {
  if(ptr == nullptr) { throw std::runtime_error("Null pointer"); }
  else { return ptr; }
}

}
}
