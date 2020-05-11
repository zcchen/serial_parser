#ifndef CTOOLINGS_SERIALHEX2STRUCT_H
#define CTOOLINGS_SERIALHEX2STRUCT_H

#include <stdint.h>
#include <stdlib.h>

typedef unsigned char uint8_t;

enum serialhex_2_struct_err_t {
    SERIALHEX_2_STRUCT_OK = 0,
    SERIALHEX_2_STRUCT_ERR_UNKNOWN = 1,
    SERIALHEX_2_STRUCT_ERR_EMPTY_INPUT = 2,
    SERIALHEX_2_STRUCT_ERR_SIZE_UNMATCHED = 3,
    SERIALHEX_2_STRUCT_ERR_COPY_FAILED = 4,
};

int _shadow_serialhex_2_struct(const uint8_t *data, const size_t data_len,
                               void* ret_struct, size_t struct_size);
// because the `ret_struct` is undefined yet, the endian-convection should be done by you.

#define serialhex_2_struct(serial_data, data_len, ret_struct)   \
        _shadow_serialhex_2_struct(serial_data, data_len, &ret_struct, sizeof(ret_struct))

#endif /* ifndef CTOOLINGS_SERIALHEX2STRUCT_H */
