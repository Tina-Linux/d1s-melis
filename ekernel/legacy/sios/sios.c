/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                 Sios Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sios.c
* By      : kevin.z
* Version : v1.0
* Date    : 2010-11-19 13:37
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-19 13:37    kevin.z     1.1     build the file.
*********************************************************************************************************
*/
#include <kapi.h>
#include <log.h>
#include <string.h>
#include <port.h>
#include "dfs_posix.h"
#include "rtthread.h"
#include "../aw/multi_console/cli_console.h"

#define SIOS_BUFFER_LEN     2048
typedef struct SIOS_BUFFER
{
    uint8_t     *pBuffer;
    uint32_t    BufferLen;
    uint32_t    BufferPos;
} sios_buffer_t;

sios_buffer_t SiosBuffer;
static  int hOutFile = 0;

/*
*********************************************************************************************************
*                                   SIOS INIT
*
* Description: sios init.
*
* Arguments  : none
*
* Returns    : result
*
* Note       :
*********************************************************************************************************
*/
int32_t SIOS_Init(void)
{
    hOutFile = 0;
    //initialize sios buffer
    SiosBuffer.BufferLen = SIOS_BUFFER_LEN;
    SiosBuffer.pBuffer   = k_malloc(SiosBuffer.BufferLen);
    if (SiosBuffer.pBuffer == NULL)
    {
        __err("fatal error, malloc sios buffer failed!");
        return EPDK_FAIL;
    }
    SiosBuffer.BufferPos = 0;
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   SIOS EXIT
*
* Description: sios exit
*
* Arguments  : none
*
* Returns    : result
*
* Note       :
*********************************************************************************************************
*/
int32_t SIOS_Exit(void)
{
    if (hOutFile)
    {
        //write buffer data
        if (SiosBuffer.BufferPos)
        {
            write(hOutFile, SiosBuffer.pBuffer, SiosBuffer.BufferPos);
            SiosBuffer.BufferPos = 0;
        }
        //close output file
        close(hOutFile);
    }
    k_free(SiosBuffer.pBuffer);

    return EPDK_OK;
}



//write buffer to file
int32_t SIOS_putstrtofile(const char *str)
{
    uint32_t  Left = strlen(str);
    uint32_t  CopyLen;
    uint8_t  *pSrc = (__u8 *)str;
    uint8_t  *pDst = SiosBuffer.pBuffer + SiosBuffer.BufferPos;
    while (Left)
    {
        if (SiosBuffer.BufferPos + Left < SiosBuffer.BufferLen)
        {
            CopyLen = Left;
        }
        else
        {
            CopyLen = SiosBuffer.BufferLen - SiosBuffer.BufferPos;
        }
        rt_memcpy(pDst, pSrc, CopyLen);
        SiosBuffer.BufferPos += CopyLen;
        Left -= CopyLen;
        pDst += CopyLen;
        pSrc += CopyLen;
        if (SiosBuffer.BufferPos == SiosBuffer.BufferLen)
        {
            write(hOutFile, SiosBuffer.pBuffer, SiosBuffer.BufferLen);
            SiosBuffer.BufferPos = 0;
            pDst = SiosBuffer.pBuffer;
        }
    }
    return 0;
}

uint8_t esSIOS_getchar(void)
{
    uint8_t ch = 0;

    cli_console_read(get_clitask_console(), &ch, 1);
    return ch;
}

int32_t esSIOS_putchar(char data)
{
    return cli_console_write(get_clitask_console(), &data, 1);
}

/*
*********************************************************************************************************
*                                   SIOS PUT STRING
*
* Description: sios put string
*
* Arguments  : str  string need be output.
*
* Returns    : none.
*
* Note       :
*********************************************************************************************************
*/
void esSIOS_putstr(const char *str)
{
    if (hOutFile)
    {
        SIOS_putstrtofile(str);
        return;
    }
#if 0
    else
    {
        struct kstatpt dpart;
        memset(&dpart, 0x00, sizeof(dpart));
        if (esFSYS_statpt("F:\\", (__hdle)&dpart) == EPDK_OK)
        {
            int esSIOS_SetOutput(const char *filename, const char *mode);
            esSIOS_SetOutput("f:\\log.txt", "a+");
            esSIOS_SetOutput("sync", "a+");
        }
    }
#endif
    return cli_console_write(get_clitask_console(), str, rt_strlen(str));
}


/*
*********************************************************************************************************
*                                   SIOS GET STRING
*
* Description: sios get string.
*
* Arguments  : str      string buffer for get string.
*              size     size of the string buffer.
*
* Returns    : result.
*
* Note       :
*********************************************************************************************************
*/
void esSIOS_gets(char *str)
{
    //it is not safe to put char at str, which is a poiter unknown length(boundry).
}


/*
*********************************************************************************************************
*                                   SIOS PUT ARGUMENT
*
* Description: sios put argument.
*
* Arguments  : arg      argument for output.
*              format   format for output.
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void esSIOS_putarg(__u32 arg, char format)
{
//    UART_PutArg(arg, format);
}

#if 0
/*
*********************************************************************************************************
*                                   SIOS GET CHAR
*
* Description: sios get char;
*
* Arguments  : none;
*
* Returns    : character got from uart;
*
* Note       :
*********************************************************************************************************
*/
uint8_t esSIOS_getchar(void)
{
    return UART_GetChar();
}

int32_t esSIOS_putchar(char data)
{
    return UART_PutChar(data);
}
#endif
/*
*********************************************************************************************************
*                                   SIOS SET BAUND RATE
*
* Description: sios debug set baund rate;
*
* Arguments  : rate     baund rate for uart;
*
* Returns    : result;
*                   EPDK_OK,    sios set uart baund rate successed;
*                   EPDK_FAILED,sios set uart baund rate failed;
*
* Note       : the baund rate will be re-set when adjust apb clock.
*********************************************************************************************************
*/
void esSIOS_setbaud(__u32 baud)
{
    //UART_SetRate(baud);
}

int32_t esSIOS_SetOutput(const char *filename, const char *mode)
{
    int hNewOut;
    if (strcmp(filename, "uart") == 0)
    {
        if (hOutFile)
        {
            if (SiosBuffer.BufferPos)
            {
                write(hOutFile, SiosBuffer.pBuffer, SiosBuffer.BufferPos);
                SiosBuffer.BufferPos = 0;
            }
            close(hOutFile);
            hOutFile = 0;
        }
        return EPDK_OK;
    }
    else if (strcmp(filename, "sync") == 0)
    {
        if (hOutFile)
        {
            if (SiosBuffer.BufferPos)
            {
                write(hOutFile, SiosBuffer.pBuffer, SiosBuffer.BufferPos);
                SiosBuffer.BufferPos = 0;
            }
            fsync(hOutFile);
        }
        return EPDK_OK;
    }
    hNewOut = open(filename, 0);
    if (hNewOut == 0)
    {
        __wrn("open file [%s] mode [%s] failed\n", filename, mode);
        return EPDK_FAIL;
    }
    if (hOutFile)
    {
        if (SiosBuffer.BufferPos)
        {
            write(hOutFile, SiosBuffer.pBuffer, SiosBuffer.BufferPos);
            SiosBuffer.BufferPos = 0;
        }
        close(hOutFile);
    }
    hOutFile = hNewOut;
    return EPDK_OK;
}
