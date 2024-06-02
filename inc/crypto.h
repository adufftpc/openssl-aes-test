#ifndef __INC_CRYPTO_H__
#define __INC_CRYPTO_H__

#include "parameters.h"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct {
    KeyLengths_t  keyLenBits;
    uint8_t *key;

    // Not sure, probably in further might be reduced to just 1 context
    EVP_CIPHER_CTX* ctxEncrypt;
    EVP_CIPHER_CTX* ctxDecrypt;
    const EVP_CIPHER* (*cbAlgo)(void);
} CryptoDsc_t;


void keyGenAlloc(CryptoDsc_t *ossl, KeyLengths_t keyLenBits);

bool openSslInit(CryptoDsc_t *ossl, KeyLengths_t  keyBitLen);

size_t openSslEncrypt(CryptoDsc_t *ossl, const char* plain, size_t plainLen, char* cipher);

size_t openSslDecrypt(CryptoDsc_t *ossl, const char* cipher, int cipherLen, char* plain);

void openSslDeinit(CryptoDsc_t *ossl);

#endif //__INC_CRYPTO_H__
