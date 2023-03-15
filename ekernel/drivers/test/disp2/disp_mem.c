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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <rtthread.h>
#include <log.h>
#include <hal_mem.h>
#include <video/sunxi_display2.h>


struct info_mm {
	void *info_base;	/* Virtual address */
	u32 mem_start;	/* Start of frame buffer mem */
	/* (physical address) */
	u32 mem_len;		/* Length of frame buffer mem */
};

static struct info_mm g_disp_mm[10];
static int g_disp_mem_id = -1;

#define DISP_TEST_BYTE_ALIGN(x) (((x + (4*1024-1)) >> 12) << 12)

static void *disp_malloc(u32 num_bytes, void *phys_addr)
{
	u32 actual_bytes;
	void *address = NULL;

	if (num_bytes != 0) {
		actual_bytes = DISP_TEST_BYTE_ALIGN(num_bytes);

		address = hal_malloc(actual_bytes);
		*(uint32_t *)phys_addr = __va_to_pa((uint32_t)address);
		if (address) {
		__wrn	
				("hal_malloc ok, phy addr=0x%p, vir addr=%p size=0x%x\n",
				 (void *)(*(unsigned long *)phys_addr), address, num_bytes);
			return address;
		}

		printf("dma_alloc_coherent fail, size=0x%lx\n", num_bytes);
		return NULL;
	}

	printf("%s size is zero\n", __func__);

	return NULL;
}

static void disp_free(void *virt_addr, void *phys_addr, u32 num_bytes)
{
	u32 actual_bytes;

	actual_bytes = DISP_TEST_BYTE_ALIGN(num_bytes);
	if (phys_addr && virt_addr)
		hal_free(virt_addr);
}

static int disp_mem_release(int sel)
{
	if (g_disp_mm[sel].info_base == NULL)
		return -1;

	__inf("disp_mem_release, mem_id=%d, phy_addr=0x%p\n", sel,
	      (void *)g_disp_mm[sel].mem_start);
	disp_free((void *)g_disp_mm[sel].info_base,
		  (void *)g_disp_mm[sel].mem_start, g_disp_mm[sel].mem_len);
	memset(&g_disp_mm[sel], 0, sizeof(struct info_mm));
	g_disp_mem_id = -1;
	return 0;
}

static int disp_mem_request(int sel, u32 size)
{

	uintptr_t phy_addr;

	if ((sel >= 10) ||
	    (g_disp_mm[sel].info_base != NULL)) {
		printf("invalid param\n");
		return -1;
	}

	g_disp_mm[sel].info_base = disp_malloc(size, (void *)&phy_addr);
	if (g_disp_mm[sel].info_base) {
		g_disp_mm[sel].mem_start = phy_addr;
		g_disp_mm[sel].mem_len = size;
		memset(g_disp_mm[sel].info_base, 0, size);
		__inf("pa=0x%p va=0x%p size:0x%x\n",
		      (void *)g_disp_mm[sel].mem_start,
		      g_disp_mm[sel].info_base, size);
		g_disp_mem_id = sel;

		return 0;
	}

	printf("disp_malloc fail!\n");
	return -1;
}


u32 disp_mem_getadr(u32 memid)
{
	if (memid < 10)
		return g_disp_mm[memid].mem_start;
	return 0;
}

int disp_mem(u32 mem_id, u32 width, u32 height, u32 clear_flag, char *filename)
{
	unsigned long arg[6];
	int phy_adr, i, n, fb_width, fb_height;
	int ret = -1;
	FILE *fh = NULL;
	void *mem = NULL;

	if(clear_flag) {
		/* release memory && clear layer */
		disp_mem_release(mem_id);
		return 0;
	}

	//for_test
	fh = fopen(filename, "r");
	if(!fh) {
		printf("open file %s fail. \n", filename);
		goto OUT;
	}

	fb_width = width;
	fb_height = height;

	ret = disp_mem_request(mem_id, fb_width*fb_height*4);
	if(ret) {
		printf("DISP_MEM_REQUEST 0\n");
		goto FILE_CLOSE;
	}

	phy_adr = g_disp_mm[mem_id].mem_start;
	mem = g_disp_mm[mem_id].info_base;
	memset((void*)mem, 0x0, fb_width*fb_height*4);
	if(fh != NULL) {
		fread((void *)mem, fb_width * fb_height * 4, 1, fh);
	}

FILE_CLOSE:
	fclose(fh);
OUT:
	return ret;
}

int disp_mem_clear(u32 mem_id)
{
	return disp_mem(mem_id, 0, 0, 1, NULL);
}
