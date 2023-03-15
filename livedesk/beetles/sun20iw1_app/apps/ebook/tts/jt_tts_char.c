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
