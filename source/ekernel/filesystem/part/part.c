/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : part.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: partition management of file system.
* Update  : date                auther      ver     notes
*           2011-3-15 14:57:08  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "part.h"
#include "fsys_debug.h"
#include <string.h>
#include <sys_fsys.h>

extern __fsys_part_t     *pPartFTbl[];   /* 自由分配的分区列表 */
extern __fix_part_t       pPartXTbl[];   /* 固定分配的分区列表 */
extern __fix_part_t       pPartUTbl[];   /* 方案用户自定义分区的分区列表 */
extern __fsys_pd_t       *pPDRoot;
extern struct file_system_type  *pFSRoot;

/*
**********************************************************************************************************************
*
*             fsys_find_part
*
*  Description:
*  FSYS internal function. Find correct index in the device information
*  table referred by fsys_pDevInfo for a given fully qualified name.
*
*  Parameters:
*  pFullName   - Fully qualified name.
*  pFilename   - Address of a pointer, which is modified to point to
*                the file name part of pFullName.
*
*  Return value:
*  <0          - Unable to find the device.
*  >=0         - Index of the device in the device information table.
**********************************************************************************************************************
*/
__fsys_part_t *fsys_find_part(const char *pFullName, char **pFileName)
{
    __fsys_part_t   *part = NULL;
    uint32_t        letter;
    char            *s;

    letter  = *pFullName;

    /* 获取盘符 */
    s = strchr((char *)pFullName, ':');

    if ( (!s) || ((int32_t)(s - pFullName)) != 1)
    {
        return 0;
    }

    /* 获取路径串中的文件名部分指针 */
    if (pFileName)
    {
        *pFileName  = s + 1;
    }

    /* 盘符转换为大写字母 */
    if ((letter >= 'a') && (letter <= 'z'))
    {
        letter      -= ('a' - 'A');
    }

    /* 自由分配的分区 */
    if ((letter >= PART_LETTER_FREESTART) && letter < (PART_LETTER_FREESTART + FSYS_MAX_FPARTS))
    {
        letter      = letter - PART_LETTER_FREESTART;
        part        = pPartFTbl[letter];
    }
    else if ((letter > (PART_LETTER_USERSTART - FSYS_MAX_UPARTS)) && (letter <= (PART_LETTER_USERSTART)))
    {
        /* 方案用户自定义分区的分区 */
        uint32_t    i;

        for (i = 0; i < FSYS_MAX_UPARTS; i++)
        {
            if (letter == pPartUTbl[i].letter)
            {
                part    = pPartUTbl[i].part;
                break;
            }
        }
    }
    else
    {
        /* 固定分配的分区 */
        int32_t     i;
        for (i = 0; i < FSYS_MAX_XPARTS; i++)
        {
            if (letter == pPartXTbl[i].letter)
            {
                part    = pPartXTbl[i].part;
                break;
            }
        }
    }

    return part;
}

