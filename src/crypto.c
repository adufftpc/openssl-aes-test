#include "crypto.h"
#include "utils.h"
#include "parameters.h"

#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


void keyGenAlloc(CryptoDsc_t *ossl, KeyLengths_t keyLenBits)
{
    size_t lenBytes = keyLenBits >> 3;

    ossl->key = (uint8_t*) malloc(sizeof(uint8_t) * lenBytes);
    if (NULL != ossl->key) {
        RAND_bytes(ossl->key, lenBytes);
    }

    _mDump(ossl->key, lenBytes, "Generated key");
}

bool openSslInit(CryptoDsc_t *ossl, KeyLengths_t  keyBitLen)
{
    bool status = true;

    // Here just clear the structure, it is a dump way as will cause a leak while re-init.
    // TODO: return an error in case the structure is not empty.
    memset(ossl, 0, sizeof(CryptoDsc_t));
    ossl->keyLenBits = (KeyLengths_t) keyBitLen;

    switch(keyBitLen) {
        case KeyLength_128_BIT:
            ossl->cbAlgo = EVP_aes_128_cbc;
            break;
        case KeyLength_192_BIT:
            ossl->cbAlgo = EVP_aes_192_cbc;
            break;
        case KeyLength_256_BIT:
            ossl->cbAlgo = EVP_aes_256_cbc;
            break;
        default:
            ossl->cbAlgo = NULL;
            return false;
    }

    keyGenAlloc(ossl, keyBitLen);
    ossl->ctxEncrypt = EVP_CIPHER_CTX_new();
    ossl->ctxDecrypt = EVP_CIPHER_CTX_new();

    status = ossl->key && ossl->ctxEncrypt && ossl->ctxDecrypt;
    if (!status) {
        openSslDeinit(ossl);
    }
    return status;
}

void openSslDeinit(CryptoDsc_t *ossl)
{
    if (ossl->key) {
        free(ossl->key);
    }
    if (ossl->ctxEncrypt) {
        EVP_CIPHER_CTX_free(ossl->ctxEncrypt);
    }
    if (ossl->ctxDecrypt) {
        EVP_CIPHER_CTX_free(ossl->ctxDecrypt);
    }
    memset(ossl, 0, sizeof(CryptoDsc_t));
}

size_t openSslEncrypt(CryptoDsc_t *ossl, const char* plain, size_t plainLen, char* cipher)
{
    size_t cipherLen;
    int len;

    if (!ossl->cbAlgo || !ossl->ctxEncrypt) {
        fprintf(stderr, "Couldn't encrypt");
        return 0;
    }

    EVP_EncryptInit_ex(ossl->ctxEncrypt, ossl->cbAlgo(), NULL, ossl->key, ossl->key);
    EVP_EncryptUpdate(ossl->ctxEncrypt, (unsigned char*)cipher, &len, (unsigned char*)plain, plainLen);
    cipherLen = len;
    EVP_EncryptFinal_ex(ossl->ctxEncrypt, (unsigned char*)cipher + len, &len);
    cipherLen += len;

    return cipherLen;
}

size_t openSslDecrypt(CryptoDsc_t *ossl, const char* cipher, int cipherLen, char* plain)
{
    size_t plainLen;
    int len;

    if (!ossl->cbAlgo || !ossl->ctxDecrypt) {
        fprintf(stderr, "Couldn't decrypt");
        return 0;
    }

    EVP_DecryptInit_ex(ossl->ctxDecrypt, ossl->cbAlgo(), NULL, ossl->key, ossl->key);
    EVP_DecryptUpdate(ossl->ctxDecrypt, (unsigned char*)plain, &len, (unsigned char*)cipher, cipherLen);
    plainLen = len;
    EVP_DecryptFinal_ex(ossl->ctxDecrypt, (unsigned char*)plain + len, &len);
    plainLen += len;

    return plainLen;
}
