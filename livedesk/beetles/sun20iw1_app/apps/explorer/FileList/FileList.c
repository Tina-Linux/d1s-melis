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
#include <log.h>
#include "beetles_app.h"

#include "FileList.h"

#define     FileListMalloc  eLIBs_malloc

/*#define KARAOKE_LYRIC_USE_MALLOC 1


#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/
//递归函数中调用，放在函数中的话，会引起堆栈溢出
//char            fullpath[RAT_MAX_FULL_PATH_LEN];


/*
************************************************************************************************************************
*Function   :
*
*Description    :
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
//将小写字母转换成大小字母
static __s32  LowerCase_2_UpperCase(char *str)
{
    char *p_str = NULL ;

    if (!str)
    {
        __msg("string is null , return \n");
        return EPDK_FAIL ;
    }

    p_str = str ;

    while (*p_str != '\0')
    {
        if ((*p_str <= 'z') && (*p_str >= 'a'))
        {
            *p_str =  *p_str - 'a' + 'A' ;
        }

        p_str++ ;
    }

    return EPDK_OK ;
}

// 文件列表排序函数，以文件名来进行排序
static file_item_t  *file_list_sort(file_item_t  *p_file_item_list, __s32 file_item_total_num)
{
    __u32   i,  j ;
    file_item_t *p_cur_item = NULL, *p_next_item = NULL, *p_head_item = NULL ;
    __s32   ret =  0 ;
    __bool  is_swap = EPDK_FALSE ;

    if (!p_file_item_list)
    {
        __msg(" file list is null , return \n ") ;
        return NULL;
    }

    if (file_item_total_num <= 0)
    {
        __msg(" file item total num is invalue return \n ") ;
        return NULL ;
    }

    for (i = 0 ; i < file_item_total_num - 1 ; i++)
    {
        p_head_item = p_file_item_list ;
        p_cur_item = p_head_item ;
        is_swap = EPDK_FALSE ;

        for (j = 0 ; j < file_item_total_num - i - 1 ; j++)
        {
            char temp_str1[RAT_MAX_FULL_PATH_LEN] = {0}, temp_str2[RAT_MAX_FULL_PATH_LEN] = {0} ;
            p_next_item = p_cur_item->next ;
            eLIBs_strcpy(temp_str1, p_cur_item->name);  //先保存好文件名
            eLIBs_strcpy(temp_str2, p_next_item->name);
            LowerCase_2_UpperCase(p_cur_item->name);    //将文件名中的小写字母部分统一转换成大写
            LowerCase_2_UpperCase(p_next_item->name);
            ret = eLIBs_strcmp((const char *)(p_cur_item->name), (const char *)(p_next_item->name)) ;    //比较文件名
            //__msg(" ret = %d\n " , ret );
            eLIBs_strcpy(p_cur_item->name, temp_str1);
            eLIBs_strcpy(p_next_item->name, temp_str2);

            if (ret < 0)
            {
                //__msg( " not need to change file pointer \n " ) ;
                p_cur_item = p_cur_item->next ; //没有交换指针，当前结点指向下一个
            }
            else if (ret > 0)
            {
                file_item_t *nod1, *nod4 ;

                // __msg( " start to swap file pointer \n" );
                if (p_cur_item == p_head_item)   //当前指针为头指针
                {
                    p_head_item = p_next_item ;
                    p_file_item_list = p_head_item ;
                }

                // 调整指针
                if(file_item_total_num > 2)//如果循环链表只有2个元素，只需要把链表头往后推一个位置就已经交换位置
                {
                    nod1 = p_cur_item->previous ;
                    nod4 = p_next_item->next ;
                    nod1->next = p_next_item ;
                    nod4->previous = p_cur_item ;
                    p_cur_item->next = nod4 ;
                    p_cur_item->previous = p_next_item ;
                    p_next_item->previous = nod1 ;
                    p_next_item->next = p_cur_item ;
                }

                is_swap =  EPDK_TRUE ;      //置标志位，表示有交换过位置
            }
            else
            {
                __msg(" unvaluable value \n ");
            }
        }

        if (EPDK_FALSE == is_swap)
        {
            __msg("file has been sorted finished , break \n");
            break ;
        }
    }

    /*
    {       // for test
        file_item_t* temp ;
        temp = p_file_item_list ;
        while( temp->next != p_file_item_list )
        {
            __msg("temp->name = %s\n " , temp->name);
            temp = temp->next ;
        }

        __msg("temp->name = %s\n " , temp->name);
    }
    */
    return p_file_item_list ;
}
static __s32 FileList(void)
{
    file_item_t *item;
    file_list_t *list;
    return EPDK_OK;
}

