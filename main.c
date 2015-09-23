#include <stdio.h>
#include "posix_tai.h"

int main(void)
{
    generate_leap_second_tbl(NULL);
    int res = posix_tai_unit_test();
    if (res < 0) {
        fprintf(stderr, "Unit test failure: %d\n", res);
        return 1;
    }
    puts("Unit test succeeded");
    return 0;
}

