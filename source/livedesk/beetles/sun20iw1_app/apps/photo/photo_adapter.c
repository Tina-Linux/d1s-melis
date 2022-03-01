/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : photo_adapter.c
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/20 9:58  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "photo_adapter.h"
//////////////////////////////////////////////////////////////////////////
//是否立即保存注册表数据
#define SAVE_REG_IMMEDIATELY 0
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
//取速度
/************************************************************************/
__u32 photo_get_ratio(void)
{
    reg_photo_para_t *para;
    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    if (para)
    {
        return para->ratio;
    }
    else
    {
        __err("dsk_reg_get_para_by_app fail...\n");
        return 0;
    }
}

/************************************************************************/
//设置速度
/************************************************************************/
__u32 photo_set_ratio(__u32 ratio)
{
    reg_photo_para_t *para;
    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);
    para->ratio = ratio;
#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->ratio;
#endif
}

/************************************************************************/
//取速度标志(是否自定义速度)
/************************************************************************/
__u32 photo_get_speedflag(void)
{
    reg_photo_para_t *para;
    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    if (para)
    {
        return para->speedflag;
    }
    else
    {
        __err("dsk_reg_get_para_by_app fail...\n");
        return 0;
    }
}

/************************************************************************/
//设置速度标志(是否自定义速度)
/************************************************************************/
__u32 photo_set_speedflag(__u32 speedflag)
{
    reg_photo_para_t *para;
    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);
    para->speedflag = speedflag;
#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->speedflag;
#endif
}

/************************************************************************/
//取特效
/************************************************************************/
__u32 photo_get_effect(void)
{
    reg_photo_para_t *para;
    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);

    if (para)
    {
        return para->effect;
    }
    else
    {
        __err("dsk_reg_get_para_by_app fail...\n");
        return 0;
    }
}

/************************************************************************/
//保存特效
/************************************************************************/
__u32 photo_set_effect(__u32 effect)
{
    reg_photo_para_t *para;
    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);
    para->effect = effect;
#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->effect;
#endif
}

/************************************************************************/
//获取背景音乐开关
/************************************************************************/
__u32 photo_get_bgmusic(void)
{
    return is_app_exist(APP_MUSIC);
}

/************************************************************************/
//保存背景音乐开关
/************************************************************************/
__u32 photo_set_bgmusic(__u32 bgmusic)
{
    reg_photo_para_t *para;
    get_reg_pointer(para, reg_photo_para_t, REG_APP_PHOTO);
    para->bgmusic = bgmusic;
#if SAVE_REG_IMMEDIATELY
    return update_to_reg();
#else
    return para->bgmusic;
#endif
}

/************************************************************************/
//保存断点
/************************************************************************/
__s32 photo_save_play_info(rat_type_t root_type)
{
    __s32 index;
    char filename[MAX_FILE_NAME_LEN] = {0};
    index = anole_npl_get_cur();
    anole_npl_index2file(index, filename);
    DEBUG_photo("save file pos(%d, %s)\n", index, filename);
    return dsk_reg_save_cur_play_info(REG_APP_PHOTO, index, filename, root_type);
}


//////////////////////////////////////////////////////////////////////////
static __hdle g_hMoveStr = NULL;
#define APP_COLOR_CLEAR                        0x0f
//滚动长字符串
__s32 PhotoLongStringShow(H_LYR hLayer, RECT *pRt, char *String, GUI_COLOR TxtClr)
{
    __show_info_move_t  str_move;
    __msg("PhotoLongStringShow(%x, [%d, %d, %d, %d], %s, %x)\n",
          hLayer, pRt->x, pRt->y, pRt->width, pRt->height, String, TxtClr);
    str_move.fontColor       = TxtClr;
    str_move.hlayer          = hLayer;
    str_move.string          = String;
    str_move.region.x        = pRt->x;
    str_move.region.y        = pRt->y;
    str_move.region.width    = pRt->width;
    str_move.region.height   = pRt->height;
    str_move.encode_id       = EPDK_CHARSET_ENM_UTF8;
    ASSERT(SWFFont);
    str_move.pFont           = SWFFont;
    str_move.bkColor         = APP_COLOR_CLEAR;
    str_move.align           = GUI_TA_LEFT | GUI_TA_TOP;
    str_move.bmp             = NULL;
    str_move.alpha_en        = 1;

    if (g_hMoveStr)
    {
        GUI_LongStringStop(g_hMoveStr);
        GUI_LongStringReset(g_hMoveStr, &str_move);
    }
    else
    {
        g_hMoveStr = GUI_LongStringCreate(&str_move);

        if (g_hMoveStr == NULL)
        {
            __wrn("create move handle failed!\n");
        }
    }

    GUI_LongStringStart(g_hMoveStr);
    return EPDK_OK;
}

