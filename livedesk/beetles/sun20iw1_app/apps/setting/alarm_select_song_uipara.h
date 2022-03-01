
#ifndef __ALARM_SELECT_SONG_UIPARA_H_
#define  __ALARM_SELECT_SONG_UIPARA_H_

#include "app_setting_i.h"

typedef struct alarm_sel_song_pos_s
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
} alarm_sel_song_pos_t;

typedef struct alarm_sel_song_uipara_s
{
    alarm_sel_song_pos_t    FbRect ;
    alarm_sel_song_pos_t    ScnWinRect ;
    alarm_sel_song_pos_t    SrcWinRect ;
    alarm_sel_song_pos_t    ListBarAreaRect ;
    alarm_sel_song_pos_t    ListBarItemRect;
    alarm_sel_song_pos_t    OkBmpRect ;
    alarm_sel_song_pos_t    CancleBmpRect ;
    alarm_sel_song_pos_t    ListenTestBmpRect ;

} alarm_sel_song_uipara_t;


extern alarm_sel_song_uipara_t *alarm_sel_song_get_uipara(void);

#endif      /*__ALARM_SELECT_SONG_UIPARA_H_*/


