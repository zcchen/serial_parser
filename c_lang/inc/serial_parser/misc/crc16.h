#ifndef SERIAL_PARSER_CRC16_H
#define SERIAL_PARSER_CRC16_H

#include <stdlib.h>
#include <stdint.h>
typedef unsigned char uint8_t;

/* Implements the standard CRC-16:
 *   Width 16
 *   Poly  0x8005 (x^16 + x^15 + x^2 + 1)
 *   Init  0
 *   */

/** CRC table for the CRC-16. The poly is 0x8005 (x^16 + x^15 + x^2 + 1) */
extern uint16_t const crc16_table[256];

static inline uint16_t crc16_byte(uint16_t crc, const uint8_t data) {
    return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
};

uint16_t crc16(uint16_t crc, const uint8_t *buffer, size_t len);
#endif /* ifndef SERIAL_PARSER_CRC16_H */
