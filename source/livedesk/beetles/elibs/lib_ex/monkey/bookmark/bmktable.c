/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                                   (c) Copyright 2006-2007, ANDY, China
*                                            All Rights Reserved
*
* File    : bmktable.c
* By      : Andy.zhang
* Version : V1.00
*********************************************************************************************************
*/

#include "bmktable.h"

#define MAX_BMK_TABLE           100
#define BMK_TABLE_DATA_BASE     "z:\\txt\\bmk_table_date_base.bmt"
#define BMK_PATH                "z:\\txt\\bmk\\"


typedef struct _bmk_table_item
{
    char filename[4096];
    char bmkname[256];
} bmk_table_item;

typedef struct _bmk_table
{
    ES_FILE *fd;
    int max_item;
    int cur_index;
    bmk_table_item table[MAX_BMK_TABLE];
} bmk_table;


void file_2_bmk_name(char *filename, char *bmkname)
{
    char tmp[4];
    bmk_table   *table_list;
    int len;
    int i;
    ES_DIR *dir;
    /* 查询bmk保存目录是否存在 */
    dir = eLIBs_opendir("z:\\txt");

    if (dir == NULL)
    {
        eLIBs_mkdir("z:\\txt\\");
    }
    else
    {
        eLIBs_closedir(dir);
    }

    dir = eLIBs_opendir("z:\\txt\\bmk");

    if (dir == NULL)
    {
        eLIBs_mkdir("z:\\txt\\bmk");
    }
    else
    {
        eLIBs_closedir(dir);
    }

    table_list = (bmk_table *)esMEMS_Palloc(1 + sizeof(bmk_table) / 1024, 0);
    eLIBs_memset(table_list, 0, sizeof(bmk_table));
    table_list->max_item = MAX_BMK_TABLE;
    table_list->fd       = eLIBs_fopen(BMK_TABLE_DATA_BASE, "r");

    //如果映射数据库文件没有，就建立数据库文件
    if (table_list->fd == NULL)
    {
        table_list->fd = eLIBs_fopen(BMK_TABLE_DATA_BASE, "wb");
        table_list->cur_index = 0;
        eLIBs_strncpy(table_list->table[0].filename, filename, eLIBs_strlen(filename));
        //建立bmk文件名
        eLIBs_strcpy(table_list->table[0].bmkname, BMK_PATH);
        eLIBs_memset(tmp, 0, 4);
        eLIBs_int2str_dec(table_list->cur_index,   tmp);
        eLIBs_strcat(table_list->table[0].bmkname, tmp);
        eLIBs_strcat(table_list->table[0].bmkname, ".bmk");
        eLIBs_fseek(table_list->fd, 0, ELIBS_SEEK_END);
        eLIBs_fwrite(&(table_list->table[0]), 1, sizeof(bmk_table_item), table_list->fd);
        eLIBs_fclose(table_list->fd);
        //bmkname = table_list->table[0].bmkname;
        eLIBs_strcpy(bmkname, table_list->table[0].bmkname);
        esMEMS_Pfree(table_list, 1 + sizeof(bmk_table) / 1024);
        return;
    }

    eLIBs_fclose(table_list->fd);
    table_list->fd = eLIBs_fopen(BMK_TABLE_DATA_BASE, "rb+");
    eLIBs_fseek(table_list->fd, 0, ELIBS_SEEK_END);
    len = eLIBs_ftell(table_list->fd);
    table_list->cur_index = len / sizeof(bmk_table_item) - 1;
    eLIBs_fseek(table_list->fd, 0, ELIBS_SEEK_SET);
    eLIBs_fread(table_list->table, len, sizeof(char), table_list->fd);

    //查询数据库，看是否已经存在bmk文件
    for (i = 0; i <= table_list->cur_index; i++)
    {
        if (0 == eLIBs_strcmp((table_list->table)[i].filename, filename))    //找到数据库文件
        {
            eLIBs_strcpy(bmkname, table_list->table[i].bmkname);
            eLIBs_fclose(table_list->fd);
            esMEMS_Pfree(table_list, 1 + sizeof(bmk_table) / 1024);
            return;
        }
    }

    //建立bmk文件名
    if (i >= table_list->max_item)           //循环管理
    {
        i = i % (table_list->max_item);
        eLIBs_strcpy(bmkname, table_list->table[i].bmkname);
        eLIBs_fclose(table_list->fd);
        eLIBs_remove(table_list->table[i].bmkname); //删除文件
        esMEMS_Pfree(table_list, 1 + sizeof(bmk_table) / 1024);
        return;
    }

    eLIBs_strncpy(table_list->table[i].filename, filename, eLIBs_strlen(filename));
    eLIBs_strcpy(table_list->table[i].bmkname, BMK_PATH);
    eLIBs_memset(tmp, 0, 4);
    eLIBs_int2str_dec(i, tmp);
    eLIBs_strcat(table_list->table[i].bmkname, tmp);
    eLIBs_strcat(table_list->table[i].bmkname, ".bmk");
    eLIBs_fseek(table_list->fd, 0, ELIBS_SEEK_END);
    eLIBs_fwrite(&(table_list->table[i]), 1, sizeof(bmk_table_item), table_list->fd);
    eLIBs_fclose(table_list->fd);
    eLIBs_strcpy(bmkname, table_list->table[i].bmkname);
    esMEMS_Pfree(table_list, 1 + sizeof(bmk_table) / 1024);
    return;
}



