//根据搜索到的文件信息, 新建一个文件条目,存储文件或文件夹相关的信息
file_item_t *new_file_item(ES_DIRENT   *pDirent)
{
    file_item_t *file_item = NULL;
    __inf("Enter %s\r\n", __FUNCTION__);

    if (pDirent == NULL)
    {
        return NULL;
    }

    //file_item = (file_item_t*)esMEMS_Balloc(sizeof(file_item_t));
    __wrn("new_file_item 1\n");
    file_item = (file_item_t *)eLIBs_malloc(sizeof(file_item_t));

    if (file_item  == NULL)
    {
        __wrn("memory is not enough for new a file list\n");
        return NULL;
    }

    __wrn("new_file_item 2\n");
    eLIBs_memset(file_item, 0, sizeof(file_item_t));
    __wrn("new_file_item 3\n");
    file_item->name_len = eLIBs_strlen(pDirent->d_name);

    if (file_item->name_len != 0)
    {
        file_item->name_len += 5;       //需多申请一个byte
        __wrn("new_file_item 4\n");
        file_item->name = (char *)eLIBs_malloc(file_item->name_len);

        if (file_item->name == NULL)
        {
            eLIBs_free(file_item);
            return NULL;
        }

        eLIBs_memset(file_item->name, 0, file_item->name_len);
        eLIBs_strcpy(file_item->name, pDirent->d_name);
    }

    file_item->file_size = pDirent->d_size;
    file_item->fatdirattr = pDirent->fatdirattr;
    __wrn("new_file_item 5\n");
    return file_item;
}

//删除一个文件条目,释放相关的内存
__s32 delete_file_item(file_item_t *file_item)
{
    if (file_item == NULL)
    {
        return EPDK_FAIL;
    }

    if (file_item->name_len != 0)
    {
        if (file_item->name != NULL)
        {
            //esMEMS_Bfree(file_item->name, file_item->name_len);
            eLIBs_free(file_item->name);
            file_item->name = NULL;
        }
    }

    //esMEMS_Bfree(file_item, sizeof(file_item));
    eLIBs_free(file_item);
    return EPDK_OK;
}