/*
**********************************************************************************************************************
*
*             esFSYS_popen
*
*  Description:
*  API function. Open an existing file or create a new one.
*
*  Parameters:
*  pFileName   - Fully qualified file name.
*  pMode       - Mode for opening the file.
*
*  Return value:
*  ==0         - Unable to open the file.
*  !=0         - Address of an FS_FILE data structure.
**********************************************************************************************************************
*/
__hdle esFSYS_popen(const char *PartName, const char *pMode)
{
    int32_t         omode;
    char            *s;
    __fsys_part_t   *pPart;

    fs_log_trace1("popn:%s", PartName);

    esKRNL_SemPend(pPartSem, 0, NULL);

    /* Check for valid mode string and set flags in file handle */
    if (strcmp(pMode, "r") == 0)
    {
        omode = FSYS_PARTATTR_OPENR;
    }
    else if (strcmp(pMode, "w") == 0)
    {
        omode = FSYS_PARTATTR_OPENW;
    }
    else if (strcmp(pMode, "r+") == 0)
    {
        omode = FSYS_PARTATTR_OPENW | FSYS_PARTATTR_OPENR;
    }
    else
    {
        fs_log_error("invalid popen mode.\n");
        pPart = NULL;
        goto out;
    }

    /* Find correct FSL  (device:unit:name)                     */
    pPart = fsys_find_part(PartName, &s);
    if (!pPart)
    {
        fs_log_warning("partition not installed!\n");
        pPart = NULL;
        goto out;
    }

    /* 分区只能被互斥打开，在被文件系统打开使用的情况下，先卸载文
     * 文件系统再打开，在被直接用open打开的情况下，直接返回     */
    if (pPart->status == FSYS_PARTSTATUS_RAWUSED)
    {
        fs_log_warning("part can't be twice open by esFSYS_popen()!\n");
        pPart = NULL;
        goto out;
    }
    else if (pPart->status == FSYS_PARTSTATUS_DEAD)
    {
        fs_log_warning("part has been in dead!\n");
        pPart = NULL;
        goto out;
    }
    else if (pPart->status == FSYS_PARTSTATUS_UNUSED)
    {
        goto getit;

        /* 支持不带文件系统裸分区 */
    }
    else if (pPart->status != FSYS_PARTSTATUS_FSUSED)
    {
        fs_log_warning("unkown part status, exit!\n");
        //pPart = NULL;
        goto out;
    }

    if (esFSYS_umntfs(pPart, 0) == EPDK_FAIL)
    {
        fs_log_error("umnt fs of %c fail.\n", pPart->letter);
        pPart = NULL;
        goto out;
    }

getit:
    pPart->status   = FSYS_PARTSTATUS_RAWUSED;
    pPart->attr     = (pPart->attr & ~FSYS_PARTATTR_OPENRWM) | omode;
    /* 分区被打开后，可能原有文件系统会被破坏，所以设置分区为无文件系统属性 */
    pPart->attr     = pPart->attr & ~FSYS_PARTATTR_FS;

out:
    esKRNL_SemPost(pPartSem);

    fs_log_trace1("O%x\n", pPart);
    return (__hdle)pPart;
}

/*
**********************************************************************************************************************
*
*            esFClose
*
*  Description:
*  API function. Close a file referred by pFile.
*
*  Parameters:
*  pFile       - Pointer to a FS_FILE data structure.
*
*  Return value:
*  None.
**********************************************************************************************************************
*/
int32_t esFSYS_pclose(__hdle hPart)
{
    int32_t         res;
    __fsys_part_t   *pPart = (__fsys_part_t *)hPart;

    fs_log_trace1("pcls:");

    esKRNL_SemPend(pPartSem, 0, NULL);

    /* 被unload的分区，进行销毁分区句柄的操作                   */
    if (pPart->status == FSYS_PARTSTATUS_UNUSED)
    {
        fs_log_error("pclose a unused part!\n");
        res = EPDK_FAIL;
        goto out;
    }
    else if (pPart->status == FSYS_PARTSTATUS_DEAD)
    {
        deactivate_part(pPart, 0);
        res = EPDK_OK;
        goto out;
    }

    /************************************************************/
    /* 挂接文件系统驱动                                         */
    /************************************************************/
    pPart->status   = FSYS_PARTSTATUS_FSUSED;
    if (esFSYS_mntfs(pPart) != EPDK_OK)
    {
        pPart->status   = FSYS_PARTSTATUS_UNUSED;
        pPart->attr     &= ~FSYS_PARTATTR_FS;
    }
    else
    {
        pPart->attr     |= FSYS_PARTATTR_FS;
    }

    res     = EPDK_OK;

out:
    esKRNL_SemPost(pPartSem);

    fs_log_trace1("O\n");
    return res;
}

