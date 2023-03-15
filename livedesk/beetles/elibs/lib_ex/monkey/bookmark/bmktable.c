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
