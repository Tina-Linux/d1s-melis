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
#ifndef __sfte_os_c
#define __sfte_os_c


#include "lib_i.h"
#include <kapi.h>



#if 0//defined( TEST_FILE_MEM_LEAK )
__u32  file_count = 0;
G_FILE  *g_fopen(const char *f, const char *m)
{
    G_FILE *fp;
    fp = t_fopen(f, m);

    if (fp != NULL)
    {
        __msg("OPEN FILE %s. The number of file open is %u.\n", f, ++file_count);
    }

    return fp;
}
__s32 g_fclose(G_FILE *fp)
{
    __msg("CLOSE FILE ! The number of file open is %u.\n", --file_count);
    return t_fclose(fp);
}
#endif  // #if defined( TEST_FILE_MEM_LEAK )


#if 0//defined( TEST_MALLOC_MEM_LEAK )
__u32  malloc_count = 0;
void     *malloc_addr = NULL;
#endif  // #if defined( TEST_MALLOC_MEM_LEAK )





/*--------------------------------------- WINDOWS OS platform ------------------------------------*/
#if 0//define SFT_OS_WIN32



size_t g_fread(void *ptr, size_t size, size_t nobj, G_FILE *stream)
{
    fseek(stream, 0, SEEK_CUR);
    return fread(ptr, size, nobj, stream);
}

size_t g_fwrite(const void *ptr, size_t size, size_t nobj, G_FILE *stream)
{
    fseek(stream, 0, SEEK_CUR);
    return fwrite(ptr, size, nobj, stream);
}


#include <direct.h>
#include <windows.h>
__s32 g_mkdir(const char *dirname)
{
    if (0 == _mkdir(dirname))
    {
        return 0;
    }

    return -1;
}


__s32 g_rmdir(const char *dirname)
{
    if (!dirname)
    {
        return __LINE__;
    }

    if (0 == g_strlen(dirname))
    {
        return __LINE__;
    }

    if (RemoveDirectory(dirname))
    {
        return 0;
    }
    else
    {
        return __LINE__;
    }
}



/*----------------------------------- mutex ----------------------------------*/
g_mutex_t g_create_mutex(void)
{
    return (g_mutex_t)CreateMutex(NULL, 0, NULL);
}

g_mutex_t g_delete_mutex(g_mutex_t mutex, __u8 *err_p)
{
    if (CloseHandle((HANDLE)mutex)  ==  0)
    {
        *err_p = EVENT_ERR;
        return mutex;
    }
    else
    {
        *err_p = EVENT_OK;
        return NULL;
    }
}

void      g_pend_mutex(g_mutex_t mutex, __u8 *err_p)
{
    if (WaitForSingleObject((HANDLE)mutex, INFINITE) != WAIT_OBJECT_0)
    {
        *err_p = EVENT_ERR;
    }
    else
    {
        *err_p = EVENT_OK;
    }
}

__u8     g_post_mutex(g_mutex_t mutex)
{
    if (ReleaseMutex((HANDLE)mutex)  ==  0)
    {
        return EVENT_ERR;
    }
    else
    {
        return EVENT_OK;
    }
}




/*----------------------------------------- EPOS OS -------------------------------------------*/
#else


__s32 g_feof(G_FILE *stream)
{
    return (eLIBs_ferror(stream) == FSYS_ERR_EOF);
}


/*----------------------------------- mutex ----------------------------------*/
g_mutex_t g_create_mutex(void)
{
    return (g_mutex_t)esKRNL_SemCreate(1);
}

g_mutex_t g_delete_mutex(g_mutex_t mutex, __u8 *err_p)
{
    __krnl_event_t *temp;
    temp = esKRNL_SemDel((__krnl_event_t *)mutex, OS_DEL_NO_PEND, err_p);

    if (*err_p != EVENT_OK)
    {
        *err_p = EVENT_ERR;
    }
    else
    {
        *err_p = EVENT_OK;
    }

    return (g_mutex_t)temp;
}

void      g_pend_mutex(g_mutex_t mutex, __u8 *err_p)
{
    esKRNL_SemPend((__krnl_event_t *)mutex, 0, err_p);

    if (*err_p != EVENT_OK)
    {
        *err_p = EVENT_ERR;
    }
    else
    {
        *err_p = EVENT_OK;
    }
}

__u8     g_post_mutex(g_mutex_t mutex)
{
    if (esKRNL_SemPost((__krnl_event_t *)mutex) != EVENT_OK)
    {
        return EVENT_ERR;
    }
    else
    {
        return EVENT_OK;
    }
}




/*----------------------------------------- platform end -----------------------------------------*/
#endif







#endif     //  ifndef __sfte_os_c

/* end of sfte_os.c  */
