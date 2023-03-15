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
#ifndef  __misc_c
#define  __misc_c
#include <log.h>
#include <emodules/mod_display.h>
#include "misc.h"
#include <log.h>

#define COM_GET_R_VAL(color) ( ((__u32)color)&0xff )
#define COM_GET_G_VAL(color) ( (((__u32)color)>>8)&0xff )
#define COM_GET_B_VAL(color) ( (((__u32)color)>>16)&0xff )
#define COM_GET_ALPHA_VAL(color) ( (((__u32)color)>>24)&0xff )

#define PIOC_MEM_BASE   (0xf1c20800)

#define PIO_REG_CFG(n, i)               ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i)            ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)              ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n)                 ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + 0x10))

//libaiao, 2011.5.15
//根据指定长度从src中截取字符串
//返回值为窃取字符串的长度(byte为单位)
__s32 GUI_GetStringByWidth(char *dst, const char GUI_UNI_PTR *src, __u32 width)
{
    const char GUI_UNI_PTR *s = src;
    U16 r = 0, w = 0, len = 0;
    r   = GUI_UC_GetCharCode(s);

    if (r == 0)
    {
        return 0;
    }

    while (r != 0)
    {
        if (w >= width)
        {
            break;
        }

        r   = GUI_UC_GetCharCode(s + len);
        len  += GUI_UC_GetCharSize(s + len);    //指向下一个字符
        w += GUI_GetCharDistX(r);
    }

    eLIBs_strncpy(dst, src, len);
    return len;
}

//libaiao, 2011.5.15
//extern __s32 GUI_GetStringByWidth(char *dst, const char GUI_UNI_PTR *src, __u32 width);
//根据可显示区域截取字串
//如果字串宽度大于显示区域,则在可显示字串后加三个省略号
//注意该函数只判断宽度，不判断高度
//append为追加在后面的省略号
__s32 GetClippedString(GUI_RECT *region, char *src_str, char *dst_str, const char *append)
{
    __u32 src_width = 0, dst_width = 0, append_width = 0;
    __u32 len = 0, char_width = 0;
    __u32 gap = 0;
    dst_width = region->x1 - region->x0;
    src_width = GUI_GetStringDistX(src_str);

    if (dst_width >= src_width)
    {
        eLIBs_strcpy(dst_str, src_str);
        return EPDK_OK;
    }

    append_width = GUI_GetStringDistX(append);

    if (append_width >= dst_width)
    {
        eLIBs_strcpy(dst_str, append);
        return EPDK_OK;
    }

    gap = dst_width - append_width;
    GUI_GetStringByWidth(dst_str, src_str, gap);
    //GUI_DispStringLen();
    eLIBs_strcat(dst_str, append);
    return EPDK_OK;
}

void time2time(__u32 second_time, cuckoo_time_t *format_time)
{
    __u32  total_minute;
    total_minute = second_time / 60;
    format_time->second = second_time % 60;
    format_time->minute = total_minute % 60;
    format_time->hour   = total_minute / 60;
}

void   uint2str(__u32 input, char *str)
{
    char stack[11] ;
    int i ;
    int j ;
    int count;

    if (input == 0)
    {
        str[0] = '0';
        str[1] = '0';
        str[2] = '\0';                   // 'str' must end with '\0'
        return ;
    }

    for (i = 0; input > 0; ++i)
    {
        stack[i] = input % 10 + '0';     // characters in reverse order are put in 'stack' .
        input /= 10;
    }                                    // at the end of 'for' loop, 'i' is the number of characters.

    count = i;

    for (j = 0, i = 0; i < (2 - count); i++)    // pad '0'
    {
        str[j++] = '0';
    }

    for (i = count, --i; i >= 0; --i, ++j)
    {
        str[j] = stack[i];
    }

    str[j] = '\0';                       // 'str' must end with '\0'
}

