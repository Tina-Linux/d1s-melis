/*
*********************************************************************************************************
*File   :   Decord.cpp
*version:   v1.0    2008-10-28
*By     :   Andy.zhang
*Brief  :   按照规定的格式读取data文件中的内容
*********************************************************************************************************
*/

#include "LangDec_private.h"
#ifdef SIM_PC_WIN
extern __declspec(dllimport) FILE *OpenLangue(const char *szAppFile);
#pragma pack(1)
#endif
/**
*@brief:  打开指定路径文件，返回文件句柄；
*@param:  szAppFile [in]    文件路径名
*@param:  mode      [in]    打开模式，目前版本此参数没有意义
*@return: HLANG     HHEAD*  数据结构
**/
HLANG   Lang_Open(char *szAppFile, __u32 mode)
{
    FILE        *fp;
    HHEAD       *pHHead;
    LANG_HEAD   m_LangHead;
    LANG        m_Lang;
    char        *m_langbuffer;
    unsigned int i;
    /*
    #if LANGDEC_CHECK_ARG_EN > 0
        if(szAppFile == NULL)
        {
            __wrn("szAppFile is empty!");
            return NULL;
        }
    #endif
    */
    mode = mode;
    //分配内存HHEAD *
    pHHead = (HHEAD *)malloc(sizeof(HHEAD));

    if (pHHead == NULL)
    {
#if LANGDEC_CHECK_ARG_EN > 0
        __wrn("pHHead is empty!");
#endif
        return NULL;
    }

    fp = fopen1(szAppFile, "r_lang");//(FILE*)esRESM_ROpen(szAppFile, "r_lang");//
    if (fp == NULL)
    {
#if LANGDEC_CHECK_ARG_EN > 0
        __wrn("open file fail!");
#endif
        free((void *)pHHead);
        return NULL;
    }
    pHHead->fp = fp;
    //读取lang_head
    fseek1(fp, 0, SEEK_SET);
    fread1(&m_LangHead, sizeof(m_LangHead), 1, fp);

    //判断文件头，是否指定类型
    if (strcmp((char *)m_LangHead.SecName, SEC_NAME))
    {
#if LANGDEC_CHECK_ARG_EN > 0
        __inf("the file is not lang bin file");
#endif
        fclose1(fp);
        free((void *)pHHead);
        return NULL;
    }

    //根据读取文件头，初始化HHEAD *数据结构
    pHHead->LangTabOff  = m_LangHead.LangTabOff;
    pHHead->LangSize    = m_LangHead.LangSize;
    pHHead->LangNum     = m_LangHead.LangNum;
    pHHead->StringSize  = m_LangHead.StringSize;
    pHHead->StringNum   = m_LangHead.StringNum;
    pHHead->align       = m_LangHead.align;

    if (m_LangHead.version < 0x0200)
    {
        pHHead->pIdTab = NULL;
    }
    else
    {
        pHHead->pIdTab = (__u16 *)malloc(m_LangHead.StringNum * sizeof(__u16));
        fseek1(fp, 44, SEEK_SET);
        fread1(pHHead->pIdTab, 1, m_LangHead.StringNum * sizeof(__u16), fp);
    }

    //分配内存，保存lang_list
    m_langbuffer = (char *)malloc(m_LangHead.LangNum * m_LangHead.LangSize * sizeof(char));
    //寻找到lang_list起始地址
    fseek1(fp, m_LangHead.LangTabOff, SEEK_SET);
    //读取lang_list
    fread1(m_langbuffer, sizeof(char), m_LangHead.LangNum * m_LangHead.LangSize, fp);
    pHHead->pHLangList  = (HLANGLIST *)malloc(sizeof(HLANGLIST) * pHHead->LangNum);

    for (i = 0; i < pHHead->LangNum; i++)
    {
        //4字节对齐,注意拷贝长度
        memcpy(&m_Lang, m_langbuffer + i * m_LangHead.LangSize, sizeof(m_Lang)); //4字节对齐；
        pHHead->pHLangList[i].LangID        = m_Lang.LangID;
        pHHead->pHLangList[i].StringTabOff  = m_Lang.StringTabOff;
    }

    //释放内存
    free(m_langbuffer);
    return (void *)pHHead;
}

