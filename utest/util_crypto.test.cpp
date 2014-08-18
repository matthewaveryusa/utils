#include <gtest/gtest.h>
#include <util_crypto.hpp>
#include <string>
#include <vector>

TEST(util_crypto, rsa) {
  using namespace util::crypto::rsa;
  using namespace util::bytes;
  using util::bytes_t;
  using util::byte_t;

  KeyPair pair1024;
  KeyPair pair2048;
  KeyPair pair4096;
  bytes_t public1024;
  bytes_t public2048;
  bytes_t public4096;


  EXPECT_TRUE(key(1024,pair1024));  
  EXPECT_TRUE(key(2048,pair2048));
  EXPECT_TRUE(key(4096,pair4096));

  EXPECT_GT(pair2048.priv,pair1024.priv);
  EXPECT_GT(pair4096.priv,pair2048.priv);
  EXPECT_GT(pair2048.pub,pair1024.pub);
  EXPECT_GT(pair4096.pub,pair2048.pub);

  EXPECT_TRUE(get_public(pair1024.priv,public1024,resize_callback(public1024)));  
  EXPECT_TRUE(get_public(pair2048.priv,public2048,resize_callback(public2048)));
  EXPECT_TRUE(get_public(pair4096.priv,public4096,resize_callback(public4096)));

  EXPECT_EQ(pair1024.pub,public1024);  
  EXPECT_EQ(pair2048.pub,public2048);  
  EXPECT_EQ(pair4096.pub,public4096);  

  bytes_t vmessage = {1,2,3,4,5,6,7};

  bytes_t encrypted_output;
  bytes_t decrypted_output;
  EXPECT_TRUE(encrypt(vmessage,public1024,encrypted_output,resize_callback(encrypted_output)));
  EXPECT_TRUE(decrypt(encrypted_output,pair1024.priv,decrypted_output,resize_callback(decrypted_output)));
  EXPECT_EQ(vmessage,decrypted_output);
  
  EXPECT_TRUE(encrypt(vmessage,public2048,encrypted_output,resize_callback(encrypted_output)));
  EXPECT_TRUE(decrypt(encrypted_output,pair2048.priv,decrypted_output,resize_callback(decrypted_output)));
  EXPECT_EQ(vmessage,decrypted_output);
  
  EXPECT_TRUE(encrypt(vmessage,public4096,encrypted_output,resize_callback(encrypted_output)));
  EXPECT_TRUE(decrypt(encrypted_output,pair4096.priv,decrypted_output,resize_callback(decrypted_output)));
  EXPECT_EQ(vmessage,decrypted_output);

  bytes_t signature;
  EXPECT_TRUE(sign256(vmessage,pair1024.priv,signature,resize_callback(signature)));
  EXPECT_TRUE(verify256(vmessage,signature,pair1024.pub));
  
  EXPECT_TRUE(sign512(vmessage,pair1024.priv,signature,resize_callback(signature)));
  EXPECT_TRUE(verify512(vmessage,signature,pair1024.pub));
  
  EXPECT_TRUE(sign256(vmessage,pair2048.priv,signature,resize_callback(signature)));
  EXPECT_TRUE(verify256(vmessage,signature,pair2048.pub));
  
  EXPECT_TRUE(sign512(vmessage,pair2048.priv,signature,resize_callback(signature)));
  EXPECT_TRUE(verify512(vmessage,signature,pair2048.pub));

  EXPECT_TRUE(sign256(vmessage,pair4096.priv,signature,resize_callback(signature)));
  EXPECT_TRUE(verify256(vmessage,signature,pair4096.pub));
  
  EXPECT_TRUE(sign512(vmessage,pair4096.priv,signature,resize_callback(signature)));
  EXPECT_TRUE(verify512(vmessage,signature,pair4096.pub));
  
}
