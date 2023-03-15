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
#include <typedef.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "ktype.h"
#include "kapi.h"

#include <hal_mem.h>
/*
*********************************************************************************************************
*                                       RESTHEM OPEN
*
* Description: open a resthem file.
*
* Arguments  : file     : the path of open resthem file,
*              pMode    : the mode of open file.
*
* Returns    : the handler of resthem, NULL if failed.
*
* Note       :
*********************************************************************************************************
*/
void *esRESM_ROpen(const char *file, const char *mode)
{
    unsigned char   res_head[16] = {0};
    __resm_rsb_t    *hRes;

    if (file == NULL)
    {
        __err("invalid file path for resthem open.");
        return NULL;
    }

    //allocate resouce manager handler
    hRes = (__resm_rsb_t *)hal_malloc(sizeof(__resm_rsb_t));
    if (hRes == (__resm_rsb_t *)0)
    {
        __err("allocate buffer for res manager failed.");
        return NULL;
    }

    // open res file
    hRes->hResfile = esFSYS_fopen(file, "r");
    if (hRes->hResfile == 0)
    {
        __err("cannot open file %s.", file);
        hal_free((void *)hRes);
        return NULL;
    }
    //--------------------------------------------------------------------------
    //check if the file is a valid res file
    //--------------------------------------------------------------------------
    esFSYS_fread(res_head, 16, 1, hRes->hResfile);
    //check if the file contain valid res magic
    if (!strncmp((char *)res_head, ".resthem", 8) || !strncmp((char *)res_head, ".langdat", 8))
    {
        //current file is a valid res file, set some prameter, and return
        esFSYS_fseek(hRes->hResfile, 0, SEEK_END);
        hRes->size = esFSYS_ftell(hRes->hResfile);
        hRes->offset = 0;
        esFSYS_fseek(hRes->hResfile, 0, SEEK_SET);

        if (hRes->size == 0)
        {
            __err("res file size is zero.");
        }
        return hRes;
    }

    __err("invalid resthem file [%s] access.", file);
    esFSYS_fclose(hRes->hResfile);
    hal_free((void *)hRes);

    return NULL;
}

/*
*********************************************************************************************************
*                                       RESTHEM CLOSE
*
* Description: close a resthem file.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t esRESM_RClose(__resm_rsb_t *res)
{
    if (!res || !res->hResfile)
    {
        __err("close res file failure.");
        return 0;
    }
    esFSYS_fclose(res->hResfile);
    hal_free((void *)res);
    return 0;
}

/*
*********************************************************************************************************
*                                       RESTHEM READ
*
* Description: read resthem file.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
uint32_t esRESM_RRead(void *pdata, uint32_t size, uint32_t n, __resm_rsb_t *res)
{
    if (!pdata || !res || !res->hResfile)
    {
        __err("read res file failure.");
        return 0;
    }
    return esFSYS_fread(pdata, size, n, res->hResfile);
}

/*
*********************************************************************************************************
*                                       RESTHEM SEEK
*
* Description: seek resthem file.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t esRESM_RSeek(__resm_rsb_t *res, int32_t Offset, int32_t Whence)
{
    if (!res || !res->hResfile)
    {
        __err("seek res file failure.");
        return 0;
    }
    return esFSYS_fseek(res->hResfile, Offset + res->offset, Whence);
}