//-------------------------------------------------------------------------------
//function      :
//descriptions      : 创建一个新的文件列表，该函数搜索指定目录下所有文件夹和文件
//               该函数执行时间由文件个数决定
//               文件夹单独形成一个链表
//              文件单独形成一个链表
//              最后把两个链表合并，文件夹在前面
//arguments     :   ext_para, 为扩展参数
//return            : 返回搜索到文件的个数,
//-------------------------------------------------------------------------------
__s32  new_file_list(const char *path, file_item_t **pp_item_list, void *ext_para)
{
    ES_DIRENT   *pDirent = NULL;
    ES_DIR      *pDir = NULL;
    file_item_t *list = NULL;
    file_item_t *first = NULL;
    file_item_t *next = NULL;
    file_item_t *folder_first = NULL;           //文件夹列表
    file_item_t *folder_list = NULL;
    file_item_t *folder_next = NULL;
    __bool  no_folder = 0, no_file = 0 ;
    __u32 count = 0, folder_count = 0;
    __s32 ret = 0;
    pDir = eLIBs_opendir(path);

    if (pDir == NULL)
    {
        return 0;
    }

    eLIBs_rewinddir(pDir);

    /*
        pDirent = eLIBs_readdir(pDir);              //获得第一个文件或文件夹
        if(pDirent == NULL)
        {
            return 0;
        }

        if(pDirent->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            folder_first = new_file_item(pDirent);
            if(folder_first != NULL)
            {
                folder_first->fatdirattr = FSYS_ATTR_DIRECTORY;
                folder_list = folder_first;
            }
        }
        else
        {
            first = new_file_item(pDirent);
            if(first == NULL)
            {
                return 0;
            }
            list = first;
            //pp_item_list = first;                     //列表首地址返回给调用者
            count++;
        }
    */
    while (1)
    {
        pDirent = eLIBs_readdir(pDir);

        if (pDirent == NULL)
        {
            break;
        }

        /* skip "." and ".." sub directory */
        if (0 == eLIBs_strcmp(".", pDirent->d_name)
            || 0 == eLIBs_strcmp("..", pDirent->d_name))
        {
            continue;
        }

        /*
        ret = eLIBs_strcmp(pDirent->d_name, ".");
        if(ret == 0)
        {
            continue;
        }
        ret = eLIBs_strcmp(pDirent->d_name, "..");
        if(ret == 0)
        {
            continue;
        }
        */
        if (pDirent->fatdirattr & FSYS_ATTR_DIRECTORY)      //文件夹单独形成一个链表
        {
            if (folder_first == NULL)
            {
                folder_first = new_file_item(pDirent);

                if (folder_first != NULL)
                {
                    folder_first->fatdirattr = FSYS_ATTR_DIRECTORY;
                    folder_list = folder_first;
                    folder_count++;
                }
                else
                {
                    break;
                }
            }
            else
            {
                folder_next = new_file_item(pDirent);

                if (folder_next != NULL)
                {
                    folder_list->next = folder_next;
                    folder_next->previous = folder_list;
                    folder_list = folder_next;                  //folder_list 为链表的最后一个点
                    folder_count++;
                }
                else
                {
                    break;
                }
            }
        }
        else                                        //文件单独形成一个链表
        {
            if (first == NULL)
            {
                first = new_file_item(pDirent);

                if (first == NULL)
                {
                    break;
                }

                list = first;
                count++;
            }
            else
            {
                next = new_file_item(pDirent);

                if (next != NULL)
                {
                    list->next = next;
                    next->previous = list;
                    list = next;                                //list 为链表的最后一个点
                    count++;
                }
                else
                {
                    break;
                }
            }
        }
    }

    __msg("get %d folders & %d files totally\n", folder_count, count);

    if (folder_first == NULL)       //文件夹链表为空，表示没有文件，将no_folder置1，表示没有文件夹
    {
        no_folder = 1 ;
    }

    if (first == NULL)
    {
        no_file = 1 ;
    }

    if (!no_file)    //将文件形成一个双向循环链表
    {
        list->next = first;
        first->previous = list;     //形成双向链表
        first = file_list_sort(first, count) ;       //将文件按文件名进行排序, 返回排好序的链表首地址
        list = first->previous ;         //将排序后的文件链表进行调整，使list 重新指向文件链表的结尾
    }

    if (!no_folder)  //将文件夹形成一个双向循环链表
    {
        folder_list->next = folder_first;
        folder_first->previous = folder_list;
        folder_first = file_list_sort(folder_first, folder_count) ;
        folder_list = folder_first->previous ;   //将排序后的文件链表进行调整，使folder_list  重新指向链表的结尾
    }

    //将两个链表合并起来,文件夹链表在前
    if (!no_folder)
    {
        if (!no_file)
        {
            folder_list->next = first;                      //文件夹与文件链接起来
            first->previous = folder_list;                  //连接文件夹
            list->next = folder_first;                      //文件列表的结尾指向文件夹列表开始的地方
            folder_first->previous = list;
        }

        *pp_item_list = folder_first;                       //列表首地址返回给调用者
    }
    else
    {
        if (!no_file)                                     //只有文件
        {
            *pp_item_list = first;                      //列表首地址返回给调用者
        }
        else
        {
            __msg("nothing in this folder\n");
        }
    }

    eLIBs_closedir(pDir);
    return count + folder_count;
}
//删除一个文件列表(使用链表删除)
//item_list = 列表首地址
//total = 总类表个数
__s32 delete_file_list(file_item_t  *item_list, __u32 total)
{
    __u32 i = 0;
    file_item_t *list = NULL;
    file_item_t *temp = NULL;

    if (item_list == NULL)
    {
        return EPDK_FAIL;
    }

    list = item_list;

    for (i = 0; i < total; i++)
    {
        if (list == NULL)
        {
            break;
        }

        if (list->next == NULL)
        {
            break;
        }

        temp = list->next;              //先保存下一个条目地址

        if (temp != NULL)
        {
            delete_file_item(list);
            list = temp;
        }
        else
        {
            break;
        }
    }

    return EPDK_OK;
}

