#ifndef __RECORD_LIST_VIEW_DOC_H__
#define __RECORD_LIST_VIEW_DOC_H__


#include "record_ui.h"

#define RECORD_MUSIC_NAME_HEIGHT    38//19
#define RECORD_MUSIC_LIST_NUM       5//10   //RECORD_MUSIC_NAME_HEIGHT*RECORD_MUSIC_LIST_NUM < 192

#define RECORD_LIST_SPCTRM_LEVEL    20

#define REC_LIST_TIMER_TIMES        80
#define VOLUME_EXIST_TIMES          (10*100/REC_LIST_TIMER_TIMES)       //10s


#define RECORD_LIST_FILTER                  ".mp3|.wav|.wma"

typedef struct tag_record_list_doc
{
    __s32   curRootType;
    char    curSearchPath[1024];

    __s32   total;
    __s32   curIndex;
    __s32   playIndex;
    __s32   topIndex;

    __u16   oldSpctrmLevel[RECORD_LIST_SPCTRM_LEVEL];
    __u16   spctrmLevel[RECORD_LIST_SPCTRM_LEVEL];

    __cedar_status_t    playSt;

    __u32   musicTimeID;
    __u32   musicMaxTimes;
    __u32   musicCurTimes;

    HRAT    rat_handle;

} record_list_doc_t, *precord_list_doc_t;







//Rat ³õÊ¼»¯
extern __s32 record_list_MusicInit(precord_list_doc_t precord_list_doc);
//rat release
extern __s32 record_list_MusciUninit(precord_list_doc_t precord_list_doc);

extern __s32 record_list_getFileName(precord_list_doc_t precord_list_doc, __s32 index, char *name);
extern __s32 record_list_getFilePath(precord_list_doc_t precord_list_doc, __s32 index, char *name);
extern __s32 record_list_play(precord_list_doc_t precord_list_doc);
extern __s32 record_list_playNext(precord_list_doc_t precord_list_doc);
extern __s32 record_list_continue(precord_list_doc_t precord_list_doc);
extern __s32 record_list_pause(precord_list_doc_t precord_list_doc);
extern __s32 record_list_next(precord_list_doc_t precord_list_doc);
extern __s32 record_list_pre(precord_list_doc_t precord_list_doc);
extern __s32 record_list_backward(precord_list_doc_t precord_list_doc);
extern __s32 record_list_forward(precord_list_doc_t precord_list_doc);
extern __s32 record_list_delete(precord_list_doc_t precord_list_doc);
extern __s32 record_list_timer(precord_list_doc_t precord_list_doc);
















#endif//__RECORD_LIST_VIEW_DOC_H__
