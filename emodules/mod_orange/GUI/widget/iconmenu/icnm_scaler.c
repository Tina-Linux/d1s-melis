/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : icnm_scaler.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-30 8:37:07
**********************************************************************************************************************
*/

#include "icnm_private.h"

/*
**********************************************************************************************************************
*                                               IMENU_InitScaler
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 IMENU_InitScaler(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               IMENU_ExitScaler
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 IMENU_ExitScaler(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               IMENU_Scaler
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 ICNM_Scaler(__lion_icnm_icon_t *desicon, __lion_icnm_icon_t *srcicon)
{
    FB                          srcfb;
    FB                          desfb;
    __disp_scaler_para_t        scaler_para;
    ES_FILE       *p_disp;
    __hdle         h_scaler;
    __u64 dispbuffer[3];
    __disp_fb_t src_disp_fb, des_disp_fb;
    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __wrn("open display device fail!\n");
        return EPDK_FAIL;
    }

    h_scaler = eLIBs_fioctrl(p_disp, MOD_DISP_LAYER_REQUEST, SEL_SCREEN, 0); //modified by Derek,2010-12-9 19:35:03

    if (!h_scaler)
    {
        __wrn("h_scaler cannot be request!\n");
        eLIBs_fclose(p_disp);
        return EPDK_FAIL;
    }

    srcfb.size                  = srcicon->size;
    srcfb.addr[0]               = (void *)((__u32)srcicon->argbbuf);
    srcfb.fmt.type              = FB_TYPE_RGB;
    srcfb.fmt.fmt.rgb.pixelfmt  = PIXEL_COLOR_ARGB8888;
    srcfb.fmt.fmt.rgb.br_swap   = 0;
    srcfb.fmt.fmt.rgb.pixseq    = 0;
    desfb.size                  = desicon->size;
    desfb.addr[0]               = desicon->argbbuf;
    desfb.fmt.type              = FB_TYPE_RGB;
    desfb.fmt.fmt.rgb.pixelfmt  = PIXEL_COLOR_ARGB8888;
    desfb.fmt.fmt.rgb.br_swap   = 0;
    desfb.fmt.fmt.rgb.pixseq    = 0;
    /*modified by Derek,2010-12-11 10:12:09*/
    GUI_Fb2Dispfb(&srcfb, &src_disp_fb);
    GUI_Fb2Dispfb(&desfb, &des_disp_fb);
    scaler_para.input_fb      = src_disp_fb;
    scaler_para.output_fb     = des_disp_fb;
    scaler_para.source_regn.x = 0;
    scaler_para.source_regn.y = 0;
    scaler_para.source_regn.width  = src_disp_fb.size.width;
    scaler_para.source_regn.height = src_disp_fb.size.height;
    dispbuffer[0] = h_scaler;
    dispbuffer[1] = (__u32)&scaler_para;
    dispbuffer[2] = 0;

    //if (eLIBs_fioctrl(p_disp, DISP_CMD_SCALER_EXECUTE, SEL_SCREEN, (void *)dispbuffer) != EPDK_OK)
    {
        __wrn("scaler fail!\n");
    }

    /*modified by Derek,2010-12-9 19:42:00*/
    dispbuffer[0] = h_scaler;
    dispbuffer[1] = 0;
    dispbuffer[2] = 0;
    eLIBs_fioctrl(p_disp, MOD_DISP_LAYER_RELEASE, SEL_SCREEN, (void *)dispbuffer); //end modified
    eLIBs_fclose(p_disp);
    return EPDK_FAIL;
}

