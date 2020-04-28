#include "serial_parser.h"
#include "crc16.h"

int serial_encode(const uint8_t *payload, uint8_t *dest, size_t len)
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

int _serial_check(const uint8_t *msg, size_t msg_len, size_t max_msglen)
{
    const uint8_t *the_header = msg;
    const uint8_t *the_len = msg + 1;
    union {
        char c[2];
        uint16_t uint;
    } the_crc;
    if (*the_header != SERIAL_PARSER_HEADER) {
        return SERIAL_MSG_ERR_HEADER_UNMATCHED;
    }
    if (max_msglen > 0) {
        if (*the_len + 2 > max_msglen) {
            return SERIAL_MSG_ERR_HEADER_FAKED;
        }
        else {
            ; // goto crc checking to confirm.
        }
    }
    else {
        if (*the_len + 2 > msg_len) {
            return SERIAL_MSG_ERR_NOT_COMPLETED;
        }
        else {
            ; // goto crc checking to confirm.
        }
    }
    the_crc.c[0] = *(msg + 1 + *the_len - 1);
    the_crc.c[1] = *(msg + 1 + *the_len - 0);
    uint16_t crc_raw = serial2local_16(the_crc.uint);
    uint16_t crc_recalc = crc16(0, (uint8_t*)msg, *the_len);
    if (crc_raw != crc_recalc) {
        return SERIAL_MSG_ERR_CRC;
    }
    return *the_len + 2;
}

int serial_decode(const uint8_t *msg, uint8_t *dest, size_t msg_len, size_t max_msglen)
{
    int ret;
    ret = _serial_check(msg, msg_len, max_msglen);
    if (ret >= 0) {
        for (int i = 0; i < ret - 2; ++i) {
            *(dest + i) = *(msg + 2 + i);
        }
        return ret - 4;
    }
    else {
        return ret;
    }
}

int serial_find(const uint8_t *msg, uint8_t *dest, size_t msg_len, size_t max_msglen)
{
    size_t i = 0;
    int ret = 0;
    while (i < msg_len) {
        int checking_ret = _serial_check( (msg + i), msg_len, max_msglen);
        if (checking_ret > 0) { // found some msg, go to copy section.
            goto COPY_FOUND_RET;
        }
        else {
            switch (checking_ret) {
                case SERIAL_MSG_ERR_HEADER_UNMATCHED:
                case SERIAL_MSG_ERR_HEADER_FAKED:
                case SERIAL_MSG_ERR_CRC:
                    break;
                case SERIAL_MSG_ERR_NOT_COMPLETED:
                    goto COPY_FOUND_RET;
                default:
                    return -1;
            }
        }
        i ++;
    }
COPY_FOUND_RET:
    for (int j = 0; j < msg_len - i; ++j) {
        *(dest + j) = *(msg + j);
        ret ++;
    }
    return ret;
}

