//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <algorithm>
#include <chrono>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
#include <util_base64.hpp>
#include <iostream>
#include <iomanip>

// from http://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
namespace {
inline std::string escape_json_string(const char * input) {
    std::ostringstream ss;
    while (*input != '\0') {
        switch (*input) {
            case '\\': ss << "\\\\"; break;
            case '"': ss << "\\\""; break;
            case '/': ss << "\\/"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << *input; break;
        }
        ++input;
    }
    return ss.str();
}
}

static std::mutex logging_mutex;

namespace util {
namespace logging {
  inline std::string make_log(std::string severity, std::string file, std::string function,  int line, const std::string& data) {
    static uint64_t increment = 0;
    std::ostringstream log_uri_oss;
    auto pos = std::find(file.rbegin(),file.rend(),'/').base();
    if(pos == file.end()) { 
      pos = file.begin(); 
    }
    file = std::string(pos,file.end());
    uint64_t log_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::ostringstream fixed;
    fixed << log_time%1000000 << "." << increment++ << " @ " << std::this_thread::get_id() << "/" << severity << "/" << file << "/" << function << "/" << line << "\"";

    std::ostringstream oss;
    oss << "["
      << std::left << std::setw(70) << std::setfill(' ') << fixed.str() << ","
      << "\"" << data << "\""
      << "]\n";

    return oss.str();
  }
}
}

#ifdef LOG_LEVEL_TRACE
#define LOG_TRACE(_x) {\
  std::ostringstream _log_format_oss;\
  _log_format_oss << _x;\
  std::lock_guard<std::mutex> lock(logging_mutex);\
  std::cout << util::logging::make_log("TRACE",__FILE__,__func__,__LINE__,escape_json_string(_log_format_oss.str().c_str()));}
#define LOG_TRACE_RUN(x) x;
#define LOG_LEVEL_DEBUG
#else
#define LOG_TRACE(x) ;
#define LOG_TRACE_RUN(x) ;
#endif

#ifdef LOG_LEVEL_DEBUG
#define LOG_DEBUG(_x) {\
  std::ostringstream _log_format_oss;\
  _log_format_oss << _x;\
  std::lock_guard<std::mutex> lock(logging_mutex);\
  std::cout << util::logging::make_log("DEBUG",__FILE__,__func__,__LINE__,escape_json_string(_log_format_oss.str().c_str()));}
#define LOG_DEBUG_RUN(x) x;
#define LOG_LEVEL_INFO
#else
#define LOG_DEBUG(x) ;
#define LOG_DEBUG_RUN(x) ;
#endif

#ifdef LOG_LEVEL_INFO
#define LOG_INFO(_x) {\
  std::ostringstream _log_format_oss;\
  _log_format_oss << _x;\
  std::lock_guard<std::mutex> lock(logging_mutex);\
  std::cout << util::logging::make_log("INFO",__FILE__,__func__,__LINE__,escape_json_string(_log_format_oss.str().c_str()));}
#define LOG_INFO_RUN(x) x;
#define LOG_LEVEL_WARNING
#else
#define LOG_INFO(x) ;
#define LOG_INFO_RUN(x) x;
#endif

#ifdef LOG_LEVEL_WARNING
#define LOG_WARNING(_x) {\
  std::ostringstream _log_format_oss;\
  _log_format_oss << _x;\
  std::lock_guard<std::mutex> lock(logging_mutex);\
  std::cout << util::logging::make_log("WARNING",__FILE__,__func__,__LINE__,escape_json_string(_log_format_oss.str().c_str()));}
#define LOG_WARNING_RUN(x) x;
#define LOG_LEVEL_ERROR
#else
#define LOG_WARNING(x) ;
#define LOG_WARNING_RUN(x) ;
#endif

#ifdef LOG_LEVEL_ERROR
#define LOG_ERROR(_x) {\
  std::ostringstream _log_format_oss;\
  _log_format_oss << _x;\
  std::lock_guard<std::mutex> lock(logging_mutex);\
  std::cout << util::logging::make_log("ERROR",__FILE__,__func__,__LINE__,escape_json_string(_log_format_oss.str().c_str()));}
#define LOG_ERROR_RUN(x) x;
#define LOG_LEVEL_FATAL
#else
#define LOG_ERROR(x) ;
#define LOG_ERROR_RUN(x) ;
#endif

#ifdef LOG_LEVEL_FATAL
#define LOG_FATAL(_x) {\
  std::ostringstream _log_format_oss;\
  _log_format_oss << _x;\
  std::lock_guard<std::mutex> lock(logging_mutex);\
  std::cout << util::logging::make_log("FATAL",__FILE__,__func__,__LINE__,escape_json_string(_log_format_oss.str().c_str()));}
#define LOG_FATAL_RUN(x) x;
#else
#define LOG_FATAL(x);
#define LOG_FATAL_RUN(x) ;
#endif