/*
**********************************************************************************************************************
*
*            esFRead
*
*  Description:
*  API function. read data from a file.
*
*  Parameters:
*  pdata    - Pointer to a data buffer for storing data transferred from file
*  size     - Size of an element to be transferred from file to data buffer
*  n        - Number of elements to be transferred from the file
*  fp       - Pointer to a __fp data structure.
*
*  Return value:
*  Number of elements read.
**********************************************************************************************************************
*/
uint32_t esFSYS_pread(void *pData, uint32_t sector, uint32_t n, __hdle hPart)
{
    uint32_t        res;
    __fsys_part_t   *pPart = (__fsys_part_t *)hPart;

    fs_log_trace1("prd:");

    if (pPart == NULL)
    {
        fs_log_error("parameter is invalid!\n");
        res = 0;
        goto out;
    }
    if (pPart->status == FSYS_PARTSTATUS_INVALID)
    {
        fs_log_error("part is not accessable!\n");
        res = 0;
        goto out;
    }

    if (!(pPart->attr & FSYS_PARTATTR_DEVR) || (pPart->status == FSYS_PARTSTATUS_RAWUSED && !(pPart->attr & FSYS_PARTATTR_OPENR)))
    {
        fs_log_error("part is not readable!\n");
        res = 0;
        goto out;
    }

    if (pPart->pPD && pPart->pPD->Ops.read)
    {
        res = pPart->pPD->Ops.read(pData, sector, n, pPart);
        fs_log_trace1("R%ds,F%d,", n, sector);
    }
    else
    {
        fs_log_error("part has no read opers, fail\n");
        res = 0;
    }

out:
    fs_log_trace1("O\n");
    return res;
}
/*
**********************************************************************************************************************
*
*            esFWrite
*
*  Description:
*       API function. write data to a file.
*
*  Parameters:
*       pdata    - Pointer to a data  to be written to a file
*       size     - Size of an element to be transferred.
*       n        - Number of elements to be transferred to the file
*       fp       - Pointer to a __fp data structure.
*
*  Return value:
*       Number of elements written.
**********************************************************************************************************************
*/
uint32_t esFSYS_pwrite(const void *pData, uint32_t Sector, uint32_t N, __hdle hPart)
{
    uint32_t        res;
    __fsys_part_t   *pPart = (__fsys_part_t *)hPart;

    fs_log_trace1("pwr:");

    if (pPart == NULL)
    {
        fs_log_error("parameter is invalid!\n");

        res     = 0;
        goto out;
    }

    if (pPart->status == FSYS_PARTSTATUS_INVALID)
    {
        fs_log_error("part is not accessable!\n");

        res     = 0;
        goto out;
    }

    if (!(pPart->attr & FSYS_PARTATTR_DEVW) || (pPart->status == FSYS_PARTSTATUS_RAWUSED && !(pPart->attr & FSYS_PARTATTR_OPENW)))
    {
        fs_log_error("part is not readable!\n");

        res     = 0;
        goto out;
    }

    if (pPart->pPD && pPart->pPD->Ops.write)
    {
        pPart->updateflag   = EPDK_TRUE;
        res                 = pPart->pPD->Ops.write(pData, Sector, N, pPart);

        fs_log_trace1("W%ds,T%d,", N, Sector);
    }
    else
    {
        fs_log_error("part has no write opers, fail\n");

        res     = 0;
    }

out:
    fs_log_trace1("O\n");
    return res;
}
/*
**************************************************************************************************
*
*             FS_IoCtl
*
  Description:
  API function. Execute device command.

  Parameters:
  pDevName    - Fully qualified directory name.
  Cmd         - Command to be executed.
  Aux         - Parameter depending on command.
  pBuffer     - Pointer to a buffer used for the command.

  Return value:
  Command specific. In general a negative value means an error.
**************************************************************************************************
*/

int32_t esFSYS_pioctrl(__hdle hPart, uint32_t cmd, long aux, void *pbuffer)
{
    int32_t         res;
    __fsys_part_t   *pPart = (__fsys_part_t *)hPart;

    fs_log_trace1("pctl:");

    if (pPart->status == FSYS_PARTSTATUS_INVALID)
    {
        fs_log_error("part is not accessable!\n");
        res = EPDK_FAIL;
        goto out;
    }

    if (IS_PARTIOCSYS(cmd))
    {
        switch (cmd)
        {
            case PART_IOC_SYS_GETNAME:
                *((char **)pbuffer)     = pPart->dname;
                res = EPDK_OK;
                break;

            case PART_IOC_SYS_GETLETTER:
                *((char *)pbuffer)      = pPart->letter;
                res = EPDK_OK;
                break;

            case PART_IOC_SYS_GETFSPRIV:
                *((__hdle *)pbuffer)    = pPart->hFSPrivate;
                res = EPDK_OK;
                break;

            case PART_IOC_SYS_SETFSPRIV:
                pPart->hFSPrivate       = (__hdle)aux;
                res = EPDK_OK;
                break;

            case PART_IOC_SYS_GETDEV:
                *((__hdle *)pbuffer)    = pPart->hDev;
                res = EPDK_OK;
                break;
            case PART_IOC_SYS_GETLASTPART:
                *((uint32_t *)pbuffer)  = pPart->last_lun;
                res = EPDK_OK;
                break;

            default:
                res = EPDK_FAIL;
                break;
        }

        goto out;
    }
    else
    {
        res = pPart->pPD->Ops.ioctl(pPart, cmd, aux, pbuffer);
    }

out:
    fs_log_trace1("O%d\n", res);
    return res;
}

