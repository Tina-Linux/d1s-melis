/*
*********************************************************************************************************
*File   :   Decord.h
*version:   v1.0    2008-10-28
*By     :   Andy.zhang
*Brief  :   按照规定的格式读取data文件中的内容 (vc++ 6.0)
*********************************************************************************************************
*/

#ifndef __LANGDEC__H__
#define __LANGDEC__H__


#define HLANG   void *
#define OK      0
#define Error   -1

//打开语言数据文件
HLANG   Lang_Open(char *szAppFile, __u32 mode);

//读语言数据
int Lang_Read(HLANG hLang, int address, int length, char *buffer);

//取得语言字符串地址
int Lang_GetStringAddress(HLANG hLang, short LangID, short StringID);

//取得语言字符串长度
int Lang_GetStringSize(HLANG hLang, short LangID, short StringID);

//取得语言字符串数据
int Lang_GetString(HLANG hLang, short LangID, short StringID, char *buffer, int length);

//关闭语言数据
int Lang_Close(HLANG hLang);

#endif /* __LANGDEC__H__ */
