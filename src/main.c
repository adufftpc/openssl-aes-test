#include "utils.h"
#include "crypto.h"
#include "parameters.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


void usageExit(const char* exe)
{
    fprintf(stderr,
            "\nUsage: %s -l login -p password -k key_length (default: %s bit)\n\n",
                exe, KEY_LENGTH_DEFAULT_STR);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int opt;
    char *login = NULL;
    char *password = NULL;
    char *keyLenPtr = KEY_LENGTH_DEFAULT_STR;

    KeyLengths_t keyLenBits;
    CryptoDsc_t ossl = { 0 };

    size_t loginLen, passwordLen, loginLenEnc, loginLenDec, passwordLenEnc, passwordLenDec;

    /*******************************************************
     * Parse arguments and do some validations
    ********************************************************/
    while ((opt = getopt(argc, argv, "l:p:k:")) != -1) {
        switch (opt) {
            case 'l':
                login = optarg;
                break;
            case 'p':
                password = optarg;
                break;
            case 'k':
                keyLenPtr = optarg;
                break;
            default:
                usageExit(argv[0]);
        }
    }

    if (login == NULL || password == NULL) {
        usageExit(argv[0]);
    }

    loginLen = validateLogin(login);
    if (!loginLen) {
        fprintf(stderr,
            "Invalid login. Only alphanumeric characters are allowed.\n");
        usageExit(argv[0]);
    }

    passwordLen = validatePassword(password);
    if (!passwordLen) {
        fprintf(stderr,
            "Invalid password. Must contain uppercase, lowercase, "
            "digit, and special characters. Min length is %d\n", PASSWORD_MIN_LENGTH);
        usageExit(argv[0]);
    }

    keyLenBits = getKeyLength(keyLenPtr);
    if (KeyLength_INVALID == keyLenBits) {
        fprintf(stderr, "Invalid key length. Valid lengths: 128, 192, 256 bits\n");
        usageExit(argv[0]);
    }

    printf("Key length: %d bit\n", keyLenBits);
    printf("Login: '%s'\n", login);
    printf("Password: '%s'\n", password);

    /*******************************************************
     * Allocate buffers
    ********************************************************/
    char* loginEnc = (char*) calloc(sizeof(char), loginLen + 256);
    if (!loginEnc) {
        fprintf(stderr, "Couldn't allocate login encrypted buffer");
        exit(EXIT_FAILURE);
    }

    char* loginDec = (char*) calloc(sizeof(char), loginLen + 256);
    if (!loginDec) {
        fprintf(stderr, "Couldn't allocate login decrypted buffer");
        exit(EXIT_FAILURE);
    }

    char* passwordEnc = (char*) calloc(sizeof(char), loginLen + 256);
    if (!passwordEnc) {
        fprintf(stderr, "Couldn't allocate password encrypted buffer");
        exit(EXIT_FAILURE);
    }

    char* passwordDec = (char*) calloc(sizeof(char), loginLen + 256);
    if (!passwordDec) {
        fprintf(stderr, "Couldn't allocate password decrypted buffer");
        exit(EXIT_FAILURE);
    }

    printf("Allocations done\n\n");

    /*******************************************************
     * Encrypt everything
    ********************************************************/
    if (!openSslInit(&ossl, keyLenBits)) {
        fprintf(stderr, "Couldn't initialize descriptor");
        exit(EXIT_FAILURE);
    }

    loginLenEnc = openSslEncrypt(&ossl, login, loginLen, loginEnc);
    printf("Encrypted Login \t%ld bytes @ %p:\n", loginLenEnc, loginEnc);
    _mDump(loginEnc, loginLenEnc, "  dump:");

    passwordLenEnc = openSslEncrypt(&ossl, password, passwordLen, passwordEnc);
    printf("Encrypted Password \t%ld bytes @ %p:\n", passwordLenEnc, passwordEnc);
    _mDump(passwordEnc, passwordLenEnc, "  dump:");

    printf("Encryption done\n\n");

    /*******************************************************
     * Decrypt everything
    ********************************************************/
    loginLenDec = openSslDecrypt(&ossl, loginEnc, loginLenEnc, loginDec);
    printf("Decrypted Login \t%ld bytes @ %p: '%s'\n", loginLenDec, loginDec, loginDec);
    _mDump(loginDec, loginLenDec, "  dump:");

    passwordLenDec = openSslDecrypt(&ossl, passwordEnc, passwordLenEnc, passwordDec);
    printf("Decrypted Password \t%ld bytes @ %p: '%s'\n", passwordLenDec, passwordDec, passwordDec);
    _mDump(passwordDec, passwordLenDec, "  dump:");

    openSslDeinit(&ossl);

    printf("Decryption done\n\n");

    /*******************************************************
     * Check if data was restorred correctly
    ********************************************************/
    bool restored = loginLen == loginLenDec;
    restored &= !strncmp(login, loginDec, loginLen);
    printf("Login:\n");
    printf("  [%s] %s -> %s\n", restored ? "Same" : "Differs", login, loginDec);

    restored = passwordLen == passwordLenDec;
    restored &= !strncmp(password, passwordDec, passwordLen);
    printf("Password:\n");
    printf("  [%s] %s -> %s\n", restored ? "Same" : "Differs", password, passwordDec);


    /*******************************************************
     * Free the buffers
    ********************************************************/
    free(loginEnc);
    free(loginDec);
    free(passwordEnc);
    free(passwordDec);

    return 0;
}