/**
*@brief:  读取文件指定地址处的数据
*@param:  hLang     [in]    文件句柄；
*@param:  address   [in]    相对文件起点处的偏移地址；
*@param:  length    [in]    读取的数据长度
*@param:  length    [out]   输出读取的数据
*@return: 实际读取的字节数
**/
int Lang_Read(HLANG hLang, int address, int length, char *buffer)
{
    FILE    *fp;
    int     nr_read;
    HHEAD   *pHHead;
#if LANGDEC_CHECK_ARG_EN > 0

    if (hLang == NULL)
    {
        __wrn("hlang is null!");
        return 0;
    }

#endif
    pHHead  = (HHEAD *)hLang;
    fp      = pHHead->fp;
    //寻找到指定内存
    fseek1(fp, address, SEEK_SET);
    //读取数据
    nr_read = fread1(buffer, sizeof(char), length, fp);
    return nr_read;
}

/**
*@brief:  查询数据地址
*@param:  hLang     [in]    文件句柄；
*@param:  LangID    [in]    语言ID
*@param:  StringID  [in]    SringID, 在Lang.bat生成的Lang.h文件中定义
*@return: 地址(相对文件起始偏移量)
**/
int Lang_GetStringAddress(HLANG hLang, short LangID, short StringID)
{
    unsigned int i;
    FILE    *fp;
    LANG_STRING m_String;
    HHEAD       *pHhead;
    HLANGLIST    m_HLangList;
    char *m_strbuffer;
    int  addr = 0;
#if LANGDEC_CHECK_ARG_EN > 0

    if (hLang == NULL)
    {
        __wrn("hlang is null!");
        return 0;
    }

#endif
    pHhead = (HHEAD *)hLang;
    fp     = pHhead->fp;

    //查询HHEAD数据结构，寻找LangID项
    for (i = 0; i < pHhead->LangNum; i++)
    {
        m_HLangList = pHhead->pHLangList[i];

        //找到LangID项
        //注意StringID从1开始，依次递增的存取在bin文件中，可以直接寻找到起始地址
        if (m_HLangList.LangID == LangID)
        {
            //移动文件指针到StringID项起始地址
            //////////////////////////////////////////////////////////////////////////
            // 查找索引
            int nIndex = StringID - 1;
            int n;

            if (pHhead->pIdTab)
            {
                nIndex = -1;

                for (n = 0; n < pHhead->StringNum; n++)
                {
                    if (pHhead->pIdTab[n] == StringID)
                    {
                        nIndex = n;
                        break;
                    }
                }

                //分配缓存，保存stringlist数据
            }

            if (nIndex == -1)
            {
                __wrn("strID error!");
                return 0;
            }

            fseek1(fp, m_HLangList.StringTabOff + (nIndex) * (pHhead->StringSize), SEEK_SET);
            m_strbuffer = (char *)malloc(pHhead->StringSize);
            fread1(m_strbuffer, sizeof(char), pHhead->StringSize, fp);
            //4字节对齐，
            memcpy(&m_String, m_strbuffer, sizeof(m_String));

            if (m_String.StringID == StringID)
            {
                addr = m_String.offset;
            }
            else
            {
                addr = 0;
            }

            free(m_strbuffer);
        }
    }

    return addr;
}

/**
*@brief:  查询数据长度
*@param:  hLang     [in]    文件句柄；
*@param:  LangID    [in]    语言ID
*@param:  StringID  [in]    SringID, 在Lang.bat生成的Lang.h文件中定义
*@return: 查询数据长度(byte)
**/
int Lang_GetStringSize(HLANG hLang, short LangID, short StringID)
{
    unsigned int i;
    FILE    *fp;
    LANG_STRING m_String;
    HHEAD       *pHhead;
    HLANGLIST   m_HLangList;
    char *m_strbuffer;
    int  size = 0;
#if LANGDEC_CHECK_ARG_EN > 0

    if (hLang == NULL)
    {
        __wrn("hlang is null!");
        return 0;
    }

#endif
    pHhead = (HHEAD *)hLang;
    fp     = pHhead->fp;

    for (i = 0; i < pHhead->LangNum; i++)
    {
        m_HLangList = pHhead->pHLangList[i];

        if (m_HLangList.LangID == LangID)
        {
            //////////////////////////////////////////////////////////////////////////
            // 查找索引
            int nIndex = StringID - 1;
            int n;

            if (pHhead->pIdTab)
            {
                nIndex = -1;

                for (n = 0; n < pHhead->StringNum; n++)
                {
                    if (pHhead->pIdTab[n] == StringID)
                    {
                        nIndex = n;
                        break;
                    }
                }

                //分配缓存，保存stringlist数据
            }

            if (nIndex == -1)
            {
                __wrn("strID error!");
                return 0;
            }

            fseek1(fp, m_HLangList.StringTabOff + (nIndex) * (pHhead->StringSize), SEEK_SET);
            m_strbuffer = (char *)malloc(pHhead->StringSize);
            fread1(m_strbuffer, sizeof(char), pHhead->StringSize, fp);
            memcpy(&m_String, m_strbuffer, sizeof(m_String));

            if (m_String.StringID == StringID)
            {
                //返回数据长度
                size = m_String.size;
            }
            else
            {
                size = 0;
            }

            free(m_strbuffer);
        }
    }

    return size;
}

