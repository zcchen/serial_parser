#include "testing_helper.h"
#include "command_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_PAYLOAD_SIZE 16

int clean_commands(void);
int run_command(int *func_ret, const uint8_t cmd_index,
               const void *param_struct, const size_t param_size);

int get_command_from_payload(const uint8_t* payload, const size_t payload_size,
                             uint8_t *cmd_got, void *param_struct, size_t param_size);

struct tmp_param_struct_t {
    uint8_t abc;
} __attribute__((packed));

int mockup_func_4_testing(const void *param, const size_t len)
{
    printf("mockup func..\n");
    printf("The param pointer is <%p>.\n", param);
    return 0;
}

int test__command_handler__add__normal(void)
{
    clean_commands();
    printf("\n---------- test: command_handler__add() -----------------\n");
    printf(">>> Added one mockup func, and then exec it...\n");

    uint8_t cmd_num = 1;
    command_handler__add(cmd_num, &mockup_func_4_testing);
    int mockup_ret = 0;
    int ret = run_command(&mockup_ret, cmd_num, NULL, 0);

    printf("--------------------------------------------------\n");
    clean_commands();

    return ret;
}

int test__get_command_from_payload__void_param(void)
{
    clean_commands();
    printf("\n---------- test: get_command_from_payload() -----------------\n");
    printf(">>> get command from 'payload + null payload'...\n");

    uint8_t cmd_num = 1;
    command_handler__add(cmd_num, &mockup_func_4_testing);

    uint8_t payload[1];
    payload[0] = cmd_num;

    uint8_t cmd_got;
    void *param_struct = NULL;
    int ret = get_command_from_payload(payload, sizeof(payload), &cmd_got, param_struct, 0);

    printf("--------------------------------------------------\n");
    clean_commands();

    assert(cmd_got == cmd_num);
    assert(param_struct == NULL);
    if (ret == COMMAND_HANLDER_ERR_EMPTY_INPUT) {
        return 0;
    }
    return ret;
}

int test__get_command_from_payload__with_param(void)
{
    clean_commands();
    printf("\n---------- test: get_command_from_payload() -----------------\n");
    printf(">>> get command from 'payload + null payload'...\n");

    uint8_t cmd_num = 2;
    command_handler__add(cmd_num, &mockup_func_4_testing);

    uint8_t payload[2];
    payload[0] = cmd_num;
    payload[1] = 0x7F;

    uint8_t cmd_got = 0;
    struct tmp_param_struct_t param_struct;

    int ret = get_command_from_payload(payload, sizeof(payload),
                                       &cmd_got, &param_struct, sizeof(param_struct));

    for (int i = 0; i < sizeof(payload); ++i) {
        printf("payload[%d]: 0x%x\n", i, payload[i]);
    }
    printf("param_struct.sizeof(): %ld\n", sizeof(param_struct));
    printf("param_struct.abc     : 0x%x\n", param_struct.abc);
    printf("--- ret: <%d> ------------------------------------\n", ret);
    clean_commands();

    assert(cmd_got == payload[0]);
    assert(param_struct.abc == payload[1]);
    return ret;
}

int command_handler__exec__checking(const uint8_t* payload, const size_t payload_size,
        uint8_t expect_cmd_got, int expect_exec_ret, int expect_func_ret)
{
    printf(">>> payload: \n");
    for (int i = 0; i < payload_size; ++i) {
        printf(">>> payload[%d]: 0x%x\n", i, payload[i]);
    }
    printf("------------------------------\n");
    uint8_t cmd_got = 0;
    int exec_ret = 0;
    int cmd_func_ret = 0;
    command_handler__exec(payload, payload_size, &cmd_got, &exec_ret, &cmd_func_ret);
    assert(cmd_got == expect_cmd_got);
    assert(exec_ret == expect_exec_ret);
    assert(cmd_func_ret == expect_func_ret);
    return 0;
}

int test__command_handler__exec__normally(void)
{
    clean_commands();
    printf("\n---------- test: command_handler__exec() ---------------\n");
    printf(">>> exec the command handler testing...\n");

    command_handler__add(1, &mockup_func_4_testing);
    command_handler__add(2, &mockup_func_4_testing);

    uint8_t payload[MAX_PAYLOAD_SIZE];
    for (int i = 0; i < MAX_PAYLOAD_SIZE; ++i) {
        payload[i] = '\0';
    }

    payload[0] = 1;
    command_handler__exec__checking(payload, 1, 1, 0, 0);

    payload[0] = 2;
    payload[1] = 0x7F;
    command_handler__exec__checking(payload, 2, 2, 0, 0);

    payload[0] = 3;
    payload[1] = 0x7F;
    command_handler__exec__checking(payload, 2, 3, COMMAND_HANLDER_ERR_CMD_NOT_FOUND, 0);

    return 0;
}

int main(void)
{
    TEST_RETURN(test__command_handler__add__normal());
    TEST_RETURN(test__get_command_from_payload__void_param());
    TEST_RETURN(test__get_command_from_payload__with_param());
    TEST_RETURN(test__command_handler__exec__normally());
    return 0;
}