static void __time2str(const cuckoo_time_t *time, char *str)
{
    uint2str(time->hour,   str);
    str[2] = ':';
    uint2str(time->minute, str + 3);
    str[5] = ':';
    uint2str(time->second, str + 6);
}

void  time2str(__u32 ms, char *str)
{
    __u32  second;
    cuckoo_time_t  time;
    second = ms / 1000;
    time2time(second, &time);
    __time2str(&time, str);
}


//根据要显示的格式转换
void time2str_by_format(__u32 ms, char *str, time_format_e format)
{
    __u32  second;
    cuckoo_time_t  time;
    second = ms / 1000;
    time2time(second, &time);

    switch (format)
    {
        case TIME_S:
            uint2str(time.second, str);
            break;

        case TIME_MS:
            uint2str(time.minute, str);
            str[2] = ':';
            uint2str(time.second, str + 3);
            break;

        case TIME_HMS:
            time2str(ms, str);
            break;

        case TIME_AUTO_HMS:
            uint2str(time.minute, str);
            str[2] = ':';
            uint2str(time.second, str + 3);

            if (time.hour != 0)
            {
                time2str(ms, str);
            }

            break;

        default:
            break;
    }
}


//转换文件size 到字符串
//add by libaiao, 2011.5.8
void filesize2str(__u32 size, char *str)
{
    __msg("size = %d\n", size);

    if (size < 1024)    // 小于1 k
    {
        eLIBs_sprintf(str, "%d B", size);
    }
    else if (size < (1024 * 1024))  // 小于 1 M
    {
        eLIBs_sprintf(str, "%d K", size / 1024);
    }
    else if (size < (1024 * 1024 * 1024)) // 小于 1 G
    {
        //eLIBs_sprintf(str, "%d.%d M",size/(1024*1024), (size%(1024*1024))*100/(1024*1024)/10);
        eLIBs_sprintf(str, "%d.%d%d M", size / (1024 * 1024), ((size % (1024 * 1024)) / 1024) * 1000 / 1024 / 100, ((size % (1024 * 1024)) / 1024) * 1000 / 1024 % 100 / 10); //保留两位小数
    }
    else
    {
        //eLIBs_sprintf(str, "%d.%d G",size/(1024*1024*1024),(size%(1024*1024*1024))*100/(1024*1024*1024)/10);
        eLIBs_sprintf(str, "%d.%d%d G", size / (1024 * 1024 * 1024), (size % (1024 * 1024 * 1024)) / (1024 * 1024) * 1000 / 1024 / 100, (size % (1024 * 1024 * 1024)) / (1024 * 1024) * 1000 / 1024 % 100 / 10);
    }
}





//传入24时制小时值，传出12时制小时值，返回0为am，非0为pm
__s32 hour24_to_hour12(__s32 *hour)
{
    if (*hour == 0)
    {
        //凌晨12点
        *hour = 12;
    }
    else if (*hour == 12)
    {
        //中午12点
        return 1;
    }
    else if (12 < *hour)
    {
        *hour -= 12;
        return 1;
    }

    return 0;
}

//查询app_name名称的应用是否存在
//查询背景音乐是否存在可以用is_app_exist(APP_MUSIC)
__s32 is_app_exist(char *app_name)
{
    H_WIN root, child;
    char  winname[256];
    __bool exist;
    root  = GUI_WinGetHandFromName(APP_ROOT);

    if (!root)
    {
        return 0;
    }

    child = GUI_WinGetFirstChild(root);
    exist = 0;

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);
        __msg("winname = %s \n", winname);

        if (0 == eLIBs_strcmp(app_name, winname))
        {
            exist = 1;
            break;
        }

        child = GUI_WinGetNextBro(child);
    }

    return exist;
}

