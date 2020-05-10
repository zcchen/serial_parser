#include "testing_helper.h"
#include "command_handler.h"
#include <stdio.h>
#include <stdlib.h>

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

int main(void)
{
    TEST_RETURN(test__add_command__normal());
    return 0;
}
