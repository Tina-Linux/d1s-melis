/*
************************************************************************************************************************
*                                                   language resource
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_theme.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.29
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.10.29        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_THEME_C__
#define  __DSK_THEME_C__


#include "functions_i.h"
#include <libc/eLIBs_az100.h>
#include <kconfig.h>

#define USE_BUFFER  1// 0  
#define ID2HDL_NUM  19 //1024   //在进入karaoke_mainwin    时 预先加载大容量的图片数量

typedef struct _theme_t
{
    __u32   id;
    __u32   size;//原始的大小
    __u32   size_com;//存储的大小
    void    *buf;
} theme_t, *HTHEME_i;

typedef struct
{
    __s32   id;
    HTHEME  hdl;
} __id2hdl_t;


static HRES         hres    = NULL;
static HRES         h_kres  = NULL;// add by hot lee for  karaoke
static __u32        style   = 0;
static __id2hdl_t   g_id2hdl[ID2HDL_NUM]    = {0};

static void  _release_theme_res(HTHEME_i htheme);


/*
****************************************************************************************************
*Name        : dsk_theme_init
*Prototype   : __s32 dsk_theme_init( const char *file )
*Arguments   : file    input. the theme resource file.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : init theme resource module.
*Other       :
****************************************************************************************************
*/
int32_t dsk_theme_init(const char *file)
{
    if (hres == NULL)
    {
        hres = OpenRes((char *)file, 0);

        if (hres == NULL)
        {
            __err("Error in opening resource file \"%s\".", file);
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

/*
****************************************************************************************************
*Name        : dsk_theme_init_ex
*Prototype   : __s32 dsk_theme_init( const char *file )
*Arguments   : file    input. the theme resource file.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : init theme resource module.
*Other       :
****************************************************************************************************
*/
int32_t dsk_theme_init_ex(const char *file)
{
    if (h_kres == NULL)
    {
        h_kres = OpenRes((char *)file, 0);

        if (h_kres == NULL)
        {
            __wrn("Error in opening resource file \"%s\".", file);
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}


/*
****************************************************************************************************
*Name        : dsk_theme_exit
*Prototype   : void dsk_theme_exit( void )
*Arguments   : void
*Return      : void
*Description : exit theme resource module.
*Other       :
****************************************************************************************************
*/
void  dsk_theme_exit(void)
{
    if (hres != NULL)
    {
        CloseRes(hres);
        hres = NULL;
    }
}

/*
****************************************************************************************************
*Name        : dsk_theme_exit_ex
*Prototype   : void dsk_theme_exit( void )
*Arguments   : void
*Return      : void
*Description : exit theme resource module.
*Other       :
****************************************************************************************************
*/
void  dsk_theme_exit_ex(void)
{
    if (h_kres != NULL)
    {
        CloseRes(h_kres);
        h_kres = NULL;
    }
}


/*
****************************************************************************************************
*Name        : dsk_theme_set_style
*Prototype   : __s32 dsk_theme_set_style( __u32 style_id )
*Arguments   : style_id    input. theme style id.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set theme style.
*Other       :
****************************************************************************************************
*/
int32_t dsk_theme_set_style(uint32_t style_id)
{
    style   = style_id;
    return EPDK_OK;
}


/*
****************************************************************************************************
*Name        : dsk_theme_get_style
*Prototype   : __s32 dsk_theme_get_style( void )
*Arguments   : void
*Return      : the current theme style id.
*Description : get the current theme style.
*Other       :
****************************************************************************************************
*/
int32_t dsk_theme_get_style(void)
{
    return style;
}


/*
****************************************************************************************************
*Name        : dsk_theme_open
*Prototype   : HTHEME dsk_theme_open( __u32 theme_id )
*Arguments   : theme_id   input. the ID of a theme
*Return      : the handle of a theme. NULL means error.
*Description : open a handle of a theme.
*Other       :
****************************************************************************************************
*/
HTHEME dsk_theme_open(uint32_t theme_id)
{
    int32_t     ret;
    HTHEME_i    htheme = NULL;
    void        *pbuf;

    if (hres == NULL)
    {
        return 0;
    }

    htheme  = (HTHEME_i) esMEMS_Malloc(0, sizeof(theme_t));

    if (htheme == NULL)
    {
        __wrn("Error in allocating memory.");
        return 0;
    }

    eLIBs_memset(htheme, 0, sizeof(theme_t));
    htheme->id          = theme_id;
    htheme->size_com    = GetResSize(hres, style, theme_id);
    pbuf                = esMEMS_Balloc(htheme->size_com);

    if (NULL == pbuf)
    {
        __wrn("*************mem not enough, theme_id=%d***********", theme_id);
        goto error;
    }

    ret     = GetRes(hres, 0, theme_id, pbuf, htheme->size_com);

    //__msg("hres=%x,theme_id=%d,pbuf=%x,htheme->size_com=%d", hres,theme_id, pbuf, htheme->size_com);
    if (-1 == ret)
    {
        __wrn("***********GetRes fail***********");
        goto error;
    }

    if (EPDK_TRUE == AZ100_IsCompress(pbuf, htheme->size_com))
    {
        htheme->size = AZ100_GetUncompressSize(pbuf, htheme->size_com);
    }
    else//未压缩
    {
        htheme->size = htheme->size_com;
    }

    if (htheme->size == 0)
    {
        __wrn("Fail in getting the size of res %u.", htheme->id);
        goto error;
    }

    if (pbuf)
    {
        esMEMS_Bfree(pbuf, htheme->size_com);
        pbuf = NULL;
    }

    return (HTHEME) htheme;
error:
    _release_theme_res(htheme);

    if (pbuf)
    {
        esMEMS_Bfree(pbuf, htheme->size_com);
        pbuf = NULL;
    }

    return 0;
}

/*
****************************************************************************************************
*Name        : dsk_theme_open_ex
*Prototype   : HTHEME dsk_theme_open( __u32 theme_id )
*Arguments   : theme_id   input. the ID of a theme
*Return      : the handle of a theme. NULL means error.
*Description : open a handle of a theme.
*Other       :
****************************************************************************************************
*/
HTHEME dsk_theme_open_ex(__u32 theme_id)
{
    int32_t     ret;
    HTHEME_i    htheme = NULL;
    void        *pbuf;

    if (h_kres == NULL)
    {
        return 0;
    }

    htheme  = (HTHEME_i) esMEMS_Malloc(0, sizeof(theme_t));

    if (htheme == NULL)
    {
        __wrn("Error in allocating memory.");
        return 0;
    }

    eLIBs_memset(htheme, 0, sizeof(theme_t));

    htheme->id          = theme_id;
    htheme->size_com    = GetResSize(h_kres, style, theme_id);
    pbuf                = esMEMS_Balloc(htheme->size_com);

    if (NULL == pbuf)
    {
        __wrn("*************mem not enough, theme_id=%d***********", theme_id);
        goto error;
    }

    ret = GetRes(h_kres, 0, theme_id, pbuf, htheme->size_com);

    //__msg("hres=%x,theme_id=%d,pbuf=%x,htheme->size_com=%d", hres,theme_id, pbuf, htheme->size_com);
    if (-1 == ret)
    {
        __wrn("***********GetRes fail***********");
        goto error;
    }

    if (EPDK_TRUE == AZ100_IsCompress(pbuf, htheme->size_com))
    {
        htheme->size = AZ100_GetUncompressSize(pbuf, htheme->size_com);
    }
    else//未压缩
    {
        htheme->size = htheme->size_com;
    }

    if (htheme->size == 0)
    {
        __wrn("Fail in getting the size of res %u.", htheme->id);
        goto error;
    }

    if (pbuf)
    {
        esMEMS_Bfree(pbuf, htheme->size_com);
        pbuf = NULL;
    }

    return (HTHEME) htheme;
error:
    _release_theme_res(htheme);

    if (pbuf)
    {
        esMEMS_Bfree(pbuf, htheme->size_com);
        pbuf = NULL;
    }

    return 0;
}

/*
****************************************************************************************************
*Name        : dsk_theme_close
*Prototype   : void  dsk_theme_close( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : void
*Description : close a handle of a theme.
*Other       :
****************************************************************************************************
*/
void  dsk_theme_close(HTHEME handle)
{
    if (handle != 0)
    {
        _release_theme_res((HTHEME_i) handle);
    }
}

/*
****************************************************************************************************
*Name        : dsk_theme_close_ex
*Prototype   : void  dsk_theme_close( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : void
*Description : close a handle of a theme.
*Other       :
****************************************************************************************************
*/
void  dsk_theme_close_ex(HTHEME handle)
{
    if (handle != 0)
    {
        _release_theme_res((HTHEME_i) handle);
    }
}

void  dsk_theme_really_close_ex(HTHEME handle)
{
    if (handle != 0)
    {
#if  USE_BUFFER
        {
            //遍历id表，从id表删除
            __s32   i;
            __s32   cnt;

            cnt = sizeof(g_id2hdl) / sizeof(g_id2hdl[0]);

            for (i = 0 ; i < cnt ; i++)
            {
                if (g_id2hdl[i].id > 0)
                {
                    if (g_id2hdl[i].hdl == handle)
                    {
                        _release_theme_res((HTHEME_i) handle);
                        g_id2hdl[i].id = 0;
                        g_id2hdl[i].hdl = 0;
                        return ;
                    }
                }
            }
        }
#endif
        //没有在id表里面，直接删除(此时可能id表满了)
        __wrn("not found in idlist, handle=0x%x", handle);
        _release_theme_res((HTHEME_i) handle);
    }
}
void  dsk_theme_really_close_by_id_ex(__u32 theme_id)
{
    if (theme_id > 0)
    {
#if  USE_BUFFER
        {
            //遍历id表，从id表删除
            __s32   i;
            __s32   cnt;

            cnt = sizeof(g_id2hdl) / sizeof(g_id2hdl[0]);

            for (i = 0 ; i < cnt ; i++)
            {
                if (g_id2hdl[i].id == theme_id)
                {
                    if (g_id2hdl[i].hdl)
                    {
                        _release_theme_res((HTHEME_i)(g_id2hdl[i].hdl));
                        g_id2hdl[i].id = 0;
                        g_id2hdl[i].hdl = 0;
                        return ;
                    }
                }
            }
        }
#endif
    }
}

/*
****************************************************************************************************
*Name        : dsk_theme_hdl2size
*Prototype   : __s32 dsk_theme_hdl2size( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : the size of a theme. return value <= 0, means error.
*Description : get the size of a theme.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_hdl2size(HTHEME handle)
{
    HTHEME_i  htheme    = (HTHEME_i) handle;

    if (handle == 0)
    {
        __wrn("handle is null, handle to size failed!");
        return -1;
    }

    return htheme->size;
}

/*
****************************************************************************************************
*Name        : dsk_theme_hdl2size_ex
*Prototype   : __s32 dsk_theme_hdl2size_ex( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : the size of a theme. return value <= 0, means error.
*Description : get the size of a theme.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_hdl2size_ex(HTHEME handle)
{
    HTHEME_i  htheme    = (HTHEME_i) handle;

    if (handle == 0)
    {
        __wrn("handle is null, handle to size failed!");
        return -1;
    }

    return htheme->size;
}




/*
****************************************************************************************************
*Name        : dsk_theme_hdl2buf
*Prototype   : void *dsk_theme_hdl2buf( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : the pointer to the buffer of a theme. NULL means error.
*Description : get the pointer to the buffer of a theme.
*Other       :
****************************************************************************************************
*/
void *dsk_theme_hdl2buf(HTHEME handle)
{
    __s32       size;
    __s32       ret;
    void        *pbuf;
    HTHEME_i    htheme = (HTHEME_i) handle;

    if (handle == 0)
    {
        __wrn("handle is null, handle to buf failed!");
        return NULL;
    }

    if (htheme->buf != NULL)
    {
        return htheme->buf;
    }

    htheme->buf = (void *)esMEMS_Balloc(htheme->size);

    if (htheme->buf == NULL)
    {
        __wrn("Error in alloc size %x.", htheme->size);
        return NULL;
    }

    pbuf = esMEMS_Balloc(htheme->size_com);

    if (NULL == pbuf)
    {
        __wrn("*************mem not enough***********");
        esMEMS_Bfree(htheme->buf, htheme->size);

        htheme->buf = NULL;
        return NULL;
    }

    ret = GetRes(hres, style, htheme->id, pbuf, htheme->size_com);

    if (-1 == ret)
    {
        __wrn("***********GetRes fail***********");
        esMEMS_Bfree(htheme->buf, htheme->size);
        esMEMS_Bfree(pbuf, htheme->size_com);

        htheme->buf = NULL;
        pbuf        = NULL;
        return NULL;
    }

    if (EPDK_TRUE == AZ100_IsCompress(pbuf, htheme->size_com))
    {
        __s32 ret;

        ret = AZ100_Uncompress(pbuf, htheme->size_com, htheme->buf, htheme->size);

        if (EPDK_FAIL == ret)
        {
            __wrn("***********AZ100_Uncompress fail***********");
            esMEMS_Bfree(htheme->buf, htheme->size);
            esMEMS_Bfree(pbuf, htheme->size_com);

            htheme->buf = NULL;
            pbuf        = NULL;
            return NULL;
        }
    }
    else
    {
        eLIBs_memcpy(htheme->buf, pbuf, htheme->size_com);
    }

    esMEMS_Bfree(pbuf, htheme->size_com);

    pbuf = NULL;
    return htheme->buf;
}

/*
****************************************************************************************************
*Name        : dsk_theme_hdl2buf_ex
*Prototype   : void *dsk_theme_hdl2buf_ex( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : the pointer to the buffer of a theme. NULL means error.
*Description : get the pointer to the buffer of a theme.
*Other       :
****************************************************************************************************
*/
void *dsk_theme_hdl2buf_ex(HTHEME handle)
{
    __s32       size;
    __s32       ret;
    void        *pbuf;
    HTHEME_i    htheme = (HTHEME_i) handle;

    if (handle == 0)
    {
        __wrn("handle is null, handle to buf failed!");
        return NULL;
    }

    if (htheme->buf != NULL)
    {
        return htheme->buf;
    }

    htheme->buf = (void *)esMEMS_Balloc(htheme->size);

    if (htheme->buf == NULL)
    {
        __wrn("Error in alloc size %x.", htheme->size);
        return NULL;
    }

    pbuf = esMEMS_Balloc(htheme->size_com);

    if (NULL == pbuf)
    {
        __wrn("*************mem not enough***********");
        esMEMS_Bfree(htheme->buf, htheme->size);

        htheme->buf = NULL;
        return NULL;
    }

    ret = GetRes(h_kres, style, htheme->id, pbuf, htheme->size_com);

    if (-1 == ret)
    {
        __wrn("***********GetRes fail***********");
        esMEMS_Bfree(htheme->buf, htheme->size);
        esMEMS_Bfree(pbuf, htheme->size_com);

        htheme->buf = NULL;
        pbuf        = NULL;
        return NULL;
    }

    if (EPDK_TRUE == AZ100_IsCompress(pbuf, htheme->size_com))
    {
        __s32 ret;

        ret = AZ100_Uncompress(pbuf, htheme->size_com, htheme->buf, htheme->size);

        if (EPDK_FAIL == ret)
        {
            __wrn("***********AZ100_Uncompress fail***********");
            esMEMS_Bfree(htheme->buf, htheme->size);
            esMEMS_Bfree(pbuf, htheme->size_com);

            htheme->buf = NULL;
            pbuf        = NULL;
            return NULL;
        }
    }
    else
    {
        eLIBs_memcpy(htheme->buf, pbuf, htheme->size_com);
    }

    esMEMS_Bfree(pbuf, htheme->size_com);

    pbuf    = NULL;
    return htheme->buf;
}

static void  _release_theme_res(HTHEME_i htheme)
{
    if (htheme != NULL)
    {
        if (htheme->buf != NULL)
        {
            esMEMS_Bfree(htheme->buf, htheme->size);
            htheme->buf = NULL;
        }

        esMEMS_Mfree(0, htheme);
    }
}



#endif     //  ifndef __DSK_THEME_C__

/* end of dsk_theme.c */
