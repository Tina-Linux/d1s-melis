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
#include "gui_core_kernel_private.h"
#include <kconfig.h>
//#include "..\\core_i.h"
//#include "gui_core_kernel_i.h"

/*
**********************************************************************************************************************
*                                               GUI_Fb2Dispfb
*
* author:
*
* date:             2010-12-8 16:29:59
*
* Description:      struct fb to struct dispfb
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_Fb2Dispfb(FB *fb, __disp_fb_t *dispfb)
{
    if ((!fb) || (!dispfb))
    {
        __wrn("input invalid!");
        return EPDK_FAIL;
    }
	//void*fb_addr1 = eLIBs_malloc(fb->size.width*fb->size.height*4);
	//eLIBs_memset(fb_addr1,0xff,fb->size.width*fb->size.height*4);
	__inf("fb->size.width = %d fb->size.height = %d",fb->size.width,fb->size.height);
    //dispfb->addr[0] = esMEMS_VA2PA((__u32)(fb_addr1));
    dispfb->lbc_mode = 0;    
    dispfb->addr[0] = (unsigned long)(fb->addr[0]);
    dispfb->addr[1] = (unsigned long)(fb->addr[1]);
    dispfb->addr[2] = (unsigned long)(fb->addr[2]);
    //dispfb->size = (__disp_rectsz_t)fb->size;
    dispfb->size.width = fb->size.width;
    dispfb->size.height = fb->size.height;
    dispfb->cs_mode = fb->fmt.cs_mode;

	__inf(" fb->fmt.fmt.rgb.pixelfmt = %x", fb->fmt.fmt.rgb.pixelfmt);
	__inf(" fb->fmt.fmt.rgb.pixseq = %x", fb->fmt.fmt.rgb.pixseq);

    if (fb->fmt.type == FB_TYPE_RGB)
    {
		__inf("fb->fmt.type == FB_TYPE_RGB");
		if(fb->fmt.fmt.rgb.pixelfmt == PIXEL_COLOR_ARGB8888)
		{
		    dispfb->format = DISP_FORMAT_ARGB_8888;
		}
		else if(fb->fmt.fmt.rgb.pixelfmt == PIXEL_COLOR_RGB0888)
		{
		    dispfb->format = DISP_FORMAT_ARGB_8888;
		}
		else if(fb->fmt.fmt.rgb.pixelfmt == PIXEL_COLOR_ARGB1555)
		{
		    dispfb->format = DISP_FORMAT_ARGB_1555;
		}
		else if(fb->fmt.fmt.rgb.pixelfmt == PIXEL_COLOR_RGBA5551)
		{
		    dispfb->format = DISP_FORMAT_RGBA_5551;
		}
		else if(fb->fmt.fmt.rgb.pixelfmt == PIXEL_COLOR_RGB565)
		{
		    dispfb->format = DISP_FORMAT_RGB_565;
		}
		else
		{
		    dispfb->format = PIXEL_COLOR_ARGB8888;
		    __err("pixel format not support!");
			return -1;
		}

		
		if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_ARGB)
		{
		    dispfb->seq = DISP_SEQ_ARGB;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_BRGA)
		{
		    dispfb->seq = DISP_SEQ_BGRA;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_P10)
		{
		    dispfb->seq = DISP_SEQ_P10;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_P01)
		{
		    dispfb->seq = DISP_SEQ_P01;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_P3210)
		{
		    dispfb->seq = DISP_SEQ_P3210;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_P0123)
		{
		    dispfb->seq = DISP_SEQ_P0123;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_P76543210)
		{
		    dispfb->seq = DISP_SEQ_P76543210;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_P67452301)
		{
		    dispfb->seq = DISP_SEQ_P67452301;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_P10325476)
		{
		    dispfb->seq = DISP_SEQ_P10325476;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_P01234567)
		{
		    dispfb->seq = DISP_SEQ_P01234567;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_2BPP_BIG_BIG)
		{
		    dispfb->seq = DISP_SEQ_2BPP_BIG_BIG;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_2BPP_BIG_LITTER)
		{
		    dispfb->seq = DISP_SEQ_2BPP_BIG_LITTER;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_2BPP_LITTER_BIG)
		{
		    dispfb->seq = DISP_SEQ_2BPP_LITTER_BIG;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_2BPP_LITTER_LITTER)
		{
		    dispfb->seq = DISP_SEQ_2BPP_LITTER_LITTER;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_1BPP_BIG_BIG)
		{
		    dispfb->seq = DISP_SEQ_1BPP_BIG_BIG;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_1BPP_BIG_LITTER)
		{
		    dispfb->seq = DISP_SEQ_1BPP_BIG_LITTER;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_1BPP_LITTER_BIG)
		{
		    dispfb->seq = DISP_SEQ_1BPP_LITTER_BIG;
		}
		else if(fb->fmt.fmt.rgb.pixseq == RGB_SEQ_1BPP_LITTER_LITTER)
		{
		    dispfb->seq = DISP_SEQ_1BPP_LITTER_LITTER;
		}
		else
		{
			__err("pixseq not support!");
			return -1;
		}    
        /*convert pixelfmt*/
        //dispfb->format = fb->fmt.fmt.rgb.pixelfmt;

        /*convert seq*/
		/*
        if ((RGB_SEQ_ARGB == fb->fmt.fmt.rgb.pixseq) || (fb->fmt.fmt.rgb.pixseq == RGB_SEQ_BRGA))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq;
        }
        else if ((0x10 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x11))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 3;
        }
        else if ((0x20 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x21))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 0x11;
        }
        else if ((0x30 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x33))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 0x1F;
        }
        else if ((0x40 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x43))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 0x2B;
        }
        else if ((0x50 <= fb->fmt.fmt.rgb.pixseq) && (fb->fmt.fmt.rgb.pixseq <= 0x53))
        {
            dispfb->seq = fb->fmt.fmt.rgb.pixseq - 0x37;
        }
        else
        {
            __wrn("fb rgb seq is invalid!");
        }
*/
        /*convert mode*/
        dispfb->mode = DISP_MOD_INTERLEAVED;
        /*convert br_swap*/
        dispfb->br_swap = fb->fmt.fmt.rgb.br_swap;
    }
    else if (fb->fmt.type == FB_TYPE_YUV)
    {
        /*convert pixelfmt*/
        if (fb->fmt.fmt.yuv.pixelfmt == PIXEL_OTHERFMT)
        {
            __log("fb pixelfmt is invalid!");
            return EPDK_FAIL;
        }
        else
        {
            dispfb->format = DISP_FORMAT_YUV420_P/* - 5*/;
        }

        /*convert seq*/
		/*
        if ((0x0 <= fb->fmt.fmt.yuv.yuvseq) && (fb->fmt.fmt.yuv.yuvseq <= 0x3))
        {
            dispfb->seq = fb->fmt.fmt.yuv.yuvseq + 3;
        }
        else if ((0x10 <= fb->fmt.fmt.yuv.yuvseq) && (fb->fmt.fmt.yuv.yuvseq <= 0x11))
        {
            dispfb->seq = fb->fmt.fmt.yuv.yuvseq - 9;
        }
        else if ((0x20 <= fb->fmt.fmt.yuv.yuvseq) && (fb->fmt.fmt.yuv.yuvseq <= 0x21))
        {
            dispfb->seq = fb->fmt.fmt.yuv.yuvseq - 0x17;
        }
        else if (fb->fmt.fmt.yuv.yuvseq == 0xff)
        {
            dispfb->seq = DISP_SEQ_UVUV;
        }
        else
        {
            __wrn("fb yuv seq is invalid!");
            return EPDK_FAIL;
        }
        */
		dispfb->seq = DISP_SEQ_VUVU;

        /*convert mode*/
        if (fb->fmt.fmt.yuv.mod == YUV_MOD_INTERLEAVED)
        {
            dispfb->mode = DISP_MOD_INTERLEAVED;
        }
        else if (fb->fmt.fmt.yuv.mod == YUV_MOD_NON_MB_PLANAR)
        {
            dispfb->mode = DISP_MOD_NON_MB_PLANAR;
        }
        else if (fb->fmt.fmt.yuv.mod == YUV_MOD_MB_PLANAR)
        {
            dispfb->mode = DISP_MOD_MB_PLANAR;
        }
        else if (fb->fmt.fmt.yuv.mod == YUV_MOD_UV_NON_MB_COMBINED)
        {
            dispfb->mode = DISP_MOD_NON_MB_UV_COMBINED;
        }
        else if (fb->fmt.fmt.yuv.mod == YUV_MOD_UV_MB_COMBINED)
        {
            dispfb->mode = DISP_MOD_MB_UV_COMBINED;
        }
        else
        {
            __wrn("yuv mode is invalid!");
            return EPDK_FAIL;
        }
        //dispfb->mode = fb->fmt.fmt.yuv.mod;
        /*convert br_swap*/
        dispfb->br_swap = 0;
	//dispfb->mode = 0;
	//dispfb->format = DISP_FORMAT_YUV420_P;
	//dispfb->seq = DISP_SEQ_VUVU;
	//dispfb->cs_mode = DISP_BT709;		
    }
    else
    {
        __wrn("fb type is invalid!");
        return EPDK_FAIL;
    }
	
	__inf(" dispfb->format = %x", dispfb->format);
	__inf(" dispfb->seq = %x", dispfb->seq);
	__inf(" dispfb->mode = %x", dispfb->mode);
	__inf(" dispfb->cs_mode = %x", dispfb->cs_mode);
	
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_Dispfb2Fb
*
* author:
*
* date:             2010-12-8 16:29:59
*
* Description:      struct fb to struct dispfb
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_Dispfb2Fb(__disp_fb_t *dispfb, FB *fb)
{
    if ((!fb) || (!dispfb))
    {
        __wrn("input invalid!");
        return EPDK_FAIL;
    }

    dispfb->lbc_mode = 0;
    fb->addr[0] = (void *)(dispfb->addr[0]);
    fb->addr[1] = (void *)(dispfb->addr[1]);
    fb->addr[2] = (void *)(dispfb->addr[2]);
    //fb->size = (SIZE)dispfb->size;
    fb->size.width = dispfb->size.width;
    fb->size.height = dispfb->size.height;
    fb->fmt.cs_mode = dispfb->cs_mode;
    /*convert pixel format*/
#if defined(CONFIG_SOC_SUN3IW2) || defined(CONFIG_SOC_SUN8IW19) || defined (CONFIG_SOC_SUN20IW1)
	if(dispfb->format == DISP_FORMAT_ARGB_8888 )
	{
	    fb->fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
	}
	else if(dispfb->format == DISP_FORMAT_ARGB_8888 )
	{
	    fb->fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_RGB0888;
	}
	else if(dispfb->format == DISP_FORMAT_ARGB_1555 )
	{
	    fb->fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB1555;
	}
	else if(dispfb->format == DISP_FORMAT_RGBA_5551 )
	{
	    fb->fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_RGBA5551;
	}
	else if(dispfb->format == DISP_FORMAT_RGB_565 )
	{
	    fb->fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_RGB565;
	}
    else
    {
        fb->fmt.fmt.yuv.pixelfmt = dispfb->format ;
        fb->fmt.type = 1;
    }

#elif defined CONFIG_SOC_SUN3IW1

    if (dispfb->format <= 0xa)
    {
        fb->fmt.fmt.rgb.pixelfmt = dispfb->format;
        fb->fmt.type = 0;
    }
    else
    {
        fb->fmt.fmt.yuv.pixelfmt = dispfb->format + 5;
        fb->fmt.type = 1;
    }

#endif

    /*convert seq*/
		if(dispfb->seq == DISP_SEQ_ARGB )
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_ARGB;
		}
		else if(dispfb->seq == DISP_SEQ_BGRA)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_BRGA;
		}
		else if(dispfb->seq == DISP_SEQ_P10)
		{
            fb->fmt.fmt.rgb.pixseq= RGB_SEQ_P10;
		}
		else if(dispfb->seq == DISP_SEQ_P01)
		{
            fb->fmt.fmt.rgb.pixseq = RGB_SEQ_P01;
		}
		else if(dispfb->seq == DISP_SEQ_P3210)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_P3210;
		}
		else if(dispfb->seq == DISP_SEQ_P0123)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_P0123;
		}
		else if(dispfb->seq == DISP_SEQ_P76543210)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_P76543210;
		}
		else if(dispfb->seq == DISP_SEQ_P67452301)
		{
		   fb->fmt.fmt.rgb.pixseq = RGB_SEQ_P67452301;
		}
		else if(dispfb->seq == DISP_SEQ_P10325476)
		{
		  fb->fmt.fmt.rgb.pixseq = RGB_SEQ_P10325476;
		}
		else if(dispfb->seq == DISP_SEQ_P01234567)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_P01234567;
		}
		else if(dispfb->seq == DISP_SEQ_2BPP_BIG_BIG)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_2BPP_BIG_BIG;
		}
		else if( dispfb->seq == DISP_SEQ_2BPP_BIG_LITTER)
		{
		   fb->fmt.fmt.rgb.pixseq = RGB_SEQ_2BPP_BIG_LITTER;
		}
		else if(dispfb->seq == DISP_SEQ_2BPP_LITTER_BIG)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_2BPP_LITTER_BIG;
		}
		else if(dispfb->seq == DISP_SEQ_2BPP_LITTER_LITTER)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_2BPP_LITTER_LITTER;
		}
		else if(dispfb->seq == DISP_SEQ_1BPP_BIG_BIG)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_1BPP_BIG_BIG;
		}
		else if(dispfb->seq == DISP_SEQ_1BPP_BIG_LITTER)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_1BPP_BIG_LITTER;
		}
		else if(dispfb->seq == DISP_SEQ_1BPP_LITTER_BIG)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_1BPP_LITTER_BIG;
		}
		else if(dispfb->seq == DISP_SEQ_1BPP_LITTER_LITTER)
		{
		    fb->fmt.fmt.rgb.pixseq = RGB_SEQ_1BPP_LITTER_LITTER;
		}
		else
		{
			__err("pixseq not support!  dispfb->seq = 0x%x",dispfb->seq);
			return -1;
		}  
