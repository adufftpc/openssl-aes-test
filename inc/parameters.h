#ifndef __INC_PARAMETERS_H__
#define __INC_PARAMETERS_H__

#include <stdlib.h>


#define PASSWORD_MIN_LENGTH         (8)
#define KEY_LENGTH_DEFAULT_STR      "256"


typedef enum {
    KeyLength_INVALID = 0,
    KeyLength_128_BIT = 128,
    KeyLength_192_BIT = 192,
    KeyLength_256_BIT = 256,
} KeyLengths_t;



#endif //__INC_PARAMETERS_H__
