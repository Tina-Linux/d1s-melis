#include <log.h>
#include <ktype.h>
#include <hal_dma.h>
#include <csp_dma_para.h>
/*
*********************************************************************************************************
*                                   REQUEST DMA CHANNEL
*
* Description: request dma channel;
*
* Arguments  : get dma_chan_source[i] address from hal 
*
* Returns    : dma handle
*
* Note       :
*********************************************************************************************************
*/
void* esDMA_Request(void)
{
    struct sunxi_dma_chan *dma_chan = 0;

    if(0 == hal_dma_chan_request(&dma_chan))
    {
        __err("dma request failed!\r\n");
        dma_chan = 0;
    }
    return (void*)dma_chan;
}


/*
*********************************************************************************************************
*                                   RELEASE DMA CHANNEL
*
* Description: release dma channel;
*
* Arguments  : dma      dma handle;
*
* Returns    : result,  0 OK/ -1 ERROR/ -2 INVALID PARAMETER;
*
* Note       :
*********************************************************************************************************
*/
int32_t esDMA_Release(void* dma)
{
    return (int32_t)hal_dma_chan_free((struct sunxi_dma_chan *)dma);;
}


/*
*********************************************************************************************************
*                                   CONFIG DMA
*
* Description: config dma;
*
* Arguments  : dma      dma handle;
*              arg      argument for config dma;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
extern void *rt_memcpy(void *dst, const void *src, unsigned long count);
int32_t esDMA_Setting(void* dma, void *arg)
{
    csp_dma_slave_config *config = (csp_dma_slave_config *)arg;
    struct dma_slave_config hal_config = {0};

    rt_memcpy((void*)&hal_config, (void*)config, sizeof(struct dma_slave_config));
    hal_dma_slave_config((struct sunxi_dma_chan *)dma, &hal_config);

	if((config->cyclic) && ((CSP_DMA_MEM_TO_DEV == config->direction) || (CSP_DMA_DEV_TO_MEM == config->direction) || (CSP_DMA_DEV_TO_DEV == config->direction)))
	{
		return (int32_t)hal_dma_prep_cyclic((struct sunxi_dma_chan *)dma, config->dst_addr, config->total_bytes, config->period_bytes, config->direction);
	}
	else if(CSP_DMA_MEM_TO_MEM == config->direction)
	{
		return (int32_t)hal_dma_prep_memcpy((struct sunxi_dma_chan *)dma, config->dst_addr, config->src_addr, config->total_bytes);
	}
	else if((CSP_DMA_MEM_TO_DEV == config->direction) || (CSP_DMA_DEV_TO_MEM == config->direction) || (CSP_DMA_DEV_TO_DEV == config->direction))
	{
		return (int32_t)hal_dma_prep_device((struct sunxi_dma_chan *)dma, config->dst_addr, config->src_addr, config->total_bytes, config->direction);
	}
	else
	{
		__err("dma para error cyclic: %d, direction: %d\r\n", config->cyclic, config->direction);
	}
    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                   DMA TRANSFER START
*
* Description: dma transfer start;
*
* Arguments  : dma      dma handle;
*              saddr    source address;
*              daddr    destination address;
*              len      data size;
*
* Returns    : resut;
*
* Note       :
*********************************************************************************************************
*/
int32_t esDMA_Start(void* dma)
{
    
    return (int32_t)hal_dma_start((struct sunxi_dma_chan *)dma);
}


/*
*********************************************************************************************************
*                                   STOP DMA TRANSFER
*
* Description: stop dam transfer;
*
* Arguments  : dma      dma handle;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esDMA_Stop(void* dma)
{
    return (int32_t)hal_dma_stop((struct sunxi_dma_chan *)dma);
}


/*
*********************************************************************************************************
*                                   RESTART DMA TRANSFER
*
* Description: restart dam transfer;
*
* Arguments  : dma      dma handle;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esDMA_Restart(void* dma)
{
    __err("not support syscall!");
    return 0;
}


/*
*********************************************************************************************************
*                                   QUERRY DMA STATUS
*
* Description: querry dma status;
*
* Arguments  : dma      dma handle;
*
* Returns    : result, dma status;
*
* Note       :
*********************************************************************************************************
*/
csp_dma_status esDMA_QueryStat(void* dma)
{
	uint32_t left_count = 0;
    return (csp_dma_status)hal_dma_tx_status((struct sunxi_dma_chan *)dma, &left_count);
}