#if 0
    if ((0x3 <= dispfb->seq) && (dispfb->seq <= 0x6))
    {
        fb->fmt.fmt.yuv.yuvseq = dispfb->seq - 3;
    }
    else if ((0x7 <= dispfb->seq) && (dispfb->seq <= 0x8))
    {
        fb->fmt.fmt.yuv.yuvseq = dispfb->seq + 9;
    }
    else if ((0x9 <= dispfb->seq) && (dispfb->seq <= 0xa))
    {
        fb->fmt.fmt.yuv.yuvseq = dispfb->seq + 0x17;
    }
    else if ((0xb <= dispfb->seq) && (dispfb->seq <= 0xc))
    {
        fb->fmt.fmt.yuv.yuvseq = YUV_SEQ_OTHRS;
    }
    else if ((0x0 == dispfb->seq) || (dispfb->seq == 0x2))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq;
    }
    else if ((0xd <= dispfb->seq) && (dispfb->seq >= 0xe))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 3;
    }
    else if ((0xf <= dispfb->seq) && (dispfb->seq >= 0x10))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 0x11;
    }
    else if ((0x11 <= dispfb->seq) && (dispfb->seq >= 0x14))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 0x1f;
    }
    else if ((0x15 <= dispfb->seq) && (dispfb->seq >= 0x18))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 0x2b;
    }
    else if ((0x19 <= dispfb->seq) && (dispfb->seq >= 0x1c))
    {
        fb->fmt.fmt.rgb.pixseq = dispfb->seq + 0x37;
    }
    else
    {
        __wrn("dispfb seq is invalid!");
        return EPDK_FAIL;
    }
