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
#ifndef __SUNXI_INPUT_H
#define __SUNXI_INPUT_H

#include <hal_sem.h>
//#include "input_event_codes.h"
#include <sys_input.h>
#ifdef __cplusplus
extern "C" {
#endif


/***************************************list define *******************************/
struct input_list_head
{
    struct input_list_head *next, *prev;
};
#define INPUT_LIST_HEAD_INIT(name) { &(name), &(name) }

#define INPUT_LIST_HEAD(name) \
    struct input_list_head name = INPUT_LIST_HEAD_INIT(name)

#define INPUT_INIT_LIST_HEAD(ptr) do { \
        (ptr)->next = (ptr); (ptr)->prev = (ptr); \
    } while (0)

static inline void input_list_add(struct input_list_head *newitem,
                                  struct input_list_head *prev,
                                  struct input_list_head *next)
{
    next->prev = newitem;
    newitem->next = next;
    newitem->prev = prev;
    prev->next = newitem;
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void input_list_add_tail(struct input_list_head *newitem, struct input_list_head *head)
{
    input_list_add(newitem, head->prev, head);
}

/**
 * list_entry - get the struct for this entry
 * @ptr:    the &struct list_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */
#define input_list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop counter.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define input_list_for_each_entry(pos, head, member, type)              \
    for (pos = input_list_entry((head)->next, type, member);    \
         &pos->member != (head);                    \
         pos = input_list_entry(pos->member.next, type, member))



/*********************************bit define******************************************/
//#define INPUT_BITS_PER_BYTE 8
//#define INPUT_DIV_ROUND_UP(n,d)     (((n) + (d) - 1) / (d))
//#define INPUT_BITS_TO_LONGS(nr)     INPUT_DIV_ROUND_UP(nr, INPUT_BITS_PER_BYTE * sizeof(long))


static inline void input_set_bit(int nr,  volatile void *addr)
{
    ((int *) addr)[nr >> 5] |= (1UL << (nr & 31));
}

static inline int input_test_bit(int nr, const volatile void *addr)
{
    return (1UL & (((const int *) addr)[nr >> 5] >> (nr & 31))) != 0UL;
}

static inline int __input_test_bit(unsigned int code, unsigned int key)
{
    return code == key;
}

static inline void __input_change_bit(unsigned int code, unsigned int *key)
{
    *key = code;
}

static inline void __input_clear_bit(unsigned int *key)
{
    *key = 0;
}
/******************************input event define***************************************/
#define EVENT_BUFFER_SIZE 64U

struct sunxi_input_event
{
    unsigned int type;
    unsigned int code;
    unsigned int value;
};

struct sunxi_input_dev
{
    const char *name;

    unsigned int key;

    unsigned long evbit[INPUT_BITS_TO_LONGS(EV_CNT)];
    unsigned long keybit[INPUT_BITS_TO_LONGS(KEY_CNT)];
    unsigned long relbit[INPUT_BITS_TO_LONGS(REL_CNT)];
    unsigned long absbit[INPUT_BITS_TO_LONGS(ABS_CNT)];
    unsigned long mscbit[INPUT_BITS_TO_LONGS(MSC_CNT)];

    struct input_list_head        h_list;
    struct input_list_head        node;

};

//one task that read input dev, corresponds one sunxi_evdev
struct sunxi_evdev
{
    int fd;
    const char *name;
    hal_sem_t sem;

    struct sunxi_input_event buffer[EVENT_BUFFER_SIZE];
    unsigned int head;
    unsigned int tail;
    unsigned int packet_head;

    struct input_list_head        h_list;
    struct input_list_head        node;

};

void input_set_capability(struct sunxi_input_dev *dev, unsigned int type, unsigned int code);
struct sunxi_input_dev *sunxi_input_allocate_device(void);
int sunxi_input_register_device(struct sunxi_input_dev *dev);
void sunxi_input_event(struct sunxi_input_dev *dev,
                       unsigned int type, unsigned int code, unsigned int value);
int sunxi_input_read(int fd, void *buffer, unsigned int size);
int sunxi_input_open(const char *name);
struct sunxi_input_dev *find_input_dev_by_name(const char *name);

static inline void input_report_key(struct sunxi_input_dev *dev, unsigned int code, int value)
{
    sunxi_input_event(dev, EV_KEY, code, value);
}

static inline void input_report_rel(struct sunxi_input_dev *dev, unsigned int code, int value)
{
    sunxi_input_event(dev, EV_REL, code, value);
}

static inline void input_report_abs(struct sunxi_input_dev *dev, unsigned int code, int value)
{
    sunxi_input_event(dev, EV_ABS, code, value);
}
static inline void input_report_misc(struct sunxi_input_dev *dev, unsigned int code, int value)
{
    sunxi_input_event(dev, EV_MSC, code, value);
}
static inline void input_sync(struct sunxi_input_dev *dev)
{
    sunxi_input_event(dev, EV_SYN, SYN_REPORT, 0);
}

//TODO temp impl
typedef void(*KeyCbPtr)(int code);
int register_keyboard_event_callback(KeyCbPtr cb);

//for boot key status check.
int ag_recovery_by_keys(void);

#ifdef __cplusplus
}
#endif

#endif//__SUNXI_INPUT_H
