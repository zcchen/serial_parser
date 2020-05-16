#include "serial_parser/misc/crc16.h"
#include "testing_helper.h"
#include <stdint.h>
#include <string.h>

uint16_t crc16_benchmark(const uint8_t *data, size_t size)
{
#define CRC16_MAGIC 0x8005
    uint16_t out = 0;
    int bits_read = 0, bit_flag;

    /* Sanity check: */
    if(data == NULL)
        return 0;

    while(size > 0)
    {
        bit_flag = out >> 15;
        /* Get next bit: */
        out <<= 1;
        out |= (*data >> bits_read) & 1; // item a) work from the least significant bits
        bits_read++;
        if(bits_read > 7)
        {
            bits_read = 0;
            data++;
            size--;
        }
        /* Cycle check: */
        if(bit_flag)
            out ^= CRC16_MAGIC;
    }
    // item b) "push out" the last 16 bits
    int i;
    for (i = 0; i < 16; ++i) {
        bit_flag = out >> 15;
        out <<= 1;
        if(bit_flag)
            out ^= CRC16_MAGIC;
    }
    // item c) reverse the bits
    uint16_t crc = 0;
    i = 0x8000;
    int j = 0x0001;
    for (; i != 0; i >>=1, j <<= 1) {
        if (i & out) crc |= j;
    }
    return crc;
#undef CRC16_MAGIC
}

int test_crc16(char *s)
{
    uint16_t impl_ret, benchmark_ret;
    impl_ret = crc16(0, (uint8_t*)s, strlen(s));
    benchmark_ret = crc16_benchmark((uint8_t*)s, strlen(s));
    printf("The crc result of \"%s\" is 0x%x (impl) & 0x%x (benchmark)\n",
            s, impl_ret, benchmark_ret);
    if (impl_ret == benchmark_ret) {
        return 0;
    }
    else {
        return 1;
    }
}

int main(void)
{
    TEST_RETURN(test_crc16("123456789"));
    TEST_RETURN(test_crc16("abcdefg"));
    return 0;
}
