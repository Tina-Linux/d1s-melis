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
#include "az100.h"
#include <string.h>
#include <log.h>

/*
*********************************************************************************************************
*                                    CHECK DATA BUFFER IS COMPRESSED OR NOT
*
* Description: check a input buffer is use compressed by az100 or not.
*
* Arguments  : pBuffer  :  the pointer of input buffer,
*              Size     :  the size of input buffer.
*
* Returns    : return EPDK_TRUE if compressed by az100, return EPDK_FALSE if not.
*
* Note       :
*********************************************************************************************************
*/
__bool AZ100_IsCompress(void *pBuffer, uint32_t Size)
{
    uint8_t  *pHeader;

    if (pBuffer == NULL || Size < AZ100_HEADER_LEN)
    {
        __err("invalid input argument for uncompress check");
        return EPDK_FALSE;
    }

    pHeader = (uint8_t *)pBuffer;

    //check az100 magic valid or not
    if (strncmp((const char *)pHeader, AZ100_MAGIC, 4) == 0)
    {
        //valid az100 compressed data
        return EPDK_TRUE;
    }
    return EPDK_FALSE;
}

/*
*********************************************************************************************************
*                                       GET UNCOMPRESS SIZE
*
* Description: get the uncompress size of a az100 compressed buffer.
*
* Arguments  : pBuffer  :  the pointer of input buffer,
*              Size     :  the size of input buffer.
*
* Returns    : the size of uncompress buffer, 0 if failed.
*
* Note       :
*********************************************************************************************************
*/
uint32_t AZ100_GetUncompressSize(void *pBuffer, uint32_t Size)
{
    uint32_t    i;
    uint8_t     *pHeader;
    uint32_t    UncompressSize;

    if (pBuffer == NULL || Size < AZ100_HEADER_LEN)
    {
        //__wrn("invalid input argument for uncompress size get");
        return 0;
    }

    //check az100 magic valid or not
    if (strncmp((const char *)pBuffer, AZ100_MAGIC, 4) != 0)
    {
        //valid az100 compressed data
        //__wrn("invalid az100 compressed data for uncompress size get");
        return 0;
    }

    pHeader = (__u8 *)pBuffer;

    //should skip uncompress size : 4byte
    //by sunny at 2011-4-11 15:11:13.
    pHeader += AZ100_MAGIC_LEN;

    //get uncompress data size
    UncompressSize = 0;
    for (i = 0; i < 4; i++)
    {
        UncompressSize += ((__u32)pHeader[i] << (i * 8));
    }

    return UncompressSize;
}

/*
*********************************************************************************************************
*                                     UNCOMPRESS DATA BUFFER
*
* Description: uncompress a az100 compressed buffer.
*
* Arguments  : pSource      :  the pointer of source buffer,
*              SourceSize   :  the size of source buffer.
*              pDest        :  the pointer of output buffer,
*              DestSize     :  the size of output buffer.
*
* Returns    : EPDK_OK if succeeded, EPDK_FAIL if failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t AZ100_Uncompress(void *pSource, uint32_t SourceSize, void *pDest, uint32_t DestSize)
{
    uint8_t  *pHeader;

    if (pSource == NULL || SourceSize < AZ100_HEADER_LEN || pDest == NULL)
    {
        //__wrn("invalid input argument for data uncompress");
        return 0;
    }
    pHeader = (uint8_t *)pSource;

    //get uncompress data size
    //should skip uncompress size and magic header,
    //by sunny at 2011-4-11 15:20:10.
    return AZ100_DataUncompress((uint8_t *)pDest, DestSize, pHeader + 8, SourceSize);
}