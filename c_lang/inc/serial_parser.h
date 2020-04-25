#ifndef SERIAL_PARSER_H
#define SERIAL_PARSER_H

#include <stdint.h>
#include <stdlib.h>
#include "endian_helper.h"

//! \brief SERIAL_PARSER_HEADER is to define the header char
#define SERIAL_PARSER_HEADER 'S'

//! \brief SERIAL_PARSER_ENDER is to define the header char
//#define SERIAL_PARSER_ENDER ''

//! \brief serial_encode()  The encoding function from payload to msg
//! \param uint8_t *payload The pointer to payload msg.
//! \param uint8_t *dest    The pointer to encoded msg.
//! \param size_t len       The len for the payload msg.
//! \return msg_len         The length for the encoded msg.
int serial_encode(uint8_t *payload, uint8_t *dest, size_t len);

//! \brief serial_decode()  The decoding function from payload to msg
//! \param uint8_t *msg     The pointer to serial msg.
//! \param uint8_t *dest    The pointer to decoded msg.
//! \param size_t msg_len   The len for the serial msg.
//! \return decode_len      The length for the decoded msg.
int serial_decode(uint8_t *msg, uint8_t *dest, size_t msg_len);

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

static inline uint16_t serial2local_16(uint16_t num)
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

static inline uint32_t serial2local_32(uint32_t num)
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
