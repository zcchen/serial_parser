#include "serial_parser.h"
#include "crc16.h"
#include "testing_helper.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int test_serial_encoder(char *payload_for_test)
{
    printf("\n========= encode testing =========\n");
    uint8_t buff[256];
    size_t payload_len = strlen(payload_for_test);
    int ret = serial_encode((uint8_t*)payload_for_test, buff, payload_len);

    printf(">>> Encoded msg: \n");
    for (int i = 0; i < ret; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, buff[i]);
    }
    printf("-------------\n");

    if (buff[0] != SERIAL_PARSER_HEADER) {
        return 1;
    }
    if (buff[1] != payload_len + 2) {
        return 2;
    }
    union {
        uint8_t c[2];
        uint16_t uint;
    } _crc_raw;
    _crc_raw.c[0] = buff[ret - 2];
    _crc_raw.c[1] = buff[ret - 1];
    uint16_t crc_raw = serial2local_16(_crc_raw.uint);
    uint16_t crc_recalc = crc16(0, (uint8_t*)buff, buff[1]);
    if (crc_raw != crc_recalc) {
        printf("crc_raw: 0x%x\n", crc_raw);
        printf("crc_recalc: 0x%d\n", crc_recalc);
        return 3;
    }
    return 0;
}

int test_serial_decoder_correct(char *payload_for_test)
{
    printf("\n======= correct decode testing ========\n");
    uint8_t encoded_msg[256];
    uint8_t decoded_msg[256];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    printf(">>> Encoded msg: \n");
    for (int i = 0; i < msg_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, encoded_msg[i]);
    }
    printf("-------------\n");

    int ret = serial_decode((uint8_t*)encoded_msg, decoded_msg, msg_len, 0);

    printf(">>> Decoded msg: \n");
    for (int i = 0; i < ret; ++i) {
        printf("decoded_msg[%d]: 0x%x.\n", i, decoded_msg[i]);
    }
    printf("-------------\n");

    if (ret == payload_len) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_decoder_header_unmatched(char *payload_for_test)
{
    printf("\n======= header unmatched decode testing ========\n");
    uint8_t encoded_msg[256];
    uint8_t decoded_msg[256];
    uint8_t msg_for_testing[256];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    size_t offset_index = 1;
    msg_for_testing[0] = 'a';
    memcpy(msg_for_testing + offset_index, encoded_msg, msg_len);

    printf(">>> Encoded msg: \n");
    for (int i = 0; i < strlen((char*)msg_for_testing); ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    int ret = serial_decode((uint8_t*)msg_for_testing, decoded_msg, msg_len, 0);

    if (ret == SERIAL_MSG_ERR_HEADER_UNMATCHED) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_decoder_crc_err(char *payload_for_test)
{
    printf("\n======= CRC-error decode testing ========\n");
    uint8_t encoded_msg[256];
    uint8_t decoded_msg[256];
    uint8_t msg_for_testing[256];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    memcpy(msg_for_testing, encoded_msg, msg_len);
    msg_for_testing[msg_len - 1] -= 1;

    printf(">>> Encoded msg: \n");
    for (int i = 0; i < strlen((char*)msg_for_testing); ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    int ret = serial_decode((uint8_t*)msg_for_testing, decoded_msg, msg_len, 0);

    if (ret == SERIAL_MSG_ERR_CRC) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_decoder_header_faked_with_max_msglen(char *payload_for_test)
{
    printf("\n======= header faked testing ========\n");
    uint8_t encoded_msg[256];
    uint8_t decoded_msg[256];
    uint8_t msg_for_testing[256];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    size_t offset_index = 2;
    msg_for_testing[0] = SERIAL_PARSER_HEADER;
    msg_for_testing[1] = encoded_msg[1] + offset_index;
    memcpy(msg_for_testing + offset_index, encoded_msg, msg_len);

    printf("get msg len: %d (max: %d)\n", msg_for_testing[1] + 2, msg_len);
    printf(">>> Encoded msg: \n");
    for (int i = 0; i < strlen((char*)msg_for_testing); ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    int ret = serial_decode((uint8_t*)msg_for_testing, decoded_msg,
                            msg_len + offset_index, msg_len);

    if (ret == SERIAL_MSG_ERR_HEADER_FAKED) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_decoder_msg_uncomplete_because_header_faked_without_max_msglen(
        char *payload_for_test)
{
    printf("\n======= msg uncompleted (header faked without max msglen) testing ========\n");
    uint8_t encoded_msg[256];
    uint8_t decoded_msg[256];
    uint8_t msg_for_testing[256];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    size_t offset_index = 2;
    msg_for_testing[0] = SERIAL_PARSER_HEADER;
    msg_for_testing[1] = encoded_msg[1] + offset_index + 1;
    memcpy(msg_for_testing + offset_index, encoded_msg, msg_len);

    printf("get msg len: %d (recv msg len: %ld)\n",
           msg_for_testing[1] + 2, msg_len + offset_index);
    printf(">>> Encoded msg: \n");
    for (int i = 0; i < strlen((char*)msg_for_testing); ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    int ret = serial_decode((uint8_t*)msg_for_testing, decoded_msg,
                            msg_len + offset_index, 0);

    if (ret == SERIAL_MSG_ERR_NOT_COMPLETED) {
        return 0;
    }
    else {
        return ret;
    }
}

int main(void)
{
    TEST_RETURN(test_serial_encoder("123456789"));
    TEST_RETURN(test_serial_decoder_correct("123456789"));
    TEST_RETURN(test_serial_decoder_header_unmatched("123456789"));
    TEST_RETURN(test_serial_decoder_crc_err("123456789"));
    TEST_RETURN(test_serial_decoder_header_faked_with_max_msglen("123456789"));
    TEST_RETURN(test_serial_decoder_msg_uncomplete_because_header_faked_without_max_msglen(
                "123456789"));
    return 0;
}
