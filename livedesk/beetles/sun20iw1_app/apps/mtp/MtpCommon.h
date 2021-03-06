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
