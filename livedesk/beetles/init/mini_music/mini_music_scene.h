#ifndef __MINI_MUSIC_SCENE_H__
#define __MINI_MUSIC_SCENE_H__

/*font define*/
#define SMALL_FONT_SIZE                     24
#define SMALL_FONT_FILE                     BEETLES_APP_ROOT"res\\fonts\\font24.sft"
#define BIG_FONT_SIZE                       32
#define BIG_FONT_FILE                       BEETLES_APP_ROOT"res\\fonts\\font32.sft"

typedef enum
{
    MINI_STC_ID_NAME = 0,
    MINI_STC_ID_TIME,
    MINI_STC_ID_NUM
} mini_static;

typedef enum
{
    MINI_BTN_ID_PRE = MINI_STC_ID_NUM,
    MINI_BTN_ID_NEXT,
    MINI_BTN_ID_PLAYPAUSE,
    MINI_BTN_ID_EXIT,
    MINI_BTN_ID_QUIT,
    MINI_BTN_ID_NUM
} mini_button;

/*typedef struct tag__MINI_ADD_DATA
{
    static_para_t *static_para;
    button_para_t *button_para;
}__mini_add_data_t;*/
typedef struct
{
    H_WIN       parent;         // ¸¸´°¿Ú
    __s32       x;
    __s32       y;
} __mini_music_create_para_t;

H_WIN   scene_create(__mini_music_create_para_t *create_para);
__s32   scene_delete(void);
__bool  scene_exist(void);


#endif /* __MUSIC_SCENE_H__ */
