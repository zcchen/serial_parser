#include "testing_helper.h"
#include "command_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int mockup_func_4_testing(const void *param, const size_t len)
{
    printf("mockup func..\n");
    printf("The param pointer is <%p>.\n", param);
    return 0;
}

int test__add_command__normal(void)
{
    clean_commands();
    printf("\n---------- test: add_command() -----------------\n");
    printf(">>> Added one mockup func, and then exec it...\n");

    uint8_t cmd_num = 1;
    add_command(cmd_num, &mockup_func_4_testing);
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
    add_command(cmd_num, &mockup_func_4_testing);

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
    add_command(cmd_num, &mockup_func_4_testing);

    uint8_t payload[2];
    payload[0] = cmd_num;
    payload[1] = 0x7F;

    uint8_t cmd_got = 0;
    struct tmp_param_struct_t {
        uint8_t abc;
    } __attribute__((packed));
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

int main(void)
{
    TEST_RETURN(test__add_command__normal());
    TEST_RETURN(test__get_command_from_payload__void_param());
    TEST_RETURN(test__get_command_from_payload__with_param());
    return 0;
}
