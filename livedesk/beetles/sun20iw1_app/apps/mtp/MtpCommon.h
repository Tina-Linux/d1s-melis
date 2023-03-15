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
#ifndef _MTPCOMMON_H
#define _MTPCOMMON_H
#include <time.h>
#include <stdio.h>
#include "elibs_stdio.h"
#include "stdlib.h"
#ifndef PATH_MAX
#define PATH_MAX    (512)
#endif

#undef MEM_DEBUG

void formatDateTime(ES_TIMESTAMP *Time, char *buffer, int bufferLength);
bool parseDateTime(const char *dateTime, ES_TIMESTAMP *Time);



typedef struct
{
    int num;
    void **object;
} Vector;

void VectorAdd(void *newObject, Vector *vector);
void VectorRemove(int index, Vector *vector);
void VectorDestroy(Vector *vector);
static inline size_t VectorSize(Vector *vector)
{
    return vector->num;
}
static inline void *VectorObject(int index, Vector *vector)
{
    return vector->object[index];
}

#define vector_for_each(head) \
    int Vector_local_i; \
    for (Vector_local_i = 0; Vector_local_i < head.num; Vector_local_i++)

#define vector_entry(head) \
    head.object[Vector_local_i]

#define VectorDestroyWithObject(vector, type, release_func) \
    do \
    { \
        vector_for_each(vector) { \
            type *object = vector_entry(vector); \
            release_func(object); \
        } \
        if (vector.object != NULL){ \
            esMEMS_Mfree(0, vector.object); \
            vector.object = NULL; \
        } \
    } while (0);


#define MTP_STRING_MAX_CHARACTER_NUMBER 255

struct MtpStringBuffer
{
    uint8_t mBuffer[MTP_STRING_MAX_CHARACTER_NUMBER * 3 + 1];
    int mCharCount;
    int mByteCount;
};


#ifdef MEM_DEBUG

void *malloc_wrapper(size_t size);
void *calloc_wrapper(size_t nmemb, size_t size);
void *realloc_wrapper(void *ptr, size_t size);
char *strdup_wrapper(const char *s);
void free_wrapper(void *ptr);
void memleak_print(void);
void memleak_exit(void);
int scandir_wrapper(const char *dirp, ES_DIRENT **namelist,
                    int (*filter)(const ES_DIRENT *),
                    int (*compar)(const ES_DIRENT **, const ES_DIRENT **));
#else
int scandir_wrapper(const char *dirp, ES_DIRENT **namelist,
                    int (*filter)(const ES_DIRENT *),
                    int (*compar)(const ES_DIRENT **, const ES_DIRENT **));
#define malloc_wrapper(size)                malloc(size)
#define calloc_wrapper(nmemb, size)         calloc(nmemb, size)
#define realloc_wrapper(ptr, size)          realloc(ptr, size)
#define strdup_wrapper(s)               strdup(s)
#define free_wrapper(ptr)               free(ptr)
#define memleak_print()
#define memleak_exit()

#endif
int melis_access(const char *path);
#endif
