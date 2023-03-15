/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/include/isp_ae_data.h
 *
 * Copyright (c) 2007-2020 Allwinnertech Co., Ltd.
 *
 * Authors:  Yang Feng <yangfeng@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _ISP_AE_DATA_H_
#define _ISP_AE_DATA_H_

#define AE_TBL_LEN	1024

extern int Ae_Log2[256];
extern unsigned int Ae_DeltaLvTbl[AE_TBL_LEN];
extern unsigned char AeConverData[32][128];
extern int AeHistData[20][31];
extern short AeGammaPre[11][256];
extern unsigned int AeProbData[6][12];

#endif /*_ISP_AE_DATA_H_*/