#endif
    /*convert mode*/
    if (dispfb->mode == DISP_MOD_INTERLEAVED)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_INTERLEAVED;
    }
    else if (dispfb->mode == DISP_MOD_NON_MB_PLANAR)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_NON_MB_PLANAR;
    }
    else if (dispfb->mode == DISP_MOD_MB_PLANAR)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_MB_PLANAR;
    }
    else if (dispfb->mode == DISP_MOD_NON_MB_UV_COMBINED)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_UV_NON_MB_COMBINED;
    }
    else if (dispfb->mode == DISP_MOD_MB_UV_COMBINED)
    {
        fb->fmt.fmt.yuv.mod = YUV_MOD_UV_MB_COMBINED;
    }
    else
    {
        __wrn("dispfb mod is invalid!");
    }

    /*convert br_swap*/
    fb->fmt.fmt.rgb.br_swap = dispfb->br_swap;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LogicLyr2DispLyr
*
* author:
*
* date:             2010-12-9 16:37:16
*
* Description:      logic lyare to display layer
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_LogicLyr2DispLyr(__disp_layer_para_t *log_lyr, __disp_layer_info_t *disp_lyr)
{
    if ((!log_lyr) || (!disp_lyr))
    {
        __wrn("lyr pointer is invalid!");
        return EPDK_FAIL;
    }

    disp_lyr->alpha_en = log_lyr->alpha_en;
    disp_lyr->alpha_val = log_lyr->alpha_val;
    disp_lyr->mode = log_lyr->mode;
    disp_lyr->pipe = log_lyr->pipe;
    disp_lyr->prio = log_lyr->prio;
    //disp_lyr->scn_win = (__disp_rect_t)log_lyr->scn_win;
    disp_lyr->scn_win.x = log_lyr->scn_win.x;
    disp_lyr->scn_win.y = log_lyr->scn_win.y;
    disp_lyr->scn_win.width = log_lyr->scn_win.width;
    disp_lyr->scn_win.height = log_lyr->scn_win.height;
    //disp_lyr->src_win = (__disp_rect_t)log_lyr->src_win;
    disp_lyr->src_win.x = log_lyr->src_win.x;
    disp_lyr->src_win.y = log_lyr->src_win.y;
    disp_lyr->src_win.width = log_lyr->src_win.width;
    disp_lyr->src_win.height = log_lyr->src_win.height;
    disp_lyr->ck_enable = log_lyr->ck_mode;
    GUI_Fb2Dispfb(log_lyr->fb, &(disp_lyr->fb));
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_DispLyr2LogicLyr
*
* author:
*
* date:             2010-12-9 16:37:16
*
* Description:      logic lyare to display layer
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_DispLyr2LogicLyr(__disp_layer_para_t *log_lyr, __disp_layer_info_t *disp_lyr)
{
    if ((!log_lyr) || (!disp_lyr))
    {
        __wrn("lyr pointer is invalid!");
        return EPDK_FAIL;
    }

    log_lyr->alpha_en = disp_lyr->alpha_en;
    log_lyr->alpha_val = disp_lyr->alpha_val;
    log_lyr->mode = disp_lyr->mode;
    log_lyr->pipe = disp_lyr->pipe;
    log_lyr->prio = disp_lyr->prio;
    //log_lyr->scn_win = (RECT)disp_lyr->scn_win;
    log_lyr->scn_win.x = disp_lyr->scn_win.x;
    log_lyr->scn_win.y = disp_lyr->scn_win.y;
    log_lyr->scn_win.width = disp_lyr->scn_win.width;
    log_lyr->scn_win.height = disp_lyr->scn_win.height;
    //log_lyr->src_win = (RECT)disp_lyr->src_win;
    log_lyr->src_win.x = disp_lyr->src_win.x;
    log_lyr->src_win.y = disp_lyr->src_win.y;
    log_lyr->src_win.width = disp_lyr->src_win.width;
    log_lyr->src_win.height = disp_lyr->src_win.height;
    log_lyr->ck_mode = disp_lyr->ck_enable;
    GUI_Dispfb2Fb(&(disp_lyr->fb), log_lyr->fb);
    log_lyr->channel = DISP_LAYER_OUTPUT_CHN_DE_CH1;
    return EPDK_OK;
}
