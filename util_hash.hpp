//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <util_base64.hpp>
#include <util_swiss.hpp>
#include <vector>
#include <limits>

namespace util {
namespace hash {
  namespace k {
    constexpr uint64_t SHA512_byte_size = SHA512_DIGEST_LENGTH;
    constexpr uint64_t SHA256_byte_size = SHA256_DIGEST_LENGTH;
  }

inline bool SHA256(const bytes::View input, const bytes::View salt, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  if(!resize(k::SHA256_byte_size,'\0',output)) { return false; }
  SHA256_CTX ctx;
  if(!SHA256_Init(&ctx)) { return false; }
  if(!SHA256_Update(&ctx, salt.data(), salt.size())) { return false;}
  const byte_t* input_iterator = input.data();
  bool ret = swiss::numeric_limit_split<int>([&](int size)->bool{
    if(!SHA256_Update(&ctx, input_iterator,size)) { return false; }
    input_iterator += size;
    return true;
  },input.size());
  if(!ret) { return false; }
  if(!SHA256_Final(output.data(), &ctx)) { return false; }
  return true;
}

inline bool SHA512(const bytes::View input, const bytes::View salt, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  if(!resize(k::SHA512_byte_size,'\0',output)) { return false; }
  SHA512_CTX ctx;
  if(!SHA512_Init(&ctx)) { return false; }
  if(!SHA512_Update(&ctx, salt.data(), salt.size())) { return false;}
  const byte_t* input_iterator = input.data();
  bool ret = swiss::numeric_limit_split<int>([&](int size)->bool{
    if(!SHA512_Update(&ctx, input_iterator,size)) { return false; }
    input_iterator += size;
    return true;
  },input.size());
  if(!ret) { return false; }
  if(!SHA512_Final(output.data(), &ctx)) { return false; }
  return true;
}

inline bool HMAC256(const bytes::View input, const bytes::View key, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  if(!resize(k::SHA256_byte_size,'\0',output)) { return false; }
  HMAC_CTX ctx;
  if(!HMAC_Init(&ctx,key.data(),key.size(),EVP_sha256())) { return false; }
  const byte_t* input_iterator = input.data();
  bool ret = swiss::numeric_limit_split<int>([&](int size)->bool{
    if(!HMAC_Update(&ctx, input_iterator,size)) { return false; }
    input_iterator += size;
    return true;
  },input.size());
  if(!ret) { return false; }

  if(!HMAC_Final(&ctx,output.data(),NULL)) { return false; }
  return true;
}

inline bool HMAC512(const bytes::View input, const bytes::View key, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  if(!resize(k::SHA512_byte_size,'\0',output)) { return false; }
  HMAC_CTX ctx;
  if(!HMAC_Init(&ctx,key.data(),key.size(),EVP_sha512())) { return false; }
  const byte_t* input_iterator = input.data();
  bool ret = swiss::numeric_limit_split<int>([&](int size)->bool{
    if(!HMAC_Update(&ctx, input_iterator,size)) { return false; }
    input_iterator += size;
    return true;
  },input.size());
  if(!ret) { return false; }

  if(!HMAC_Final(&ctx,output.data(),NULL)) { return false; }
  return true;
}

}
}
