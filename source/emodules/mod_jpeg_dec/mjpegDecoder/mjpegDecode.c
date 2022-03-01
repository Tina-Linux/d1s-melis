#include "video_coder_i.h"
#include "mjpegDecode.h"
#include "macc_bsp.h"
#include "elibs_cacheop.h"

static __hdle pSemVeRdy;
#ifdef readl
#undef readl
#endif
#define readl(addr)                     ( *((volatile __u32 *)(addr)))
#ifdef writel
#undef writel
#endif
#define writel(value, addr)             ((*((volatile __u32 *)(addr))) = (value))
__u32 g_ve_status = 0;


__s32 MJPEG_SendVeReady(void)
{
    OS_SEM_DATA   tmpSemDat;
    eLIBs_memset(&tmpSemDat, 0x00, sizeof(OS_SEM_DATA));

    if (pSemVeRdy == NULL)
    {
        __err("fatal error.");
        return -1;
    }

    //it's really the video engine finish interupt
    esKRNL_SemQuery(pSemVeRdy, &tmpSemDat);

    if (tmpSemDat.OSCnt == 0)
    {
        esKRNL_SemPost(pSemVeRdy);
    }
    else
    {
        __msg("happened some of interrupt, %d", tmpSemDat.OSCnt);
    }

    return 0;
}

__s32 MJPEG_WaitVeReady(void)
{
    __u8    err;

    if (pSemVeRdy == NULL)
    {
        return -1;
    }

    esKRNL_SemPend(pSemVeRdy, 500, &err);

    if (err != OS_NO_ERR)
    {
        __log("VE hasn't any response!");
        return -1;
    }

    return 0;
}
__s32 VideoEngineInterupt(int irq, void *para)
{
    unsigned long ve_int_status_reg;
    unsigned long ve_int_ctrl_reg;
    unsigned int status = 0;
    volatile int val;
    int modual_sel;
    unsigned int interrupt_enable;
	__u32 base_address_macc = 0x01c0e000;//0x01c0e000
	
    modual_sel = readl(base_address_macc + 0);
   
    //jpgctx->ve_status = SYS_ReadDWord(base_address_macc+0);
   	//__wrn("----ve_status:0x%x-----\n",jpgctx->ve_status);
  //  __log("----modual_sel:0x%x----\n",modual_sel);
#ifdef CONFIG_SOC_SUN3IW1
    if (modual_sel&0xa) 
    {
        if ((modual_sel&0xb) == 0xb) 
        {
            /*jpg enc*/
            ve_int_status_reg = (unsigned long)(base_address_macc + 0xb00 + 0x1c);
            ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0xb00 + 0x14);
            interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0x7);
            status = readl((void *)ve_int_status_reg);
            status &= 0xf;
        } else {
            /*isp*/
            ve_int_status_reg = (unsigned long)(base_address_macc + 0xa00 + 0x10);
            ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0xa00 + 0x08);
            interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0x1);
            status = readl((void *)ve_int_status_reg);
            status &= 0x1;
        }

        if (status && interrupt_enable) 
        {
            /*disable interrupt*/
            if ((modual_sel & 0xb) == 0xb) 
            {
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0xb00 + 0x14);
                val = readl((void *)ve_int_ctrl_reg);
                writel(val & (~0x7), (void *)ve_int_ctrl_reg);
            } else{
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0xa00 + 0x08);
                val = readl((void *)ve_int_ctrl_reg);
                writel(val & (~0x1), (void *)ve_int_ctrl_reg);
            }
            g_ve_status = status;
            __wrn("jpgctx->ve_status:0x%x\n", status);

            MJPEG_SendVeReady();
        }
    }
#else
    if (modual_sel&(3<<6)) 
    {
        if (modual_sel&(1<<7)) 
        {
            /*avc enc*/
            ve_int_status_reg = (unsigned long)(base_address_macc + 0xb00 + 0x1c);
            ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0xb00 + 0x14);
            interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0x7);
            status = readl((void *)ve_int_status_reg);
            status &= 0xf;
        } else {
            /*isp*/
            ve_int_status_reg = (unsigned long)(base_address_macc + 0xa00 + 0x10);
            ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0xa00 + 0x08);
            interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0x1);
            status = readl((void *)ve_int_status_reg);
            status &= 0x1;
        }

        /*modify by fangning 2013-05-22*/
        if (status && interrupt_enable) 
        {
            /*disable interrupt*/
            /*avc enc*/
            if (modual_sel&(1<<7)) 
            {
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0xb00 + 0x14);
                val = readl((void *)ve_int_ctrl_reg);
                writel(val & (~0x7), (void *)ve_int_ctrl_reg);
            } else {
                /*isp*/
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0xa00 + 0x08);
                val = readl((void *)ve_int_ctrl_reg);
                writel(val & (~0x1), (void *)ve_int_ctrl_reg);
            }
            g_ve_status = status;
            __wrn("g_ve_status:0x%x\n", status);

            MJPEG_SendVeReady();

        }
    }