__u32 get_memory_info(char *title, void *pExtPara)
{
    static __u32 last_free_size = 0;
    __u32 free_size = 0, total_size = 0;
    __u32 size;
    //__msg("\n");
    //__msg("=====================title: %s====================\n", title);
    free_size = esMEMS_FreeMemSize();

    if (free_size > last_free_size)
    {
        size = free_size - last_free_size;

        if (last_free_size == 0)
        {
            __msg("=====================malloc memory size is %d=======\n", last_free_size);
        }
        else
        {
            __msg("=====================release memory size is %d=======\n", size);
        }
    }
    else if (free_size <= last_free_size)
    {
        size = last_free_size - free_size;
        __msg("=====================malloc memory size is %d=======\n", size);
    }

    total_size = esMEMS_TotalMemSize();
    __msg("=====================free memory size is %d=======\n", free_size);
    __msg("=====================used memory size is %d=======\n", total_size - free_size);
    __msg("=====================total memory size is %d=======\n", total_size);
    last_free_size = free_size;
    return 0;
}


void *My_Palloc(__u32 npage, __u32 mode)
{
    void *p;
    //if(npage)
    p = esMEMS_Palloc(npage, mode);
    __wrn("palloc p=%x, npage=%d\n", p, npage);
    return p;
}

void My_Pfree(void *mblk, __u32 npage)
{
    __wrn("Pfree p=%x\n", mblk);
    esMEMS_Pfree(mblk, npage);
}

void *My_Balloc(__u32 size)
{
    static __u32 index = 0;
    void *p = esMEMS_Balloc(size);

    if (52 == size)
    {
        // __wrn("********My_Balloc******\n");
    }

    __wrn("Balloc index=%d p=%x, size=%d\n", index, (__u32)p, size);
    index++;
    return p;
}

void  My_Bfree(void *mblk, __u32 size)
{
    __wrn("Bfree p=%x, size=%d\n", (__u32)mblk, size);
    esMEMS_Bfree(mblk, size);
}

void *My_Malloc(void *heap, __u32 size)
{
    static __u32 index = 0;
    void *p = esMEMS_Malloc(heap,  size);
    __wrn("Malloc index=%d p=%x\n", index, (__u32)p);
    index++;
    return p;
}


void My_Mfree(void *heap, void *f)
{
    __wrn("Mfree p=%x\n", (__u32)f);
    esMEMS_Mfree(heap, f);
}


__s32 test_flush_nor(__s32 i)
{
#if 0 /*langaojie temp test*/
    __s32 result;
    ES_FILE *fp = NULL;
    char buf[3000] = {0};
    char path[] = "e:\\test.bin";
    eLIBs_printf("i=%d\n", i);

    if (1) //
    {
        fp = eLIBs_fopen(path, "wb");

        if (fp == NULL)
        {
            eLIBs_printf("open file fail:%s\n", path);
            return EPDK_FAIL;
        }

        result = eLIBs_fwrite(buf, 1, sizeof(buf), fp);

        if (result < sizeof(buf))
        {
            eLIBs_fclose(fp);
            eLIBs_printf("write file fail:%s\n", path);
            return EPDK_FAIL;
        }

        eLIBs_fclose(fp);
        {
            //
            __s32 i, num;
            ES_FILE *hFile;

            for (i = 0 ; i < 1 ; i++)
            {
                //flush user disk data to NOR flash
                hFile = eLIBs_fopen("b:\\STORAGE\\SPINOR", "rb");

                if (hFile == NULL)
                {
                    eLIBs_printf("open NOR file failed\n");
                    return EPDK_FAIL;
                }

                eLIBs_printf("i=%d flush file 1234....\n", i);
                esMODS_MIoctrl(hFile, DEV_IOC_USR_FLUSH_CACHE, 0, 0);
                eLIBs_printf("i=%d flush end 1234....\n", i);
                eLIBs_fclose(hFile);
                esKRNL_TimeDly(2);
            }
        }
    }
#endif
    return EPDK_OK;
}


