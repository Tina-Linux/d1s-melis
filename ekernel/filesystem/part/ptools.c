/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : ptools.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: partition tools of file system.
* Update  : date                auther      ver     notes
*           2011-3-15 14:58:06  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "part.h"
#include "fsys_debug.h"

extern __fsys_part_t *fsys_find_part(const char *pFullName, char **pFileName);

int32_t esFSYS_statpt(const char *path, __hdle buf)
{
    int32_t             res;
    __fsys_part_t       *pPart;
    struct kstatpt      *stat   = (struct kstatpt *)buf;
    char                *class_name;
    char                *dev_name;

    fs_log_trace0("statpt:%s,", path);

    esKRNL_SemPend(pPartSem, 0, NULL);

    /* Find correct FSL  (device:unit:name)                     */
    pPart   = fsys_find_part(path, NULL);
    if (!pPart)
    {
        fs_log_warning("%s partition not installed!\n", path);
        res = EPDK_FAIL;
        goto out;
    }

    stat->p_attr        = pPart->attr;
    stat->p_status      = pPart->status;
    stat->p_blocks      = 0;                /* need to be set by part drivers */
    stat->p_bsize       = 0;                /* need to be set by part drivers */
    stat->p_pdname      = pPart->pPD->name;

    strncpy(stat->p_name, pPart->dname, MAX_PART_NAME_LEN);

    /* set device information */
    esDEV_Ioctl(pPart->hDev, DEV_IOC_SYS_GET_CLSNAME, 0, &class_name);
    strncpy(stat->p_devclassname, class_name, MAX_DEV_CLASS_NAME_LEN);

    esDEV_Ioctl(pPart->hDev, DEV_IOC_SYS_GET_DEVNAME, 0, &dev_name);
    strncpy(stat->p_devname, dev_name, MAX_DEV_NODE_NAME_LEN);

    esDEV_Ioctl(pPart->hDev, DEV_IOC_SYS_GET_ATTRIB,  0, &(stat->p_devattr));

    res = EPDK_OK;

out:
    esKRNL_SemPost(pPartSem);
    fs_log_trace0("O\n");
    return res;
}

/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t esFSYS_querypartupdateflag(const char *path, __bool *flag)
{
    __fsys_part_t       *pPart;

    fs_log_trace0("querypartupdateflag:%s,", path);

    if (flag == NULL)
    {
        return EPDK_FAIL;
    }

    esKRNL_SemPend(pPartSem, 0, NULL);

    /* Find correct FSL  (device:unit:name)                     */
    pPart = fsys_find_part(path, NULL);
    if (!pPart)
    {
        fs_log_warning("%s partition not installed!\n", path);
        esKRNL_SemPost(pPartSem);
        return EPDK_FAIL;
    }

    *flag = pPart->updateflag;
    esKRNL_SemPost(pPartSem);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                       FULL FUNCTION NAME
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
__s32  esFSYS_clearpartupdateflag(const char *path)
{
    __fsys_part_t      *pPart;

    fs_log_trace0("querypartupdateflag:%s,", path);

    esKRNL_SemPend(pPartSem, 0, NULL);

    /* Find correct FSL  (device:unit:name)                     */
    pPart = fsys_find_part(path, NULL);
    if (!pPart)
    {
        fs_log_warning("%s partition not installed!\n", path);
        esKRNL_SemPost(pPartSem);
        return EPDK_FAIL;
    }

    pPart->updateflag = EPDK_FALSE;
    esKRNL_SemPost(pPartSem);
    return EPDK_OK;
}
