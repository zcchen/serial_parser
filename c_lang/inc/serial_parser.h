#ifndef SERIAL_PARSER_H
#define SERIAL_PARSER_H

#include <stdint.h>
#include <stdlib.h>
#include "endian_helper.h"

//! \brief SERIAL_PARSER_HEADER is to define the header char
#define SERIAL_PARSER_HEADER 'S'

int serial_encode(uint8_t *raw, uint8_t *dest, size_t len);

static inline uint16_t local2serial_16(uint16_t num)
{
    switch (ENDIANNESS) {
        case 'l':
            return num;
        case 'b':
            return swap16(num);
        default:
            return 0;
    }
};

static inline uint32_t local2serial_32(uint32_t num)
{
    switch (ENDIANNESS) {
        case 'l':
            return num;
        case 'b':
            return swap32(num);
        default:
            return 0;
    }
};

#endif /* ifndef SERIAL_PARSER_H */
