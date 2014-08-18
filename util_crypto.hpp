#pragma once
#include <iterator>
#include <limits>
#include <memory>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <ostream>
#include <util_base64.hpp>
#include <util_swiss.hpp>
#include <vector>

namespace util {
namespace crypto {
namespace rsa {
  namespace k {
    constexpr uint64_t f4 = 65537;
  }
struct KeyPair {
  bytes_t pub;
  bytes_t priv;
  void clear() {
    pub.clear();
    priv.clear();
  }
};

inline bool key(int length, KeyPair &pair) {
  auto rsa = std::unique_ptr<RSA, void (*)(RSA*)>(RSA_new(),&RSA_free);
  auto big_num = std::unique_ptr<BIGNUM,void (*)(BIGNUM *)>(BN_new(),&BN_free);
  
  if (!rsa || !big_num || !big_num.get() || !BN_set_word(big_num.get(), k::f4)) { return false; }
  if (!RSA_generate_key_ex(rsa.get(), length, big_num.get(), NULL)) { return false; }


  {
  auto priv_bio = std::unique_ptr<BIO,void (*)(BIO *)>(BIO_new(BIO_s_mem()),&BIO_free_all);
  if(!priv_bio) { return false; };
  auto evp = std::unique_ptr<EVP_PKEY,void (*)(EVP_PKEY *)>(EVP_PKEY_new(),&EVP_PKEY_free);
  if(!EVP_PKEY_set1_RSA(evp.get(), rsa.get())) { return false; }
  if(!PEM_write_bio_PKCS8PrivateKey(priv_bio.get(), evp.get(), NULL, NULL, 0, NULL, NULL)) { return false; }
  pair.priv.resize(BIO_pending(priv_bio.get()),'\0');
  if( 0 > BIO_read(priv_bio.get(), pair.priv.data(), pair.priv.size())) { return false; }
  }
 
  {
  auto pub_bio = std::unique_ptr<BIO,void (*)(BIO *)>(BIO_new(BIO_s_mem()),&BIO_free_all);
  if(!pub_bio) { return false; }
  if(!PEM_write_bio_RSA_PUBKEY(pub_bio.get(), rsa.get())) { return false; }
  pair.pub.resize(BIO_pending(pub_bio.get()),'\0');
  if( 0 > BIO_read(pub_bio.get(), pair.pub.data(), pair.pub.size())) { return false; }
  }
  
  return true;
}

inline bool get_public(const bytes::ConstView private_pem, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  auto priv_bio = std::unique_ptr<BIO,void (*)(BIO *)>(BIO_new_mem_buf(const_cast<byte_t*>(private_pem.data()),private_pem.size()),&BIO_free_all);
  auto rsa = std::unique_ptr<RSA, void (*)(RSA*)>( PEM_read_bio_RSAPrivateKey(priv_bio.get(), NULL, NULL, NULL),&RSA_free);
  
  if(!rsa || !priv_bio) { return false; } 

  {
  auto pub_bio = std::unique_ptr<BIO,void (*)(BIO *)>(BIO_new(BIO_s_mem()),&BIO_free_all);
  if(!pub_bio) { return false; }
  if(!PEM_write_bio_RSA_PUBKEY(pub_bio.get(), rsa.get())) { return false; }
  if(!resize(BIO_pending(pub_bio.get()),'\0',output)) { return false; }
  if( 0 > BIO_read(pub_bio.get(), output.data(), output.size())) { return false; }
  }

  return true;
}


inline bool sign(const bytes::ConstView input, const bytes::ConstView private_pem, int NID_option, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  auto priv_bio = std::unique_ptr<BIO,void (*)(BIO *)>(BIO_new_mem_buf(const_cast<byte_t*>(private_pem.data()),private_pem.size()),&BIO_free_all);
  auto rsa = std::unique_ptr<RSA, void (*)(RSA*)>( PEM_read_bio_RSAPrivateKey(priv_bio.get(), NULL, NULL, NULL),&RSA_free);
  if(!rsa || !priv_bio) { return false; }

  if(!resize(RSA_size(rsa.get()),'\0',output)) { return false; }
  unsigned int output_size = output.size();
  if( 0 == RSA_sign(NID_option,input.data(),input.size(),output.data(), &output_size,rsa.get())){ return false; }
  return resize(output_size,'\0',output);
}

inline bool sign256(const bytes::View input, const bytes::View private_pem, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) { return sign(input,private_pem,NID_sha256,output,resize); }
inline bool sign512(const bytes::View input, const bytes::View private_pem, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) { return sign(input,private_pem,NID_sha512,output,resize); }

inline bool verify(const bytes::View input, const bytes::View signature, const bytes::View public_pem, int NID_option) {
  auto pub_bio = std::unique_ptr<BIO,void (*)(BIO *)>(BIO_new_mem_buf(const_cast<byte_t*>(public_pem.data()),public_pem.size()),&BIO_free_all);
  auto rsa = std::unique_ptr<RSA, void (*)(RSA*)>( PEM_read_bio_RSA_PUBKEY(pub_bio.get(), NULL, NULL, NULL),&RSA_free);
  if(!rsa || !pub_bio) { return false; }

  if( 0 == RSA_verify(NID_option,input.data(),input.size(),signature.data(), signature.size(),rsa.get())){ return false; }
  return true;
}

inline bool verify256(const bytes::View input, const bytes::View signature, const bytes::View public_pem) { return verify(input,signature,public_pem,NID_sha256); }
inline bool verify512(const bytes::View input, const bytes::View signature, const bytes::View public_pem) { return verify(input,signature,public_pem,NID_sha512); }

inline bool encrypt(const bytes::View input, const bytes::View public_pem, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  auto pub_bio = std::unique_ptr<BIO,void (*)(BIO *)>(BIO_new_mem_buf(const_cast<byte_t*>(public_pem.data()),public_pem.size()),&BIO_free_all);
  auto rsa = std::unique_ptr<RSA, void (*)(RSA*)>( PEM_read_bio_RSA_PUBKEY(pub_bio.get(), NULL, NULL, NULL),&RSA_free);
  if(!rsa || !pub_bio) { return false; }

  if(!resize(RSA_size(rsa.get()),'\0',output)) {return false;}
  return RSA_public_encrypt(input.size(), const_cast<byte_t*>(input.data()),output.data(),rsa.get(), RSA_PKCS1_OAEP_PADDING) != -1;
}

inline bool decrypt(const bytes::View input, const bytes::View private_pem, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
  auto priv_bio = std::unique_ptr<BIO,void (*)(BIO *)>(BIO_new_mem_buf(const_cast<byte_t*>(private_pem.data()),private_pem.size()),&BIO_free_all);
  auto rsa = std::unique_ptr<RSA, void (*)(RSA*)>( PEM_read_bio_RSAPrivateKey(priv_bio.get(), NULL, NULL, NULL),&RSA_free);
  if(!rsa || !priv_bio) { return false; }

  if(!resize(RSA_size(rsa.get()),'\0',output)) { return false; }
  int decrypt_size = RSA_private_decrypt(input.size(),const_cast<byte_t*>(input.data()), output.data(), rsa.get(), RSA_PKCS1_OAEP_PADDING);
  if(decrypt_size == -1) { return false; }
  return resize(decrypt_size,'\0',output);
}

}

namespace aes {
  inline bool encrypt(const bytes::ConstView input, const bytes::ConstView iv, const bytes::ConstView key, const EVP_CIPHER *cipher, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
    int temp_size, encrypted_size;

    auto evp_ctx = std::unique_ptr<EVP_CIPHER_CTX,void(*)(EVP_CIPHER_CTX*)>(EVP_CIPHER_CTX_new(),EVP_CIPHER_CTX_free);
    if(!evp_ctx){ return false; }

    if(!EVP_EncryptInit_ex(evp_ctx.get(), cipher, NULL, key.data(), iv.data())) { return false; }

    byte_t* output_iterator = output.data();
    const byte_t* input_iterator  = input.data();

    bool ret = swiss::numeric_limit_split<int>([&](int size)->bool {
        if(!EVP_EncryptUpdate(evp_ctx.get(), output_iterator, &encrypted_size, input_iterator, size)) { return false; }
        input_iterator += size, output_iterator += encrypted_size;      
        return true; 
        },input.size());
    if(!ret) { return false; }

    if(!EVP_EncryptFinal_ex(evp_ctx.get(), output_iterator + encrypted_size, &temp_size)) { return false; }

    return resize(encrypted_size + temp_size,'\0',output);
  }

