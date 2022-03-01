#include "video_coder_i.h"

#include "mjpegDecode.h"
#include "JpgencLib.h"
#include "macc_bsp.h"

static const __u32 REG_GROUP_OFFSET_ARR[] = {0, 0x100, 0x200, 0x300, 0x400};

typedef enum VE_REGISTER_GROUP
{
    REG_GROUP_VETOP             = 0,
    REG_GROUP_MPEC_DECODER      = 1,
    REG_GROUP_H264_DECODER      = 2,
    REG_GROUP_VC1_DECODER       = 3,
    REG_GROUP_RV_DECODER        = 4
} ve_register_group_e;

static void *ve_get_reglist(ve_register_group_e reg_group_id)
{
    void *reglist;
    __u32   tmp;
    tmp = 0xf1c0e000;
    tmp += REG_GROUP_OFFSET_ARR[reg_group_id];
    reglist = (void *)tmp;
    return reglist;
}

static void clear_status_reg(void)
{
    volatile u32* ve_status_reg;
    __u32 baseAddr = MPEG_REGS_BASE;
    ve_status_reg = (u32 *)(baseAddr + 0x1c);
    *ve_status_reg |= 0xf;

    return;
}

__s32 SetJpegFormat(JpegDecoderCtx jpgctx)
{
    __s32 fmt;
    fmt = jpgctx->cur_comp_info[0].h_samp_factor << 20 |
          jpgctx->cur_comp_info[0].v_samp_factor << 16 |
          jpgctx->cur_comp_info[1].h_samp_factor << 12 |
          jpgctx->cur_comp_info[1].v_samp_factor <<  8 |
          jpgctx->cur_comp_info[2].h_samp_factor <<  4 |
          jpgctx->cur_comp_info[2].v_samp_factor;
    jpgctx->max_h_samp_factor = MAX(MAX(jpgctx->cur_comp_info[0].h_samp_factor,
                                        jpgctx->cur_comp_info[1].h_samp_factor),
                                    jpgctx->cur_comp_info[2].h_samp_factor);
    jpgctx->max_v_samp_factor = MAX(MAX(jpgctx->cur_comp_info[0].v_samp_factor,
                                        jpgctx->cur_comp_info[1].v_samp_factor),
                                    jpgctx->cur_comp_info[2].v_samp_factor);

    if (fmt == 0x211111)
    {
        jpgctx->JPEG_Format = JPEG422;
    }
    else if (fmt == 0x221111)
    {
        jpgctx->JPEG_Format = JPEG420;
    }
    else if (fmt == 0x121111)
    {
        jpgctx->JPEG_Format = JPEG422T;
    }
    else if (fmt == 0x411111)
    {
        jpgctx->JPEG_Format = JPEG411;
    }
    else if (fmt == 0x111111)
    {
        jpgctx->JPEG_Format = JPEG444;
    }
    else if (fmt == 0x110000)
    {
        jpgctx->JPEG_Format = JPEG400;
    }
    else
    {
        jpgctx->JPEG_Format = JPEGERR;
        return FALSE;
    }

    __msg("jpgctx->JPEG_Format=%d", jpgctx->JPEG_Format);
    return TRUE;
}