//从列表中删除一个节点
__s32 delete_file_item_from_list(file_item_t *item, file_list_t *list)
{
    file_item_t *previous = NULL;
    file_item_t *next = NULL;
    __s32 ret = 0;

    if (item == NULL)
    {
        return EPDK_FAIL;
    }

    if (list == NULL)
    {
        return EPDK_FAIL;
    }

    if (list->item_list == item)
    {
        list->item_list = item->next;       //删除的是第一个头节点,需要改变item_list的值
    }

    if (item->next == item)                 //链表中只有一个节点
    {
        __msg("=========================only one item\n");
        ret = delete_file_item(item);
        list->cur_item = NULL;
        list->item_list = NULL;
        return ret;
    }
    else
    {
        if (item->previous != NULL)
        {
            previous = item->previous;
        }

        if (item->next != NULL)
        {
            next = item->next;
        }

        if (previous == next)
        {
            //相等表示只有两个条目
            __msg("=========================there are two items \n");
            next->next = next;
            next->previous = next;
            ret = delete_file_item(item);
        }
        else                        //超过两个
        {
            __msg("=========================more than two items \n");
            previous->next = next;
            next->previous = previous;
            ret = delete_file_item(item);
        }
    }

    return ret;
}

//cur_item为当前条目
//item为要插入的条目,插入到cur_item的后面
__s32 insert_file_item_to_list(file_item_t *cur_item, file_item_t *item)
{
    file_item_t *previous = NULL;
    file_item_t *next = NULL;

    if (cur_item == NULL)
    {
        return EPDK_FAIL;
    }

    if (item == NULL)
    {
        return EPDK_FAIL;
    }

    next = cur_item->next;
    cur_item->next = item;
    item->previous = cur_item;
    item->next = next;
    next->previous = item;
    return EPDK_OK;
}

//为filelist 链表申请一个节点
file_list_t *new_file_list_nod(const char *path, void *ext_para)
{
    file_list_t *nod = NULL;
    nod = (file_list_t *)eLIBs_malloc(sizeof(file_list_t));

    if (nod == NULL)
    {
        __err("memeor is not enough for file list\n");
    }

    eLIBs_memset((void *)nod, 0, sizeof(file_list_t));
    nod->total = new_file_list(path, &(nod->item_list), ext_para);

    if (nod->total != 0)
    {
        __msg("total = %d\n", nod->total);
    }

    eLIBs_strcpy(nod->file_path, path);
    return nod;
}

//删除一个节点,且删除节点下的itemlist
__s32 delete_file_list_nod(file_list_t *nod)
{
    __s32 ret = 0;

    if (nod == NULL)
    {
        return EPDK_FAIL;
    }

    if (nod->item_list != NULL)
    {
        ret = delete_file_list(nod->item_list, nod->total);
    }

    eLIBs_free(nod);
    return ret;
}

