#ifndef COMMAND_HANLDER_H
#define COMMAND_HANLDER_H

#include <stdint.h>
#include <stddef.h>

typedef unsigned char uint8_t;

#define MAX_COMMAND_SUPPORT 128
#if (MAX_COMMAND_SUPPORT > 0xFF || MAX_COMMAND_SUPPORT < 0)
    #error("max command support is 255.")
#endif

enum command_handler_err_t {
    COMMAND_HANLDER_OK = 0,
    COMMAND_HANLDER_ERR_UNKNOWN = 1,
    COMMAND_HANLDER_ERR_CMD_NOT_FOUND = 2,
    COMMAND_HANLDER_ERR_CMD_EXISTS = 3,
    COMMAND_HANLDER_ERR_CMD_INVALID = 4,
    COMMAND_HANLDER_ERR_NO_SPACE = 5,
    COMMAND_HANLDER_ERR_NULL_FUNCTION = 6,
    COMMAND_HANLDER_ERR_EMPTY_INPUT = 7,
    COMMAND_HANLDER_ERR_SIZE_UNMATCHED = 8,
    COMMAND_HANLDER_ERR_CMD_MISSING = 9,
};

typedef int (*COMMAND_FUNC_T)(const void*, const size_t);

// once you added a command, it should not be redefined again.
int add_command(const uint8_t cmd_num, COMMAND_FUNC_T func_ptr);

int clean_commands(void);

/*
 *    // #define a macro for easy-to-use
 *#define get_command_from_payload(payload, payload_size, cmd_got, param_struct)    \
 *        __shadow_get_command_from_payload(payload, payload_size,        \
 *                                          &cmd_got, &param_struct, sizeof(param_struct))
 */

int run_command(int *func_ret, const uint8_t cmd_index,
               const void *param_struct, const size_t param_size);

int get_command_from_payload(const uint8_t* payload, const size_t payload_size,
                             uint8_t *cmd_got, void *param_struct, size_t param_size);

#endif /* ifndef COMMAND_HANLDER_H */
