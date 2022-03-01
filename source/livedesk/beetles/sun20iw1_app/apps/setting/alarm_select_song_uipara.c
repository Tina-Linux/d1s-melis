
#include "alarm_select_song_uipara.h"

static alarm_sel_song_uipara_t alarm_sel_song_uipara ;

alarm_sel_song_uipara_t *alarm_sel_song_get_uipara(void)
{
    alarm_sel_song_uipara.FbRect.x = 0;
    alarm_sel_song_uipara.FbRect.y = 0;
    alarm_sel_song_uipara.FbRect.w = 240;
    alarm_sel_song_uipara.FbRect.h = 200;
    alarm_sel_song_uipara.ScnWinRect.x = (480 - 240) / 2;
    alarm_sel_song_uipara.ScnWinRect.y = (272 - 200) / 2;
    alarm_sel_song_uipara.ScnWinRect.w = 240;
    alarm_sel_song_uipara.ScnWinRect.h = 200;
    alarm_sel_song_uipara.SrcWinRect.x  = 0;
    alarm_sel_song_uipara.SrcWinRect.y  = 0;
    alarm_sel_song_uipara.SrcWinRect.w  = 240;
    alarm_sel_song_uipara.SrcWinRect.h  = 200;
    alarm_sel_song_uipara.ListBarAreaRect.x = 30;
    alarm_sel_song_uipara.ListBarAreaRect.y = 5;
    alarm_sel_song_uipara.ListBarAreaRect.w = 182;
    alarm_sel_song_uipara.ListBarAreaRect.h = 144;
    alarm_sel_song_uipara.ListBarItemRect.x = 1;    //ListBarItemRect 是相对于ListBarAreaRect 来说的
    alarm_sel_song_uipara.ListBarItemRect.y = 0;
    alarm_sel_song_uipara.ListBarItemRect.w = 180;
    alarm_sel_song_uipara.ListBarItemRect.h = 24;
    alarm_sel_song_uipara.OkBmpRect.x = 30;
    alarm_sel_song_uipara.OkBmpRect.y = 162;
    alarm_sel_song_uipara.OkBmpRect.w = 37;
    alarm_sel_song_uipara.OkBmpRect.h = 18;
    alarm_sel_song_uipara.CancleBmpRect.x = 164;
    alarm_sel_song_uipara.CancleBmpRect.y = 162;
    alarm_sel_song_uipara.CancleBmpRect.w = 37;
    alarm_sel_song_uipara.CancleBmpRect.h = 18;
    alarm_sel_song_uipara.ListenTestBmpRect.x = 97;
    alarm_sel_song_uipara.ListenTestBmpRect.y = 162;
    alarm_sel_song_uipara.ListenTestBmpRect.w = 37;
    alarm_sel_song_uipara.ListenTestBmpRect.h = 18;
    return &alarm_sel_song_uipara;
}