static __s32 com_copy_rect_32bpp(__u32 *pDst, __s32 dst_x, __s32 dst_y, __s32 dst_line_w, __u32 *pSrc,
                                 __s32 src_x, __s32 src_y, __s32 src_line_w, __s32 w, __s32 h)
{
    __s32 i, j;

    for (i = 0 ; i < w ; i++)
    {
        for (j = 0 ; j < h ; j++)
        {
            DATA_OF_ADRR(__u32, pDst, dst_x + i, dst_y + j, dst_line_w) = DATA_OF_ADRR(__u32, pSrc, src_x + i, src_y + j, src_line_w);
        }
    }

    return EPDK_OK;
}

static __s32 com_copy_rect_8bpp(char *pDst, __s32 dst_x, __s32 dst_y, __s32 dst_line_w, char *pSrc,
                                __s32 src_x, __s32 src_y, __s32 src_line_w, __s32 w, __s32 h)
{
    __s32 i, j;
    src_line_w = (src_line_w + 3) / 4 * 4;
    // dst_line_w = (dst_line_w+3)/4*4;

    for (i = 0 ; i < w ; i++)
    {
        for (j = 0 ; j < h ; j++)
        {
            DATA_OF_ADRR(__u8, pDst, dst_x + i, dst_y + j, dst_line_w) = 0xf0; //DATA_OF_ADRR(__u8, pSrc, src_x+i, src_y+j, src_line_w);
        }
    }

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// pFileData --- 资源函数获得的数据(也是32位bmp文件数据)
// x，y      --- 目标坐标
// x0, y0, x1, y1 --- 源区域(即bmp图片某一区域)
/************************************************************************/
__s32 APP_BMP_DrawEx(H_LYR hlyr, const void *pFileData, int x, int y, int x0, int y0, int x1, int y1)
{
    __s32 Ret = EPDK_OK;
    __u32 *pSrc32;
    char *pSrc8;
    int bmp_w, bmp_h;
    int w, h;
    GUI_RECT rt;
    FB fb;
    __u32 *pfb_addr;
    __s32 i, j;
    __s32 nBitCount;
    __s32 offbits;

    if (!hlyr)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinGetFB(hlyr, &fb);
    //检查区域范围
    bmp_w = GUI_BMP_GetXSize(pFileData);

    if (! ValueBetweenRangeEx(0, x0, x1, bmp_w))
    {
        return EPDK_FAIL;
    }

    bmp_h = GUI_BMP_GetYSize(pFileData);

    if (! ValueBetweenRangeEx(0, y0, y1, bmp_h))
    {
        return EPDK_FAIL;
    }

    if (x1 - x0 + x >= fb.size.width
        || y1 - y0 + y >= fb.size.height)
    {
        return EPDK_FAIL;
    }

    nBitCount = *((char *)pFileData + 28);
    offbits = *(__u32 *)((char *)pFileData + 10);
    pSrc8 = (char *)pFileData + offbits;
    pSrc32 = (__u32 *)pSrc8;
    pfb_addr = (__u32 *)(fb.addr[0]);

    if (32 == nBitCount)
    {
        com_copy_rect_32bpp(pfb_addr, x, y, fb.size.width, pSrc32, x0, y0, bmp_w, x1 - x0 + 1, y1 - y0 + 1);
    }
    else if (8 == nBitCount)
    {
        com_copy_rect_8bpp((char *)pfb_addr, x, y, fb.size.width, pSrc8, x0, y0, bmp_w, x1 - x0 + 1, y1 - y0 + 1);
    }
    else
    {
        __err("not 32bpp or 8bpp...\n");
    }

    return Ret;
}

__s32 app_set_title(__s32 res_id)
{
    char title[32] = {0};
    dsk_langres_get_menu_text(res_id, title, sizeof(title));
    return gscene_hbar_set_title(title, sizeof(title));
}


__s32 com_set_backcolor(__u32 hlay, __u32 color)
{
    dsk_display_set_backcolor(hlay,color);
    return EPDK_OK;
}

void pull_down_gpio(__u32 port, __u32 port_num)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __u32               tmp1;
    __u32               tmp2;
    //设置为输出
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |= (0x01 << tmp1);
    *tmp_addr = tmp_data;
    //设置为下拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |= (0x02 << tmp2);
    *tmp_addr = tmp_data;
    //输出低电平
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    tmp_data &= ~(1 << port_num);
    //tmp_data |=  (1 << port_num);
    *tmp_addr = tmp_data;
}

