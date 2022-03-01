#include "jt_tts.h"
#include <log.h>
#include "dfs_posix.h"
#define   MAX_CHARS_NO   (50)   /*目前audio dev 驱动的缓冲区最大允许50个字左右的音频数值大小*/

static __u8 g_new_buf[MAX_CHARS_NO * 3 + 3]; //添加结尾空格给TTS

/****************************************************************************
对文件数据进行分析，分析出来的的一句话进行音频的整合
每当遇到。?!的时候或者大于50 个文字,单字符情况下(英文情况下
大于50个字符就以最后一个空格作为一句话的结束)
的时候认为是一句话的结束
*****************************************************************************/

/*
**********************************************************************************************************************
*                                               __tts_analysis_utf8_oneline
*
* Description: 分析输入数据的内容，返回第一个句内容的起始地址和长度（UTF8编码格式）
*            认为一句话的规则为:
                            1、双字节遇到。!   ? 时候，否则= 50 个字符认为是一句
                            2、连续50单字节找最后一个空格符作为一句，或者遇到.!?也为一句，
                                50个也作为一句
                            3、遇到换行符也认为是一句
* Arguments  :
*           @data: 输入的内容地址
*           @size:输入的内容大小
*           @outputdata: 分析出来的第一句话的起始地址
*           @output_size:分析出来的第一句话的长度
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __tts_analysis_utf8_oneline(char *data, __u32  size, char **outputdata, __u32 *output_size)
{
    __u16                   tmp;
    char                   *p_data;
    __u32 index_size = 0;       /*记录是否是达到内容结尾*/
    __s32 one_byt_no = 0;       /*连续单字节累加数*/
    __s32 char_no = 0;          /*解析的字符数目*/
    char                   *p_first_one_byt;        /*指向第一个连续单字节流数据位置*/
    *outputdata =  data;
    *output_size = 0;
    p_data = data;

    while (index_size < size)
    {
        if ((*p_data == 0x0a) || (*p_data == 0x0d))
        {
            //换行符检查
            if (*(p_data + 1) == 0x0a)
            {
                p_data = p_data + 2;
                *output_size += 2;
                index_size += 2;
                char_no++;
            }
            else
            {
                p_data = p_data + 1;
                *output_size += 1;
                index_size += 1;
                char_no++;
            }

            __msg("--------------------------分析完毕!\n");
            break;
        }
        else if ((((*p_data) & 0xff) == 0xff) && ((*(p_data + 1) & 0xff) == 0xfe))
        {
            /*已经到内容的结尾了*/
            __msg("--------------------------分析完毕!\n");
            break;
        }
        else if ((*p_data & 0xf0) == 0xe0)
        {
            // 三字节流数据
            tmp = ((*p_data & 0x0f) << 12) | ((*(p_data + 1) & 0x3f) << 6) | (*(p_data + 2) & 0x3f);
            char_no++;
            *output_size += 3;
            index_size += 3;
            p_data = p_data + 3;

            if (0/*tmp == (((0xef & 0x0f) << 12) | ((0xbc & 0x3f) << 6) | (0x81 & 0x3f))*/)
            {
                /*遇到!号*/
                __msg("--------------------------分析完毕!\n");
                break;
            }

            if (0/*tmp == (((0xe3 & 0x0f) << 12) | ((0x80 & 0x3f) << 6) | (0x82 & 0x3f))*/)
            {
                /*句号*/
                __msg("--------------------------分析完毕!\n");
                break;
            }

            if (0/*tmp == (((0xef & 0x0f) << 12) | ((0xbc & 0x3f) << 6) | (0x8c & 0x3f))*/)
            {
                /*道号*/
                __msg("--------------------------分析完毕!\n");
                break;
            }

            if (0/*tmp == (((0xef & 0x0f) << 12) | ((0xbc & 0x3f) << 6) | (0x9f & 0x3f))*/)
            {
                /*问号*/
                __msg("--------------------------分析完毕!\n");
                break;
            }

            if (char_no >= MAX_CHARS_NO)
            {
                /*超过50 个文字*/
                __msg("--------------------------分析完毕!\n");
                break;
            }

            one_byt_no = 0;
        }
        else if ((*p_data & 0xe0) == 0xc0)
        {
            // 双字节流数据
            tmp = ((*p_data & 0x3f) << 6) | (*(p_data + 1) & 0x3f);
            *output_size += 2;
            char_no++;
            one_byt_no = 0;
            p_data = p_data + 2;
            index_size += 2;

            if (char_no >= MAX_CHARS_NO)
            {
                /*超过50 个文字*/
                __msg("--------------------------分析完毕!\n");
                break;
            }
        }
        else
        {
            // 单字节流数据
            tmp = *p_data & 0xff;

            if (one_byt_no == 0)        //开始记录单个字符的位置
            {
                p_first_one_byt = p_data;
            }

            p_data = p_data + 1;
            *output_size += 1 ;

            if (tmp == 9)
            {
                tmp = 0x20;
            }

            if (tmp == 0x21)
            {
                /*叹号*/
                __msg("--------------------------分析完毕!\n");
                break;
            }

            if (tmp == 0x3f)
            {
                /*问号*/
                __msg("--------------------------分析完毕!\n");
                break;
            }

            if (tmp == 0x2c)
            {
                /*逗号*/
                __msg("--------------------------分析完毕!\n");
                break;
            }

            one_byt_no++;
            char_no++;
            index_size += 1;

            if (char_no >= MAX_CHARS_NO) /*超过50 个文字*/
            {
                if (one_byt_no >= MAX_CHARS_NO)
                {
                    /*已经是连续50 个是单字节流数据了，可能是英文文件查找最后一个空格的地方*/
                    int i;
                    __u32  old_size = *output_size ;

                    for (i = MAX_CHARS_NO; i > 0; i--)
                    {
                        if ((*p_data & 0xff) == 0x20 || (*p_data & 0xff) == 9)
                        {
                            __msg("--------------------------分析完毕!\n");
                            break;
                        }

                        p_data -- ;
                        *output_size -= 1 ;
                    }

                    if (i == 0)         /*向前查询了MAX_CHARS_NO 个字符发现没有空格符*/
                    {
                        *output_size = old_size;
                        __msg("--------------------------分析完毕!\n");
                        break;
                    }
                }
                else
                {
                    /*退回到最后一个不是单字节流数后面那个地方*/
                    *output_size = *output_size - (p_data - p_first_one_byt);
                }

                __msg("--------------------------分析完毕!\n");
                break;
            }
        }
    }
}



