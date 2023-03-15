/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
            case KPAD_UP :
                printf("KEY_UP press\n");
                break;
            case KPAD_DOWN :
                printf("KEY_DOWN press\n");
                break;
            case KPAD_MENU :
                printf("KEY_MENU press\n");
                break;
            case KPAD_ENTER :
                printf("KEY_ENTER press\n");
                break;
            case KPAD_RETURN :
                printf("KEY_RETURN press\n");
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