/*
*********************************************************************************************************
*                                   QUERRY SOURCE ADDRESS
*
* Description: querry source address;
*
* Arguments  : dma      dma handle;
*
* Returns    : result, source address;
*
* Note       :
*********************************************************************************************************
*/
unsigned long esDMA_QuerySrc(void* dma)
{
    csp_dma_slave_config config = ((csp_dma_chan *)dma)->cfg;
    return config.src_addr;
}


/*
*********************************************************************************************************
*                                   QUERRY DESTINATION ADDRESS
*
* Description: querry destination address;
*
* Arguments  : dma      dma handle;
*
* Returns    : result, destination address;
*
* Note       :
*********************************************************************************************************
*/
unsigned long esDMA_QueryDst(void* dma)
{
    csp_dma_slave_config config = ((csp_dma_chan *)dma)->cfg;
    return config.dst_addr;
}


/*
*********************************************************************************************************
*                                   ENABLE DMA INTERRUPT
*
* Description: enable dma interrupt;
*
* Arguments  : dma      dma handle;
*              type     dma interrupt type;
*              mode     dma interrupt mode;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esDMA_EnableINT(void* dma, int32_t type)
{
    return 0;
}


/*
*********************************************************************************************************
*                                   DISABLE DMA INTERRUPT
*
* Description: disable dma interrupt;
*
* Arguments  : dma      dma handle;
*              type     dma interrupt type;
*              mode     dma interrupt mode;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esDMA_DisableINT(void* dma, int32_t type)
{
    return 0;
}


/*
*********************************************************************************************************
*                                   QUERRY DMA REST COUNT
*
* Description: querry dma rest count;
*
* Arguments  : dma      dma handle;
*
* Returns    : result, dma rest count;
*
* Note       :
*********************************************************************************************************
*/
unsigned long esDMA_QueryRestCount(void* dma)
{
    uint32_t left_count = 0;
    csp_dma_status status = hal_dma_tx_status((struct sunxi_dma_chan *)dma, &left_count);
    return left_count;
}


/*
*********************************************************************************************************
*                                   CHANGE DMA WORK MODE
*
* Description: change dma work mode;
*
* Arguments  : dma      dma handle;
*              mode     dma work mode;
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esDMA_ChangeMode(void* dma, int32_t mode)
{
    return 0;
}


/*
*********************************************************************************************************
*                                   REGISTER DMA HANDLER
*
* Description: register dma handler.
*
* Arguments  : dma      dma handle;
*              type     dma hander type;
*              hdler    handler for dma call-back;
*              arg      argument for dma handler;
*
* Returns    : result
*                   EPDK_OK,    register dma handler successed;
*                   EPDK_FAIL,  register dma handler failed;
*
* Note       :
*********************************************************************************************************
*/
int32_t esDMA_RegDmaHdler(void* dma, void* hdler, void *arg)
{
	hal_dma_callback_install((struct sunxi_dma_chan *)dma, hdler, arg);
    return 0;
}


/*
*********************************************************************************************************
*                                   UNREGISTER DMA HANDLER
*
* Description: unregister dma handler.
*
* Arguments  : dma      dma handle;
*              type     dma hander type;
*              hdler    dma call-back handler;
*
* Returns    : result
*                   EPDK_OK,    register dma handler successed;
*                   EPDK_FAIL,  register dma handler failed;
*
* Note       :
*********************************************************************************************************
*/
int32_t esDMA_UnregDmaHdler(void* dma, int32_t type, __pCBK_t hdler)
{
	//hal_dma_callback_install((struct sunxi_dma_chan *)dma, NULL, NULL);//NULL is invalid parameter defined by hal layer
    return 0;
}


/*
*********************************************************************************************************
*                                   SHOW DMA DISTRIBUTE INFORMATION
*
* Description: show dma distribute information;
*
* Arguments  : none
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void esDMA_Information(void)
{
    __err("not support syscall!");
    return;
}