  inline bool decrypt(const bytes::ConstView input, const bytes::ConstView iv, const bytes::ConstView key, const EVP_CIPHER *cipher, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
    int temp_size, decrypted_size;

    auto evp_ctx = std::unique_ptr<EVP_CIPHER_CTX,void(*)(EVP_CIPHER_CTX*)>(EVP_CIPHER_CTX_new(),EVP_CIPHER_CTX_free);
    if(!evp_ctx){ return false; }

    if(!EVP_DecryptInit_ex(evp_ctx.get(), cipher, NULL, key.data(), iv.data())) { return false; }

    byte_t* output_iterator = output.data();
    const byte_t* input_iterator  = input.data();

    bool ret = swiss::numeric_limit_split<int>([&](int size)->bool {
        if(!EVP_DecryptUpdate(evp_ctx.get(), output_iterator, &decrypted_size, input_iterator, size)) { return false; }
        input_iterator += size, output_iterator += decrypted_size;      
        return true; 
        },input.size());
    if(!ret) { return false; }

    if(!EVP_DecryptFinal_ex(evp_ctx.get(), output_iterator + decrypted_size, &temp_size)) { return false; }

    return resize(decrypted_size + temp_size,'\0',output);
  }

}

namespace  aes128{
  namespace k{
    constexpr int iv_byte_size = 8;
    constexpr int key_byte_size = 16;
    constexpr size_t encrypt_upper_bound_size(size_t message_size) { return message_size + key_byte_size-1; }
    constexpr size_t decrypt_upper_bound_size(size_t message_size) { return message_size + key_byte_size; }
  }