#endif

#if ((defined CONFIG_SOC_SUN8IW19P1))
    if (modual_sel&(0x20)) 
    {
        ve_int_status_reg = (unsigned long)(base_address_macc + 0xe00 + 0x1c);
        ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0xe00 + 0x14);
        interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0x38);

        status = readl((void *)ve_int_status_reg);

        if ((status&0x7) && interrupt_enable) 
        {
            /*disable interrupt*/
            val = readl((void *)ve_int_ctrl_reg);
            writel(val & (~0x38), (void *)ve_int_ctrl_reg);
            
            g_ve_status = status;
            __wrn("g_ve_status:0x%x\n", status);

            MJPEG_SendVeReady();

        }
    }
#endif

    modual_sel &= 0xf;
    if (modual_sel <= 4) 
    {
        //estimate Which video format.
        switch (modual_sel)
        {
            case 0: /*mpeg124*/
                ve_int_status_reg = (unsigned long)(base_address_macc + 0x100 + 0x1c);
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0x100 + 0x14);
                interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0x7c);
                break;

            case 1: /*h264*/
                ve_int_status_reg = (unsigned long)(base_address_macc + 0x200 + 0x28);
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0x200 + 0x20);
                interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0xf);
                break;

            case 2: /*vc1*/
                ve_int_status_reg = (unsigned long)(base_address_macc + 0x300 + 0x2c);
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0x300 + 0x24);
                interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0xf);
                break;

            case 3: /*rv*/
                ve_int_status_reg = (unsigned long)(base_address_macc + 0x400 + 0x1c);
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0x400 + 0x14);
                interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0xf);
                break;

            case 4: /*hevc*/
                ve_int_status_reg = (unsigned long)(base_address_macc + 0x500 + 0x38);
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0x500 + 0x30);
                interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0xf);
                break;

            default:
                ve_int_status_reg = (unsigned long)(base_address_macc + 0x100 + 0x1c);
                ve_int_ctrl_reg = (unsigned long)(base_address_macc + 0x100 + 0x14);
                interrupt_enable = readl((void *)ve_int_ctrl_reg) & (0xf);
                __wrn("ve mode :%x " "not defined!\n", modual_sel);
                break;
	    }

	    status = readl((void *)ve_int_status_reg);
        __wrn("modual_sel:0x%x,ve_int_status_reg:0x%x\n", modual_sel,ve_int_status_reg);
        __wrn("ve mode :%x,interrupt_enable:0x%x\n", modual_sel,interrupt_enable);

	    //disable interrupt
	    if ((status&0xf) && interrupt_enable)
	    {
	        if(modual_sel == 0) 
	        {
	            val = readl((void *)ve_int_ctrl_reg);
	            writel(val & (~0x7c), (void *)ve_int_ctrl_reg);
	        } else {
	            val = readl((void *)ve_int_ctrl_reg);
	            writel(val & (~0xf), (void *)ve_int_ctrl_reg);
	        }
            
            g_ve_status = status;
            
            __wrn("jpgctx->ve_status:0x%x\n", status);
            MJPEG_SendVeReady();
	    }
    }

    g_ve_status = status;
    __wrn("VideoEngineInterupt ret:0x%x\n", status);

    return 0;
}

void *MJPEG_init(void)
{
    struct JpegDecoderContex *pJPEG = NULL;
    pJPEG = (JpegDecoderCtx)esMEMS_Malloc(0, sizeof(JpegDecoderContex));

    if (!pJPEG)
    {
        __wrn("vdrv_jpeg MOpen create semaphore (pSemVeRdy) failed!");
        return NULL;
    }

    pSemVeRdy = esKRNL_SemCreate(0);

    if (!pSemVeRdy)
    {
        __wrn("vdrv_jpeg MOpen create semaphore (pSemVeRdy) failed!");
        return NULL;
    }

    return (void *)pJPEG;
}