//检测存储设备是否ready
//
file_list_t *new_file_root_list_nod(void *ext_para)
{
    ES_DIRENT dirent;
    file_item_t *item = NULL;
    file_list_t *root_nod = NULL;
    __s32 ret = 0, count = 0;
    char disk_name[RAT_MAX_PARTITION][4] = {{0},};
    __inf("Enter %s\r\n", __FUNCTION__);
    root_nod = (file_list_t *)eLIBs_malloc(sizeof(file_list_t));

    if (root_nod == NULL)
    {
        __err("memeor is not enough for file list\n");
        return NULL;
    }

    eLIBs_memset(root_nod, 0, sizeof(file_list_t));
    ret = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);

    if ((ret == EPDK_OK) && (eLIBs_strlen(disk_name[0]) != 0))
    {
        //__here__ ;
        eLIBs_memset(&dirent, 0, sizeof(ES_DIRENT));
        dirent.d_size = 0;
        dirent.fatdirattr = RAT_MEDIA_TYPE_USB_DEVICE;
        //eLIBs_strcpy(root_nod->USB_path, disk_name[0]);       //保存盘符
        eLIBs_strcpy(dirent.d_name, disk_name[0]);
        // __wrn("new_file_root_list_nod 2\n");
        item = new_file_item(&dirent);

        if (item == NULL)
        {
            __err("%s %d error in file list!\r\n", __FILE__, __LINE__);
        }
        else
        {
            item->next = item;                  // add by linyaoshu 2011.9.1
            item->previous = item;              // add by linyaoshu 2011.9.1
            count++;
        }
    }

    eLIBs_memset(disk_name, 0, sizeof(disk_name));
    ret = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);

    if ((ret == EPDK_OK) && (eLIBs_strlen(disk_name[0]) != 0))
    {
        char    disk_name_t[16] = {0}; //chengf  add
        //__here__ ;
        eLIBs_memset(&dirent, 0, sizeof(ES_DIRENT));
        dirent.d_size = 0;
        dirent.fatdirattr = RAT_MEDIA_TYPE_SD_DEVICE;
#if  TF_ENCRYPTION_EX

        if (rat_is_encryption_partition_Insert(disk_name_t, 1)) //存在加密分区//chengf  add
        {
            eLIBs_strcpy(dirent.d_name, disk_name[1]);          //chengf  add
        }
        else
        {
            eLIBs_strcpy(dirent.d_name, disk_name[0]);
        }

#else
        eLIBs_strcpy(dirent.d_name, disk_name[0]);
#endif

        if (item == NULL)                               //只有一个盘符
        {
            // __wrn("new_file_root_list_nod 4\n");
            item = new_file_item(&dirent);

            //__wrn("new_file_root_list_nod item=%08x\n",item);
            if (item != NULL)
            {
                item->next = item;
                item->previous = item;
                count++;
            }
        }
        else
        {
            // __wrn("new_file_root_list_nod 5\n");
            item->next = new_file_item(&dirent);

            if (item->next != NULL)
            {
                item->next->previous = item;            //有两个盘符
                item->next->next = item;
                item->previous  = item->next;
                count++;
            }
            else
            {
                item->next = item;                  //只有一个盘符
                item->previous = item;
            }
        }
    }

    if (item != NULL)
    {
        // __inf("new_file_root_list_nod 6\n");
        root_nod->item_list = item ;
        root_nod->total = count ;
    }
    else
    {
        //__wrn("new_file_root_list_nod 7\n");
        __msg(" item is null ,get new_file_root_list_nod fail \n");
        eLIBs_free(root_nod) ;   //一个盘符都没有
        root_nod = NULL ;
    }

    //__here__ ;
    return root_nod;
}

char root_device_str[2][20] =
{
    {"USB"},
    {"SD"}
};

char *get_root_device_disp_name(char *string, __u8 attr)
{
    char *str;

    if (attr == RAT_MEDIA_TYPE_USB_DEVICE)
    {
        str = root_device_str[0];
        eLIBs_strcpy(string, str);
        return str;
    }
    else if (attr == RAT_MEDIA_TYPE_SD_DEVICE)
    {
        str = root_device_str[1];
        eLIBs_strcpy(string, str);
        return str;
    }

    return NULL;
}


__s32 file_list_get_root_path(char *path, file_list_t *list, __u32 index)
{
    if (list == NULL)
    {
        return EPDK_FAIL;
    }

    if (index >= list->total)
    {
        return EPDK_FAIL;
    }

    return EPDK_FAIL;
}
__s32 update_file_root_list_nod(void *ext_para)
{
    return EPDK_OK;
}
//根据索引号从nod中搜索
file_item_t *get_file_list_item(file_list_t *nod, __u32 index)
{
    __u32 i = 0, count = 0;
    file_item_t *temp = NULL;
    file_item_t *list = NULL;

    if (nod == NULL)
    {
        return NULL;
    }

    __msg("nod->total = %d  index = %d \n ", nod->total, index);

    if (nod->item_list == NULL)
    {
        __msg("nod->item_list is null \n");
        return NULL;
    }

    if (index >= nod->total)
    {
        __msg("index is bigger than total \n");
        return NULL;
    }

    list = nod->item_list;

    for (i = 0; i <= index; i++)
    {
        temp =  list;
        list = list->next;
    }

    return temp;
}


char *get_file_list_item_name(file_list_t *nod, __u32 index)
{
    file_item_t *temp = NULL;
    temp = get_file_list_item(nod, index);

    if (temp == NULL)
    {
        return NULL;
    }

    return temp->name;
}

