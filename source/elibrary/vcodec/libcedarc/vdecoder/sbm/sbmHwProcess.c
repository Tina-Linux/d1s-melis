
/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : sbm.c
* Description :This is the stream buffer module. The SBM provides
*              methods for managing the stream data before decode.
* History :
*   Author  : xyliu <xyliu@allwinnertech.com>
*   Date    : 2016/04/13
*   Comment :
*
*
*/

#include <stdlib.h>
#include<string.h>
#include <pthread.h>
#include "sbm.h"

//#include "secureMemoryAdapter.h"

#include "cdc_log.h"

volatile u32 dwVal;

//* should include other  '*.h'  before CdcMalloc.h
//#include "CdcMalloc.h"

#define SYS_WriteDWord(uAddr, dwVal) \
do{*(volatile u32 *)(uAddr) = (dwVal);}while(0)

#define SYS_ReadDWord(uAddr) \
    dwVal = (*(volatile u32 *)(uAddr));

#define REG_FUNC_CTRL               0x20
#define REG_VLD_BITSTREAM_ADDR      0x30
#define REG_VLD_OFFSET              0x34
#define REG_VLD_BIT_LENGTH          0x38
#define REG_VLD_END_ADDR            0x3c
#define REG_TRIGGER_TYPE            0x24
#define REG_FUNC_STATUS             0x28
#define REG_STCD_OFFSET             0xf0

#define STARTCODE_DETECT_E          (1<<25)
#define EPTB_DETECTION_BY_PASS      (1<<24)
#define INIT_SWDEC                  7
#define START_CODE_DETECT           12
#define START_CODE_TERMINATE        13
#define GET_BITS                    2
#define REG_BASIC_BITS_RETURN_DATA  0xdc


#define HEVC_BITS_BASE_ADDR_REG     0x40
#define HEVC_BITS_OFFSET_REG        0x44
#define HEVC_BITS_LEN_REG           0x48
#define HEVC_BITS_END_ADDR_REG      0x4C
#define HEVC_TRIGGER_TYPE_REG       0x34
#define HEVC_FUNCTION_STATUS_REG    0x38
#define HEVC_FUNC_CTRL_REG          0x30
#define HEVC_FUNCTION_STATUS_REG    0x38
#define HEVC_BITS_RETDATA_REG       0xDC
#define HEVC_STCD_HW_BITOFFSET_REG  0xF0



