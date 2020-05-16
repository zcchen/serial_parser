#include "serial_parser/serial_parser.h"
#include "testing_helper.h"
#include <stdint.h>

union data16_t {
    uint8_t c[2];
    uint16_t uint;
};

union data32_t {
    uint8_t c[4];
    uint32_t uint;
};

int test_endian_local_big_to_little_16(void)
{
    union data16_t d = {{0x12, 0x34}};
    if (local2serial_16(d.uint) == 0x3412) {
        return 0;
    }
    else {
        return 1;
    }
}

int test_endian_local_big_to_little_32(void)
{
    union data32_t d = {{0x12, 0x34, 0x56, 0x78}};
    if (local2serial_32(d.uint) == 0x78563412) {
        return 0;
    }
    else {
        return 1;
    }
}

int test_endian_little_to_local_big_16(void)
{
    union data16_t d = {{0x12, 0x34}};
    if (serial2local_16(d.uint) == 0x3412) {
        return 0;
    }
    else {
        return 1;
    }
}

int test_endian_little_to_local_big_32(void)
{
    union data32_t d = {{0x12, 0x34, 0x56, 0x78}};
    if (serial2local_32(d.uint) == 0x78563412) {
        return 0;
    }
    else {
        return 1;
    }
}

int main(void)
{
    TEST_RETURN(test_endian_local_big_to_little_16());
    TEST_RETURN(test_endian_local_big_to_little_32());
    return 0;
}