/**
*@brief:  查询数据
*@param:  hLang     [in]    文件句柄；
*@param:  LangID    [in]    语言ID
*@param:  StringID  [in]    SringID, 在Lang.bat生成的Lang.h文件中定义
*@param:  buffer    [out]   数据输出缓冲区
*@param:  length    [in]    数据输出缓冲区长度
*@return: 数据实际长度(byte)
**/
int Lang_GetString(HLANG hLang, short LangID, short StringID, char *buffer, int length)
{
    unsigned int i;
    FILE    *fp;
    LANG_STRING m_String;
    HHEAD       *pHhead;
    HLANGLIST   m_HLangList;
    char *m_strbuffer;
#if LANGDEC_CHECK_ARG_EN > 0

    if (hLang == NULL)
    {
        __wrn("hlang is null!");
        return 0;
    }

#endif
    pHhead = (HHEAD *)hLang;
    fp     = pHhead->fp;

    for (i = 0; i < pHhead->LangNum; i++)
    {
        m_HLangList = pHhead->pHLangList[i];

        if (m_HLangList.LangID == LangID)
        {
            //////////////////////////////////////////////////////////////////////////
            // 查找索引
            int nIndex = StringID - 1;
            int n;

            if (pHhead->pIdTab)
            {
                nIndex = -1;

                for (n = 0; n < pHhead->StringNum; n++)
                {
                    if (pHhead->pIdTab[n] == StringID)
                    {
                        nIndex = n;
                        break;
                    }
                }

                //分配缓存，保存stringlist数据
            }

            if (nIndex == -1)
            {
                __wrn("strID error!");
                return 0;
            }

            fseek1(fp, m_HLangList.StringTabOff + (nIndex) * (pHhead->StringSize), SEEK_SET);
            //  fseek1(fp, m_HLangList.StringTabOff+(StringID-1)*(pHhead->StringSize), SEEK_SET);
            m_strbuffer = (char *)malloc(pHhead->StringSize);
            fread1(m_strbuffer, sizeof(char), pHhead->StringSize, fp);
            memcpy(&m_String, m_strbuffer, sizeof(m_String));

            if (m_String.StringID == StringID)
            {
                //移动文件指针到数据的起始地址
                fseek1(fp, m_String.offset, SEEK_SET);

                if (m_String.size <= (unsigned)length)
                {
                    //读数据到buffer
                    fread1(buffer, sizeof(char), m_String.size, fp);
                    free(m_strbuffer);
                    return m_String.size;
                }
                else
                {
                    fread1(buffer, sizeof(char), length, fp);
                    free(m_strbuffer);
                    return length;
                }
            }

            free(m_strbuffer);
            return 0;
        }
    }

    return 0;
}
/**
*@brief:  关闭句柄，释放内存
*@param:  hLang     [in]    文件句柄；
**/

int Lang_Close(HLANG hLang)
{
    FILE        *fp;
    HHEAD       *pHhead;
#if LANGDEC_CHECK_ARG_EN > 0
    if (hLang == NULL)
    {
        __wrn("hlang is null!");
        return 0;
    }
#endif
    pHhead = (HHEAD *)hLang;
    fp     = pHhead->fp;
    //关闭文件
    fclose1(fp);
    //释放内存；
    free(pHhead->pHLangList);
    free(pHhead);
    return OK;
}