void PhotoLongStringDestroy(void)
{
    if (g_hMoveStr)
    {
        GUI_LongStringDelete(g_hMoveStr);
        g_hMoveStr = NULL;
    }
}
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// 画BMP部分(拷贝了三次，效率较低，TODO:改到底层实现为一次拷贝，提高效率)
// pFileData --- 资源函数获得的数据(也是bmp文件数据)
// x，y      --- 目标坐标
// x0, y0, x1, y1 --- 源区域(即bmp图片某一区域)
/************************************************************************/
__s32 APP_BMP_Draw(const void *pFileData, int x, int y, int x0, int y0, int x1, int y1)
{
    __s32 Ret = EPDK_OK;
    void *pBuffer;
    int bmp_w, bmp_h;
    int w, h;
    //检查区域范围
    bmp_w = GUI_BMP_GetXSize(pFileData);

    if (! ValueBetweenRangeEx(0, x0, x1, bmp_w))
    {
        ASSERT(0);
        return EPDK_FAIL;
    }

    bmp_h = GUI_BMP_GetYSize(pFileData);

    if (! ValueBetweenRangeEx(0, y0, y1, bmp_h))
    {
        ASSERT(0);
        return EPDK_FAIL;
    }

    w = x1 - x0;
    h = y1 - y0;
    //申请缓冲
    APP_NewMem_B(pBuffer, w * h * 4);

    if (pBuffer != NULL)
    {
        GUI_MEMDEV_Handle draw_mem;
        //使用内存设备
        draw_mem = GUI_MEMDEV_Create(0, 0, bmp_w, bmp_h);

        if (draw_mem != NULL)
        {
            GUI_RECT rt;
            GUI_MEMDEV_Handle old_mem;
            old_mem = GUI_MEMDEV_Select(draw_mem);
            GUI_BMP_Draw(pFileData, 0, 0);
            rt.x0 = x0;
            rt.x1 = x1;
            rt.y0 = y0;
            rt.y1 = y1;
            GUI_GetRectMem(&rt, pBuffer, w, h, 0, 0);
            //删除内存设备
            GUI_MEMDEV_Select(old_mem);
            GUI_MEMDEV_Delete(draw_mem);
            //拷贝到目标位置
            rt.x0 = x;
            rt.y0 = y;
            rt.x1 = rt.x0 + w;
            rt.y1 = rt.y0 + h;
            GUI_SetRectMem(&rt, pBuffer, w, h, 0, 0);
        }
        else
        {
            Ret = EPDK_FAIL;
        }

        APP_DelMem_B(pBuffer, w * h * 4);
    }
    else
    {
        Ret = EPDK_FAIL;
    }

    return Ret;
}

