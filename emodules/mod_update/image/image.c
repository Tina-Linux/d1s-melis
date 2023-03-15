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
#include "encode.h"
#include "image.h"
#include <string.h>
#include <libc.h>
#include <log.h>

#define ITEM_PHOENIX_TOOLS "PXTOOLSB "

#define min( x, y )         ( (x) < (y) ? (x) : (y) )
#define max( x, y )         ( (x) > (y) ? (x) : (y) )

#define HEAD_ID             0       // 头加密接口索引
#define TABLE_ID            1       // 表加密接口索引
#define DATA_ID             2       // 数据加密接口索引
#define IF_CNT              3       // 加密接口个数  现在只有头加密，表加密，数据加密3种
#define MAX_KEY_SIZE        32      // 密码长度

#pragma pack(push, 1)

typedef struct tag_IMAGE_HANDLE
{

    ES_FILE  *fp;           //

    ImageHead_t  ImageHead;     //img头信息

    ImageItem_t *ItemTable;     //item信息表

    RC_ENDECODE_IF_t rc_if_decode[IF_CNT];//解密接口

    char            bWithEncpy; // 是否加密
} IMAGE_HANDLE;


#define INVALID_INDEX       0xFFFFFFFF

#pragma pack(push, 1)
typedef struct tag_ITEM_HANDLE
{
    __u32 index;                    // 在ItemTable中的索引
    __u32 pos;
} ITEM_HANDLE;
#pragma pack(pop)

static __u8 *buffer_encode;

#define ENCODE_BUFFER_SIZE 0x00010000 // 64KB

