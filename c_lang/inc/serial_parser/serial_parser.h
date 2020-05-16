#ifndef SERIAL_PARSER_H
#define SERIAL_PARSER_H

#include "misc/endian_helper.h"
#include <stdlib.h>
#include <stdint.h>
typedef unsigned char uint8_t;

//! \brief SERIAL_PARSER_HEADER is to define the header char
#define SERIAL_PARSER_HEADER 'S'

enum serial_msg_error_t {
    SERIAL_MSG_ERR_UNKNOWN = -1,
    SERIAL_MSG_ERR_HEADER_UNMATCHED = -2,
    SERIAL_MSG_ERR_CRC = -3,
    SERIAL_MSG_ERR_HEADER_FAKED = -4,
    SERIAL_MSG_ERR_NOT_COMPLETED = -5,
};

//! \brief serial_encode()  The encoding function from payload to msg
//! \param uint8_t *payload The pointer to payload msg.
//! \param uint8_t *dest    The pointer to encoded msg.
//! \param size_t len       The len for the payload msg.
//! \return msg_len         The length for the encoded msg.
int serial_encode(const uint8_t *payload, uint8_t *dest, size_t len);

//! \brief serial_decode()      The decoding function from payload to msg
//! \param uint8_t *msg         The pointer to serial msg.
//! \param uint8_t *dest        The pointer to decoded msg.
//! \param size_t msg_len       The len for the serial msg.
//! \param size_t max_msglen    The max length for one serial msg.
//! \return decode_len          The length for the decoded msg.
int serial_decode(const uint8_t *msg, uint8_t *dest, size_t msg_len, size_t max_msglen);


//! \return msg_copy_bytes  The bytes copied to *dest_this, negative num means errors
int serial_find(const uint8_t *msg, uint8_t *dest_this, uint8_t *dest_rest,
                size_t msg_len, size_t max_msglen);

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
