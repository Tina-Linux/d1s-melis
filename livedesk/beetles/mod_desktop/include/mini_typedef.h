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
#ifndef __MINI_TYPEDEF_H__
#define __MINI_TYPEDEF_H__

#include "../util/mini_allocator.h"
#include "../util/mini_allocator_default.h"

#if 0
#define MALLOC(s)      	malloc(s)
#define REALLOC(p, s)  	realloc(p, s)
#define ZALLOC(s)      	calloc(1, s)
#define FREE(p)        	if(p) {free(p); p = NULL;}
#define ZFREE(p, size) 	if(p) {memset((p), 0x00, (size)); free(p); p = NULL;}
#else
#define MALLOC(s)      	mini_allocator_alloc(mini_allocator_default_create(), s)
#define REALLOC(p, s)  	mini_allocator_realloc(mini_allocator_default_create(), p, s)
#define ZALLOC(s)      	mini_allocator_zalloc(mini_allocator_default_create(), s)
#define FREE(p)        	mini_allocator_free(mini_allocator_default_create(), p); p = NULL
#define ZFREE(p, size) 	mini_allocator_zfree(mini_allocator_default_create(), p, size); p = NULL
//#define FTK_STRDUP(p)      p != NULL ? ftk_strdup(p) : NULL
#endif

#define g_malloc    	MALLOC
#define g_free      	FREE
#define g_memset   	 	memset
#define g_strcpy    	strcpy
#define g_strncpy   	strncpy
#define g_strcmp    	strcmp
#define g_strncmp   	strncmp

#endif /* __MINI_TYPEDEF_H__ */