/*
*********************************************************************************************************
* Function   : Img_Open
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
HIMAGE  Img_Open(const char *ImageFile)
{
    IMAGE_HANDLE *pImage = NULL;
    ImageHead_t ImageHead;
    __u32 dwLen;
    char seed[]  = "img";
    char key_buff[MAX_KEY_SIZE] = {0};
    __u32 key_len = MAX_KEY_SIZE;
    __u32 i;
    __u8 *pHead;
    __u8 *pHeadDecode;
    __u8 *pItemTableDecode ;
    __u32 ItemTableSize ;
    __u8 *ItemTableBuf ; //用于解密table的buffer

#ifdef	UPDATE_DEBUG
	eLIBs_printf("Img_Open\n");
#endif
    if (NULL == ImageFile)
    {
        return NULL;
    }

    if (0 == eLIBs_strlen(ImageFile))
    {
        return NULL;
    }

#ifdef	UPDATE_DEBUG
    eLIBs_printf("malloc size=%d ", sizeof(IMAGE_HANDLE));
#endif
    pImage = (IMAGE_HANDLE *)eLIBs_malloc(sizeof(IMAGE_HANDLE));

    if (NULL == pImage)
    {
        return NULL;
    }

    eLIBs_memset(pImage, 0, sizeof(IMAGE_HANDLE));
    //------------------------------------------------
    //初始化解密接口
    //------------------------------------------------

    for (i = 0; i < IF_CNT; i++)
    {
        pImage->rc_if_decode[i].handle      = NULL;
        pImage->rc_if_decode[i].Initial     = Initial;
        pImage->rc_if_decode[i].EnDecode    = Decode;
        pImage->rc_if_decode[i].UnInitial   = UnInitial;
        eLIBs_memset(key_buff, i, key_len);     //前面的数据初始化为0 note
        key_buff[key_len - 1] = seed[i];    //最后一个字节修改    note
        pImage->rc_if_decode[i].handle = pImage->rc_if_decode[i].Initial(key_buff, &key_len, i);

        if (NULL == pImage->rc_if_decode[i].handle)
        {
            eLIBs_free(pImage);
            __msg("rc_if_decode");
            return NULL;
        }
    }

    //------------------------------------------------
    //打开img文件
    //------------------------------------------------
#ifdef	UPDATE_DEBUG
    eLIBs_printf("ImageFile = %s", ImageFile);
#endif
    pImage->fp = eLIBs_fopen(ImageFile, "rb");

    if (NULL == pImage->fp)
    {
        eLIBs_free(pImage);
        __msg("pImage");
        return NULL;
    }

    //------------------------------------------------
    //读img头
    //------------------------------------------------
    eLIBs_fread(&ImageHead, 1, sizeof(ImageHead_t), pImage->fp);
    pHead = (__u8 *) &ImageHead;
    pHeadDecode = (__u8 *)&pImage->ImageHead;

    if (eLIBs_strncmp(ImageHead.magic, IMAGE_MAGIC, 8) == 0)
    {
        pImage->bWithEncpy = EPDK_FALSE;
    }
    else
    {
        pImage->bWithEncpy = EPDK_TRUE;
    }
#ifdef	UPDATE_DEBUG
    eLIBs_printf("pImage->bWithEncpy = %d", pImage->bWithEncpy);
#endif
    if (pImage->bWithEncpy)
    {
        for (i = 0; i < sizeof(ImageHead_t); i += ENCODE_LEN)
        {
            __u8 *pin = pHead + i;
            __u8 *pout = pHeadDecode + i;

            if (EPDK_OK  != pImage->rc_if_decode[HEAD_ID].EnDecode(pImage->rc_if_decode[HEAD_ID].handle, pin, pout))
            {
                eLIBs_fclose(pImage->fp);
                eLIBs_free(pImage);
                return NULL;
            }
        }
    }
    else
    {
        eLIBs_memcpy(pHeadDecode, pHead, sizeof(ImageHead_t));
    }
    //------------------------------------------------
    //比较magic
    //------------------------------------------------
    if (eLIBs_strncmp(pImage->ImageHead.magic, IMAGE_MAGIC, 8) != 0)
    {
        eLIBs_fclose(pImage->fp);
        __wrn("IMAGE_MAGIC = %s", IMAGE_MAGIC);
        eLIBs_free(pImage);
        return NULL;
    }
#ifdef	UPDATE_DEBUG
    eLIBs_printf("pImage->ImageHead.version=%x........", pImage->ImageHead.version);
#endif
    //esKRNL_TimeDly(100);
    ///////////////////////////////////////////////////////////////////////////
    // 增加版本控制
    if (pImage->ImageHead.version < IMAGE_HEAD_VERSION)
    {
        eLIBs_fclose(pImage->fp);
        eLIBs_free(pImage);
        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    ItemTableSize = pImage->ImageHead.itemcount * sizeof(ImageItem_t);
    pImage->ItemTable = (ImageItem_t *)eLIBs_malloc(ItemTableSize);
    if (NULL == pImage->ItemTable)
    {
        eLIBs_fclose(pImage->fp);
        eLIBs_free(pImage);
        return NULL;
    }

    //------------------------------------------------
    //read ItemTable
    //------------------------------------------------
    ItemTableBuf = (__u8 *)eLIBs_malloc(ItemTableSize);//用于解密table的buffer
    if (NULL == ItemTableBuf)
    {
        eLIBs_fclose(pImage->fp);
        //eLIBs_fclose(pImage->fp);
        eLIBs_free(pImage->ItemTable);
        eLIBs_free(pImage);
        return NULL;
    }

	eLIBs_memset(ItemTableBuf, 0x00, sizeof(ItemTableSize));
    eLIBs_fseek(pImage->fp, pImage->ImageHead.itemoffset, SEEK_SET);
    eLIBs_fread(ItemTableBuf, 1, ItemTableSize, pImage->fp);
    //------------------------------------------------
    // decode ItemTable
    //------------------------------------------------
    pItemTableDecode = (__u8 *)pImage->ItemTable;
    if (pImage->bWithEncpy)
    {
        for (i = 0; i < ItemTableSize; i += ENCODE_LEN)
        {
            __u8 *pin = ItemTableBuf + i;
            __u8 *pout = pItemTableDecode + i;

            if (EPDK_OK  != pImage->rc_if_decode[TABLE_ID].EnDecode(pImage->rc_if_decode[TABLE_ID].handle, pin, pout))
            {
                eLIBs_free(ItemTableBuf);
                eLIBs_fclose(pImage->fp);
                eLIBs_free(pImage->ItemTable);
                eLIBs_free(pImage);
                return NULL;
            }
        }
    }
    else
    {
        eLIBs_memcpy(pItemTableDecode, ItemTableBuf, ItemTableSize);
    }
	
#ifdef	UPDATE_DEBUG
    for (i = 0; i < pImage->ImageHead.itemcount; i++) {
		eLIBs_printf("ItemTable[%d].mainType:%s ", i, pImage->ItemTable[i].mainType);
		eLIBs_printf("ItemTable[%d].subType:%s ", i, pImage->ItemTable[i].subType);
		eLIBs_printf("ItemTable[%d].name:%s\n", i, pImage->ItemTable[i].name);		
	}
#endif
    eLIBs_free(ItemTableBuf);
    return pImage;
}


/*
*********************************************************************************************************
* Function   : Img_OpenItem
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
HIMAGEITEM  Img_OpenItem(HIMAGE hImage, char *MainType, char *subType)
{
    IMAGE_HANDLE *pImage = (IMAGE_HANDLE *)hImage;
    ITEM_HANDLE *pItem  = NULL;
    __u32 dwLen;
    __u32 i;
    char szMainType[30] = "";
    char szSubType[17] = "";
    eLIBs_strncpy(szMainType, MainType, 8);
    eLIBs_strncpy(szSubType, subType, 16);
    __msg("Img_OpenItem now! ");

    if (NULL == pImage || NULL == MainType || NULL == subType)
    {
        return NULL;
    }

    pItem = (ITEM_HANDLE *) eLIBs_malloc(sizeof(ITEM_HANDLE));

    if (NULL == pItem)
    {
        __err("Img_OpenItem", __FILE__, __LINE__, "malloc");
        return NULL;
    }

    pItem->index = INVALID_INDEX;
    pItem->pos = 0;
    __msg("Img_OpenItem[%s][%s]", MainType, subType);
    for (i = 0; i < pImage->ImageHead.itemcount ; i++)
    {
        int nCmp = eLIBs_strnicmp(ITEM_PHOENIX_TOOLS, MainType, MAINTYPE_LEN);

        if (nCmp == 0)//
        {
            if (eLIBs_strnicmp(szMainType, pImage->ItemTable[i].mainType, MAINTYPE_LEN) == 0)
            {
                pItem->index = i;
                return pItem;
            }
        }
        else
        {
            nCmp = eLIBs_strnicmp(szMainType, pImage->ItemTable[i].mainType, MAINTYPE_LEN);

            if (nCmp == 0)
            {
                nCmp = eLIBs_strnicmp(szSubType,  pImage->ItemTable[i].subType,  SUBTYPE_LEN)  ;

                if (nCmp == 0)
                {
                    pItem->index = i;
                    return pItem;
                }
            }
        }
    }

    __msg("Img_OpenItem: cannot find item %s %s", MainType, subType);
    eLIBs_free(pItem) ;
    pItem = NULL;
    return NULL;
}


/*
*********************************************************************************************************
* Function   : Img_GetItemSize
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__u64 Get64bitLen(__u32 lLow, __u32 lHi)
{
    __u64 nRet = lHi;

    if (nRet > 0x00002)
    {
        nRet = 0;
    }

    nRet = nRet << 32;
    nRet = nRet | (0xffffffff & (__u64)lLow);
    return nRet;
}

__u64 Img_GetItemSize(HIMAGE hImage, HIMAGEITEM hItem)
{
    IMAGE_HANDLE *pImage = (IMAGE_HANDLE *)hImage;
    ITEM_HANDLE *pItem  = (ITEM_HANDLE *)hItem;
    __u64 nRet;

    if (NULL == pItem)
    {
        return 0;
    }
    nRet = Get64bitLen(pImage->ItemTable[pItem->index].filelenLo, pImage->ItemTable[pItem->index].filelenHi);
    return nRet;   // 由于有第一层加密，因此都会按照分组进行
}


#define  SIZE_10M   (10*1024*1024)
static __u64 __Img_ReadItemData(HIMAGE hImage, HIMAGEITEM hItem, void *buffer, __u64 Length);

// 根据分组进行加速处理的版本 scott 2009-06-22 10:37:17
//////////////////////////////////////////////////////////////////////////
//每次读取的大小扩展到10M

__u64 Img_ReadItemData(HIMAGE hImage, HIMAGEITEM hItem, void *buffer, __u64 Length)
{
    __u64 readlen = 0;
    __u64 nRet = 0;
    IMAGE_HANDLE *pImage = (IMAGE_HANDLE *)hImage;
    ITEM_HANDLE *pItem  = (ITEM_HANDLE *)hItem;
    __u64 dwLen;
    //__u8 buffer_encode[SIZE_32K];
    __u8 *buffer_encode = NULL;
    __u64 this_read;
    __u64 pos = 0;
    __u64 nLenTmp;
    __u64 nFileLen;
    pEnDecode pfDecode = pImage->rc_if_decode[DATA_ID].EnDecode;
    __u8 *pInPutBuffer = (__u8 *)buffer;
    __u8 *pBufferRead = pInPutBuffer ;
    __u32 i;
    //为了速度不进行参数的检测工作 scott 2009-06-22
    nLenTmp = Get64bitLen(pImage->ItemTable[pItem->index].filelenLo, pImage->ItemTable[pItem->index].filelenHi);
    nFileLen = nLenTmp;

	if (pImage->bWithEncpy == EPDK_FALSE) {
		buffer_encode = (__u8 *)esMEMS_Malloc(0, SIZE_10M);
	}
	
    if (pItem->pos >= nLenTmp) //filelen <= datalen
    {
        __err("Img_ReadItemData", __FILE__, __LINE__,
              "pos(%x) >= pItem->filelen(%x)", nLenTmp);
		if (buffer_encode) {
			eLIBs_free(buffer_encode);
			buffer_encode = NULL;
		}
        goto readEnd;
    }

    //------------------------------------------------
    //约束数据不会超出加密数据的范围
    //------------------------------------------------
    nLenTmp = Get64bitLen(pImage->ItemTable[pItem->index].datalenLo, pImage->ItemTable[pItem->index].datalenHi);
    Length = min(Length, nLenTmp - pItem->pos);

    //------------------------------------------------
    //加密后的数据以16byte进行分组，需要处理跨边界的情况
    //------------------------------------------------
    if ((pItem->pos % ENCODE_LEN) == 0) //pos正好是分组的边界的情况
    {
        nLenTmp = Get64bitLen(pImage->ItemTable[pItem->index].offsetLo, pImage->ItemTable[pItem->index].offsetHi);
        pos = nLenTmp + pItem->pos;
        eLIBs_fseek(pImage->fp, pos, SEEK_SET);
        readlen = 0;

        while (readlen < Length)
        {
            __u32 n ;
            __u32 nReadAlian;
            //------------------------------------------------
            //每次读取n个分组
            //------------------------------------------------
            this_read = min(SIZE_10M, (Length - readlen));
            n = (this_read + ENCODE_LEN - 1) / ENCODE_LEN;  //
            if (buffer_encode)
            {
                eLIBs_memset(buffer_encode, 0, n * ENCODE_LEN);
            }
            nReadAlian =  n * ENCODE_LEN;

            if (pImage->bWithEncpy == EPDK_FALSE)
            {
                pBufferRead = pInPutBuffer + readlen;
                eLIBs_fread(pBufferRead, 1, nReadAlian, pImage->fp);
            }
            else
            {
                __u8 *pin = buffer_encode;
                __u8 *pout = (__u8 *)buffer;

                eLIBs_fread(buffer_encode, 1, nReadAlian, pImage->fp);
                //------------------------------------------------
                //分组数据解密
                //------------------------------------------------
                pout     = pout + readlen;  //实际输出数据的偏移量

                for (i = 0; i < n; i++)     //逐个分组进行解密
                {
                    //------------------------------------------------
                    //每次解密一个分组
                    //------------------------------------------------
                    if (EPDK_OK !=  pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin, pout))
                    {
						if (buffer_encode) {
							eLIBs_free(buffer_encode);
							buffer_encode = NULL;
						}
                        return 0;
                    }
                    pin += ENCODE_LEN;
                    pout += ENCODE_LEN;
                }
            }

            //------------------------------------------------
            //计算实际有效数据长度
            //------------------------------------------------
            readlen += this_read;
        }

        if ((pItem->pos + readlen) >= nFileLen)
        {
            readlen = nFileLen - pItem->pos;
        }

        pItem->pos += readlen;
        nRet = readlen;
        goto readEnd;
    }
    else
    {
        //------------------------------------------------
        //这里强制只处理分组对齐的情况，对于以前的一些固件包可能会引起不兼容的问题，
        //那种情况下只好启用原始版本来处理了
        //------------------------------------------------
        __msg("请按照对齐规则来处理！");
        nRet =   __Img_ReadItemData(hImage, hItem,  buffer, Length);
    }

readEnd:
    if (buffer_encode)
    {
        eLIBs_free(buffer_encode);
    }
    return nRet;
}

//原始的版本，可以运行，不过每次读img文件是16byte，速度不高，需要进行提速
__u64 __Img_ReadItemData(HIMAGE hImage, HIMAGEITEM hItem, void *buffer, __u64 Length)
{
    __u64 readlen = 0;
    IMAGE_HANDLE *pImage = (IMAGE_HANDLE *)hImage;
    ITEM_HANDLE *pItem  = (ITEM_HANDLE *)hItem;
    __u8 buffer_encode[ENCODE_LEN];
    __u64 pos = 0;
    __u64 dwLen;
    __u64  Left_Length ;
    __u64  Left_readlen = 0;
    __u64 nTmp;
    pEnDecode pfDecode = pImage->rc_if_decode[DATA_ID].EnDecode;

    if (NULL == pImage || NULL == pItem || NULL == buffer || 0 == Length)
    {
        return 0;
    }

    if (pItem->pos >= pImage->ItemTable[pItem->index].filelenLo) //filelen <= datalen
    {
        __err("Img_ReadItemData", __FILE__, __LINE__, "pos >= pItem->filelen");
        return 0;
    }

    //------------------------------------------------
    //约束数据不会超出加密数据的范围
    //------------------------------------------------
    nTmp = Get64bitLen(pImage->ItemTable[pItem->index].datalenLo, pImage->ItemTable[pItem->index].datalenHi);
    Length = min(Length, nTmp - pItem->pos);

    if (pImage->bWithEncpy == EPDK_FALSE)
    {
        nTmp = Get64bitLen(pImage->ItemTable[pItem->index].offsetLo, pImage->ItemTable[pItem->index].offsetHi);
        pos = nTmp + pItem->pos;
        eLIBs_fseek(pImage->fp, pos, SEEK_SET);
		eLIBs_printf("%s %s %d Length:%d\n", __FILE__, __func__, __LINE__, Length);
        eLIBs_fread(buffer, 1, Length, pImage->fp);
        pItem->pos += Length;
        return Length;
    }

    //------------------------------------------------
    //加密后的数据以16byte进行分组，需要处理跨边界的情况
    //------------------------------------------------
    if ((pItem->pos % ENCODE_LEN) == 0) //pos正好是分组的边界的情况
    {
        nTmp = Get64bitLen(pImage->ItemTable[pItem->index].offsetLo, pImage->ItemTable[pItem->index].offsetHi);
        pos = nTmp + pItem->pos;
        eLIBs_fseek(pImage->fp, pos, SEEK_SET);

        while (readlen < Length)
        {
            __u8 *pin;
            __u8 *pout;
            //每次读取一个分组
            eLIBs_memset(buffer_encode, 0, ENCODE_LEN);
            eLIBs_fread(buffer_encode, 1, ENCODE_LEN, pImage->fp);
            //分组数据解密
            pin = buffer_encode;
            pout = (__u8 *)buffer;
            pout     = pout + readlen;

            if (EPDK_OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin, pout))
            {
                return 0;
            }

            //计算实际有效数据长度
            readlen += min(Length - readlen, ENCODE_LEN);
        }

        pItem->pos += readlen;
        return readlen;
    }
    else //pos不在边界
    {
        //pos不在边界，向头方向seek
        pos = pImage->ItemTable[pItem->index].offsetLo +
              pItem->pos - (pItem->pos % ENCODE_LEN);
        eLIBs_fseek(pImage->fp, pos, SEEK_SET);

        //-----------------------------------
        //**********************OOOOOOOOOOOOO     *表示已经读取得数据 O表示未读取得数据
        //-----------------------------------
        if ((0 < Length) && (Length < ENCODE_LEN)) //读取的数据不足一个分组长度
        {
            __u32 read = ENCODE_LEN - (pItem->pos % ENCODE_LEN); //分组中未读取的数据长度

            if (Length <= read) //需要读取得数据小于等于分组中未读取的数据长度 只用读一个分组即可
            {
                //-----------------------------------
                //**********************OOOOOOOOOOOOO     *表示已经读取得数据 O表示未读取得数据
                //-----------------------------------
                __u32 read = ENCODE_LEN - pItem->pos % ENCODE_LEN;
                __u8 *pin = buffer_encode;
                __u8 *pout = (__u8 *)buffer;
                eLIBs_memset(buffer_encode, 0, ENCODE_LEN);
                eLIBs_fread(buffer_encode, 1, ENCODE_LEN, pImage->fp);
                //分组数据解密
                pout     = pout + readlen;

                if (EPDK_OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin, pout))
                {
                    return 0;
                }

                readlen = Length;
                pItem->pos += readlen;
                return readlen;
            }
            else //需要读两个分组的数据
            {
                //----------------------------------- //-----------------------------------
                //**********************OOOOOOOOOOOOO //OOOOOOOOOO
                //----------------------------------- //-----------------------------------
                //第一个分组
                __u8 *pin ;
                __u8 *pout;
                __u64 Left_Length;
                __u32 read = ENCODE_LEN - pItem->pos % ENCODE_LEN;
                eLIBs_memset(buffer_encode, 0, ENCODE_LEN);
                eLIBs_fread(buffer_encode, 1, ENCODE_LEN, pImage->fp);
                //分组数据解密
                pin = buffer_encode;
                pout = (__u8 *)buffer;
                pout     = pout + readlen;

                if (EPDK_OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin, pout))
                {
                    return 0;
                }

                readlen += read;
                //第二个分组
                Left_Length = Length - read;            //剩余的数据
                eLIBs_memset(buffer_encode, 0, ENCODE_LEN);
                eLIBs_fread(buffer_encode, 1, ENCODE_LEN, pImage->fp);
                //分组数据解密
                pin = buffer_encode;
                pout = (__u8 *)buffer;
                pout     = pout + readlen;

                if (EPDK_OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin, pout))
                {
                    return 0;
                }

                readlen += Left_Length;
                pItem->pos += readlen;
                return readlen;
            }
        }
        else if (Length >= ENCODE_LEN) //读取的数据不少于一个分组长度
        {
            //-----------------------------------
            //**********************OOOOOOOOOOOOO     *表示已经读取得数据 O表示未读取得数据
            //-----------------------------------
            __u32 read = ENCODE_LEN - pItem->pos % ENCODE_LEN;
            __u8 *pin = buffer_encode;
            __u8 *pout = (__u8 *)buffer;
            eLIBs_memset(buffer_encode, 0, ENCODE_LEN);
            eLIBs_fread(buffer_encode, 1, ENCODE_LEN, pImage->fp);
            //分组数据解密
            pout     = pout + readlen;

            if (EPDK_OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin, pout))
            {
                return 0;
            }

            readlen += read;
            //------------------------------------------------
            //剩余的数据按照分组进行处理
            //------------------------------------------------
            Left_Length = Length - read;
            Left_readlen = 0;

            while (Left_readlen < Left_Length)
            {
                __u8 *pin = buffer_encode;
                __u8 *pout = (__u8 *)buffer;
                //每次读取一个分组
                eLIBs_memset(buffer_encode, 0, ENCODE_LEN);
                eLIBs_fread(buffer_encode, 1, ENCODE_LEN, pImage->fp);
                //分组数据解密
                pout     = pout + readlen;

                if (EPDK_OK != pfDecode(pImage->rc_if_decode[DATA_ID].handle, pin, pout))
                {
                    return 0;
                }

                //计算实际有效数据长度
                Left_readlen += min(Left_Length - Left_readlen, ENCODE_LEN);
            }

            readlen += Left_readlen;
        }

        pItem->pos += readlen;
        return readlen;
    }

    return 0;
}



//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
__s32 Img_CloseItem(HIMAGE hImage, HIMAGEITEM hItem)
{
    ITEM_HANDLE *pItem = (ITEM_HANDLE *)hItem;

    if (NULL == pItem)
    {
        return EPDK_FAIL;
    }

    eLIBs_free(pItem);
    pItem = NULL;
    return EPDK_OK;
}



//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
void  Img_Close(HIMAGE hImage)
{
    IMAGE_HANDLE *pImage = (IMAGE_HANDLE *)hImage;
    __msg("Closing image now! ");

    if (NULL == pImage)
    {
        return ;
    }

    if (NULL != pImage->fp)
    {
        eLIBs_fclose(pImage->fp);
        pImage->fp = NULL;
    }

    if (NULL != pImage->ItemTable)
    {
        eLIBs_free(pImage->ItemTable);
        pImage->ItemTable = NULL;
    }

    eLIBs_memset(pImage, 0, sizeof(IMAGE_HANDLE));
    eLIBs_free(pImage);
    pImage = NULL;
    __msg("CloseImage EPDK_OK ");
    return ;
}
