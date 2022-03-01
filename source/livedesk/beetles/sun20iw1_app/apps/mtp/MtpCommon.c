#include "MtpCommon.h"
#include <time.h>
#include <string.h>
#include <elibs_string.h>

#ifdef MEM_DEBUG
#if 0
#define MEMLOG(fmt, arg...)               eLIBs_printf("<mem-debug>[%s:%u] "fmt"\n", __FUNCTION__, __LINE__, ##arg)
#else
#define MEMLOG(fmt, arg...)
#endif

typedef struct
{
    void *ptr;
    int32_t match_num;
} ptr_t;

typedef struct
{
    char name[32];
    uint32_t type;
    uint32_t count;
    ptr_t *ptr_s;
    uint32_t match;
} memleak_t;

enum
{
    MEMLEAK_MALLOC = 0,
    MEMLEAK_CALLOC,
    MEMLEAK_REALLOC,
    MEMLEAK_STRDUP,
    MEMLEAK_SCANDIR,
    MEMLEAK_FREE,
};

const memleak_t gMemLeak_init[] =
{
    {"malloc", MEMLEAK_MALLOC, 0, NULL, 0},
    {"calloc", MEMLEAK_CALLOC, 0, NULL, 0},
    {"realloc", MEMLEAK_REALLOC, 0, NULL, 0},
    {"strdup", MEMLEAK_STRDUP, 0, NULL, 0},
    {"scandir", MEMLEAK_SCANDIR, 0, NULL, 0},
    {"free", MEMLEAK_FREE, 0, NULL, 0},
};

memleak_t gMemLeak[] =
{
    {"malloc", MEMLEAK_MALLOC, 0, NULL, 0},
    {"calloc", MEMLEAK_CALLOC, 0, NULL, 0},
    {"realloc", MEMLEAK_REALLOC, 0, NULL, 0},
    {"strdup", MEMLEAK_STRDUP, 0, NULL, 0},
    {"scandir", MEMLEAK_SCANDIR, 0, NULL, 0},
    {"free", MEMLEAK_FREE, 0, NULL, 0},
};

static void memleak_add_count(void *ptr, uint32_t type)
{
    gMemLeak[type].count++;
    gMemLeak[type].ptr_s = realloc(gMemLeak[type].ptr_s, sizeof(ptr_t) * gMemLeak[type].count);
    gMemLeak[type].ptr_s[gMemLeak[type].count - 1].ptr = ptr;
    gMemLeak[type].ptr_s[gMemLeak[type].count - 1].match_num = -1;
}

void *malloc_wrapper(size_t size)
{
    void *ptr = NULL;
    ptr = malloc(size);
    memleak_add_count(ptr, MEMLEAK_MALLOC);
    return ptr;
}

void *calloc_wrapper(size_t nmemb, size_t size)
{
    void *ptr = NULL;

    ptr = calloc(nmemb, size);
    memleak_add_count(ptr, MEMLEAK_CALLOC);
    MEMLOG("--------calloc ptr:%p\n", ptr);
    return ptr;
}

void *realloc_wrapper(void *ptr, size_t size)
{
    void *p = NULL;
    uint32_t i;

    p = realloc(ptr, size);
    for (i = 0; i < gMemLeak[MEMLEAK_REALLOC].count; i++)
    {
        if (ptr == gMemLeak[MEMLEAK_REALLOC].ptr_s[i].ptr)
        {
            gMemLeak[MEMLEAK_REALLOC].ptr_s[i].ptr = p;
            MEMLOG("--------realloc ptr:%p(instead)\n", p);
            return p;
        }
    }
    memleak_add_count(p, MEMLEAK_REALLOC);
    MEMLOG("--------realloc ptr:%p\n", p);
    return p;
}

char *strdup_wrapper(const char *s)
{
    char *ptr = NULL;

    ptr = strdup(s);
    memleak_add_count(ptr, MEMLEAK_STRDUP);
    MEMLOG("--------strdup ptr:%p\n", ptr);
    return ptr;
}

