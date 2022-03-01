
#ifndef LOGICLAYER_H__
#define LOGICLAYER_H__

#include "epdk.h"

__s32 GUI_LogicLayerSetScnWin(__u8 dir, ES_FILE *disp_handle, __hdle hlayer, __rect_t *rect);

__s32 GUI_LogicLayerSetSrcWin(__u8 dir, ES_FILE *disp_handle, __hdle hlayer, __rect_t *rect);

__s32 GUI_LogicLayerSetPara(__u8 dir, ES_FILE *disp_handle, __hdle hlayer, __disp_layer_para_t *player);

__s32 GUI_LogicLayerRequest(__u8 dir, ES_FILE *disp_handle, __hdle hlayer, __disp_layer_para_t *player);

__s32 GUI_LogicLayerSetFb(__u8 dir, ES_FILE *disp_handle, __hdle hlayer, FB *pfb);

__s32 GUI_LogicSpriteBlockRequest(__u8 dir, ES_FILE *disp_handle, __hdle hblock, __disp_sprite_block_para_t *para);

__s32 GUI_LogicSpriteBlockSetScnWin(__u8 dir, ES_FILE *disp_handle, __hdle hblock, __rect_t *scn_rect);

#endif

