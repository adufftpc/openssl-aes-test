#include "utils.h"
#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


void _mDump(void* address, size_t size, const char* label)
{
#ifdef _DEBUG
    uint8_t* ptr = (uint8_t*)address;
    if (NULL != label) {
        printf("%s", label);
    }

    for (size_t i = 0; i < size; ++i) {
        if (i % 16 == 0)
            printf("\n");
        printf(" %02X", ptr[i]);

    }
    printf("\n");
#endif //_DEBUG
}

size_t validateLogin(const char *login)
{
    bool ret = true;
    size_t length = 0;
    while (login && ('\0' != login[length])) {
        // following condition might be replaced with isalnum(login[length]) from ctypes.h
        if (('a' <= login[length] && login[length] <= 'z') ||
            ('Z' <= login[length] && login[length] <= 'Z') ||
            ('0' <= login[length] && login[length] <= '9')) {
                length++;
                continue;
            }
        else {
            ret = false;
            break;
        }
    }
    return (length && ret) ? length : 0;
}

size_t validatePassword(const char *pass)
{
    size_t chLower = 0;
    size_t chUpper = 0;
    size_t chDigit = 0;
    size_t chSpecial = 0;

    const char* ptr = pass;

    while (ptr && ('\0' != *ptr)) {
        if (!isprint(*ptr)) {
            return 0;
        } else if (islower(*ptr)) {
            chLower++;
        } else if (isupper(*ptr)) {
            chUpper++;
        } else if (isdigit(*ptr)) {
            chDigit++;
        } else {
            chSpecial++;
        }
        ptr++;
    }

    if (0 == chLower || 0 == chUpper || 0 == chDigit || 0 == chSpecial ||
        (chLower + chUpper + chDigit + chSpecial) < PASSWORD_MIN_LENGTH) {
            return 0;
    }

    return chLower + chUpper + chDigit + chSpecial;
}

KeyLengths_t getKeyLength(const char *keyLenPtr)
{
    unsigned long val = strtoul(keyLenPtr, NULL, 0);
    KeyLengths_t kLen;

    switch(val) {
        case KeyLength_128_BIT:
        case KeyLength_192_BIT:
        case KeyLength_256_BIT:
            kLen = (KeyLengths_t) val;
            break;
        default:
            kLen = KeyLength_INVALID;
    }

    return kLen;
}