int scandir_wrapper(const char *dirp, struct dirent ***namelist,
                    int (*filter)(const struct dirent *),
                    int (*compar)(const struct dirent **, const struct dirent **))
{
    struct dirent **ptr = NULL;
    int ret, i;

    ret = scandir(dirp, namelist, filter, compar);
    ptr = *namelist;
    if (ret == 0)
    {
        return ret;
    }
    for (i = 0; i < ret; i++)
    {
        MEMLOG("--------scandir ptr:%p\n", ptr[i]);
        memleak_add_count(ptr[i], MEMLEAK_SCANDIR);
    }
    memleak_add_count(ptr, MEMLEAK_SCANDIR);
    MEMLOG("--------scandir ptr:%p\n", ptr);
    return ret;
}

void free_wrapper(void *ptr)
{
    MEMLOG("--------free ptr:%p\n", ptr);
    memleak_add_count(ptr, MEMLEAK_FREE);
    return free(ptr);
}


static int memleak_match(uint32_t type)
{
    if (gMemLeak[type].count == gMemLeak[type].match)
    {
        return 0;
    }
    else
    {
        eLIBs_printf("Not match! type:%u, count:%u, match:%u\n", type, gMemLeak[type].count, gMemLeak[type].match);
        return -1;
    }
}

static void memleak_caculate(void)
{
    uint32_t i, j, k;
    int ret;

    memleak_t *free = &gMemLeak[MEMLEAK_FREE];
    for (i = 0; i < free->count; i++)
    {
        if (free->ptr_s[i].match_num >= 0)
        {
            continue;
        }
        for (j = MEMLEAK_MALLOC; j < MEMLEAK_FREE; j++)
        {
            memleak_t *alloc = &gMemLeak[j];
            for (k = 0; k < alloc->count; k++)
            {
                if (alloc->ptr_s[k].match_num >= 0)
                {
                    continue;
                }
                if (alloc->ptr_s[k].ptr == free->ptr_s[i].ptr)
                {
                    alloc->ptr_s[k].match_num = i;
                    free->ptr_s[i].match_num = k;
                    alloc->match++;
                    free->match++;
                }
            }
        }
    }
    ret = memleak_match(MEMLEAK_MALLOC);
    ret += memleak_match(MEMLEAK_CALLOC);
    ret += memleak_match(MEMLEAK_REALLOC);
    ret += memleak_match(MEMLEAK_STRDUP);
    ret += memleak_match(MEMLEAK_SCANDIR);
    ret += memleak_match(MEMLEAK_FREE);
    if (!ret)
    {
        eLIBs_printf("=========================\n");
        eLIBs_printf("=   alloc, free match   =\n");
        eLIBs_printf("=========================\n");
    }
    else
    {
        eLIBs_printf("=========================\n");
        eLIBs_printf("= alloc, free not match =\n");
        eLIBs_printf("=========================\n");
        exit(-1);
    }
}

void memleak_exit(void)
{
    uint32_t i;

    memleak_t *mem = &gMemLeak[MEMLEAK_FREE];
    for (i = MEMLEAK_MALLOC; i < MEMLEAK_FREE + 1; i++)
    {
        mem = &gMemLeak[i];
        if (mem->ptr_s != NULL)
        {
            free(mem->ptr_s);
            mem->ptr_s = NULL;
        }
    }
    memcpy(gMemLeak, gMemLeak_init, sizeof(gMemLeak_init));
}

void memleak_print(void)
{
    memleak_caculate();
#if 0
    eLIBs_printf("---------------\n");
    eLIBs_printf("malloc count:%u\n", gMemLeak[MEMLEAK_MALLOC].count);
    eLIBs_printf("calloc count:%u\n", calloc_count);
    eLIBs_printf("realloc count:%u\n", realloc_count);
    eLIBs_printf("strdup count:%u\n", strdup_count);
    eLIBs_printf("scandir count:%u\n", scandir_count);
    eLIBs_printf("free count:%u\n", free_count);
    eLIBs_printf("\n");
    eLIBs_printf("total malloc count:%u\n", malloc_count + calloc_count + realloc_count + strdup_count + scandir_count);
    eLIBs_printf("---------------\n");
#endif
}
#else
static int melis_scandir(const char *dirp, ES_DIRENT **namelist,
                    int (*filter)(const ES_DIRENT *),
                    int (*compar)(const ES_DIRENT **, const ES_DIRENT **))
{
	ES_DIR	*pCurDir = NULL;
	ES_DIRENT *pDirent = NULL;
	int count = 0, malloced = 0, ret = 0;
retry:
	count = 0;
	pCurDir = esFSYS_opendir(dirp);
	while (NULL != (pDirent = esFSYS_readdir(pCurDir)))
    {
    	ret = filter(pDirent);
    	if (ret == 1 && malloced == 0)
    	{
			count++;
		}
		else if (ret == 1 && malloced == 1)
		{
			eLIBs_memcpy(*namelist + count, pDirent, sizeof(ES_DIRENT));
			count++;
		}
	}
	esFSYS_closedir(pCurDir);
	if (malloced == 0)
	{
		*namelist = (ES_DIRENT *)esMEMS_Malloc(0, count*sizeof(ES_DIRENT));
		eLIBs_memset(*namelist, 0x00, count*sizeof(ES_DIRENT));
		malloced = 1;
		goto retry;
	}
	return count;
}
                    
