#pragma once

#include <future>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <algorithm>
#include <map>

namespace util {

template<class K, class V>
class AsyncPromises {
  public:
  std::future<V> add_promise(K key);
  void satisfy_promise(K key, V value);
  void break_promise(K key);
 
  private:
  std::map<K,std::promise<V>> promises_;
};

template<class K, class V>
inline std::future<V> AsyncPromises<K,V>::add_promise(K key) {
  auto ret = promises_.emplace(key,std::promise<V>());
  if(!ret.second) { return std::future<V>(); }
  return ret.first->second.get_future();
}

template<class K, class V>
inline void AsyncPromises<K,V>::satisfy_promise(K key, V value) {
  auto ret = promises_.find(key);
  if(ret == promises_.end()) { return; }
  ret->second.set_value(std::move(value));
  promises_.erase(ret);
}

template<class K, class V>
inline void AsyncPromises<K,V>::break_promise(K key) {
  promises_.erase(key);
}

template<class K, class C>
class Expirations {
public:
  Expirations(C&& callback);

  template<class T>
  void add(T timeout, K key);
  void remove(K key);

  void engine_loop();
  void stop_engine();

  ~Expirations();
  

private:
  C callback_;
  std::multimap<std::chrono::steady_clock::time_point,K> timeouts_;
  std::map<K,typename decltype(timeouts_)::iterator> keys_;
  std::thread expiration_engine_;
  std::condition_variable cv_;
  std::mutex mutex_;
  std::atomic<int> run_;
};

template<class K, class C>
Expirations<K, C>::Expirations(C&& callback): callback_(callback),
  run_(1),
  expiration_engine_(&Expirations<K,C>::engine_loop,this) {
}

template<class K, class C>
template<class T>
void Expirations<K, C>::add(T timeout, K key) {
  auto now = std::chrono::steady_clock::now();
  if(timeout == T::zero()) {
    callback_(now,now,key);
  } else {
    {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = timeouts_.emplace(timeout+now,key);
    keys_.emplace(key,it);
    }
    cv_.notify_one();
  }
}

template<class K, class C>
void Expirations<K, C>::remove(K key) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto key_it = keys_.find(key);
    if(key_it == keys_.end()) {
      return;
    } else {
      timeouts_.erase(key_it->second);
      keys_.erase(key_it);
    }
}

template<class K, class C>
void Expirations< K, C>::engine_loop() {
  while(run_ == 1) {
    std::unique_lock<std::mutex> lock(mutex_);
    std::cout << "wait1\n";
    cv_.wait(lock, [this]{return timeouts_.size() || run_ == 0;});
    while( timeouts_.size() != 0  && run_ == 1) {
      auto now = std::chrono::steady_clock::now();
      auto time = timeouts_.begin()->first;
      if(time <= now) {
        callback_(now,time,timeouts_.begin()->second);
        keys_.erase(timeouts_.begin()->second);
        timeouts_.erase(timeouts_.begin());
      } else {
        std::cout << "wait2\n";
        cv_.wait_for(lock, time-now, [this,&time]{
            return (timeouts_.size() && timeouts_.begin()->first < time)  || run_ == 0;});
      }
    }
  }

}

template<class K, class C>
void Expirations< K, C>::stop_engine() {
  run_ = 0;
  cv_.notify_one(); 
  if(expiration_engine_.joinable()) {
    expiration_engine_.join();
  }
}


template<class K, class C>
Expirations< K, C>::~Expirations() {
  stop_engine();
  auto now = std::chrono::steady_clock::now();
  //no mutex is required since stop_engine kills the thread there
  //is possible contention with
  std::for_each(timeouts_.begin(),timeouts_.end(),[&](const auto& val){callback_(now,val.first,val.second);});
}

}
