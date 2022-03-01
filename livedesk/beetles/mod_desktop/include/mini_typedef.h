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
