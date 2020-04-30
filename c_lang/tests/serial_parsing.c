#include "serial_parser.h"
#include "crc16.h"
#include "testing_helper.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_BUFF_SIZE 256

int msg_mix_up(uint8_t *dest, size_t max_size, uint8_t *msg, size_t msg_len,
               uint8_t *insert_msg, size_t insert_len, uint8_t *append_msg, size_t append_len)
{
    size_t rest_size = max_size;
    uint8_t *dest_ptr = dest;
    if (insert_len < rest_size) {
        if (insert_msg != NULL) {
            memcpy(dest_ptr, insert_msg, insert_len);
            rest_size -= insert_len;
            dest_ptr += insert_len;
        }
    }
    else return -1;
    if (msg_len < rest_size) {
        if (msg != NULL) {
            memcpy(dest_ptr, msg, msg_len);
            rest_size -= msg_len;
            dest_ptr += msg_len;
        }
    }
    else return -2;
    if (append_len < rest_size) {
        if (append_msg != NULL) {
            memcpy(dest_ptr, append_msg, append_len);
            rest_size -= append_len;
            dest_ptr += append_len;
        }
    }
    else return -3;
    return (dest_ptr - dest);
}

int test_serial_encode__matched(const char *payload_for_test)
{
    printf("\n========= serial_encode() testing =========\n");
    uint8_t buff[MAX_BUFF_SIZE];
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

int test_serial_decode__correct(const char *payload_for_test)
{
    printf("\n======= serial_decode() testing correct ========\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t decoded_msg[MAX_BUFF_SIZE];
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

int test_serial_decode__correct_but_overmore(const char *payload_for_test)
{
    printf("\n======= serial_decode() testing correct but overmore ========\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t decoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  NULL, 0, (uint8_t*)"a", 1);

    printf(">>> Mix-up msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    int ret = serial_decode((uint8_t*)msg_for_testing, decoded_msg, mixup_len, 0);

    if (ret == payload_len) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_decode__header_unmatched(const char *payload_for_test)
{
    printf("\n======= serial_decode() testing: header unmatched ========\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t decoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  (uint8_t*)"a", 1, NULL, 0);

    printf(">>> mix-up msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
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

int test_serial_decode__crc_err(const char *payload_for_test)
{
    printf("\n======= serial_decode() testing: CRC error ========\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t decoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    memcpy(msg_for_testing, encoded_msg, msg_len);
    msg_for_testing[msg_len - 1] -= 1;

    printf(">>> crc error msg: \n");
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

int test_serial_decode__header_faked_with_max_msglen(const char *payload_for_test)
{
    printf("\n======= serial_decode() testing: faked header ========\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t decoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    uint8_t tmp[2];
    tmp[0] = SERIAL_PARSER_HEADER;
    tmp[1] = encoded_msg[1] + 2;
    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  tmp, 2, NULL, 0);

    printf("get msg len: %d (max: %d)\n", msg_for_testing[1] + 2, msg_len);
    printf(">>> Encoded msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    int ret = serial_decode((uint8_t*)msg_for_testing, decoded_msg, mixup_len, msg_len);
    if (ret == SERIAL_MSG_ERR_HEADER_FAKED) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_decode__msg_uncomplete_because_header_faked_without_max_msglen(
        const char *payload_for_test)
{
    printf("\n======= serial_decode() test ===============\n");
    printf(">>> expect: msg uncompleted (header faked without max msglen)\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t decoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    uint8_t tmp[2];
    tmp[0] = SERIAL_PARSER_HEADER;
    tmp[1] = encoded_msg[1] + 2 + 1;
    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  tmp, 2, NULL, 0);

    printf("get msg len: %d (recv msg len: %ld)\n", msg_for_testing[1] + 2, mixup_len);
    printf(">>> Encoded msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    int ret = serial_decode((uint8_t*)msg_for_testing, decoded_msg, mixup_len, 0);
    if (ret == SERIAL_MSG_ERR_NOT_COMPLETED) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_find__found_1st_msg(const char* payload_for_test)
{
    printf("\n======= serial_find() test ========\n");
    printf(">>> expect to find the 1st msg with noisy headers & enders\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  (uint8_t*)"ab", 2, (uint8_t*)"cd", 2);

    printf(">>> msg for testing: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    uint8_t dest_this[MAX_BUFF_SIZE];
    uint8_t dest_rest[MAX_BUFF_SIZE];
    for (int i = 0; i < MAX_BUFF_SIZE; ++i) {
        dest_this[i] = '\0';
        dest_rest[i] = '\0';
    }

    int ret = serial_find(msg_for_testing, dest_this, dest_rest, mixup_len, 0);

    printf(">>> dest this msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_this[%d]: 0x%x.\n", i, dest_this[i]);
    }
    printf("-------------\n");
    printf(">>> dest rest msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_rest[%d]: 0x%x.\n", i, dest_rest[i]);
    }
    printf("-------------\n");

    if (ret == msg_len) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_find__correct_header_but_smaller_len(const char* payload_for_test)
{
    printf("\n======= serial_find() test ========\n");
    printf(">>> header is correct, but length is smaller then expect\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    uint8_t tmp[4];
    tmp[0] = 'a';
    tmp[1] = 'b';
    tmp[2] = SERIAL_PARSER_HEADER;
    tmp[3] = encoded_msg[1];
    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  tmp, sizeof(tmp), (uint8_t*)"cd", 2);

    uint8_t dest_this[MAX_BUFF_SIZE];
    uint8_t dest_rest[MAX_BUFF_SIZE];
    for (int i = 0; i < MAX_BUFF_SIZE; ++i) {
        dest_this[i] = '\0';
        dest_rest[i] = '\0';
    }

    int ret = serial_find(msg_for_testing, dest_this, dest_rest, mixup_len, 0);

    printf(">>> dest this msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_this[%d]: 0x%x.\n", i, dest_this[i]);
    }
    printf("-------------\n");
    printf(">>> dest rest msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_rest[%d]: 0x%x.\n", i, dest_rest[i]);
    }
    printf("-------------\n");

    if (ret == msg_len) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_find__correct_header_but_bigger_len(const char* payload_for_test)
{
    printf("\n======= serial_find() test ========\n");
    printf(">>> header is correct, but length is bigger then expect\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    uint8_t tmp[4];
    uint8_t delta_msg_len = 1;
    tmp[0] = 'a';
    tmp[1] = 'b';
    tmp[2] = SERIAL_PARSER_HEADER;
    tmp[3] = msg_len + delta_msg_len;
    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  tmp, sizeof(tmp), NULL, 0);
    printf(">>> msg for testing: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    uint8_t dest_this[MAX_BUFF_SIZE];
    uint8_t dest_rest[MAX_BUFF_SIZE];
    for (int i = 0; i < MAX_BUFF_SIZE; ++i) {
        dest_this[i] = '\0';
        dest_rest[i] = '\0';
    }

    int ret = serial_find(msg_for_testing, dest_this, dest_rest, mixup_len, 0);

    printf(">>> dest this msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_this[%d]: 0x%x.\n", i, dest_this[i]);
    }
    printf("-------------\n");
    printf(">>> dest rest msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_rest[%d]: 0x%x.\n", i, dest_rest[i]);
    }
    printf("-------------\n");

    if (ret == msg_len + 2) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_find__no_correct_crc(const char* payload_for_test)
{
    printf("\n======= serial_find() test ========\n");
    printf(">>> header is correct, but no correct CRC.\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    uint8_t tmp[2];
    tmp[0] = 'a';
    tmp[1] = 'b';
    encoded_msg[msg_len - 1] -= 1;
    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  tmp, sizeof(tmp), NULL, 0);
    printf(">>> msg for testing: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    uint8_t dest_this[MAX_BUFF_SIZE];
    uint8_t dest_rest[MAX_BUFF_SIZE];
    for (int i = 0; i < MAX_BUFF_SIZE; ++i) {
        dest_this[i] = '\0';
        dest_rest[i] = '\0';
    }

    int ret = serial_find(msg_for_testing, dest_this, dest_rest, mixup_len, 0);

    printf(">>> dest this msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_this[%d]: 0x%x.\n", i, dest_this[i]);
    }
    printf("-------------\n");
    printf(">>> dest rest msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_rest[%d]: 0x%x.\n", i, dest_rest[i]);
    }
    printf("-------------\n");

    if (ret == msg_len) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_find__two_msg_without_correct_header(const char* payload_for_test)
{
    printf("\n======= serial_find() test ========\n");
    printf(">>> Two msg is passed in, header msg are incorrect\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  (uint8_t*)"ab", 2, encoded_msg, msg_len);
    printf(">>> msg for testing: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    uint8_t dest_this[MAX_BUFF_SIZE];
    uint8_t dest_rest[MAX_BUFF_SIZE];
    for (int i = 0; i < MAX_BUFF_SIZE; ++i) {
        dest_this[i] = '\0';
        dest_rest[i] = '\0';
    }

    int ret = serial_find(msg_for_testing, dest_this, dest_rest, mixup_len, 0);

    printf(">>> dest this msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_this[%d]: 0x%x.\n", i, dest_this[i]);
    }
    printf("-------------\n");
    printf(">>> dest rest msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_rest[%d]: 0x%x.\n", i, dest_rest[i]);
    }
    printf("-------------\n");

    if (ret == msg_len) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_find__two_msg_with_correct_header_but_no_maxlen_limit(
        const char* payload_for_test)
{
    printf("\n======= serial_find() test ========\n");
    printf(">>> Two msg is passed in, with mix-up header, no max msg len limit.\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    uint8_t tmp[2];
    tmp[0] = SERIAL_PARSER_HEADER;
    tmp[1] = 2 * msg_len + 1;
    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  tmp, sizeof(tmp), encoded_msg, msg_len);
    printf(">>> msg for testing: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    uint8_t dest_this[MAX_BUFF_SIZE];
    uint8_t dest_rest[MAX_BUFF_SIZE];
    for (int i = 0; i < MAX_BUFF_SIZE; ++i) {
        dest_this[i] = '\0';
        dest_rest[i] = '\0';
    }

    int ret = serial_find(msg_for_testing, dest_this, dest_rest, mixup_len, 0);

    printf(">>> dest this msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_this[%d]: 0x%x.\n", i, dest_this[i]);
    }
    printf("-------------\n");
    printf(">>> dest rest msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_rest[%d]: 0x%x.\n", i, dest_rest[i]);
    }
    printf("-------------\n");

    if (ret == mixup_len) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_serial_find__two_msg_with_correct_header_and_maxlen_limit(
        const char* payload_for_test)
{
    printf("\n======= serial_find() test ========\n");
    printf(">>> Two msg is passed in, with mix-up header, and max msg len limit.\n");
    uint8_t encoded_msg[MAX_BUFF_SIZE];
    uint8_t msg_for_testing[MAX_BUFF_SIZE];
    size_t payload_len = strlen(payload_for_test);
    int msg_len = serial_encode((uint8_t*)payload_for_test, encoded_msg, payload_len);

    uint8_t tmp[2];
    tmp[0] = SERIAL_PARSER_HEADER;
    tmp[1] = 2 * msg_len + 1;
    size_t mixup_len = msg_mix_up(msg_for_testing, MAX_BUFF_SIZE, encoded_msg, msg_len,
                                  tmp, sizeof(tmp), encoded_msg, msg_len);
    printf(">>> msg for testing: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("encoded_msg[%d]: 0x%x.\n", i, msg_for_testing[i]);
    }
    printf("-------------\n");

    uint8_t dest_this[MAX_BUFF_SIZE];
    uint8_t dest_rest[MAX_BUFF_SIZE];
    for (int i = 0; i < MAX_BUFF_SIZE; ++i) {
        dest_this[i] = '\0';
        dest_rest[i] = '\0';
    }

    int ret = serial_find(msg_for_testing, dest_this, dest_rest, mixup_len, msg_len);

    printf(">>> dest this msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_this[%d]: 0x%x.\n", i, dest_this[i]);
    }
    printf("-------------\n");
    printf(">>> dest rest msg: \n");
    for (int i = 0; i < mixup_len; ++i) {
        printf("dest_rest[%d]: 0x%x.\n", i, dest_rest[i]);
    }
    printf("-------------\n");

    if (ret == msg_len) {
        return 0;
    }
    else {
        return ret;
    }
}

int main(void)
{
    TEST_RETURN(test_serial_encode__matched("123"));
    TEST_RETURN(test_serial_decode__correct("123"));
    TEST_RETURN(test_serial_decode__correct_but_overmore("123"));
    TEST_RETURN(test_serial_decode__header_unmatched("123"));
    TEST_RETURN(test_serial_decode__crc_err("123"));
    TEST_RETURN(test_serial_decode__header_faked_with_max_msglen("123"));
    TEST_RETURN(test_serial_decode__msg_uncomplete_because_header_faked_without_max_msglen(
                "123"));
    TEST_RETURN(test_serial_find__found_1st_msg("123"));
    TEST_RETURN(test_serial_find__correct_header_but_smaller_len("123"));
    TEST_RETURN(test_serial_find__correct_header_but_bigger_len("123"));
    TEST_RETURN(test_serial_find__no_correct_crc("123"));
    TEST_RETURN(test_serial_find__two_msg_without_correct_header("123"));
    TEST_RETURN(test_serial_find__two_msg_with_correct_header_but_no_maxlen_limit("123"));
    TEST_RETURN(test_serial_find__two_msg_with_correct_header_and_maxlen_limit("123"));
    return 0;
}