int SbmAvcHwSearchStartCode(SbmFrame *pSbm,char* sbmDataPtr, int sbmDataLen, \
                                int* nAfterStartCodeIdx, unsigned char* buffer)
{
     //int i=0;
     int ret = 0;
	 int nBitLen = 0;
	 //char* pNewDataPtr = NULL;
	 //char dataBuffer[32];
	 unsigned int nBitOffset = 0;
	 unsigned int  highPhyAddr = 0;
	 unsigned int lowPhyAddr = 0;
	 unsigned int nNewBitOffset = 0;
	 unsigned int nValue = 0;
     size_addr SbmBufStartPhyAddr = 0;
	 size_addr sbmBufEndPhyAddr = 0;
	 size_addr startPhyAddr = 0;
	 size_addr nRegisterBaseAddr = 0;

	 if(sbmDataLen <= 6)
	 {
	 	return -1;
	 }

	 nRegisterBaseAddr = (size_addr)CdcVeGetGroupRegAddr(pSbm->mConfig.veOpsS,
                                                         pSbm->mConfig.pVeOpsSelf,
                                                         REG_GROUP_H264_DECODER);


	//enable ve
	CdcVeLock(pSbm->mConfig.veOpsS, pSbm->mConfig.pVeOpsSelf);
	CdcVeEnableVe(pSbm->mConfig.veOpsS, pSbm->mConfig.pVeOpsSelf);

	startPhyAddr = (size_addr)CdcMemGetPhysicAddress(pSbm->mConfig.memops, pSbm->pStreamBuffer);
	startPhyAddr &= 0xffffffff;

	sbmBufEndPhyAddr = (size_t)startPhyAddr+pSbm->nStreamBufferSize-1;
	highPhyAddr = (startPhyAddr>>28) & 0x0f;
	lowPhyAddr =  startPhyAddr & 0x0ffffff0;

	SbmBufStartPhyAddr = lowPhyAddr+highPhyAddr;

	nBitOffset = (unsigned int )(sbmDataPtr-pSbm->pStreamBuffer)<<3;
	nBitLen =   sbmDataLen<<3;

    SYS_ReadDWord(nRegisterBaseAddr+REG_FUNC_CTRL);
    dwVal &= ~STARTCODE_DETECT_E;
    dwVal |= EPTB_DETECTION_BY_PASS;
    SYS_WriteDWord(nRegisterBaseAddr+REG_FUNC_CTRL, dwVal);

    SYS_WriteDWord(nRegisterBaseAddr+REG_VLD_OFFSET,nBitOffset);
    SYS_WriteDWord(nRegisterBaseAddr+REG_VLD_BIT_LENGTH, nBitLen);

    SYS_WriteDWord(nRegisterBaseAddr+REG_VLD_END_ADDR,sbmBufEndPhyAddr);
    SYS_WriteDWord(nRegisterBaseAddr+REG_VLD_BITSTREAM_ADDR,(7<<28)|SbmBufStartPhyAddr);

   	SYS_ReadDWord(nRegisterBaseAddr+REG_FUNC_CTRL);
    dwVal |= 7;
    SYS_WriteDWord(nRegisterBaseAddr+REG_FUNC_CTRL, dwVal);

    SYS_WriteDWord(nRegisterBaseAddr+REG_TRIGGER_TYPE, INIT_SWDEC);
    SYS_WriteDWord(nRegisterBaseAddr+REG_TRIGGER_TYPE, START_CODE_DETECT);

	ret = CdcVeWaitInterrupt(pSbm->mConfig.veOpsS,pSbm->mConfig.pVeOpsSelf);

	if(ret < 0)
	{
		logd("cannot find the startcode\n");
	}
	else
	{
		SYS_ReadDWord(nRegisterBaseAddr+REG_FUNC_STATUS);
		if((dwVal&0x01) > 0)
		{
		   logv("find the startcode\n");
		}
    	SYS_WriteDWord(nRegisterBaseAddr+REG_FUNC_STATUS,7);
		nNewBitOffset = SYS_ReadDWord(nRegisterBaseAddr+REG_STCD_OFFSET);

		if((nNewBitOffset-nBitOffset) >= 8*sbmDataLen)
		{
			ret = -1;
		}
		else
		{
		    ret = 0;
			SYS_WriteDWord(nRegisterBaseAddr+REG_TRIGGER_TYPE, (32<<8)|GET_BITS);
			nValue = SYS_ReadDWord(nRegisterBaseAddr+REG_BASIC_BITS_RETURN_DATA);
			buffer[0] = (nValue>>24)&0xff;
			buffer[1] = (nValue>>16)&0xff;
			buffer[2] = (nValue>>8)&0xff;
			buffer[3] = (nValue>>0)&0xff;
			*nAfterStartCodeIdx = (nNewBitOffset-nBitOffset)>>3;
        	//logd("*****xyliu: search startcode *nAfterStartCodeIdx=%d, sbmDataLen=%d\n", *nAfterStartCodeIdx, sbmDataLen);
            //logd("*****nValue=%x, buffer[0]=%x, buffer[1]=%x, buffer[2]=%x, buffer[3]=%x, buffer[4]=%x\n",
           //nValue, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
	#if 0
	      	pNewDataPtr = pSbm->pStreamBuffer+(nNewBitOffset>>3);
	      	pNewDataPtr -= 4;

		  	if(pNewDataPtr < sbmDataPtr)
		  	{
		  		pNewDataPtr = sbmDataPtr;
		  	}
			CdcMemRead(pSbm->mConfig.memops,dataBuffer,pNewDataPtr, 32);
			nValue = 0xffffffff;
			for(i=0; i<32; i++)
			{
				nValue <<= 8;
				nValue |= dataBuffer[i];
				if((nValue&0x00ffffff) == 0x000001)
				{
					break;
				}
			}
			if(i==32)
			{
				loge("xyliu:cannot find the startcode\n");
				ret = -1;
			}
			else
			{
				*nAfterStartCodeIdx = pNewDataPtr+i+1-sbmDataPtr;
		        CdcMemRead(pSbm->mConfig.memops, buffer,pNewDataPtr+i+1, 4);
				//logd("*****search startcode i=%d,*nAfterStartCodeIdx=%d, sbmDataLen=%d\n", i,*nAfterStartCodeIdx, sbmDataLen);
	            logd("*****buffer[0]=%x, buffer[1]=%x, buffer[2]=%x, buffer[3]=%x, buffer[4]=%x\n",
	            buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
			}
	#endif
		}
	}
	//disable ve
	CdcVeDisableVe(pSbm->mConfig.veOpsS,pSbm->mConfig.pVeOpsSelf);
    CdcVeUnLock(pSbm->mConfig.veOpsS, pSbm->mConfig.pVeOpsSelf);
	return ret;
}



int SbmHevcHwSearchStartCode(SbmFrame *pSbm,char* sbmDataPtr, int sbmDataLen, \
	                int* nAfterStartCodeIdx, unsigned char* buffer)
{
	 int ret = 0;
	 int nBitLen = 0;
	 unsigned int nBitOffset = 0;
	 unsigned int nNewBitOffset = 0;
     size_addr SbmBufStartPhyAddr = 0;
	 size_addr sbmBufEndPhyAddr = 0;
	 size_addr nRegisterBaseAddr = 0;

	 nRegisterBaseAddr = (size_addr)CdcVeGetGroupRegAddr(pSbm->mConfig.veOpsS,
                                                         pSbm->mConfig.pVeOpsSelf,
                                                         REG_GROUP_H265_DECODER);
	 //enable ve
	 CdcVeLock(pSbm->mConfig.veOpsS, pSbm->mConfig.pVeOpsSelf);
	 CdcVeEnableVe(pSbm->mConfig.veOpsS, pSbm->mConfig.pVeOpsSelf);

	SbmBufStartPhyAddr = (size_addr)CdcMemGetPhysicAddress(pSbm->mConfig.memops, pSbm->pStreamBuffer);
	sbmBufEndPhyAddr = (size_t)SbmBufStartPhyAddr+pSbm->nStreamBufferSize-1;

	SYS_WriteDWord(nRegisterBaseAddr+HEVC_BITS_END_ADDR_REG,sbmBufEndPhyAddr>>8);

	nBitOffset = (unsigned int )(sbmDataPtr-pSbm->pStreamBuffer)<<3;
	nBitLen =   sbmDataLen<<3;

	SYS_WriteDWord(nRegisterBaseAddr+HEVC_BITS_LEN_REG,nBitLen);

	SYS_WriteDWord(nRegisterBaseAddr+HEVC_BITS_OFFSET_REG,nBitOffset);

	SYS_WriteDWord(nRegisterBaseAddr+HEVC_BITS_BASE_ADDR_REG,(7<<28)|(SbmBufStartPhyAddr>>8));

	SYS_ReadDWord(nRegisterBaseAddr+HEVC_FUNC_CTRL_REG);
	dwVal &= ~STARTCODE_DETECT_E;
    dwVal |= EPTB_DETECTION_BY_PASS;
	dwVal |= 7;
    SYS_WriteDWord(nRegisterBaseAddr+HEVC_FUNC_CTRL_REG, dwVal);

	SYS_WriteDWord(nRegisterBaseAddr+HEVC_TRIGGER_TYPE_REG,INIT_SWDEC);
	SYS_WriteDWord(nRegisterBaseAddr+HEVC_TRIGGER_TYPE_REG,START_CODE_DETECT);

	ret = CdcVeWaitInterrupt(pSbm->mConfig.veOpsS,pSbm->mConfig.pVeOpsSelf);

	if(ret < 0)
	{
		logd("cannot find the startcode\n");
		ret = -1;
	}
	else
	{
		SYS_ReadDWord(nRegisterBaseAddr+HEVC_FUNCTION_STATUS_REG);
		if((dwVal&0x01) > 0)
		{
		   logv("find the startcode\n");
		}
		if(dwVal&0x02)
		{
			logv("search startcode failed interrupt\n");
		}

    	SYS_WriteDWord(nRegisterBaseAddr+HEVC_FUNCTION_STATUS_REG,7);
		nNewBitOffset = SYS_ReadDWord(nRegisterBaseAddr+HEVC_STCD_HW_BITOFFSET_REG);

		if((nNewBitOffset-nBitOffset) >= 8*sbmDataLen)
		{
			ret = -1;
		}
		else
		{
		    ret = 0;
			*nAfterStartCodeIdx = (nNewBitOffset-nBitOffset)>>3;
			CdcMemRead(pSbm->mConfig.memops, buffer, sbmDataPtr+*nAfterStartCodeIdx, 4);
		}
	}

	//disable ve
	CdcVeDisableVe(pSbm->mConfig.veOpsS,pSbm->mConfig.pVeOpsSelf);
	CdcVeUnLock(pSbm->mConfig.veOpsS, pSbm->mConfig.pVeOpsSelf);

    return ret;
}