static void __tts_analysis_utf8(char *data, __u32  size, __u32 *output_size)
{
    char *outputdata;
    __msg("一页大小size = %d\n", size);
    __tts_analysis_utf8_oneline(_tts_play_struct->tts_text_line_add, _tts_play_struct->tts_page_leave_char, &outputdata, output_size);
    __msg("-----data = %x   -outputdata = %x, output_size = %d,  tota_ananlysis = %d\n", data, outputdata, *output_size);

    if (0)
    {
        __s32 i;
        __inf("utf8\n");

        for (i = 0 ; i < *output_size ; i++)
        {
            __inf("0x%.2x ", outputdata[i]);
        }

        __inf("\n");
    }

    jtTTS_SynthesizeText(_tts_play_struct->jt_hTTS, (const void *)outputdata, (*output_size) * sizeof(char));

    while (1)
    {
        if (cur_audio_speark_over() == EPDK_TRUE)
        {
            ioctl(f_audiodev, AUDIO_DEV_CMD_FLUSH_BUF, 0);
            break;
        }

        if (*output_size <= 5)          /*防止一页数据完后死循环*/
        {
            esKRNL_TimeDly(20);     /*延迟20 是让audiodev 读完5大小的音频数据*/
            break;
        }

        esKRNL_TimeDly(5);
    }
}


