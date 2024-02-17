/*
* File Encryption/Decryption: Ensures that all files stored in the filesystem are encrypted 
* and can only be decrypted by the middleware when accessed by an authenticated user.
*/

#ifndef FILESERVER_ENCRYPTION_H
#define FILESERVER_ENCRYPTION_H

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define BLOCK_SIZE 16 //bytes
#define KEY_SIZE 32 //bytes
#define TAG_SIZE 16 //bytes
#define IV_SIZE 16 //bytes

class Encryption {
public:
    static void encryptFile(const std::string& filePath, const std::string& content, const std::vector<uint8_t>& key);
    static std::string decryptFile(const std::string& filePath, const std::vector<uint8_t>& key);

private:
    static void handleErrors(const std::string& message);
    static void initCipherContext(EVP_CIPHER_CTX*& ctx, const std::vector<uint8_t>& key, const uint8_t* iv, bool encrypt);
};

#endif // FILESERVER_ENCRYPTION_H
