/*
************************************************************************************************************************
*                                                       RESOURCE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : os_related.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.05.19
*
* Description :
*
* Functions list :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.05.19       1.1.0        build the file
*
************************************************************************************************************************
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
