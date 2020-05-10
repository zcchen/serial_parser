#include "serialhex2struct.h"
#include <string.h>

int _shadow_serialhex_2_struct(const uint8_t *data, const size_t data_len,
                               void* ret_struct, size_t struct_size)
{
    if (data == NULL || data_len == 0) {
        ret_struct = NULL;
        return SERIALHEX_2_STRUCT_ERR_EMPTY_INPUT;
    }
    if (data_len != struct_size) {
        return SERIALHEX_2_STRUCT_ERR_SIZE_UNMATCHED;
    }
    void *ret = memmove(ret_struct, data, data_len);
    if (ret == ret_struct) {
        return SERIALHEX_2_STRUCT_OK;
    }
    else {
        return SERIALHEX_2_STRUCT_ERR_UNKNOWN;
    }
}

