#ifndef __INC_UTILS_H__
#define __INC_UTILS_H__

#include "parameters.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


void _mDump(void* buffer, size_t size, const char* label);


/* Checks if login is in expected format (alfanumeric)
    Parameters:
      login: pointer to login buffer
    Return:
      login length on success, 0 otherwise
*/
size_t validateLogin(const char *login);

/* Checks if password is in expected format:
   chars in lower && upper cases && digits && specials.
    Parameters:
      pass: pointer to password buffer
    Return:
      Password length on success, 0 otherwise
*/
size_t validatePassword(const char *pass);

/* Parses the key length from string if it within the allowed values.
    Parameters:
      pass: pointer to keyLength buffer
    Return:
      Key bit size on success, 0 otherwise
*/
KeyLengths_t getKeyLength(const char *keyLenPtr);


#endif //__INC_UTILS_H__
