/*
 * ===========================================================================================
 *
 *       Filename:  scenes9.c
 *
 *    Description:  9 scenes test routine by socket.
 *
 *        Version:  Melis3.0
 *         Create:  2018-05-07 19:36:48
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-05-08 11:45:05
 *
 * ===========================================================================================
 */

#include <kapi.h>
#include <debug.h>
#include <log.h>

static __hdle skt12;
static __hdle skt23;
static __hdle skt34;
static __hdle skt45;
static __hdle skt56;
static __hdle skt67;
static __hdle skt78;
static __hdle skt89;

static __u32 thread1;
static __u32 thread2;
static __u32 thread3;
static __u32 thread4;
static __u32 thread5;
static __u32 thread6;
static __u32 thread7;
static __u32 thread8;
static __u32 thread9;

static uint32_t dummy;

void main_task1(void *para)
{
    while (1)
    {
        esKRNL_SktPend(skt12, KRNL_SKT_USR_IN, 0);
        __log("");
        esKRNL_TimeDly(200);
        esKRNL_SktPost(skt12, KRNL_SKT_USR_IN, &dummy);
    }
}

void main_task2(void *para)
{
    while (1)
    {
        esKRNL_SktPend(skt12, KRNL_SKT_USR_OUT, 0);
        __log("");
        esKRNL_SktPend(skt23, KRNL_SKT_USR_IN, 0);
        esKRNL_SktPost(skt23, KRNL_SKT_USR_IN, &dummy);

        if (esKRNL_SktPend(skt23, KRNL_SKT_USR_IN, 0) != NULL)
        {
            esKRNL_SktPost(skt23, KRNL_SKT_USR_OUT, &dummy);
        }

        esKRNL_SktPost(skt12, KRNL_SKT_USR_OUT, &dummy);
    }
}

void main_task3(void *para)
{
    while (1)
    {
        esKRNL_SktPend(skt23, KRNL_SKT_USR_OUT, 0);
        __log("");
        esKRNL_SktPend(skt34, KRNL_SKT_USR_IN, 0);
        esKRNL_SktPost(skt34, KRNL_SKT_USR_IN, &dummy);

        if (esKRNL_SktPend(skt34, KRNL_SKT_USR_IN, 0) != NULL)
        {
            esKRNL_SktPost(skt34, KRNL_SKT_USR_OUT, &dummy);
        }

        esKRNL_SktPost(skt23, KRNL_SKT_USR_OUT, &dummy);
    }
}

void main_task4(void *para)
{
    while (1)
    {
        esKRNL_SktPend(skt34, KRNL_SKT_USR_OUT, 0);
        __log("");
        esKRNL_SktPend(skt45, KRNL_SKT_USR_IN, 0);
        esKRNL_SktPost(skt45, KRNL_SKT_USR_IN, &dummy);

        if (esKRNL_SktPend(skt45, KRNL_SKT_USR_IN, 0) != NULL)
        {
            esKRNL_SktPost(skt45, KRNL_SKT_USR_OUT, &dummy);
        }

        esKRNL_SktPost(skt34, KRNL_SKT_USR_OUT, &dummy);
    }
}

void main_task5(void *para)
{
    while (1)
    {
        esKRNL_SktPend(skt45, KRNL_SKT_USR_OUT, 0);
        __log("");
        esKRNL_SktPend(skt56, KRNL_SKT_USR_IN, 0);
        esKRNL_SktPost(skt56, KRNL_SKT_USR_IN, &dummy);

        if (esKRNL_SktPend(skt56, KRNL_SKT_USR_IN, 0) != NULL)
        {
            esKRNL_SktPost(skt56, KRNL_SKT_USR_OUT, &dummy);
        }

        esKRNL_SktPost(skt45, KRNL_SKT_USR_OUT, &dummy);
    }
}

