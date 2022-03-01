#ifndef __MINI_MUSIC_H__
#define __MINI_MUSIC_H__

enum
{
    MUSIC_CMD_AUTO_PLAY_NEXT = GUI_MSG_USER_DEF + 1,
    MUSIC_CMD_ERROR,
    MUSIC_CMD_STATE_CHANGE,
    MUSIC_CMD_
};

__s32   mini_music_open(H_WIN parent, __s32 x, __s32 y);
__s32   mini_music_close(void);
__bool  mini_music_scene_exist(void);



#endif /* __MUSIC_SCENE_H__ */