__s32 MJPEG_exit(void *mjpegDecCtx)
{
    struct JpegDecoderContex *pJPEG = (struct JpegDecoderContex *)mjpegDecCtx;
    __u8 err;

    if (pSemVeRdy)
    {
        esKRNL_SemDel(pSemVeRdy, OS_DEL_ALWAYS, &err);
        pSemVeRdy = NULL;
    }

    if (pJPEG)
    {
        esMEMS_Mfree(0, pJPEG);
        pJPEG = NULL;
    }

    return 0;
}


__s32 MJPEG_decode(void *mjpegDecCtx, void *mjpegFrameInfo)
{
    struct JpegDecoderContex *pJPEG = (struct JpegDecoderContex *)mjpegDecCtx;
    struct JpegFrameInfo__  *pINFO = (struct JpegFrameInfo__ *)mjpegFrameInfo;
    __s32 ret = -1;
    __u32 sc_y_buf, sc_c_buf;
    __wrn("data:%x,data_len:%d,jpg_w:%d,jpg_h:%d,buf:%x",pINFO->jpegData,pINFO->jpegData_len,pINFO->pic_width,pINFO->pic_height,pINFO->yuv_buf);
    pJPEG->vbvInfo.vbv_buf_start = (__u8 *)(((__u32)pINFO->jpegData) & (~1023));
    pJPEG->vbvInfo.vbs_len = (((__u32)(pINFO->jpegData) & 1023) + pINFO->jpegData_len + 1023) & (~1023);
    pJPEG->vbvInfo.vbv_buf_end = (__u8 *)(pJPEG->vbvInfo.vbv_buf_start + pJPEG->vbvInfo.vbs_len - 1);
    pJPEG->vbvInfo.read_addr = pJPEG->vbvInfo.cur_read_addr = pINFO->jpegData;
    pJPEG->vbvInfo.frame_size = pJPEG->vbvInfo.vbs_len;
    __wrn("vbvInfo.read_addr=%x,vbvInfo.frame_size=%d", pJPEG->vbvInfo.read_addr, pJPEG->vbvInfo.frame_size);
    eLIBs_CleanDCacheRegion((void *)pINFO->jpegData, pINFO->jpegData_len);

    if (JpegDecoderMain(pJPEG, pINFO) == PIC_DEC_ERR_NONE)
    {
        //__log("decodec successful!");
        pINFO->pic_width = (pJPEG->width);
        pINFO->pic_height = (pJPEG->height);
        ret = 0;
    }

    return ret;
}

__s32 MJMACC_init(void *mjpegDecCtx, void *mjpegFrameInfo)
{
	__u64 veVersion;
    struct JpegDecoderContex *pJPEG = (struct JpegDecoderContex *)mjpegDecCtx;
    stJpegFrameInfo  *pINFO = (stJpegFrameInfo *)mjpegFrameInfo;

    if (MACC_Init(MACC_MODULE_MPEG_DEC) < 0)
    {
        return EPDK_FAIL;
    }

    //install video engine interupt isr
    esINT_InsISR(VE_IRQ_NUM, "VEngine", (__pISR_t)VideoEngineInterupt, NULL);
	//__log("irq num:%d\n",MACC_QueryINT(MACC_MODULE_MPEG_DEC, MACC_INT_VE_FINISH) >> 24);
    esINT_EnableINT(INTC_IRQNO_MACC);
	veVersion = MACC_GetMACCVersion();
    __wrn("veVersion:%llx", veVersion);
    eLIBs_memset(pJPEG, 0, sizeof(JpegDecoderContex));
    return EPDK_OK;
}

__s32 MJMACC_exit(void *mjpegDecCtx, void *mjpegFrameInfo)
{
    struct JpegDecoderContex *pJPEG = (struct JpegDecoderContex *)mjpegDecCtx;
    stJpegFrameInfo  *pINFO = (stJpegFrameInfo *)mjpegFrameInfo;
	esINT_DisableINT(VE_IRQ_NUM);
	esINT_UniISR(VE_IRQ_NUM, NULL);	
	MACC_Exit(MACC_MODULE_MPEG_DEC);
	__wrn("free size = 0x%x\n",esMEMS_FreeMemSize());

    eLIBs_memset(pJPEG, 0, sizeof(JpegDecoderContex));
    return EPDK_OK;
}
