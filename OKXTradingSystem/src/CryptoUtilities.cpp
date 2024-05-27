#include "CryptoUtilities.h"
#include <crypto++/sha.h>
#include <crypto++/hmac.h>
#include <crypto++/base64.h>
#include <crypto++/filters.h>
#include <crypto++/hex.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

std::string encodeBase64(const unsigned char* buffer, size_t length) {
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, buffer, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    std::string output(bufferPtr->data, bufferPtr->length);
    BUF_MEM_free(bufferPtr);
    return output;
}

std::string hmac_sha256(const std::string &data, const std::string &key) {
    unsigned char* hash = HMAC(EVP_sha256(), key.c_str(), key.length(), (const unsigned char*)data.c_str(), data.length(), NULL, NULL);
    return encodeBase64(hash, 32); // 32 bytes = 256 bits
}