/*
**********************************************************************************************************************
*                                               __tts_analysis_utf8_oneline
*
* Description: 分析输入数据的内容，返回第一个句内容的起始地址和长度（utf16_be编码格式）
*            认为一句话的规则为:
                            1、双字节遇到。!   ? 时候，否则= 50 个字符认为是一句
                            2、连续50单字节找最后一个空格符作为一句，或者遇到.!?也为一句，
                                50个也作为一句
                            3、遇到换行符也认为是一句
* Arguments  :
*           @data: 输入的内容地址
*           @size:输入的内容大小
*           @outputdata: 分析出来的第一句话的起始地址
*           @output_size:分析出来的第一句话的长度
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/

static void __tts_analysis_utf16_be_oneline(char *data, __u32  size, char **outputdata, __u32 *output_size)
{
    __u16                   tmp;
    __u8                   *p_data;
    __u32 index_size = 0;       /*记录是否是达到内容结尾*/
    __s32 char_no = 0;          /*解析的字符数目*/
    (*outputdata) = data;
    p_data = data;
    *output_size = 0;

    while (index_size < size)
    {
        tmp = ((*p_data & 0xff) << 8) + (*(p_data + 1) & 0xff);     /*可能要转字符*/
        char_no ++;

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            if ((*(p_data + 3) & 0xff) == 0x0a)
            {
                p_data = p_data + 4;
                *output_size += 4;
                index_size += 4;
            }
            else
            {
                p_data = p_data + 2;
                *output_size += 2;
                index_size += 2;
            }

            break;
        }
        else if (tmp == 0xfffe)
        {
            break;
        }
        else
        {
            p_data += 2;
            *output_size += 2;

            if (tmp == ((0x00 & 0xff) << 8) + (0x3f & 0xff)/* || tmp == ((0xff & 0xff)<< 8)+ (0x1f & 0xff)*/)
            {
                /*问号*/
                break;
            }

            if (tmp == ((0x00 & 0xff) << 8) + (0x21 & 0xff) /*|| tmp == ((0xff & 0xff)<< 8)+ (0x01 & 0xff)*/)
            {
                /*叹号*/
                break;
            }

            if (tmp == ((0x00 & 0xff) << 8) + (0x2c & 0xff) /*|| tmp == ((0xff & 0xff)<< 8)+ (0x0c & 0xff)*/)
            {
                /*逗号*/
                break;
            }

            if (0/* tmp == ((0x30 & 0xff)<< 8)+ (0x02 & 0xff)*/)
            {
                /*句号*/
                break;
            }

            if (char_no >= MAX_CHARS_NO)    /*字符大于MAX_CHARS_NO,有肯能是英文，向前找倒数第一个个空格的位置*/
            {
                __s32 i ;
                __u32 old_size = *output_size;

                for (i = MAX_CHARS_NO; i > 0; i --)
                {
                    tmp = ((*p_data & 0xff) << 8) + (*(p_data + 1) & 0xff);

                    if ((tmp == ((0x00 & 0xff) << 8) + 0x20) & 0xff)
                    {
                        break;
                    }

                    *output_size -= 2;
                }

                if (i == 0)
                {
                    *output_size = old_size;
                }

                break;
            }
        }
    }
}



static void __tts_analysis_utf16_be(char *data, __u32  size, __u32 *output_size)
{
    char *outputdata;
    __u32 tota_ananlysis = 0;
    __msg("一页大小size = %d\n", size);
    __tts_analysis_utf16_be_oneline(_tts_play_struct->tts_text_line_add, _tts_play_struct->tts_page_leave_char, &outputdata, output_size);

    if (0)
    {
        __s32 i;
        __inf("utf16_be\n");

        for (i = 0 ; i < *output_size ; i++)
        {
            __inf("0x%.2x ", outputdata[i]);
        }

        __inf("\n");
    }

    jtTTS_SynthesizeText(_tts_play_struct->jt_hTTS, (const void *)outputdata, (*output_size) * sizeof(char));
    __msg("tota_ananlysis  = %d\n", tota_ananlysis);

    while (1)
    {
        if (cur_audio_speark_over() == EPDK_TRUE)
        {
            ioctl(f_audiodev, AUDIO_DEV_CMD_FLUSH_BUF, 0);
            break;
        }

        esKRNL_TimeDly(5);
    }
}




