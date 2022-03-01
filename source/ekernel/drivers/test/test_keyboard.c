#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>

#include <sunxi_input.h>
#include <sunxi_drv_keyboard.h>

#define NUM_OF_KEYS 5

int cmd_keyboard(int argc, char **argv)
{
    int i, ret = -1;
    int key_count = 0;
    int fd = -1;
    int press_key_code[NUM_OF_KEYS] = {0};
    struct sunxi_input_event event;

    printf("========KEY TEST========\n");

    fd = sunxi_input_open("sunxi-keyboard");
    if (fd < 0)
    {
        printf("====keyboard open err====\n");
        printf("must run sunxi_keyboard_init\n");
        return -1;
    }

    printf("press all keys\n");
    while (key_count < NUM_OF_KEYS)
    {
        if (sunxi_input_read(fd, &event, sizeof(struct sunxi_input_event)) < 0)
        {
            continue;
        }

        if (event.type != EV_KEY)
        {
            continue;
        }

        printf("=====key code %d====\n", event.code);
        printf("=====key value  %d====\n", event.value);

        switch (event.code)
        {
            case KEY_VOLUMEUP :
                printf("KEY_VOLUMEUP press\n");
                break;
            case KEY_VOLUMEDOWN :
                printf("KEY_VOLUMEDOWN press\n");
                break;
            case KEY_MENU :
                printf("KEY_MENU press\n");
                break;
            case KEY_ENTER :
                printf("KEY_ENTER press\n");
                break;
            case KEY_HOME :
                printf("KEY_HOME press\n");
                break;

            default :
                printf("unknow key press\n");
                break;
        }

        for (i = 0; i < NUM_OF_KEYS; i++)
        {
            if (press_key_code[i] == event.code)
            {
                break;
            }
        }

        if (i == NUM_OF_KEYS)
        {
            press_key_code[key_count++] = event.code;
        }
    }

    printf("all keys press\n");

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_keyboard, cmd_keyboard, keyboard test code);