__s32 FillJpegHuffmanTable(JpegDecoderCtx jpgctx)
{
    __s32 i;
    JHUFF_TBL *pJpegHuff;
    jpeg_component_info     *compptr = 0;
    __u16 Huffman_Table_Sram[1024] = { 0 };

    if (!jpgctx->HasDht)
    {
#if 0

        if (!jpgctx->HasConfigDht)
        {
            //copy default dht to sramf
            int *ptr = (int *)DhtDefault;
            SYS_WriteDWord(MPEG_REG_SRAM_RW_OFFSET, 0);

            for (i = 0; i < 512; i++)
            {
                SYS_WriteDWord(MPEG_REG_SRAM_RW_DATA, *ptr++);
            }

            jpgctx->HasConfigDht = 1;
        }

#else
        __msg("FillJpegHuffmanTable: don't come here");
#endif
        return -1;//maybe goes here
    }

    for (i = 0; i < jpgctx->num_components; i++)
    {
        compptr = &jpgctx->cur_comp_info[i];

        if (compptr->component_id == jpgctx->seq_comp_id[0])
        {
            break;
        }
    }

    //Y DC
    pJpegHuff = &jpgctx->dc_huff_tbl[compptr->dc_tbl_no];//&mJpegHuffTable[0][dc_index[0]];

    for (i = 0; i < 16; i++)
    {
        Huffman_Table_Sram[i] = pJpegHuff->start_code[i];
    }

    for (i = 0; i < 16; i += 2)
    {
        Huffman_Table_Sram[16 + (i >> 1)] = (pJpegHuff->offset[i + 1] << 8) | pJpegHuff->offset[i];
    }

    for (i = 0; i < 8; i++)
    {
        Huffman_Table_Sram[24 + i] = 0;
    }

    for (i = 0; i < 256; i += 2)
    {
        Huffman_Table_Sram[512 + (i >> 1)] = (pJpegHuff->symbol[i + 1] << 8) | pJpegHuff->symbol[i];
    }

    //Y AC
    pJpegHuff = &jpgctx->ac_huff_tbl[compptr->ac_tbl_no];

    for (i = 0; i < 16; i++)
    {
        Huffman_Table_Sram[32 + i] = pJpegHuff->start_code[i];
    }

    for (i = 0; i < 16; i += 2)
    {
        Huffman_Table_Sram[32 + 16 + (i >> 1)] = (pJpegHuff->offset[i + 1] << 8) | pJpegHuff->offset[i];
    }

    for (i = 0; i < 8; i++)
    {
        Huffman_Table_Sram[32 + 24 + i] = 0;
    }

    for (i = 0; i < 256; i += 2)
    {
        Huffman_Table_Sram[640 + (i >> 1)] = (pJpegHuff->symbol[i + 1] << 8) | pJpegHuff->symbol[i];
    }

    for (i = 0; i < jpgctx->num_components; i++)
    {
        compptr = &jpgctx->cur_comp_info[i];

        if (compptr->component_id == jpgctx->seq_comp_id[1])
        {
            break;
        }
    }

    //C DC
    pJpegHuff = &jpgctx->dc_huff_tbl[compptr->dc_tbl_no];

    for (i = 0; i < 16; i++)
    {
        Huffman_Table_Sram[64 + i] = pJpegHuff->start_code[i];
    }

    for (i = 0; i < 16; i += 2)
    {
        Huffman_Table_Sram[64 + 16 + (i >> 1)] = (pJpegHuff->offset[i + 1] << 8) | pJpegHuff->offset[i];
    }

    for (i = 0; i < 8; i++)
    {
        Huffman_Table_Sram[64 + 24 + i] = 0;
    }

    for (i = 0; i < 256; i += 2)
    {
        Huffman_Table_Sram[768 + (i >> 1)] = (pJpegHuff->symbol[i + 1] << 8) | pJpegHuff->symbol[i];
    }

    //C AC
    pJpegHuff = &jpgctx->ac_huff_tbl[compptr->ac_tbl_no];

    for (i = 0; i < 16; i++)
    {
        Huffman_Table_Sram[96 + i] = pJpegHuff->start_code[i];
    }

    for (i = 0; i < 16; i += 2)
    {
        Huffman_Table_Sram[96 + 16 + (i >> 1)] = (pJpegHuff->offset[i + 1] << 8) | pJpegHuff->offset[i];
    }

    for (i = 0; i < 8; i++)
    {
        Huffman_Table_Sram[96 + 24 + i] = 0;
    }

    for (i = 0; i < 256; i += 2)
    {
        Huffman_Table_Sram[896 + (i >> 1)] = (pJpegHuff->symbol[i + 1] << 8) | pJpegHuff->symbol[i];
    }

    for (i = 128; i < 512; i++)
    {
        Huffman_Table_Sram[i] = 0;
    }

    {
        int *ptr = (int *)Huffman_Table_Sram;
        SYS_WriteDWord(MPEG_REG_SRAM_RW_OFFSET, 0);

        for (i = 0; i < 512; i++)
        {
            SYS_WriteDWord(MPEG_REG_SRAM_RW_DATA, *ptr++);
        }
    }

    return 0;
}

static void MjpegPrintRegister(void)
{

    __u32 i = 0;
    __u32* reg_base = NULL;

#if 1

    reg_base = (__u32*)0x01c0e000;
        __log("**************top register:0x%llx\n",reg_base);


    for(i=0;i<16;i++)
    {
        eLIBs_printf("%02x, 0x%08x 0x%08x 0x%08x 0x%08x \n",
             i, reg_base[4*i],reg_base[4*i+1],reg_base[4*i+2],reg_base[4*i+3]);
    }
    eLIBs_printf("\n");
#endif


    reg_base = (__u32*)(0x01c0e100);
        eLIBs_printf(" REG_GROUP_MPEG_DECODER register:0x%llx ",reg_base);


    for(i=0;i<16;i++)
    {
        eLIBs_printf("%02x, 0x%08x 0x%08x 0x%08x 0x%08x \n",
                i, reg_base[4*i],reg_base[4*i+1], reg_base[4*i+2],reg_base[4*i+3]);
    }
    eLIBs_printf("  ");
    eLIBs_printf("  ");
}

