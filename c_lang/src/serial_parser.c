#include "serial_parser.h"
#include "crc16.h"

int serial_encode(uint8_t *payload, uint8_t *dest, size_t len)
{
    *dest = SERIAL_PARSER_HEADER;   // header byte
    *(dest + 1) = 0;                // length byte
    uint8_t msg_len = 0;
    for (int i = 0; i < len; ++i) {
        *(dest + 2 + i) = *(payload + i);
        msg_len ++;
    }
    msg_len += 2;   // including the crc size
    *(dest + 1) = msg_len;
    union {
        char c[2];
        uint16_t uint;
    } crc;
    crc.uint = local2serial_16(crc16(0, dest, msg_len));
    *(dest + len + 2) = crc.c[0];
    *(dest + len + 2 + 1) = crc.c[1];
    return msg_len + 2; // the final msg length
}

int serial_decode(uint8_t *msg, uint8_t *dest, size_t msg_len)
{
    uint8_t *header = msg;
    if (*header != SERIAL_PARSER_HEADER) {
        return -1;
    }
    uint8_t *the_len = msg + 1;
    union {
        char c[2];
        uint16_t uint;
    } the_crc;
    the_crc.c[0] = *(msg + 1 + *the_len - 1);
    the_crc.c[1] = *(msg + 1 + *the_len - 0);
    uint16_t crc_raw = serial2local_16(the_crc.uint);
    uint16_t crc_recalc = crc16(0, (uint8_t*)msg, *the_len);
    if (crc_raw != crc_recalc) {
        return -2;
    }
    for (int i = 0; i < *the_len - 2; ++i) {
        *(dest + i) = *(msg + 2 + i);
    }
    return (*the_len - 2);
}

