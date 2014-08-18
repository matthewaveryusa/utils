//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
//various random _small_ functions

#include <string>
#include <sstream>
#include <ios>
#include <cstdio>
#include <iterator>
#include <limits>
#include <functional>

namespace util {
namespace swiss {

template<class P, class T>
const std::function<void(P *)> make_delete(T&& encapsulate) {
  class WrappedDeleter {
  public:
    WrappedDeleter(T  &&other): o_(std::move(other)) {
    }
  private:
    T o_;
  };

  WrappedDeleter *d = new WrappedDeleter(std::move(encapsulate));
  return [d](P * ptr) { delete d; };
}

template<class Function>
void for_n(size_t n, Function f)
{
  while(n-- != 0) { f(); }
}


template<class SIZE, class UPDATE>
bool numeric_limit_split(UPDATE &&update, size_t size){
  while(size > std::numeric_limits<SIZE>::max()) {
    if(!update(std::numeric_limits<SIZE>::max())) { return false; }
      size -= std::numeric_limits<SIZE>::max();
  }
  return update(static_cast<SIZE>(size));
}

}
}