//获得文件条目文件类型
rat_media_type_t  get_file_list_item_file_type(file_item_t *item)
{
    rat_media_type_t media_type = RAT_MEDIA_TYPE_UNKNOWN;

    if (item == NULL)
    {
        return RAT_MEDIA_TYPE_UNKNOWN;
    }

    if (item->fatdirattr & FSYS_ATTR_DIRECTORY)
    {
        return RAT_MEDIA_TYPE_FOLDER;
    }

    if (item->fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE)
    {
        return RAT_MEDIA_TYPE_SD_DEVICE;
    }

    if (item->fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
    {
        return RAT_MEDIA_TYPE_USB_DEVICE;
    }

    media_type = rat_get_file_type(item->name);
    return media_type;
}

//根据给出的顶部节点，删除整个filelist 链表
//注意只向下删除链表
__s32 delete_file_list_chain(file_list_t *top_nod)
{
    file_list_t *parent_nod = NULL;
    file_list_t *child_nod = NULL;

    if (top_nod == NULL)
    {
        return EPDK_FAIL;
    }

    parent_nod = top_nod;
    child_nod = parent_nod->child;              //保存子节点地址

    if (child_nod == NULL)
    {
        delete_file_list_nod(parent_nod);       //删除父节点
        return EPDK_OK;
    }

    while (child_nod != NULL)
    {
        delete_file_list_nod(parent_nod);       //删除父节点
        parent_nod = child_nod;                 //指向下一个节点
        child_nod = parent_nod->child;          //保存子节点地址
    }

    return EPDK_OK;
}

//char            sub_dir[RAT_MAX_FULL_PATH_LEN];               //放在函数中，会引起堆栈溢出
//获得一个文件夹目录下文件条目总数，包括文件夹
__s32 get_item_num_in_dir(const char *dirpath, __u32 *total)
{
    ES_DIR          *dirp;
    ES_DIRENT       *direntp;
    char            sub_dir[RAT_MAX_FULL_PATH_LEN];
    //char            *sub_dir;
    static __s32    dir_level = 0;

    if (dirpath == NULL)
    {
        return EPDK_FAIL;
    }
    else if (dir_level > C_MAX_DIR_SCAN_LEVEL)
    {
        //return EPDK_FAIL;
    }
    else
    {
        dir_level++;
        __msg("dir level is %d\n", dir_level);
    }

    //__here__;
    {
        __s32 len;
        len = eLIBs_strlen(dirpath);
        __msg("len = %d\n", len);
        __msg("dirpath=%s\n", dirpath);
    }
    dirp = (ES_DIR *)eLIBs_opendir(dirpath);

    //__here__;
    if (dirp == NULL)
    {
        //__here__;
        return EPDK_FAIL;
    }

    //__here__;
#if 0
    sub_dir = eLIBs_malloc(RAT_MAX_FULL_PATH_LEN);

    if (!sub_dir)
    {
        return EPDK_FAIL;
    }

#endif

    while ((direntp = (ES_DIRENT *)eLIBs_readdir(dirp)) != NULL)
    {
        //__here__;
        if (direntp->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            /* skip "." and ".." sub directory */
            if (0 == eLIBs_strcmp(".", direntp->d_name)
                || 0 == eLIBs_strcmp("..", direntp->d_name))
            {
                continue;
            }

            //__here__;
            eLIBs_memset(sub_dir, 0, RAT_MAX_FULL_PATH_LEN);
            eLIBs_strcpy(sub_dir, dirpath);
            eLIBs_strcat(sub_dir, "\\");
            //__here__;
            eLIBs_strcat(sub_dir, (const char *)direntp->d_name);

            //__here__;
            if (get_item_num_in_dir(sub_dir, total) == EPDK_FAIL)
            {
                return EPDK_FAIL;
            }

            if (dir_level > 0)
            {
                dir_level--;
                __msg("dir level is %d\n", dir_level);
            }

            __msg("---total: %d, dirpath: %s, sub_dir: %s\n", *total, dirpath, sub_dir);
        }
        else
        {
            (*total)++;
            __msg("---total: %d, dirpath: %s, file: %s\n", *total, dirpath, (const char *)direntp->d_name);
        }

        //__here__;
    }

#if 0

    if (sub_dir)
    {
        eLIBs_free(sub_dir);
        sub_dir = NULL;
    }

#endif

    //__here__;
    if (eLIBs_closedir(dirp) == EPDK_FAIL)
    {
        //__here__;
        return EPDK_FAIL;
    }

    (*total)++;
    __msg("----total: %d\n", *total);
    return EPDK_OK;
}


//删除一个文件夹
__s32 delete_dir(char *dir_name, file_op_info_t *op_info)
{
    ES_DIR          *dir = NULL;
    ES_DIRENT       *dirent = NULL;
    char            fullpath[RAT_MAX_FULL_PATH_LEN];
    __s32           ret = EPDK_OK;
    //static __s32    dir_level = 0;

    if (dir_name == NULL)
    {
        __msg("------------------------------------------input param error\n");
        return EPDK_FAIL;
    }
    else if (op_info->dir_level > C_MAX_DIR_SCAN_LEVEL)
    {
        if (op_info != NULL)
        {
            op_info->percent = 100;
        }

        __msg("------------------------------------------dir_level is over than MAX_DIR_SCAN_LEVEL, failed\n");
        //return EPDK_FAIL;
    }
    else
    {
        op_info->dir_level++;
        __msg("------------------------------------------dir_level is %d\n", op_info->dir_level);
    }

    /*
    if(_is_dir_exist(dir_name) == EPDK_FAIL)
    {
        info->percent = 101;
        return EPDK_FAIL;
    }
    */
    eLIBs_memset(fullpath, 0, RAT_MAX_FULL_PATH_LEN);
    /*open dir*/
    dir = eLIBs_opendir(dir_name);

    if (dir == NULL)
    {
        return EPDK_FAIL;
    }

    while (1)
    {
        /*read dir*/
        dirent = eLIBs_readdir(dir);

        if (dirent == NULL)
        {
            __msg("----read dir %s is empty\n", dir_name);
            break;
        }

        eLIBs_memset(fullpath, 0, RAT_MAX_FULL_PATH_LEN);
        eLIBs_strcpy(fullpath, dir_name);
        eLIBs_strcat(fullpath, "\\");
        eLIBs_strcat(fullpath, (const char *)dirent->d_name);
        __msg("----read dir %s ---fullpath: %s\n", (const char *)dirent->d_name, fullpath);

        if (dirent->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            if (0 == eLIBs_strcmp(".", dirent->d_name)
                || 0 == eLIBs_strcmp("..", dirent->d_name))
            {
                continue;
            }

            ret = delete_dir(fullpath, op_info);

            if (ret == EPDK_FAIL)
            {
                __msg("------------------------_inner_delete_dir failed---fullpath: %s\n", fullpath);
                ret = EPDK_FAIL;
                break;
            }
            else
            {
                __msg("------------------------_inner_delete_dir success----fullpath: %s\n", fullpath);
            }

            if (op_info->dir_level > 0)
            {
                op_info->dir_level--;
                __msg("------------------------------------------dir_level is %d\n", op_info->dir_level);
            }
        }
        else
        {
            if (eLIBs_remove(fullpath) == -1)
            {
                __msg("-------------------------remove file %s failed\n", fullpath);
                ret = EPDK_FAIL;
                break;
            }
            else
            {
                __msg("------------------------remove file %s successfully\n", fullpath);

                if (op_info != NULL)
                {
                    op_info->finish_num++;
                    op_info->percent = (op_info->finish_num * 100) / op_info->total_num;

                    if (op_info->OnDraw != NULL)
                    {
                        op_info->OnDraw(op_info);
                    }
                }
            }
        }
    }

    if (op_info != NULL)
    {
        op_info->finish_num++;
        op_info->percent = (op_info->finish_num * 100) / op_info->total_num;

        if (op_info->OnDraw != NULL)
        {
            op_info->OnDraw(op_info);
        }
    }

    eLIBs_closedir(dir);
    ret = eLIBs_rmdir(dir_name);
    __msg("------------------------eLIBs_rmdir %s--- ret: %d\n", dir_name, ret);

    if (ret == EPDK_FAIL)
    {
        if (op_info != NULL)
        {
            op_info->percent = 101;
        }
    }

    //dir_level = 0;
    return ret;
}




//复制一个文件夹
__s32 copy_dir(char *dst_dir, char *src_dir, file_op_info_t *op_info)
{
    ES_DIR          *dir = NULL;
    ES_DIRENT       *dirent = NULL;
    char            fullpath[RAT_MAX_FULL_PATH_LEN];
    char            dstpath[RAT_MAX_FULL_PATH_LEN];
    __s32           ret = EPDK_OK;
    ES_FILE *fp1, *fp2, *fp3;
    //static __s32    dir_level = 0;

    if (src_dir == NULL)
    {
        __msg("------------------------------------------input param error\n");
        return EPDK_FAIL;
    }
    else if (op_info->dir_level > C_MAX_DIR_SCAN_LEVEL)
    {
        if (op_info != NULL)
        {
            op_info->percent = 100;
        }

        __msg("------------------------------------------dir_level is over than MAX_DIR_SCAN_LEVEL, failed\n");
        //return EPDK_FAIL;
    }
    else
    {
        op_info->dir_level++;
        __msg("------------------------------------------dir_level is %d\n", op_info->dir_level);
    }

    eLIBs_memset(fullpath, 0, RAT_MAX_FULL_PATH_LEN);
    /*open dir*/
    dir = eLIBs_opendir(src_dir);

    if (dir == NULL)
    {
        return EPDK_FAIL;
    }

    while (1)
    {
        /*read dir*/
        dirent = eLIBs_readdir(dir);

        if (dirent == NULL)
        {
            __msg("----read dir %s is empty\n", src_dir);
            break;
        }

        eLIBs_memset(fullpath, 0, RAT_MAX_FULL_PATH_LEN);
        eLIBs_strcpy(fullpath, src_dir);
        eLIBs_strcat(fullpath, "\\");
        eLIBs_strcat(fullpath, (const char *)dirent->d_name);
        eLIBs_memset(dstpath, 0, RAT_MAX_FULL_PATH_LEN);
        eLIBs_strcpy(dstpath, dst_dir);
        eLIBs_strcat(dstpath, "\\");
        eLIBs_strcat(dstpath, (const char *)dirent->d_name);
        __msg("----read dir %s ---fullpath: %s\n", (const char *)dirent->d_name, fullpath);

        if (dirent->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            if (0 == eLIBs_strcmp(".", dirent->d_name)
                || 0 == eLIBs_strcmp("..", dirent->d_name))
            {
                continue;
            }

            fp1 = eLIBs_fopen(dstpath, "rb");

            if (NULL == fp1)
            {
                if (0 != eLIBs_mkdir(dstpath))
                {
                    __wrn("create dir [ %s ] fail\n", dstpath);
                    return EPDK_FAIL;
                }
            }
            else
            {
                eLIBs_fclose(fp1);
                fp1 = NULL;
            }

            ret = copy_dir(dstpath, fullpath, op_info);

            if (ret == EPDK_FAIL)
            {
                __msg("------------------------_inner_delete_dir failed---fullpath: %s\n", fullpath);
                ret = EPDK_FAIL;
                break;
            }
            else
            {
                __msg("------------------------_inner_delete_dir success----fullpath: %s\n", fullpath);
            }

            if (op_info->dir_level > 0)
            {
                op_info->dir_level--;
                __msg("------------------------------------------dir_level is %d\n", op_info->dir_level);
            }
        }
        else
        {
            __s32 len;
            __s64 total_size, file_size;
            char *pbuf = NULL;
            char path[4];
            eLIBs_strncpy(path, dstpath, 3);
            path[3] = '\0';
            total_size = eLIBs_GetVolFSpace(path);
            file_size = eLIBs_GetFileSize(fullpath);

            if (file_size > total_size)
            {
                return EPDK_FAIL;
            }

            fp1 = eLIBs_fopen(dstpath, "wb+");

            if (NULL == fp1)
            {
                return EPDK_FAIL;
            }

            fp2 = eLIBs_fopen(fullpath, "rb+");

            if (NULL == fp2)
            {
                __msg("parse file err!\n");
                eLIBs_fclose(fp1);
                return EPDK_FAIL;
            }

            pbuf = esMEMS_Palloc(CAT_READ_BUFFER_SIZE >> 10, 0);

            if (NULL == pbuf)
            {
                __wrn("palloc err\n");
                eLIBs_fclose(fp1);
                eLIBs_fclose(fp2);
                return EPDK_FAIL;
            }

            while (1)
            {
                len = eLIBs_fread(pbuf, 1, CAT_READ_BUFFER_SIZE, fp1);

                if (0 < len)
                {
                    eLIBs_fwrite(pbuf, 1, len, fp2);
                }
                else
                {
                    break;
                }

                esKRNL_TimeDly(5);
            };

            eLIBs_fclose(fp1);

            eLIBs_fclose(fp2);

            esMEMS_Pfree(pbuf, 1);
        }
    }

    eLIBs_closedir(dir);
    return ret;
}
