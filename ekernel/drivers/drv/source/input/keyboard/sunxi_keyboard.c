#include <stdio.h>
#include <stdlib.h>
#include <sunxi_drv_keyboard.h>
#include <sunxi_input.h>
#include <log.h>
#include <init.h>
#include <kapi.h>

extern int32_t console_LKeyDevEvent(__input_dev_t *dev, uint32_t type, uint32_t code, int32_t value);


static uint32_t key_flag = 0;
uint8_t filter_cnt = 5;

#if defined(CONFIG_SUNXI_QA_TEST)
struct sunxikbd_config key_config =
{
    .measure = 1800,
    .key_num = 5,
    .key_vol = {210, 410, 590, 750, 880},
    .scankeycodes = {KPAD_UP, KPAD_DOWN, KPAD_ENTER, KEY_RESERVED, KPAD_RETURN},
    .name = "sunxi-keyboard"
};
#else
struct sunxikbd_config key_config =
{
    .measure = 1800,
    .key_num = 5,
    .key_vol = {210, 410, 590, 750, 880},
    .scankeycodes = {KPAD_UP, KPAD_DOWN, KPAD_ENTER, KPAD_MENU, KPAD_RETURN},
    .name = "sunxi-keyboard"
};

#endif
static unsigned char keypad_mapindex[128] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0,      /* key 1, 0-8 */
    1, 1, 1, 1, 1,                  /* key 2, 9-13 */
    2, 2, 2, 2, 2, 2,               /* key 3, 14-19 */
    3, 3, 3, 3, 3, 3,               /* key 4, 20-25 */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    /* key 5, 26-36 */
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,    /* key 6, 37-39 */
    6, 6, 6, 6, 6, 6, 6, 6, 6,      /* key 7, 40-49 */
    7, 7, 7, 7, 7, 7, 7             /* key 8, 50-63 */
};

struct sunxikbd_drv_data *key_data  = NULL;
struct sunxi_input_dev *sunxikbd_dev = NULL;

int keyboard_irq_callback(uint32_t data_type, uint32_t data)
{
    uint32_t vol_data;
    uint8_t ch_num;
    if (data_type == GPADC_UP && key_flag == 1)
    {
        key_data->compare_later = 0;
        key_data->key_cnt = 0;
        key_flag = 0;
        __inf("GPADC_UP");
        console_LKeyDevEvent(NULL,  EV_KEY,     key_data->scankeycodes[key_data->key_code],  0);
        console_LKeyDevEvent(NULL,  EV_SYN,     0,                  0);
        // input_report_key(sunxikbd_dev, key_data->scankeycodes[key_data->key_code], 0);
        // input_sync(sunxikbd_dev);
    }
    data = ((VOL_RANGE / 4096) * data); /* 12bits sample rate */
    vol_data = data / 1000;

    if (vol_data < SUNXIKEY_DOWN)
    {
        /* MAX compare_before = 128 */
        key_data->compare_before = ((data / SCALE_UNIT) / 1000) & 0xff;;
        if (key_data->compare_before >= key_data->compare_later - 1
            && key_data->compare_before <= key_data->compare_later + 1)
        {
            key_data->key_cnt++;
        }
        else
        {
            key_data->key_cnt = 0;
        }
        key_data->compare_later = key_data->compare_before;
        if (key_data->key_cnt >= filter_cnt)
        {
            key_data->compare_later = key_data->compare_before;
            key_data->key_code = keypad_mapindex[key_data->compare_before];
            key_data->compare_later = 0;
            key_data->key_cnt = 0;
            if (key_data->key_code  < key_config.key_num)
            {
                if (key_flag == 0)
                {
                    __inf("input_report_key  %x", key_data->scankeycodes[key_data->key_code]);
                    //input_report_key(sunxikbd_dev, key_data->scankeycodes[key_data->key_code], 1);
                    // input_sync(sunxikbd_dev);
                    console_LKeyDevEvent(NULL,  EV_KEY,  key_data->scankeycodes[key_data->key_code],  1);
                    console_LKeyDevEvent(NULL,  EV_SYN,  0,  0);
                    key_flag = 1;
                }


            }
        }
    }

    return 0;
}
static int sunxikbd_data_init(struct sunxikbd_drv_data *key_data, struct sunxikbd_config *sunxikbd_config)
{
    int i, j = 0;
    int key_num = 0;
    unsigned int resol;
    unsigned int key_vol[KEY_MAX_CNT];

    key_num = sunxikbd_config->key_num;
    if (key_num < 1 || key_num > KEY_MAX_CNT)
    {
        return -1;
    }

    resol = sunxikbd_config->measure / MAXIMUM_SCALE;

    for (i = 0; i < key_num; i++)
    {
        key_data->scankeycodes[i] = sunxikbd_config->scankeycodes[i];
    }

    for (i = 0; i < key_num; i++)
    {
        key_vol[i] = sunxikbd_config->key_vol[i];
    }
    //key_vol[key_num] = sunxikbd_config->measure;

    for (i = 0; i < (key_num - 1); i++)
    {
        key_vol[i] += (key_vol[i + 1] - key_vol[i]) / 2;
    }

    for (i = 0; i < MAXIMUM_SCALE; i++)
    {
        if (i * resol > key_vol[j])
        {
            j++;
        }
        keypad_mapindex[i] = j;
    }

    key_data->last_key_code = INITIAL_VALUE;

    return 0;
}

int sunxi_keyboard_init(void)
{
    int i;
    //__log("sunxi keyboard init.");

    key_data = (struct sunxikbd_drv_data *)malloc(sizeof(struct sunxikbd_drv_data));

    if (key_data == NULL)
    {
        SUNXIKBD_ERR("alloc key data memory failed.\n");
        return -1;
    }

    memset(key_data, 0x00, sizeof(struct sunxikbd_drv_data));
    if (sunxikbd_data_init(key_data, &key_config) < 0)
    {
        SUNXIKBD_ERR("init sunxikbd data failed.\n");
        return -1;
    }

    sunxikbd_dev = sunxi_input_allocate_device();
    if (sunxikbd_dev == NULL)
    {
        SUNXIKBD_ERR("init sunxikbd dev failed.\n");
        return -1;
    }

    sunxikbd_dev->name = key_config.name;
    for (i = 0; i < key_config.key_num; i++)
    {
        input_set_capability(sunxikbd_dev, EV_KEY, key_data->scankeycodes[i]);
    }

    sunxi_input_register_device(sunxikbd_dev);

    hal_gpadc_init();
    hal_gpadc_channel_init(GP_CH_0);
    hal_gpadc_register_callback(GP_CH_0, keyboard_irq_callback);
    return 0;
}
late_initcall(sunxi_keyboard_init);