void main_task6(void *para)
{
    while (1)
    {
        esKRNL_SktPend(skt56, KRNL_SKT_USR_OUT, 0);
        __log("");
        esKRNL_SktPend(skt67, KRNL_SKT_USR_IN, 0);
        esKRNL_SktPost(skt67, KRNL_SKT_USR_IN, &dummy);

        if (esKRNL_SktPend(skt67, KRNL_SKT_USR_IN, 0) != NULL)
        {
            esKRNL_SktPost(skt67, KRNL_SKT_USR_OUT, &dummy);
        }

        esKRNL_SktPost(skt56, KRNL_SKT_USR_OUT, &dummy);
    }
}

void main_task7(void *para)
{
    while (1)
    {
        esKRNL_SktPend(skt67, KRNL_SKT_USR_OUT, 0);
        __log("");
        esKRNL_SktPend(skt78, KRNL_SKT_USR_IN, 0);
        esKRNL_SktPost(skt78, KRNL_SKT_USR_IN, &dummy);

        if (esKRNL_SktPend(skt78, KRNL_SKT_USR_IN, 0) != NULL)
        {
            esKRNL_SktPost(skt78, KRNL_SKT_USR_OUT, &dummy);
        }

        esKRNL_SktPost(skt67, KRNL_SKT_USR_OUT, &dummy);
    }
}

void main_task8(void *para)
{
    while (1)
    {
        esKRNL_SktPend(skt78, KRNL_SKT_USR_OUT, 0);
        __log("");
        esKRNL_SktPend(skt89, KRNL_SKT_USR_IN, 0);
        esKRNL_SktPost(skt89, KRNL_SKT_USR_IN, &dummy);

        if (esKRNL_SktPend(skt89, KRNL_SKT_USR_IN, 0) != NULL)
        {
            esKRNL_SktPost(skt89, KRNL_SKT_USR_OUT, &dummy);
        }

        esKRNL_SktPost(skt78, KRNL_SKT_USR_OUT, &dummy);
    }
}

void main_task9(void *para)
{
    while (1)
    {
        esKRNL_SktPend(skt89, KRNL_SKT_USR_OUT, 0);
        __log("");
        esKRNL_SktPost(skt89, KRNL_SKT_USR_OUT, &dummy);
    }
}


void test_nine_scense(void)
{
    skt12  = esKRNL_SktCreate(1, 0 | 0, 16 | 0);
    skt23  = esKRNL_SktCreate(1, 0 | 0, 16 | 0);
    skt34  = esKRNL_SktCreate(1, 0 | 0, 16 | 0);
    skt45  = esKRNL_SktCreate(1, 0 | 0, 16 | 0);
    skt56  = esKRNL_SktCreate(1, 0 | 0, 16 | 0);
    skt67  = esKRNL_SktCreate(1, 0 | 0, 16 | 0);
    skt78  = esKRNL_SktCreate(1, 0 | 0, 16 | 0);
    skt89  = esKRNL_SktCreate(1, 0 | 0, 16 | 0);

    if (!skt12 || !skt23 || !skt34 || !skt45 || !skt56 || !skt67 || !skt78 || !skt89)
    {
        __err("create socket error");
        software_break();
    }

    thread1 = esKRNL_TCreate(main_task1, 0, 0x800, KRNL_priolevel5);
    thread2 = esKRNL_TCreate(main_task2, 0, 0x800, KRNL_priolevel5);
    thread3 = esKRNL_TCreate(main_task3, 0, 0x800, KRNL_priolevel5);
    thread4 = esKRNL_TCreate(main_task4, 0, 0x800, KRNL_priolevel5);
    thread5 = esKRNL_TCreate(main_task5, 0, 0x800, KRNL_priolevel5);
    thread6 = esKRNL_TCreate(main_task6, 0, 0x800, KRNL_priolevel5);
    thread7 = esKRNL_TCreate(main_task7, 0, 0x800, KRNL_priolevel5);
    thread8 = esKRNL_TCreate(main_task8, 0, 0x800, KRNL_priolevel5);
    thread9 = esKRNL_TCreate(main_task9, 0, 0x800, KRNL_priolevel5);

    if (!thread1 || !thread2 || !thread3 || !thread4 || !thread5 || !thread6 || !thread7 || !thread8 || !thread9)
    {
        __err("create thread error");
        software_break();
    }

    return;
}