/************************************************************************/
// 由一组图片资源(拉伸)组成内存图片资源
// res_id        --- 图片资源id数组
// id_count      --- 图片资源id数组大小
// direction     --- 拉伸方向, 0 : x方向; 非0 : y方向
// len           --- 拉伸方向的长度
// return values --- 内存图片数据指针
// Remarks : 数组中间的id图片为拉伸图片
/************************************************************************/
RAM_RES_BMP_T *CreateRamBmpData(__s32 res_id[], __s32 id_count, __s32 direction, __s32 len)
{
    __s32 bmp_w, bmp_h;
    load_bmp_res_t res_bmp_mid;
    load_bmp_res_t res_bmp_tmp;
    GUI_MEMDEV_Handle draw_mem;
    RAM_RES_BMP_T *res;
    ASSERT(id_count > 0);
    ASSERT(id_count % 2);
    BallocType(res, RAM_RES_BMP_T);

    if (res == NULL)
    {
        return NULL;
    }

    create_bmp_res(res_id[id_count / 2], res_bmp_mid);

    if (direction)
    {
        bmp_w = GUI_BMP_RES_GetXSize(res_bmp_mid);
        bmp_h = len;
    }
    else
    {
        bmp_w = len;
        bmp_h = GUI_BMP_RES_GetYSize(res_bmp_mid);
    }

    //使用内存设备
    draw_mem = GUI_MEMDEV_Create(0, 0, bmp_w, bmp_h);

    if (draw_mem != NULL)
    {
        __s32 i, k;
        __s32 sp;
        __s32 start, end;
        GUI_MEMDEV_Handle old_mem;
        old_mem = GUI_MEMDEV_Select(draw_mem);
        start = 0;
        end = len;

        if (direction)
        {
            for (i = 0, k = id_count - 1; i < k; i++, k--)
            {
                //head
                create_bmp_res(res_id[i], res_bmp_tmp);
                GUI_BMP_RES_Draw(res_bmp_tmp, 0, start);
                start += GUI_BMP_RES_GetYSize(res_bmp_tmp);
                destroy_bmp_res(res_bmp_tmp);
                //tail
                create_bmp_res(res_id[k], res_bmp_tmp);
                end -= GUI_BMP_RES_GetYSize(res_bmp_tmp);
                GUI_BMP_RES_Draw(res_bmp_tmp, 0, end);
                destroy_bmp_res(res_bmp_tmp);
            }

            sp = GUI_BMP_RES_GetYSize(res_bmp_mid);

            while (start < end)
            {
                GUI_BMP_RES_Draw(res_bmp_mid, 0, start);
                start += sp;
            }
        }
        else
        {
            for (i = 0, k = id_count - 1; i < k; i++, k--)
            {
                //head
                create_bmp_res(res_id[i], res_bmp_tmp);
                GUI_BMP_RES_Draw(res_bmp_tmp, start, 0);
                start += GUI_BMP_RES_GetXSize(res_bmp_tmp);
                destroy_bmp_res(res_bmp_tmp);
                //tail
                create_bmp_res(res_id[k], res_bmp_tmp);
                end -= GUI_BMP_RES_GetXSize(res_bmp_tmp);
                GUI_BMP_RES_Draw(res_bmp_tmp, end, 0);
                destroy_bmp_res(res_bmp_tmp);
            }

            sp = GUI_BMP_RES_GetXSize(res_bmp_mid);

            while (start < end)
            {
                GUI_BMP_RES_Draw(res_bmp_mid, start, 0);
                start += sp;
            }
        }

        res->data_size = bmp_w * bmp_h * 4;
        APP_NewMem_B(res->data, res->data_size);

        if (res->data)
        {
            GUI_RECT rt;
            rt.x0 = 0;
            rt.x1 = bmp_w;
            rt.y0 = 0;
            rt.y1 = bmp_h;
            GUI_GetRectMem(&rt, res->data, bmp_w, bmp_h, 0, 0);
        }
        else
        {
            BFreeType(res, RAM_RES_BMP_T);
        }

        //删除内存设备
        GUI_MEMDEV_Select(old_mem);
        GUI_MEMDEV_Delete(draw_mem);
    }

    destroy_bmp_res(res_bmp_mid);
    return res;
}

/************************************************************************/
// 删除组合图片
/************************************************************************/
void DestroyRamBmpData(RAM_RES_BMP_T *res)
{
    ASSERT(res);
    APP_DelMem_B(res->data, res->data_size);
    BFreeType(res, RAM_RES_BMP_T);
}

//////////////////////////////////////////////////////////////////////////
//end of file

