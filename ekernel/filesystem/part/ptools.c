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