void pull_up_gpio(__u32 port, __u32 port_num)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __u32               tmp1;
    __u32               tmp2;
    //设置为输出
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |= (0x01 << tmp1);
    *tmp_addr = tmp_data;
    //设置为上拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |= (0x01 << tmp2);
    *tmp_addr = tmp_data;
    //输出高电平
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    tmp_data &= ~(1 << port_num);
    tmp_data |= (1 << port_num);
    *tmp_addr = tmp_data;
}

__s32 get_gpio_status(__u32 port, __u32 port_num)
{
    volatile  __u32    *tmp_addr;
    __u32               tmp_data;
    __s32               ret;
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    __wrn("tmp_data=0x%x\n", tmp_data);
    ret = (tmp_data & (1 << port_num)) >> port_num;
    return ret;
}

int pt_de_info(void)
{
    __mp *mp_display;
    __s32 ret = 0;
    unsigned long  pbuffer[3] = {0};
	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        eLIBs_printf("open display device fail!\n");
        return 0;
    }

    if (1)
    {
        __disp_layer_info_t     layer_para;
        __s32 i = 0;
        char *wm[6] = {"NORMAL", "PALETTE", "INTER_BUF", "GAMMA", "SCALER", "reserved"};
        char *format[17] = {"1-bpp", "2-bpp", "4-bpp", "8-bpp", "rgb655", "rgb565", "rgb556",
                            "argb1555", "rgba5551", "rgb888", "argb8888",
                            "YUV444", "YUV422", "YUV420", "YUV411", "CSI RGB", "reserved"
                           };
        char *yuv_mode[6] = {"non-macro block planar", "interleaved", "non-macro block UV combined",
                             "macro block planar", "macro block UV combined", "reserved"
                            };
        eLIBs_printf(".Layers Information Begin.................................................................................................\n");

        for (i = 0; i < 4; i++)
        {
            __s32 pixel_fmt = 0;
            __s32 pixel_mode = 0;
            __s32 pixel_seq = 0;
            pbuffer[0] = i + 100;
            pbuffer[1] = (unsigned long)&layer_para;
            ret = esMODS_MIoctrl(mp_display, MOD_DISP_CMD_LAYER_GET_PARA, 0, (void *)pbuffer);

            if (ret != EPDK_OK)
            {
                eLIBs_printf(".\tLayer %d:close\n", i);
                continue;
            }

            //eLIBs_printf(".\tLayer %d:open\n", i);
            //eLIBs_printf(".\t\twork mode:%s\n", ((__s32)layer_para.mode >= 0 && layer_para.mode < 5) ? wm[layer_para.mode] : wm[5]);
            //eLIBs_printf(".\t\tpriority:%d\n", layer_para.prio);
            pixel_fmt = layer_para.fb.format;
            pixel_mode = layer_para.fb.mode;
            pixel_seq = layer_para.fb.seq;
            eLIBs_printf(".\t\tformat:%s,", (pixel_fmt >= 0 && pixel_fmt < 16) ? format[pixel_fmt] : format[16]);
            eLIBs_printf("\tmode:%s,", (pixel_mode >= 0 && pixel_mode < 5) ? yuv_mode[pixel_mode] : yuv_mode[5]);

            if (pixel_fmt == DISP_FORMAT_ARGB_8888 && pixel_mode == DISP_MOD_INTERLEAVED)
            {
                eLIBs_printf("\tpixels sequence:%s,", (pixel_seq == DISP_SEQ_ARGB) ? "ARGB" : "BGRA");
            }
            else if (pixel_fmt == DISP_FORMAT_YUV422_P && pixel_mode == DISP_MOD_INTERLEAVED)
            {
                if (pixel_seq == DISP_SEQ_UYVY)
                {
                    eLIBs_printf("\tpixels sequence:UYVY,");
                }
                else if (pixel_seq == DISP_SEQ_YUYV)
                {
                    eLIBs_printf("\tpixels sequence:YUYV,");
                }
                else if (pixel_seq == DISP_SEQ_VYUY)
                {
                    eLIBs_printf("\tpixels sequence:VYUY,");
                }
                else if (pixel_seq == DISP_SEQ_YVYU)
                {
                    eLIBs_printf("\tpixels sequence:YVYU,");
                }
            }
            else if (pixel_fmt == DISP_FORMAT_YUV444_P && pixel_mode == DISP_MOD_INTERLEAVED)
            {
                if (pixel_seq == DISP_SEQ_AYUV)
                {
                    eLIBs_printf("\tpixels sequence:AYUV,");
                }
                else if (pixel_seq == DISP_SEQ_VUYA)
                {
                    eLIBs_printf("\tpixels sequence:VUYA,");
                }
            }
            else if (pixel_fmt == DISP_FORMAT_YUV420_P && (pixel_mode == DISP_MOD_NON_MB_UV_COMBINED || pixel_mode == DISP_MOD_MB_UV_COMBINED))
            {
                if (pixel_seq == DISP_SEQ_UVUV)
                {
                    eLIBs_printf("\tpixels sequence:UVUV,");
                }
                else if (pixel_seq == DISP_SEQ_VUVU)
                {
                    eLIBs_printf("\tpixels sequence:VUVU,");
                }
            }
            else
            {
                eLIBs_printf("\tpixels sequence:%d,", pixel_seq);
            }

            eLIBs_printf("\tB R channel swap:%s\n", layer_para.fb.br_swap == 0 ? "RGB" : "BGR");

            if (pixel_mode == DISP_MOD_INTERLEAVED)
            {
                eLIBs_printf(".\t\tframe buffer address 0:%x\n", layer_para.fb.addr[0]);
            }
            else if (pixel_mode == DISP_MOD_NON_MB_UV_COMBINED || pixel_mode == DISP_MOD_MB_UV_COMBINED)
            {
                eLIBs_printf(".\t\tframe buffer address 0:%x,\taddress 1:%x\n", layer_para.fb.addr[0], layer_para.fb.addr[1]);
            }
            else
            {
                eLIBs_printf(".\t\tframe buffer address 0:%x,\taddress 1:%x,\tddress 2:%x\n", layer_para.fb.addr[0], layer_para.fb.addr[1], layer_para.fb.addr[2]);
            }

            eLIBs_printf(".\t\tsource window(x:%d,\ty:%d,\twidth:%d,\theight:%d)\n", layer_para.src_win.x, layer_para.src_win.y, layer_para.src_win.width, layer_para.src_win.height);
            eLIBs_printf(".\t\tscreen window(x:%d,\ty:%d,\twidth:%d,\theight:%d)\n", layer_para.scn_win.x, layer_para.scn_win.y, layer_para.scn_win.width, layer_para.scn_win.height);
            eLIBs_printf(".\t\tcolor key:%s\n", layer_para.ck_enable == 0 ? "disable" : "enable");
            eLIBs_printf(".\t\talpha:%s,\talpha value:%x\n", layer_para.alpha_en == 0 ? "disable" : "enable", layer_para.alpha_val);
            eLIBs_printf(".\t\tpipe:%d\n", layer_para.pipe);
        }

        eLIBs_printf(".Layers Information End.................................................................................................\n");
    }

//    eLIBs_fclose(dispfile);
    return 0;
}



#endif     //  ifndef __misc_c

/* end of misc.c */