/*
**************************************************************************************************
*
*             FS_Remove
*
*  Description:
*  API function. Remove a file.
*  There is no real 'delete' function in the FSL, but the FSL's 'open'
*  function can delete a file.
*
*  Parameters:
*  pFileName   - Fully qualified file name.
*
*  Return value:
*  ==0         - File has been removed.
*  ==-1        - An error has occured.
**************************************************************************************************
*/
int32_t esFSYS_premove(const char *pFileName)
{
    int32_t     res;

    fs_log_trace1("prm:%s", pFileName);
    res = EPDK_FAIL;
    fs_log_trace1("O:%d\n", res);

    return res;
}
/*
**************************************************************************************************
*
*             FS_Remove
*
*  Description:
*  API function. Remove a file.
*  There is no real 'delete' function in the FSL, but the FSL's 'open'
*  function can delete a file.
*
*  Parameters:
*  pFileName   - Fully qualified file name.
*
*  Return value:
*  ==0         - File has been removed.
*  ==-1        - An error has occured.
**************************************************************************************************
*/
int32_t esFSYS_prename(const char *newfilename, const char *oldfilename)
{
    int32_t     res;

    fs_log_trace1("prm:%s,%s", newfilename, oldfilename);
    res = EPDK_FAIL;
    fs_log_trace1("O%d\n", res);

    return res;
}

/*
***************************************************************************************************
*             FS_FError
*
*  Description:
*  API function. Return error status of a file.
*
*  Parameters:
*  pFile       - Pointer to a FS_FILE data structure.
*
*  Return value:
*  ==FS_ERR_OK - No error.
*  !=FS_ERR_OK - An error has occured.
**************************************************************************************************
*/

int32_t esFSYS_perror(__hdle hPart)
{
    int32_t     x;

    fs_log_trace1("perr:");
    x   = ((__fsys_part_t *)hPart)->error;
    fs_log_trace1("O\n");

    return x;
}

/*
**************************************************************************************************
*
*             esFClearErr
*
*  Description:
*  API function. Clear error status of a file.
*
*  Parameters:
*  pFile       - Pointer to a FS_FILE data structure.
*
*  Return value:
*  None.
**************************************************************************************************
*/
void esFSYS_perrclr(__hdle hPart)
{
    fs_log_trace1("peclr:");
    ((__fsys_part_t *)hPart)->error     = EPDK_OK;
    fs_log_trace1("O\n");
}

int32_t deactivate_part(__fsys_part_t *part, int32_t force)
{
    if (part->status != FSYS_PARTSTATUS_UNUSED && !force)
    {
        return EPDK_FAIL;
    }

    if (part->hDev)
    {
        if (esDEV_Close(part->hDev) == EPDK_FAIL)
        {
            if (!force)
            {
                fs_log_error("unmount part %c : dev cannot be closed!\n", part->letter);
                return EPDK_FAIL;
            }
            else
                fs_log_warning("unmount part %c: dev can not be closed, force unmount!\n");
        }
    }

    if (part->pPD->nUsr)
    {
        part->pPD->nUsr--;
    }

    part->hDev      = NULL;
    part->pPD       = NULL;
    part->status    = FSYS_PARTSTATUS_DEAD;

    free(part);

    return EPDK_OK;
}