int scandir_wrapper(const char *dirp, ES_DIRENT **namelist,
                    int (*filter)(const ES_DIRENT *),
                    int (*compar)(const ES_DIRENT **, const ES_DIRENT **))
{
    int ret = 0;

    ret = melis_scandir(dirp, namelist, filter, compar);
    if (ret == 0)
    {
        return ret;
    }
    return ret;
}

#endif

void formatDateTime(ES_TIMESTAMP *Time, char *buffer, int bufferLength)
{
    eLIBs_snprintf(buffer, bufferLength, "%04d%02d%02dT%02d%02d%02d",
             Time->year,
             Time->month,
             Time->day, Time->hour, Time->minute, Time->second);
    return ;
}

bool parseDateTime(const char *dateTime, ES_TIMESTAMP *Time)
{
#if 0
    int year, month, day, hour, minute, second;
    struct tm tm;
    const char *tail = NULL;
    time_t dummy;
    bool useUTC = false;

//    if (sscanf(dateTime, "%04d%02d%02dT%02d%02d%02d",
//               &year, &month, &day, &hour, &minute, &second) != 6)
//    {
//        return false;
//    }
    tail = dateTime + 15;
    if (tail[0] == '.' && tail[1])
    {
        tail += 2;
    }

    useUTC = (tail[0] == 'Z');
//    localtime_r(&dummy, &tm);

    tm.tm_sec = second;
    tm.tm_min = minute;
    tm.tm_hour = hour;
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;
    tm.tm_wday = 0;
    tm.tm_isdst = -1;
    if (useUTC)
    {
//        *outSeconds = mktime(&tm);
    }
    else
    {
        /* TODO */
        eLIBs_printf("!!!! FIX convert UTC time\n");
        //*outSeconds = mktime_tz(&tm, tm.tm_zone);
    }
#endif

    return true;
}
//#define VectorScan(Vector *vector) do{ \
//	int i = 0;
//	for (i = 0; i < vector->num - 1; i++)
//	}while(0)
void VectorAdd(void *newObject, Vector *vector)
{
#if 1
    vector->num++;
    vector->object = realloc_wrapper(vector->object, vector->num * sizeof(void *));
    vector->object[vector->num - 1] = newObject;
#endif
}

void VectorRemove(int index, Vector *vector)
{
#if 1
    int i;
    if (index > vector->num)
    {
        return ;
    }
    for (i = index; i < vector->num - 1; i++)
    {
        vector->object[i] = vector->object[i + 1];
    }
    vector->object[i] = NULL;
    vector->num--;
#endif
}



void VectorDestroy(Vector *vector)
{
#if 1
    if (!vector)
    {
        return ;
    }
    if (vector->object)
    {
#if 0
        int i;
        for (i = 0; i < vector->num; i++)
        {
            if (vector->object[i])
            {
                free_wrapper(vector->object[i]);
            }
        }
#endif
        free_wrapper(vector->object);
        vector->object = NULL;
    }
#endif
}
int melis_access(const char *path)
{
	ES_FILE *pfile = NULL;
	
	pfile = eLIBs_fopen(path, "rb");
	if (pfile)
	{
		eLIBs_fclose(pfile);
		return EPDK_OK;
	}

    return EPDK_FAIL;
}