/*
**********************************************************************************************************************
*                                               __tts_analysis_utf16_le_oneline
*
* Description: 分析输入数据的内容，返回第一个句内容的起始地址和长度（utf16_be编码格式）
*            认为一句话的规则为:
                            1、双字节遇到。!   ? 时候，否则= 50 个字符认为是一句
                            2、连续50单字节找最后一个空格符作为一句，或者遇到.!?也为一句，
                                50个也作为一句
                            3、遇到换行符也认为是一句
* Arguments  :
*           @data: 输入的内容地址
*           @size:输入的内容大小
*           @outputdata: 分析出来的第一句话的起始地址
*           @output_size:分析出来的第一句话的长度
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/

static void __tts_analysis_utf16_le_oneline(char *data, __u32  size, char **outputdata, __u32 *output_size)
{
    __u16                   tmp;
    __u8                   *p_data;
    __u32 index_size = 0;       /*记录是否是达到内容结尾*/
    __s32 char_no = 0;          /*解析的字符数目*/
    *outputdata = data;
    *output_size = 0;
    p_data = data;

    while (index_size < size)
    {
        tmp = (*p_data & 0xff) + ((*(p_data + 1) & 0xff) << 8);     /*可能要转字符*/
        char_no ++;

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            if ((*(p_data + 2) & 0xff) == 0x0a)
            {
                p_data = p_data + 4;
                *output_size += 4;
                index_size += 4;
            }
            else
            {
                p_data = p_data + 2;
                *output_size += 2;
                index_size += 2;
            }

            break;
        }
        else if (tmp == 0xfeff)
        {
            break;
        }
        else
        {
            p_data += 2;
            *output_size += 2;

            if (tmp == ((0x3f & 0xff) << 8) + (0x00 & 0xff)/* || tmp == ((0x1f & 0xff)<< 8)+ (0xff & 0xff)*/)
            {
                /*问号*/
                break;
            }

            if (tmp == ((0x21 & 0xff) << 8) + (0x00 & 0xff) /*|| tmp == ((0x01 & 0xff)<< 8)+ (0xff & 0xff)*/)
            {
                /*叹号*/
                break;
            }

            if (tmp == ((0x2c & 0xff) << 8) + (0x00 & 0xff)/* || tmp == ((0x0c & 0xff)<< 8)+ (0xff & 0xff)*/)
            {
                /*逗号*/
                break;
            }

            if (0/* tmp == ((0x02 & 0xff)<< 8)+ (0x30 & 0xff)*/)
            {
                /*句号*/
                break;
            }

            if (char_no >= MAX_CHARS_NO)    /*字符大于MAX_CHARS_NO,有肯能是英文，向前找倒数第一个个空格的位置*/
            {
                __s32 i ;
                __u32 old_size = *output_size;

                for (i = MAX_CHARS_NO; i > 0; i --)
                {
                    tmp = ((*p_data & 0xff) << 8) + (*(p_data + 1) & 0xff);

                    if ((tmp == ((0x20 & 0xff) << 8) + 0x00) & 0xff)
                    {
                        break;
                    }

                    *output_size -= 2;
                }

                if (i == 0)
                {
                    *output_size = old_size;
                }

                break;
            }
        }
    }
}

static void __tts_analysis_utf16_le(char *data, __u32  size, __u32 *output_size)
{
    char *outputdata;
    __u32 tota_ananlysis = 0;
    __msg("一页大小size = %d\n", size);
    __tts_analysis_utf16_le_oneline(_tts_play_struct->tts_text_line_add, _tts_play_struct->tts_page_leave_char, &outputdata, output_size);

    if (0)
    {
        __s32 i;
        __inf("utf16_le\n");

        for (i = 0 ; i < *output_size ; i++)
        {
            __inf("0x%.2x ", outputdata[i]);
        }

        __inf("\n");
    }

    jtTTS_SynthesizeText(_tts_play_struct->jt_hTTS, (const void *)outputdata, (*output_size) * sizeof(char));
    __msg("tota_ananlysis  = %d\n", tota_ananlysis);

    while (1)
    {
        if (cur_audio_speark_over() == EPDK_TRUE)
        {
            ioctl(f_audiodev, AUDIO_DEV_CMD_FLUSH_BUF, 0);
            break;
        }

        esKRNL_TimeDly(5);
    }
}





static __u32 __local_language_count(__epdk_charset_enm_e enm)
{
    switch (enm)
    {
        case EPDK_CHARSET_ENM_GB2312:
        case EPDK_CHARSET_ENM_BIG5:
        case EPDK_CHARSET_ENM_GBK:
        case EPDK_CHARSET_ENM_SJIS:
        case EPDK_CHARSET_ENM_EUC_JP:
        case EPDK_CHARSET_ENM_EUC_KR:
        case EPDK_CHARSET_ENM_CP874:
            return 2;

        default:
            return 1;
    }
}


/*
**********************************************************************************************************************
*                                               __tts_analysis_utf16_le_oneline
*
* Description: 分析输入数据的内容，返回第一个句内容的起始地址和长度（utf16_be编码格式）
*            认为一句话的规则为:
                            1、双字节遇到。!   ? 时候，否则= 50 个字符认为是一句
                            2、连续50单字节找最后一个空格符作为一句，或者遇到.!?也为一句，
                                50个也作为一句
                            3、遇到换行符也认为是一句
* Arguments  :
*           @data: 输入的内容地址
*           @size:输入的内容大小
*           @outputdata: 分析出来的第一句话的起始地址
*           @output_size:分析出来的第一句话的长度
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/

static void __tts_analysis_gbk_oneline(char   *data, __u32 size, char **outputdata, __u32 *output_size)
{
    __u16                   tmp;
    __u8                   *p_data;
    __u32 index_size = 0;       /*记录是否是达到内容结尾*/
    __s32 char_no = 0;          /*解析的字符数目*/
    __s32 one_byt_no = 0;       /*连续单字节累加数*/
    char                   *p_first_one_byt;        /*指向第一个连续单字节流数据位置*/
    p_data = data;
    *outputdata = data;
    *output_size = 0;

    while (index_size < size)
    {
        tmp = *p_data & 0xff;

        if ((tmp == 0x0d) || (tmp == 0x0a))
        {
            if ((*(p_data + 1) & 0xff) == 0x0a)
            {
                p_data = p_data + 2;
                *output_size += 2;
            }
            else
            {
                p_data = p_data + 1;
                *output_size += 1;
                char_no++;
                continue;
            }

            char_no ++;
            __msg("----分析结束\n") ;
            break;
        }
        else if (tmp == 0xff)
        {
            if ((*(p_data + 1) & 0xff) == 0xfe)
            {
                __msg("----分析结束\n") ;
                break;
            }
            else
            {
                *output_size += 1;
                p_data += 1;
                char_no += 1;
            }
        }
        else if (tmp > 0x7f) //是中文字符的时候
        {
#if 0

            if (local_language_count(get_isn_charset()) == 2)
            {
                __u8 tmp_tem[8];
                tmp_tem[1] = *(p_data + 1) & 0xff;
                //tmp = (tmp << 8) + *(p_data + 1);
                tmp_tem[0] = *p_data & 0xff;//tmp;
                esCHS_Char2Uni(get_isn_charset(), tmp_tem, 8, &unicode);
                tmp  = unicode;
                tmp_w = p_config->col_width;//GUI_GetCharDistX(unicode);//p_config->col_width;
                p_data = p_data + 2;
            }
            else if (local_language_count(get_isn_charset()) == 1)
            {
                __u8 tmp_tem[8];
                tmp_tem[0] = tmp;
                esCHS_Char2Uni(get_isn_charset(), tmp_tem, 4, &unicode);
                tmp  = unicode;
                tmp_w = GUI_GetCharDistX(unicode);// p_config->col_width;
                p_data = p_data + 1;
            }

#else
            __u16  unicode;
            one_byt_no = 0;

            if (__local_language_count(dsk_get_isn_charset()) == 2)
            {
                __u8 tmp_tem[8];
                tmp_tem[1] = *(p_data + 1) & 0xff;
                //tmp = (tmp << 8) + *(p_data + 1);
                tmp_tem[0] = *p_data & 0xff;//tmp;
                esCHS_Char2Uni(dsk_get_isn_charset(), tmp_tem, 8, &unicode);
                tmp  = unicode;
                *output_size += 2;
                p_data += 2;
#if 0

                if (*p_data_temp == 0xa3 && *p_data_temp == 0xa1)
                {
                    /*叹号*/
                    break;
                }

                if (*p_data_temp == 0xa3 && *p_data_temp == 0xb1)
                {
                    /*问号*/
                    break;
                }

                if (*p_data_temp == 0xa3 && *p_data_temp == 0xac)
                {
                    /*逗号*/
                    break;
                }

                if (*p_data_temp == 0xa1 && *p_data_temp == 0xb3)
                {
                    /*逗号*/
                    break;
                }

#endif
            }
            else if (__local_language_count(dsk_get_isn_charset()) == 1)
            {
                __u8 tmp_tem[8];
                tmp_tem[0] = tmp;
                esCHS_Char2Uni(dsk_get_isn_charset(), tmp_tem, 4, &unicode);
                tmp  = unicode;
                *output_size += 1;
                p_data += 1;
#if 0

                if (*p_data_temp == 0xa3 && *p_data_temp == 0xa1)
                {
                    /*叹号*/
                    break;
                }

                if (*p_data_temp == 0xa3 && *p_data_temp == 0xb1)
                {
                    /*问号*/
                    break;
                }

                if (*p_data_temp == 0xa3 && *p_data_temp == 0xac)
                {
                    /*逗号*/
                    break;
                }

                if (*p_data_temp == 0xa1 && *p_data_temp == 0xb3)
                {
                    /*逗号*/
                    break;
                }

#endif
            }

            char_no++;

            /***********以大端机器来进行比较****************/
            if (unicode == ((0x00 & 0xff) << 8) + (0x3f & 0xff)/* || unicode == ((0xff & 0xff)<< 8)+ (0x1f & 0xff)*/)
            {
                /*问号*/
                break;
            }

            if (unicode == ((0x00 & 0xff) << 8) + (0x21 & 0xff)/* || unicode == ((0xff & 0xff)<< 8)+ (0x01 & 0xff)*/)
            {
                /*叹号*/
                break;
            }

            if (unicode == ((0x00 & 0xff) << 8) + (0x2c & 0xff) /*|| unicode == ((0xff & 0xff)<< 8)+ (0x0c & 0xff)*/)
            {
                /*逗号*/
                break;
            }

            if (0 /*unicode == ((0x30 & 0xff)<< 8)+ (0x02 & 0xff)*/)
            {
                /*句号*/
                break;
            }

            if (char_no >= MAX_CHARS_NO)
            {
                break;
            }

#endif
        }
        else
        {
            *output_size += 1;
            p_data += 1;
            char_no += 1;

            if (one_byt_no == 0)        //开始记录单个字符的位置
            {
                p_first_one_byt = p_data;
            }

            one_byt_no += 1;

            if (tmp == 0x21 || tmp == 0x3f || tmp == 0x2c)             // ! ? , 分句符号
            {
                __msg("----分析结束\n") ;
                break;
            }

            if (char_no >= MAX_CHARS_NO)    /*有可能是英文的情况下*/
            {
                if (one_byt_no >= MAX_CHARS_NO)
                {
                    int i;
                    __u32  old_size = *output_size ;

                    for (i = MAX_CHARS_NO; i > 0; i--)
                    {
                        if (*p_data == 0x20)
                        {
                            __msg("----分析结束\n") ;
                            break;
                        }

                        p_data -- ;
                        *output_size -= 1 ;
                    }

                    if (i == 0)
                    {
                        *output_size = old_size ;
                        __msg("----分析结束\n") ;
                    }

                    break;
                }
                else
                {
                    /*退回到最后一个不是单字节流数后面那个地方*/
                    int i ;

                    for (i = MAX_CHARS_NO; i > 0; i--)
                    {
                        if (*p_data == 0x20         //空格
                            || *p_data == 0x2E    //.
                            || *p_data == 0x2c)    //,
                        {
                            p_first_one_byt = p_data;
                            break;
                        }

                        p_data--;
                    }

                    __msg("----------p_data = %x, p_first_one_byt = %x\n", p_data, p_first_one_byt);
                    *output_size = *output_size - ((unsigned long)p_data - (unsigned long)p_first_one_byt);
                    __msg("----分析结束\n") ;
                    break;
                }
            }
        }
    }
}

static void __tts_analysis_gbk(char *data, __u32  size, __u32 *output_size)
{
    char *outputdata;
    __msg("一页大小size = %d, _tts_play_struct->tts_text_line_add= %s\n", size, _tts_play_struct->tts_text_line_add);
    __tts_analysis_gbk_oneline(_tts_play_struct->tts_text_line_add, _tts_play_struct->tts_page_leave_char, &outputdata, output_size);

    if (0)
    {
        __s32 i;
        __inf("_gbk\n");

        for (i = 0 ; i < *output_size ; i++)
        {
            __inf("0x%.2x ", outputdata[i]);
        }

        __inf("\n");
    }

    jtTTS_SynthesizeText(_tts_play_struct->jt_hTTS, (const void *)outputdata, (*output_size) * sizeof(char));
    __msg("------一句话的大小output_size  = %d\n", *output_size);

    while (1)
    {
        if (cur_audio_speark_over() == EPDK_TRUE)
        {
            ioctl(f_audiodev, AUDIO_DEV_CMD_FLUSH_BUF, 0);
            break;
        }

        esKRNL_TimeDly(5);
    }
}


/*
************************************************************************************************************************
*                                       TTS_ANALYSYS_Page
*
*Description: 写一页数据到指定的软图层上
*
*Arguments  : hdle: 操作句柄
*             lyr: 图层句柄
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   TTS_ANALYSYS_Page(__epdk_charset_enm_e  charset,  void *data, __u32  size, __u32 *output_size)
{
#if defined Have_eJTTS_lib
    switch (charset)
    {
        case MK_CHAR_ENCODE_UTF8 :
        {
            __tts_analysis_utf8(data, size, output_size);
        }
        break;

        case MK_CHAR_ENCODE_UNICODE_BIG:
        {
            __tts_analysis_utf16_be(data, size, output_size);
        }
        break;

        case MK_CHAR_ENCODE_UNICODE:
        {
            __tts_analysis_utf16_le(data, size, output_size);
        }
        break;

        case MK_CHAR_ENCODE_GBK:
        {
            __tts_analysis_gbk(data, size, output_size);
        }
        break;
#if 0

        case MK_CHAR_ENCODE_GBK:
        {
            __show_page_big5(data, size);
        }
        break;
#endif

        default:
            __tts_analysis_gbk(data, size, output_size);
            break;
    }
#endif
    return EPDK_OK;
}




