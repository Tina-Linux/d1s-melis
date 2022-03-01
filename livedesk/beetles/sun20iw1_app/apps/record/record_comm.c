#include "record_comm.h"

__s32 record_strFilter(const char *filters, __s32 index, char *filter)
{
    __s32 cnt = 0, ret = EPDK_FAIL;

    while (*filters != '\0')
    {
        if (*filters == '|')
        {
            cnt++;
        }
        else
        {
            if (cnt == index)
            {
                *filter++ = *filters;
                ret = EPDK_OK;
            }
        }

        filters++;
    }

    *filter = '\0';
    return ret;
}

__s32 record_strcmplast(const char *name, const char *filters)
{
    __s32 i, cnt, nameLen, filterLen;
    char filter[10] = {0}, nameCh, filterCh;

    for (i = 0; i < 10; i++)
    {
        if (record_strFilter(filters, i, filter) == EPDK_OK)
        {
            //nameLen = SLIB_strlen(name);
            nameLen = eLIBs_strlen(name);
            //filterLen = SLIB_strlen(filter);
            filterLen = eLIBs_strlen(filter);

            for (cnt = 0; cnt < filterLen; cnt++)
            {
                nameCh = name[nameLen - cnt - 1];
                filterCh = filter[filterLen - cnt - 1];

                if (nameCh >= 'a' && nameCh <= 'z')
                {
                    nameCh -= 32;
                }

                if (filterCh >= 'a' && filterCh <= 'z')
                {
                    filterCh -= 32;
                }

                if (nameCh != filterCh)
                {
                    break;
                }
                else if (nameCh == filterCh && cnt == filterLen - 1)
                {
                    return 0;
                }
            }
        }
    }

    return -1;
}

__u32 record_file_num(const char *dirname, const char *filter)
{
    ES_DIR      *pDir = NULL;
    ES_DIRENT   *direntp = NULL;
    __u32       fileCnt = 0;
    pDir = eLIBs_opendir(dirname);

    if (pDir == NULL)
    {
        return NULL;
    }

    while ((direntp = (ES_DIRENT *)eLIBs_readdir(pDir)) != NULL)
    {
        if (!(direntp->fatdirattr & FSYS_ATTR_DIRECTORY))
        {
            if (record_strcmplast((const char *)direntp->d_name, filter) == 0)
            {
                fileCnt++;
            }
        }
    }

    eLIBs_rewinddir(pDir);
    eLIBs_closedir(pDir);
    return fileCnt;
}

char *record_get_fileName(const char *dirname, __u32 index, const char *filter, char *fileName)
{
    ES_DIR      *pDir = NULL;
    ES_DIRENT   *direntp = NULL;
    __u32       fileCnt = 0;
    pDir = eLIBs_opendir(dirname);

    if (pDir == NULL)
    {
        return NULL;
    }

    while ((direntp = (ES_DIRENT *)eLIBs_readdir(pDir)) != NULL)
    {
        if (!(direntp->fatdirattr & FSYS_ATTR_DIRECTORY))
        {
            if (record_strcmplast((const char *)direntp->d_name, filter) == 0)
            {
                if (fileCnt == index)
                {
                    //SLIB_strcpy(fileName, (const char*)direntp->d_name);
                    eLIBs_strcpy(fileName, (const char *)direntp->d_name);
                    break;
                }

                fileCnt++;
            }
        }
    }

    eLIBs_rewinddir(pDir);
    eLIBs_closedir(pDir);
    return fileName;
}

void record_time_to_char(char *buff, __awos_time_t time, char sp)
{
    __s16 cnt = 0;
    buff[cnt++] = time.hour / 10 % 10 + '0';
    buff[cnt++] = time.hour % 10 + '0';

    if (sp != 0)
    {
        buff[cnt++] = sp;
    }

    buff[cnt++] = time.minute / 10 % 10 + '0';
    buff[cnt++] = time.minute % 10 + '0';

    if (sp != 0)
    {
        buff[cnt++] = sp;
    }

    buff[cnt++] = time.second / 10 % 10 + '0';
    buff[cnt++] = time.second % 10 + '0';
    buff[cnt++] = '\0';
}

void record_date_to_char(char *buff, __awos_date_t date, char sp)
{
    __s16 cnt = 0;
    buff[cnt++] = date.year / 1000 % 10 + '0';
    buff[cnt++] = date.year / 100 % 10 + '0';
    buff[cnt++] = date.year / 10 % 10 + '0';
    buff[cnt++] = date.year % 10 + '0';

    if (sp != 0)
    {
        buff[cnt++] = sp;
    }

    buff[cnt++] = date.month / 10 % 10 + '0';
    buff[cnt++] = date.month % 10 + '0';

    if (sp != 0)
    {
        buff[cnt++] = sp;
    }

    buff[cnt++] = date.day / 10 % 10 + '0';
    buff[cnt++] = date.day % 10 + '0';
    buff[cnt++] = '\0';
}

void record_ms_to_char(char *buff, __u32 time, char sp)
{
    __s16 cnt = 0;
    __u32 s = time / 1000;
    buff[cnt++] = s / 3600 / 10 % 10 + '0';
    buff[cnt++] = s / 3600 % 10 + '0';

    if (sp != 0)
    {
        buff[cnt++] = sp;
    }

    buff[cnt++] = s / 60 % 60 / 10 % 10 + '0';
    buff[cnt++] = s / 60 % 60 % 10 + '0';

    if (sp != 0)
    {
        buff[cnt++] = sp;
    }

    buff[cnt++] = s % 60 / 10 % 10 + '0';
    buff[cnt++] = s % 60 % 10 + '0';
    buff[cnt++] = '\0';
}

void record_s_to_char(char *buff, __u32 time, char sp)
{
    __s16 cnt = 0;
    __u32 s = time;
    buff[cnt++] = s / 3600 / 10 % 10 + '0';
    buff[cnt++] = s / 3600 % 10 + '0';

    if (sp != 0)
    {
        buff[cnt++] = sp;
    }

    buff[cnt++] = s / 60 % 60 / 10 % 10 + '0';
    buff[cnt++] = s / 60 % 60 % 10 + '0';

    if (sp != 0)
    {
        buff[cnt++] = sp;
    }

    buff[cnt++] = s % 60 / 10 % 10 + '0';
    buff[cnt++] = s % 60 % 10 + '0';
    buff[cnt++] = '\0';
}
