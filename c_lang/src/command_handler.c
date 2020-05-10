#include "command_handler.h"
#include "enumkv/enumkv.h"
#include "serialhex2struct/serialhex2struct.h"
#include <stdbool.h>

struct enumkv_elem_t command_list[MAX_COMMAND_SUPPORT];
bool command_list_init_flag = false;

int command_list_init(void)
{
    if (!command_list_init_flag) {
        enumkv_list_init(command_list, MAX_COMMAND_SUPPORT);
        command_list_init_flag = true;
    }
    return 0;
}

int clean_commands(void)
{
    if (command_list_init_flag) {
        enumkv_list_init(command_list, MAX_COMMAND_SUPPORT);
        command_list_init_flag = false;
    }
    return 0;
}

inline static int conv__enumkv_err__2__command_handler_err(enum enumkv_err_t ret)
{
    switch (ret) {
        case ENUMKV_OK: return COMMAND_HANLDER_OK;
        case ENUMKV_ERR_KEY_NOT_FOUND: return COMMAND_HANLDER_ERR_CMD_NOT_FOUND;
        case ENUMKV_ERR_KEY_EXISTS: return COMMAND_HANLDER_ERR_CMD_EXISTS;
        case ENUMKV_ERR_KEY_INVALID: return COMMAND_HANLDER_ERR_CMD_INVALID;
        case ENUMKV_ERR_NO_SPACE: return COMMAND_HANLDER_ERR_NO_SPACE;
        default: return COMMAND_HANLDER_ERR_UNKNOWN;
    }
}

inline static int conv__serialhex2structr__2__command_handler_err(enum serialhex_2_struct_err_t ret)
{
    switch (ret) {
        case SERIALHEX_2_STRUCT_OK: return COMMAND_HANLDER_OK;
        case SERIALHEX_2_STRUCT_ERR_EMPTY_INPUT: return COMMAND_HANLDER_ERR_EMPTY_INPUT;
        case SERIALHEX_2_STRUCT_ERR_SIZE_UNMATCHED: return COMMAND_HANLDER_ERR_SIZE_UNMATCHED;
        default: return COMMAND_HANLDER_ERR_UNKNOWN;
    }
}

int add_command(const uint8_t cmd_num, COMMAND_FUNC_T func_ptr)
{
    if (!command_list_init_flag) {
        command_list_init();
    }
    if (cmd_num <= 0) {
        return COMMAND_HANLDER_ERR_CMD_INVALID;
    }
    if (func_ptr == NULL) {
        return COMMAND_HANLDER_ERR_NULL_FUNCTION;
    }
    struct enumkv_elem_t elem = {.enum_k = cmd_num, .value = func_ptr};
    int ret = enumkv_list_add(command_list, MAX_COMMAND_SUPPORT, &elem);
    return conv__enumkv_err__2__command_handler_err(ret);
}

int get_command_from_payload(const uint8_t* payload, const size_t payload_size,
                             uint8_t *cmd_got, void *param_struct, size_t param_size)
{
    if (payload_size < 1) {
        return COMMAND_HANLDER_ERR_CMD_MISSING;
    }
    *cmd_got = payload[0];
    if (payload_size == 1) {
        return COMMAND_HANLDER_OK;
    }
    else {
        int r = _shadow_serialhex_2_struct(payload + 1, payload_size - 1,
                                           &param_struct, param_size);
        return conv__serialhex2structr__2__command_handler_err(r);
    }
}

int run_command(int *func_ret, const uint8_t cmd_index,
                const void *param_struct, const size_t param_size)
{
    if (!command_list_init_flag) {
        command_list_init();
    }
    COMMAND_FUNC_T func_ptr = NULL;
    int find_ret = enumkv_list_find(command_list, MAX_COMMAND_SUPPORT,
                                    cmd_index, (void**)&func_ptr);
    if (find_ret) {
        return conv__enumkv_err__2__command_handler_err(find_ret);
    }
    if (func_ptr == NULL) {
        return COMMAND_HANLDER_ERR_NULL_FUNCTION;
    }
    *func_ret = func_ptr(param_struct, param_size);
    return COMMAND_HANLDER_OK;
}