__s32 InitJpegHw(JpegDecoderCtx jpgctx)
{
    int i;
    int input_data;
    __s32 YQuantTbl;
    __s32 CQuantTbl;
    __u32 tmpVal = 0;
    __u32 output_pix_format = 0;
    //* width and height with 32 pixels align.
    __u32 width16, height16;
    //* height with 64 pixels align.
    __u32 height64;
    __u32 ysize;
    __u32 csize;
    width16 = (jpgctx->width + 15) & ~15;
    height16 = (jpgctx->height + 15) & ~15;
    ysize = width16 * height16; //* for y.
    csize = ysize >> 1; //* for u and v together.
    jpgctx->nDispYLineStride = width16;
    jpgctx->nDispCLineStride = width16 >> 1;
    jpgctx->nDispCBufferSize = ysize >> 2;
    __wrn("width16:%d,height16:%d", width16, height16);
    __wrn("nDispYLineStride:%d,nDispCLineStride:%d", jpgctx->nDispYLineStride, jpgctx->nDispCLineStride);
    jpgctx->width = width16;
    jpgctx->height = height16;
    tmpVal = 3 | (3 << 4);  //YV12
    WriteReg(MACC_REG_OUTPUT_FORMAT_REG, tmpVal);  //EC
    //__log("MACC_REG_OUTPUT_FORMAT_REG:%x", ReadReg(MACC_REG_OUTPUT_FORMAT_REG));
    //* 000: 32*32 tile-based, 001: 128*32 tile-based, 010: YU12, 011: YV12, 100: NV12, 101: NU12
    output_pix_format = 3;
    tmpVal = (0 << 28) | (1 << 29) | (output_pix_format << 30) | (jpgctx->nDispCBufferSize << 0);
    WriteReg(MACC_REG_CHROMA_BUF_LEN0_REG, tmpVal); //E8
    __msg("MACC_REG_CHROMA_BUF_LEN0_REG:%x", ReadReg(MACC_REG_CHROMA_BUF_LEN0_REG));
    tmpVal = jpgctx->nDispCBufferSize;
    WriteReg(MACC_REG_PRI_CHROMA_BUF_LEN_REG, tmpVal); //C4
    //__log("MACC_REG_PRI_CHROMA_BUF_LEN_REG:%x", ReadReg(MACC_REG_PRI_CHROMA_BUF_LEN_REG));
    tmpVal = ((jpgctx->nDispCLineStride) << 16) | (jpgctx->nDispYLineStride);
    WriteReg(MACC_REG_PRI_BUF_LINE_STRIDE_REG, tmpVal); //C8
    __msg("MACC_REG_PRI_BUF_LINE_STRIDE_REG:%x", ReadReg(MACC_REG_PRI_BUF_LINE_STRIDE_REG));
    tmpVal = ((jpgctx->nDispCLineStride) << 16) | (jpgctx->nDispYLineStride);
    WriteReg(MACC_REG_SEC_BUF_LINE_STRIDE_REG, tmpVal); //CC
    __msg("MACC_REG_SEC_BUF_LINE_STRIDE_REG:%x", ReadReg(MACC_REG_SEC_BUF_LINE_STRIDE_REG));
    __msg("MPEG_REG_JPG_RESINT:%x", SYS_ReadDWord(MPEG_REG_JPG_RESINT));
    SYS_WriteDWord(MPEG_REG_JPG_RESINT, 0 | jpgctx->restart_interval);
   // __log("MPEG_REG_JPG_RESINT:%x", SYS_ReadDWord(MPEG_REG_JPG_RESINT));
    //set picture level video engine
    SYS_WriteByte(MPEG_REG_VE_TRIGR + 3, (jpgctx->JPEG_Format << 3) | 3);
    //__log("MPEG_REG_VE_TRIGR:%x", SYS_ReadDWord(MPEG_REG_VE_TRIGR));
    SYS_WriteByte(MPEG_REG_VE_CTL + 3, (0 << 3));
    __msg("MPEG_REG_VE_CTL:%x", SYS_ReadDWord(MPEG_REG_VE_CTL));
    YQuantTbl = jpgctx->cur_comp_info[0].quant_tbl_no;
    CQuantTbl = jpgctx->cur_comp_info[1].quant_tbl_no;

    for (i = 0; i < 64; i++)
    {
        input_data = (i << 8) | (1 << 14);
        input_data |= jpgctx->QTab[YQuantTbl][i];
        SYS_WriteDWord(MPEG_REG_QMINPUT, input_data);
        __msg("MPEG_REG_QMINPUT:%x", SYS_ReadDWord(MPEG_REG_QMINPUT));
    }

    for (i = 0; i < 64; i++)
    {
        input_data = (i << 8);
        input_data |= jpgctx->QTab[CQuantTbl][i];
        SYS_WriteDWord(MPEG_REG_QMINPUT, input_data);
        __msg("MPEG_REG_QMINPUT:%x", SYS_ReadDWord(MPEG_REG_QMINPUT));
    }
	//__log("[YG]jpgctx->yuv_buf:0x%x\n",jpgctx->yuv_buf);
	__wrn("[YG]ysize:0x%x,csize:0x%x\n",ysize,csize);

    /*set out YC data phy addr, palloc memory must be 1k align*/
    jpgctx->JpegLumaOutStartAddr = (__u32)(esMEMS_VA2PA((__u32)jpgctx->yuv_buf));
    jpgctx->JpegChroOutStartAddr = (jpgctx->JpegLumaOutStartAddr + ysize + 1023) & (~(1023));
   // jpgctx->JpegChroOutStartAddr &= 0x7fffffff;
   // jpgctx->JpegLumaOutStartAddr &= 0x7fffffff;
    jpgctx->y_buf = (__u32)(jpgctx->yuv_buf);
    jpgctx->u_buf = ((__u32)(jpgctx->y_buf) + ysize + 1023) & (~(1023));
    jpgctx->v_buf = ((__u32)(jpgctx->u_buf) + (csize >> 1));
	
    //__log("jpgctx->JpegLumaOutStartAddr=%x", jpgctx->JpegLumaOutStartAddr);
   // __log("jpgctx->JpegChroOutStartAddr=%x", jpgctx->JpegChroOutStartAddr);
    SYS_WriteDWord(MPEG_REG_JPG_YADDR, jpgctx->JpegLumaOutStartAddr);
    SYS_WriteDWord(MPEG_REG_JPG_CADDR, jpgctx->JpegChroOutStartAddr);
  //  __log("MPEG_REG_JPG_YADDR:%x", SYS_ReadDWord(MPEG_REG_JPG_YADDR));
    __msg("MPEG_REG_JPG_CADDR:%x", SYS_ReadDWord(MPEG_REG_JPG_CADDR));
    {
        __s32 mcuHsize, mcuVsize;
        mcuHsize = jpgctx->max_h_samp_factor << 3;
        mcuVsize = jpgctx->max_v_samp_factor << 3;
        jpgctx->mcu_width  = (__u16)((jpgctx->width  + mcuHsize - 1) / mcuHsize); //round up
        jpgctx->mcu_height = (__u16)((jpgctx->height + mcuVsize - 1) / mcuVsize); //round up
        //scaled_width =  (jpgctx->mcu_width  * mcuHsize) >> jpgctx->HorFactor;
        //scaled_height = (jpgctx->mcu_height * mcuVsize) >> jpgctx->VerFactor;
        __wrn("jpgctx->mcu_width:%d,jpgctx->mcu_height:%d", jpgctx->mcu_width, jpgctx->mcu_height);
    }
    input_data = (jpgctx->mcu_height - 1) << 16 | (jpgctx->mcu_width - 1);
    SYS_WriteDWord(MPEG_REG_JPG_MCU0, input_data);
    //__log("input_data:%x", input_data);
    __msg("MPEG_REG_JPG_MCU0:%x", SYS_ReadDWord(MPEG_REG_JPG_MCU0));
    input_data = (jpgctx->VerFactor << 2) | (jpgctx->HorFactor);
    input_data <<= 8;
    SYS_WriteDWord(MPEG_REG_ROTAT_CTL, input_data);
    //__log("input_data:%x", input_data);
    __msg("MPEG_REG_ROTAT_CTL:%x", SYS_ReadDWord(MPEG_REG_ROTAT_CTL));

    if (FillJpegHuffmanTable(jpgctx))
    {
        return EPDK_FALSE;
    }

    jpgctx->vbvInfo.vbv_buf_end_phy = ((__u32)esMEMS_VA2PA((__u32)jpgctx->vbvInfo.vbv_buf_end)) & 0xffffffff;
    jpgctx->vbvInfo.vbv_buf_start_phy = ((__u32)esMEMS_VA2PA((__u32)jpgctx->vbvInfo.vbv_buf_start)) & 0xffffffff;
    SYS_WriteDWord(MPEG_REG_VLD_END, jpgctx->vbvInfo.vbv_buf_end_phy);
   // __log("MPEG_REG_VLD_END:%x", SYS_ReadDWord(MPEG_REG_VLD_END));

    return EPDK_TRUE;
}
static void set_internal_intr_enable_bits(void)
{
    volatile u32* ve_intr_ctrl_reg;
    __u32 baseAddr = MPEG_REGS_BASE;
    /* estimate Which video format */
    ve_intr_ctrl_reg = (u32 *)(baseAddr + 0x14);
    *ve_intr_ctrl_reg |= 0x7c;

    return;
}

