#ifndef TESTING_HELPER_H
#define TESTING_HELPER_H

#include <stdio.h>

#define TEST_RETURN(ret)    {                                       \
    int r = ret;                                                    \
    if (r) {                                                        \
        printf("Error on <%s> at line: %d.\n", __FILE__, __LINE__); \
        printf("Function returns <%d>\n", r);                       \
        return r;                                                   \
    }                                                               \
}

#endif /* ifndef TESTING_HELPER_H */
