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
#ifndef __PLAYER_FACE_H__
#define __PLAYER_FACE_H__



#include "beetles_app.h"
#include "music_list.h"
#include "music_player.h"


#define         MusicSetTransColor()    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);   \
    GUI_SetBkColor(0xFF2B2E36);\
    GUI_SetColor(GUI_WHITE)

#define         MusicSetInfoTransColor()    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);   \
    GUI_SetBkColor(0xC2434343);\
    GUI_SetColor(GUI_WHITE)

extern __s32 DrawMusicPlayModeStatus(rat_play_mode_e mode);
extern __s32 DrawMusicEQModeStatus(music_player_ctrl_t *this);
extern __s32 DrawMusicBLLastTimeStatus(music_BL_time_e BL_time);
extern __s32 DrawMusicVolumeStatus(__s32 volume);
extern __s32 DrawMusicBatteryStatus(power_level_e level);
extern __s32 DrawMusicSystemTime(__awos_time_t *sys_time);


extern __s32 DrawMusicPlayModeButton(rat_play_mode_e mode, music_btn_status_e status);
extern __s32 DrawMusicEQModeButton(music_EQ_mode_e mode, music_btn_status_e status);
extern __s32 DrawMusicBLTimeButton(music_BL_time_e time, music_btn_status_e status);
extern __s32 DrawMusicPlayPreButton(music_btn_status_e status);
extern __s32 DrawMusicPlayNextButton(music_btn_status_e status);
extern __s32 DrawMusicPlayPauseButton(music_btn_status_e status);
extern __s32 DrawMusicPlayBackwardButton(music_btn_status_e status);
extern __s32 DrawMusicPlayForwardButton(music_btn_status_e status);

extern __s32 ClearMusicPlayerContrlButtonBG(void);
extern __s32 DrawMusicPlayerContrlButton(music_player_ctrl_t *this);
extern __s32 DrawMusicPlayerContrlButtonByID(music_player_ctrl_t *this, __s32 menu_id, music_btn_status_e status);

extern void DrawMusicPlayingTimeState(__u32 curTime, __u32 total);

extern __s32 DrawMusicPlayStatusButton(music_play_status_e status);
extern __s32 CleanMusicPlayingTime(void);
extern __s32 DrawMusicPlayingTime(__u32 time);
extern __s32 CleanMusicTotalTime(void);
extern __s32 DrawMusicTotalTime(__u32 time);
extern __s32 DrawMusicProgressBarPoint(__u32 total, __u32 cur_value);
extern __s32 DrawMusicProgressBar(__u32 total, __u32 cur_value);
extern __s32 DrawMusicPerformerName(audio_file_info_t *file_info);
extern __s32 DrawMusicFileSize(__s32 file_size);
extern __s32 DrawMusicFileAlbum(audio_file_info_t *file_info);
extern __s32 DrawMusicFileBitRate(__s32 bit_rate);
extern __s32 CleanMusicButtonHintText(void);
extern __s32 DrawMusicPlayerFileInfo(audio_file_info_t *file_info);

extern void DrawMusicBottomBg(void);
extern void DrawMusicInfoBg(void);
extern void DrawMusicBg(void);
extern void DrawMusicPlayBar(void);

extern __s32 DrawMusicAlbum(music_player_ctrl_t *This, const char *file, GUI_RECT *clear);

extern void DrawMusicBottom(music_player_ctrl_t *this, __s16 focusedIndex);

extern __s32 DrawMusicHeadbar(music_player_ctrl_t *player_ctrl);
extern __s32 DrawMusicPlayer(music_player_ctrl_t *player_ctrl);

extern __s32 DrawMusicPlayerInfo(music_player_ctrl_t *this, __s32 index);

extern __s32 draw_music_player_background(music_player_ctrl_t *player_ctrl);

//extern __s32 music_player_draw_background(music_player_ctrl_t *player_ctrl);
__s32 DrawMenuBtnText(__s32 TextID);
__s32 DrawMusicMenuBtn(music_btn_status_e status);
__s32 DrawMusicShowModeMenu(music_btn_status_e status);
__s32 DrawMusicMenuAll(music_player_ctrl_t *this);

__s32 DrawMusicABMenu(music_btn_status_e status);
__s32 DrawShowTACKText(__s32 TextID);




#endif
