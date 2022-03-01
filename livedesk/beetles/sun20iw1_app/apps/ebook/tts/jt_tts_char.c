/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, RICHARD, China
*                                           All Rights Reserved
*
* File    : scan_main.c
* By      : Richard.x
* Version : V1.00
*********************************************************************************************************
*/
#include "jt_tts.h"



tts_char_parser_play  *_tts_play_struct = NULL;
void tts_char_parser_init(void)
{
    _tts_play_struct  = (tts_char_parser_play *)esMEMS_Balloc(sizeof(tts_char_parser_play));
    eLIBs_memset(_tts_play_struct, 0, sizeof(tts_char_parser_play));
}

void tts_char_parser_exit(void)
{
    if (_tts_play_struct !=  NULL)
    {
        esMEMS_Bfree(_tts_play_struct, sizeof(tts_char_parser_play));
    }

    _tts_play_struct = NULL;
}


/*获取当前文本的编码方式，并将之转化为TTS引擎识别的编码方式*/
int tts_get_txtCharEncodeType(ES_FILE *fd)
{
    int  encode;
    char     buff[3];
    eLIBs_fseek(fd, 0, SEEK_SET);
    eLIBs_fread(buff, 3, sizeof(char), fd);

    if ((buff[0] == 0xff) && (buff[1] == 0xfe))
    {
        encode = MK_CHAR_ENCODE_UNICODE;
    }
    else if ((buff[0] == 0xfe) && (buff[1] == 0xff))
    {
        encode = MK_CHAR_ENCODE_UNICODE_BIG;
    }
    else if ((buff[0] == 0xef) && (buff[1] == 0xbb) && (buff[2] == 0xbf))
    {
        encode = MK_CHAR_ENCODE_UTF8;
    }
    else
    {
        encode = MK_CHAR_ENCODE_GBK;
    }

    return encode;
}

int  tts_get_char_encode_type(int encode_type)
{
    int tts_encode_type;

    switch (encode_type)
    {
        case MK_CHAR_ENCODE_GBK:
            tts_encode_type = jtTTS_CODEPAGE_GBK;
            break;

        case MK_CHAR_ENCODE_UTF8:
            tts_encode_type = jtTTS_CODEPAGE_UTF8;
            break;

        case MK_CHAR_ENCODE_UNICODE:
            tts_encode_type = jtTTS_CODEPAGE_UTF16_LE;
            break;

        case MK_CHAR_ENCODE_UNICODE_BIG:
            tts_encode_type = jtTTS_CODEPAGE_UTF16_BE;
            break;

        default:
            tts_encode_type = jtTTS_CODEPAGE_GBK;
            break;
    }

    return tts_encode_type;
}








