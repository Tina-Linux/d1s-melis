
#include "scan_proc_uipara.h"

static dvb_scan_proc_uipara_t dvb_scan_proc_uipara =
    /* _480_270  */
{
    {0, 50, 480, 44},
    {0, 50 + 44, 480, 44},
    {0, 50 + 44 + 44, 480, 44},
    {(480 - 300) / 2, (190)}
};

dvb_scan_proc_uipara_t *submenu_dvb_scan_proc_get_uipara()
{
    __s32           screen_width;
    __s32           screen_height;

    /* get lcd size */
    //æ”÷–œ‘ æ
    if (dsk_display_get_size((__s32 *)&screen_width, (__s32 *)&screen_height) == EPDK_OK)
    {
        dvb_scan_proc_uipara.dvb_scan_str.x = 0;
        dvb_scan_proc_uipara.dvb_scan_str.y = (screen_height - 204) / 2;
        dvb_scan_proc_uipara.dvb_scan_str.width = screen_width;
        dvb_scan_proc_uipara.dvb_scan_str.height = 48;
        dvb_scan_proc_uipara.dvb_scan_freq.x = 0;
        dvb_scan_proc_uipara.dvb_scan_freq.y = (screen_height - 204) / 2 + 48;
        dvb_scan_proc_uipara.dvb_scan_freq.width = screen_width;
        dvb_scan_proc_uipara.dvb_scan_freq.height = 48;
        dvb_scan_proc_uipara.dvb_scan_ch.x = 0;
        dvb_scan_proc_uipara.dvb_scan_ch.y = (screen_height - 204) / 2 + 48 * 2;
        dvb_scan_proc_uipara.dvb_scan_ch.width = screen_width;
        dvb_scan_proc_uipara.dvb_scan_ch.height = 48;
        dvb_scan_proc_uipara.dvb_scan_proc_bmp_pos.x = (screen_width - 300) / 2;
        dvb_scan_proc_uipara.dvb_scan_proc_bmp_pos.y = (screen_height - 204) / 2 + 48 * 3;
    }

    return &dvb_scan_proc_uipara;
}