  inline bool encrypt(const bytes::ConstView input, const bytes::ConstView iv, const bytes::ConstView key, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
    if(iv.size() != k::iv_byte_size) { return false; }
    if(key.size() != k::key_byte_size) { return false; }
    if(!resize(k::decrypt_upper_bound_size(input.size()),'\0',output)) { return false; }
    return aes::encrypt(input,iv,key,EVP_aes_128_cbc(),output,resize);
  }

  inline bool decrypt(const bytes::View input, const bytes::View iv, const bytes::View key, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
    if(iv.size() != k::iv_byte_size) { return false; }
    if(key.size() != k::key_byte_size) { return false; }
    if(!resize(k::decrypt_upper_bound_size(input.size()),'\0',output)) { return false; }
    return aes::decrypt(input,iv,key,EVP_aes_128_cbc(),output,resize);
  }
}

namespace  aes256{
  namespace k{
    constexpr int iv_byte_size = 16;
    constexpr int key_byte_size = 32;
    constexpr size_t encrypt_upper_bound_size(size_t message_size) { return message_size + key_byte_size-1; }
    constexpr size_t decrypt_upper_bound_size(size_t message_size) { return message_size + key_byte_size; }
  }

  inline bool encrypt(const bytes::View input, const bytes::View iv, const bytes::View key, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
    if(iv.size() != k::iv_byte_size) { return false; }
    if(key.size() != k::key_byte_size) { return false; }
    if(!resize(k::decrypt_upper_bound_size(input.size()),'\0',output)) { return false; }
    return aes::encrypt(input,iv,key,EVP_aes_256_cbc(),output, resize);
  }

  inline bool decrypt(const bytes::View input, const bytes::View iv, const bytes::View key, bytes::View output, const bytes::resize_callback_t &resize = bytes::default_resize) {
    if(iv.size() != k::iv_byte_size) { return false; }
    if(key.size() != k::key_byte_size) { return false; }
    if(!resize(k::decrypt_upper_bound_size(input.size()),'\0',output)) { return false; }
    return aes::decrypt(input,iv,key,EVP_aes_256_cbc(),output, resize);
  }

}

}
}

inline std::ostream& operator<<(std::ostream &os, const util::crypto::rsa::KeyPair& key_pair) {
  std::copy(key_pair.priv.begin(),key_pair.priv.end(),std::ostream_iterator<char>(os,""));
  std::copy(key_pair.pub.begin(),key_pair.pub.end(),std::ostream_iterator<char>(os,""));
  return os;
}