__s32 JpegHwDec(JpegDecoderCtx jpgctx)
{
    __s32 section_size;
    __s32 ret;
    __u32 input_data = 0;
    __wrn("jpgctx->JPEG_Format:%d", jpgctx->JPEG_Format);
	set_internal_intr_enable_bits();

    if ((jpgctx->JPEG_Format == JPEG422) || (jpgctx->JPEG_Format == JPEG444))
    {
        input_data = SYS_ReadDWord(MPEG_REG_VE_CTL);
      //  __log("input_data:%#x", input_data);
        //input_data = 1<<18 | 1<<8 | 8<<4;
        input_data |= 1 << 18;
        __msg("input_data:%#x", input_data);
        SYS_WriteDWord(MPEG_REG_VE_CTL, input_data);
       // __log("MPEG_REG_VE_CTL:%x", SYS_ReadDWord(MPEG_REG_VE_CTL));
    }
    else
    {
        input_data = SYS_ReadDWord(MPEG_REG_VE_CTL);
        //__log("input_data:%#x", input_data);
        //input_data = 0<<18 | 1<<8 | 8<<4;
        input_data &= ~(1 << 18);
        __msg("input_data:%#x", input_data);
        SYS_WriteDWord(MPEG_REG_VE_CTL, input_data);
        //__log("MPEG_REG_VE_CTL:%x", SYS_ReadDWord(MPEG_REG_VE_CTL));
    }

    SYS_WriteDWord(MPEG_REG_VLD_OFFSET, (jpgctx->vbvInfo.cur_read_addr - jpgctx->vbvInfo.vbv_buf_start) << 3);
    SYS_WriteDWord(MPEG_REG_VLD_LEN, jpgctx->vbvInfo.frame_size << 3);
    //__log("MPEG_REG_VLD_OFFSET:%x", SYS_ReadDWord(MPEG_REG_VLD_OFFSET));
    __msg("MPEG_REG_VLD_LEN:%x", SYS_ReadDWord(MPEG_REG_VLD_LEN));
    __msg("jpgctx->vbvInfo.cur_read_addr=%x", jpgctx->vbvInfo.cur_read_addr);
    __msg("jpgctx->vbvInfo.vbv_buf_start=%x", jpgctx->vbvInfo.vbv_buf_start);
    __msg("jpgctx->vbvInfo.frame_size=%d", jpgctx->vbvInfo.frame_size);
    SYS_WriteDWord(MPEG_REG_VLD_START,  1 << 30 |
                   1 << 29 |
                   1 << 28 |
                   jpgctx->vbvInfo.vbv_buf_start_phy);
   // __log("MPEG_REG_VLD_START:%x", SYS_ReadDWord(MPEG_REG_VLD_START));

	//MjpegPrintRegister();
    SYS_WriteByte(MPEG_REG_VE_TRIGR, 0x0e);
   // __log("MPEG_REG_VE_TRIGR:%x", SYS_ReadByte(MPEG_REG_VE_TRIGR));
    ret = MJPEG_WaitVeReady();
	
	//__log("[YG]ret:%d\n",ret);
    clear_status_reg();

    if (ret == -1)
    {
        return EPDK_FALSE;
    }

    return EPDK_TRUE;
}

__s32 JpegDecodeRaw(JpegDecoderCtx jpgctx)
{

    if (!SetJpegFormat(jpgctx))
    {
        __msg("SetJpegFormat fail");
        return PIC_DEC_ERR_VFMT_ERR;//DEC_ERROR;
    }

    if (!InitJpegHw(jpgctx))
    {
        __log("InitJpegHw fail");
        return PIC_DEC_ERR_COMMON;//DEC_ERROR;
    }

    if (!JpegHwDec(jpgctx))
    {
        __log("JpegHwDec fail");
        return PIC_DEC_ERR_COMMON;//DEC_ERROR;
    }

    return PIC_DEC_ERR_NONE;//DEC_OK;
}


