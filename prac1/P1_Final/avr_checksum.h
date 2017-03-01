#ifndef AVR_CHECKSUM_H
#define AVR_CHECKSUM_H


//#include <util/crc16.h>

#include <stdbool.h>
#include <stdint.h>

typedef struct valor{
  uint8_t a,b;
} val;


val check_morse(char d[]);

val crc_morse(char d[]);

bool test_check_morse(char d[]);

bool test_crc_morse(char d[]);


#endif
