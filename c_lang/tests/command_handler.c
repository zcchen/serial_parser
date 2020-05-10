#include "testing_helper.h"
#include "command_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int mockup_func_4_testing(const void *param, const size_t len)
{
    printf("mockup func\n");
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

int test__get_command_from_payload__void_payload(void)
{
    clean_commands();
    printf("\n---------- test: get_command_from_payload() -----------------\n");
    printf(">>> get command from 'payload + null payload'...\n");

    uint8_t cmd_num = 1;
    add_command(cmd_num, &mockup_func_4_testing);

    uint8_t payload[1];
    payload[0] = cmd_num;
    int mockup_ret = 0;

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

int main(void)
{
    TEST_RETURN(test__add_command__normal());
    TEST_RETURN(test__get_command_from_payload__void_payload());
    return 0;
}
