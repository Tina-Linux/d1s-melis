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
#include "sys_service.h"
#include <stdint.h>
#include <log.h>
#include <port.h>
#include <kapi.h>

static __sys_resource_man_t ResMan;


/*
*********************************************************************************************************
*                                       RESOURCE RELEASE
*
* Description: release resource.
*
* Arguments  : hRes -handler of resource, return by esSVC_ResourceReq.
*
* Returns    : EPDK_OK      -release resource succeeded,
*              EPDK_FAIL    -release resource failed.
*
* Note       :
*********************************************************************************************************
*/
int32_t esSVC_ResourceRel(void* hRes)
{
    __sys_resource_t *pRes;
    uint8_t  err;

    if (hRes == NULL)
    {
        __log("invalid resource handle for release");
        return EPDK_FAIL;
    }
    pRes = (__sys_resource_t *)hRes;

    // post resource flag
    esKRNL_FlagPost(ResMan.FlagsGrp, pRes->resource, OS_FLAG_SET, &err);
    if (err != 0)
    {
        __log("post resource [%x] flag failed, [err = %d].", pRes->resource, err);
        return EPDK_FAIL;
    }

    // k_free resource structure
    k_free((void *)pRes);

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       RESOURCE REQUEST
*
* Description: request resource.
*
* Arguments  :  res     -the request resource type, RESOURCE_VE_HW
*               mode    -the work mode of request.
*               timeout -timeout that your task will wait for resource.
*
* Returns    : the handle of request resource, NULL if request failed.
*
* Note       :
*********************************************************************************************************
*/
void* esSVC_ResourceReq(uint32_t res, uint32_t mode, uint32_t timeout)
{
    __sys_resource_t *pRes;
    uint8_t  err;

    // allocate memory for resource structure
    pRes = (__sys_resource_t *)k_malloc(sizeof(__sys_resource_t));
    if (pRes == NULL)
    {
        __log("allocate memory for resource structure failed");
        return NULL;
    }

    if (mode == RESOURCE_REQ_MODE_WAIT)
    {
        // pend resource flags
        esKRNL_FlagPend(ResMan.FlagsGrp,
                        res,
                        ((OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME) << 16) + timeout,
                        &err);
        if (err != 0)
        {
            __log("accept resource flag failed, [err = %d]", err);
            k_free(pRes);
            return NULL;
        }
    }
    else if (mode == RESOURCE_REQ_MODE_NWAIT)
    {
        // try to accept resource flags, in no block mode.
        esKRNL_FlagAccept(ResMan.FlagsGrp,
                          res,
                          (OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME),
                          &err);
        if (err != 0)
        {
            __log("accept resource flag failed, [err = %d]", err);
            k_free((void *)pRes);
            return NULL;
        }
    }
    else
    {
        __log("invalid work mode [%d] for resource request");
        k_free((void *)pRes);
        return NULL;
    }

    // record resource to pRes structure
    pRes->resource = res;

    return (void*)pRes;
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
int32_t SVC_ResourceManInit(void)
{
    uint8_t err;

    // create flag group for resource management,
    // all resource initialize as useable.
    ResMan.FlagsGrp = esKRNL_FlagCreate(0xFFFFFFFF, &err);
    if (ResMan.FlagsGrp == NULL)
    {
        __log("create flag for resource management failed.");
        return EPDK_FAIL;
    }
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
int32_t SVC_ResourceManExit(void)
{
    uint8_t err;

    if (ResMan.FlagsGrp == NULL)
    {
        __log("resource management not initialized");
        return EPDK_FAIL;
    }

    // delele flag group for resource management.
    esKRNL_FlagDel(ResMan.FlagsGrp, 0, &err);
    if (err != 0)
    {
        __wrn("del resource management flag failed, [err = %d].", err);
        return EPDK_FAIL;
    }
    return EPDK_OK;
}

void SVC_default(void)
{
    __wrn("service not support!");
}

int32_t svc_init(void)
{
    // initialize resource management sevice
    if (SVC_ResourceManInit() != EPDK_OK)
    {
        return EPDK_FAIL;
    }

#if SVC_REG_EN
    // initialize register sevice
    if (SVC_RegInit() != EPDK_OK)
    {
        SVC_ResourceManExit();
        return EPDK_FAIL;
    }
#endif

    return EPDK_OK;
}

int32_t svc_exit(void)
{
#if SVC_REG_EN
    // exit register sevice
    if (SVC_RegExit() == EPDK_FAIL)
    {
        return EPDK_FAIL;
    }
#endif

    // exit resource management sevice
    SVC_ResourceManExit();

    return EPDK_OK;
}