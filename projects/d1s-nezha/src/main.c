#include <stdio.h>

int app_entry(void *param)
{
    printf("enter %s\n", __func__);
    return 0;
}
