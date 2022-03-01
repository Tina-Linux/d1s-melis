#ifndef __FILELIST_H__
#define __FILELIST_H__
#include "beetles_app.h"

#define CAT_READ_BUFFER_SIZE (64<<10)

/*!
  * \brief
  *     删除拷贝信息
*/
typedef struct file_op_info_s
{
    //item_type_t           type;                       //条目类型，文件或者目录
    __u64               total_size;         //需要拷贝的总容量;
    __u64               finish_size;        //已拷贝的总容量;
    __u32               total_num;          //目录时，需拷贝/删除的文件总数
    __u32               finish_num;         //目录时，已完成拷贝/删除的文件数
    __u32               percent;            //已拷贝/删除数据的百分比，公式=已拷贝数据/需拷贝数据总量
    __u32               dir_level;          //目录层数
    void (*OnDraw)(struct file_op_info_s *op_info);               //画删除进度回调函数
} file_op_info_t;



typedef struct file_item_s
{
    char *name;                     //文件或文件夹名字
    __u32 name_len;                 //申请和释放文件名空间的时候用

    __u8  fatdirattr;               /* FAT only. Contains the "DIR_Attr" field of an entry. */
    //是文件夹还是文件, 文件夹 = FSYS_ATTR_DIRECTORY
    __u8  status;                   //是否有子文件夹
    __u64 file_size;                //文件大小
    __u32 create_time;              //创建时间，用时间格式定义变量

    struct file_item_s *previous;   //指向上一个条目
    struct file_item_s *next;       //指向下一个条目
} file_item_t;


typedef struct file_list_s
{
    char  file_path[RAT_MAX_FULL_PATH_LEN];         //当前目录文件路径
    //char  USB_path[20];               //
    //char  SD_path[20];                //
    __u32 start_id;                 //显示在列表中的起始ID号
    __u32 focus_id;                 //显示在列表中焦点ID号
    __u32 total;                    //当前目录下总文件个数(包括文件夹,文件,和 "..")

    file_item_t    *item_list;      //条目列表首地址
    file_item_t    *cur_item;       //当前条目

    struct file_list_s  *parent;    //上一级目录 信息
    struct file_list_s  *child;     //下一级目录 信息

} file_list_t;

#define C_MAX_DIR_SCAN_LEVEL        16

extern __s32 new_file_list(const char *path, file_item_t **pp_item_list, void *ext_para);
extern __s32 delete_file_list(file_item_t  *item_list, __u32 total);

extern __s32 delete_file_item_from_list(file_item_t *item, file_list_t *list);
extern __s32 insert_file_item_to_list(file_item_t *cur_item, file_item_t *item);
extern file_item_t *get_file_list_item(file_list_t *nod, __u32 index);
extern char *get_file_list_item_name(file_list_t *nod, __u32 index);
extern rat_media_type_t  get_file_list_item_file_type(file_item_t *item);

extern file_list_t *new_file_root_list_nod(void *ext_para);
extern file_list_t *new_file_list_nod(const char *path, void *ext_para);
extern __s32 delete_file_list_nod(file_list_t *nod);
extern __s32 delete_file_list_chain(file_list_t *top_nod);

extern char *get_root_device_disp_name(char *string, __u8 attr);
extern __s32 delete_dir(char *dir_name, file_op_info_t *op_info);
__s32 copy_dir(char *dst_dir, char *src_dir, file_op_info_t *op_info);

__s32 delete_file_item(file_item_t *file_item);
file_item_t *new_file_item(ES_DIRENT   *pDirent);
__s32 get_item_num_in_dir(const char *dirpath, __u32 *total);



#endif






















