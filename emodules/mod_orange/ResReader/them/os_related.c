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
#include "os_related.h"



/*---------------------------------------- WIN32_PLATFORM ----------------------------------------*/
#if defined( WIN32_PLATFORM )



uint32 res_mkdir(const char *dirname)
{
    return 0;
}

/*----------------------------------------- EPOS_PLATFORM ----------------------------------------*/
#elif defined( EPOS_PLATFORM )

RES_FILE  *res_fopen(const char *filename, const char *mode)
{
    return eLIBs_fopen(filename, mode);
}

int32  res_fclose(RES_FILE *stream)
{
    eLIBs_fclose(stream);
    return 0;
}

int32  res_remove(const char *filename)
{
    return eLIBs_remove(filename);
}

size_t res_fread(void *ptr, size_t size, size_t nobj, RES_FILE *stream)
{
    return eLIBs_fread(ptr, size, nobj, stream);
}

size_t res_fwrite(const void *ptr, size_t size, size_t nobj, RES_FILE *stream)
{
    return eLIBs_fwrite(ptr, size, nobj, stream);
}

int32  res_fseek(RES_FILE *stream, long offset, int origin)
{
    /*
        if( origin == SEEK_END )
            return fseek( stream, stream->size + offset, SEEK_SET );
        else
            return fseek( stream, offset, origin );
    */
    return eLIBs_fseek(stream, offset, origin);
}

int32 res_feof(RES_FILE *stream)
{
    return (eLIBs_ferror(stream) == FSYS_ERR_EOF);
}

uint32 res_mkdir(const char *dirname)
{
    return eLIBs_mkdir(dirname);
}

DIR *res_opendir(const char *dirname)
{
    return eLIBs_opendir(dirname);
}

uint32 res_closedir(DIR *pdir)
{
    return eLIBs_closedir(pdir);
}

DIRENT *res_readdir(DIR *pdir)
{
    return eLIBs_readdir(pdir);
}

void res_dirent2name(DIRENT *pdirent, char *pbuffer)
{
    // FS_DirEnt2Name( pdirent,  pbuffer );
}

void res_dirent2attr(DIRENT *pdirent, uint8 *pattr)
{
    // FS_DirEnt2Name( pdirent, pattr );
}

/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif
