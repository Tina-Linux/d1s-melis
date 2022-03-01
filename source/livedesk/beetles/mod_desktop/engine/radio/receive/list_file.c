/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              LARK sample
*
*                                   (c) Copyright 2006-2007, TERRY, China
*                                            All Rights Reserved
*
* File    : list_file.c
* By      : terry
* Version : V1.00
* time    : 2009-12-01
**************************************************************************************************************
*/

#include "dsk_radio_i.h"
#include "format_output.h"

/*
**********************************************************************************************************************
*                                               myatoi
*
* Description: 将buf里的字符数据转变为整数
*
* Arguments  : buf：待转的字符传 len：字符串长度
*
* Returns    : 返回转化后的整数
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 myatoi(char *buf, __s32 len)
{
    __s32 num = 0;

    while ((len--) > 0)
    {
        num = num * 10 + (int)(*buf - '0');
        buf++;
    }

    return num;
}


/*
**********************************************************************************************************************
*                                               check_list_file
*
* Description: 判断buf里的数据是否属于FM列表文件的数据
*
* Arguments  : buf：数据 buf_len：buf长度
*
* Returns    : 正确返回0，错误返回-1
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 check_list_file(char *buf, __u32 buf_len)
{
    __s32 result = 0;
    __s32 i = 0;

    /*fm list file begine with "[FM]"*/
    if (buf[i] != 0x5B || buf[i + 1] != 0x46 || buf[i + 2] != 0x4D || buf[i + 3] != 0x5D)
    {
        return -1;
    }

    while (i < buf_len)
    {
        /*FM list file end with "[FMEND]"*/
        if (buf[i] == 0x5B && buf[i + 1] == 0x46 && buf[i + 2] == 0x4D && buf[i + 3] == 0x45 && buf[i + 4] == 0x4E && buf[i + 5] == 0x44  && buf[i + 6] == 0x5D)
        {
            result = i;
            break;
        }

        i++;
    }

    return result;
}

/*
**********************************************************************************************************************
*                                               get_channel_list_from_file
*
* Description: 从列表文件里获取频道列表
*
* Arguments  : file_name：文件名 channel_list：频道列表数据结构指针
*
* Returns    : 正确返回0，错误返回-1
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 get_channel_list_from_file(char *file_name, dsk_radio_ch_list_t *channel_list)
{
    __s32 result;
    char *buf_binary = NULL;
    __s32 end_pos;
    __s32 channel_num = 0;
    __s32 i = 0;
    __s32 j = 0;
    char *cur_p;
    /*cur channel number*/
    __s32 c_no;
    ES_FILE *fp = NULL;
    buf_binary = (char *)esMEMS_Malloc(0, CHANNEL_SIZE * MAX_CHANNEL_NUM + 14);
    return_val_if_fail(buf_binary != NULL, -1);
    eLIBs_memset(buf_binary, 0, CHANNEL_SIZE * MAX_CHANNEL_NUM + 14);
    fp = eLIBs_fopen(file_name, "r+");

    if (fp == NULL)
    {
        __wrn("Warning:Open fm list file error!\n");
        esMEMS_Mfree(0, buf_binary);
        return -1;
    }

    result = eLIBs_fread(buf_binary, 1, CHANNEL_SIZE * MAX_CHANNEL_NUM + 14, fp);

    if (result == 0)
    {
        __wrn("Warning:Read data from fm list file error!\n");
        eLIBs_fclose(fp);
        esMEMS_Mfree(0, buf_binary);
        return -1;
    }

    eLIBs_fclose(fp);
    /*check the file whether a list file,if yes,return the end positon*/
    end_pos = check_list_file(buf_binary, CHANNEL_SIZE * MAX_CHANNEL_NUM + 14);

    if (end_pos == -1)
    {
        __wrn("Warning:It's not a fm list file!\n");
        esMEMS_Mfree(0, buf_binary);
        return -1;
    }

    while (channel_num < MAX_CHANNEL_NUM)
    {
        /*channel begine with "CH"*/
        while (buf_binary[i] != 0x43 || buf_binary[i + 1] != 0x48)
        {
            i++;

            if (i >= end_pos)
            {
                break;
            }
        }

        if (i >= end_pos)
        {
            break;
        }

        i = i + 2;
        j = i;

        while (buf_binary[j] != '\n')
        {
            j++;

            if (j >= end_pos)
            {
                break;
            }
        }

        cur_p = &(buf_binary[i]);
        c_no = myatoi(cur_p, 2);
        i = i + 3;
        cur_p = &(buf_binary[i]);
        channel_list->freq_list[c_no - 1].freq = myatoi(cur_p, 6);
        i = i + 6 + 1;
        cur_p = &(buf_binary[i]);

        if ((j - i) > 0)
        {
            if (j - i > CHANNEL_NAME_LEN) //fix klocwork ABR error
            {
                eLIBs_memcpy(channel_list->freq_list[c_no - 1].name, cur_p, CHANNEL_NAME_LEN);
            }
            else
            {
                eLIBs_memcpy(channel_list->freq_list[c_no - 1].name, cur_p, j - i);
            }
        }

        i = j;

        if (channel_list->freq_list[channel_num].freq != 0)
        {
            channel_list->valid_num++;
        }

        channel_num ++;
    }

    esMEMS_Mfree(0, buf_binary);
    return 0;
}
/*
**********************************************************************************************************************
*                                               write_channel_list_to_file
*
* Description: 把目前系统运行的频道列表写回文件
*
* Arguments  : file_name：文件名 channel_list：频道列表数据结构指针
*
* Returns    : 正确返回0，错误返回-1
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 write_channel_list_to_file(char *file_name, dsk_radio_ch_list_t *channel_list)
{
    __s32 result;
    char *buf_binary = NULL;
    char channel_buf[CHANNEL_SIZE];
    ES_FILE *fp = NULL;
    __s32 i = 0;
    __msg("file_name = %s\n", file_name);
    buf_binary = (char *)esMEMS_Malloc(0, CHANNEL_SIZE * MAX_CHANNEL_NUM + 14);
    return_val_if_fail(buf_binary != NULL, -1);
    eLIBs_memset(buf_binary, 0, CHANNEL_SIZE * MAX_CHANNEL_NUM + 14);
    fp = eLIBs_fopen(file_name, "w+");

    if (fp == NULL)
    {
        __wrn("Warning:Open fm list file error!\n");
        esMEMS_Mfree(0, buf_binary);
        return -1;
    }

    my_output_format(buf_binary, "[FM]\n");

    while (i < MAX_CHANNEL_NUM)
    {
        my_output_format(channel_buf, "CH%.2d=%.6d,%s\n", i + 1, channel_list->freq_list[i].freq, channel_list->freq_list[i].name);
        i++;
        eLIBs_strcat(buf_binary, channel_buf);
    }

    eLIBs_strcat(buf_binary, "[FMEND]");
    __msg("\n");
    //{
    //__u32 i;
    //for(i=0;i<CHANNEL_SIZE*MAX_CHANNEL_NUM+14;i++)
    //{
    //  __msg("  %x",buf_binary[i]);
    //  if(i == 24)
    //      __msg("\n");
    //}
    //}
    //__msg("\n");
    result = eLIBs_fwrite(buf_binary, 1, CHANNEL_SIZE * MAX_CHANNEL_NUM + 14, fp);

    if (result < CHANNEL_SIZE * MAX_CHANNEL_NUM + 14)
    {
        __wrn("Warning:Write fm list file error!\n");
        eLIBs_fclose(fp);
        esMEMS_Mfree(0, buf_binary);
        return -1;
    }

    __msg("size = %d result = %d\n", CHANNEL_SIZE * MAX_CHANNEL_NUM + 14, result);
    result = eLIBs_fclose(fp);
    __msg("result = %d\n", result);
    esMEMS_Mfree(0, buf_binary);
    __msg("\n");
    return 0;
}

