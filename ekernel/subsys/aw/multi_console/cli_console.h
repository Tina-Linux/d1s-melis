
#ifndef CLI_CONSOLE_H
#define CLI_CONSOLE_H
#include <list.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <semaphore.h>
#define AW_AGENIE

#ifdef  AW_AGENIE
#define CLI_CONSOLE_TAG_LEN 64
#endif

#define CLI_CONSOLE_MAX_NAME_LEN 32

typedef void (* command_callback)(void * private_data);

typedef struct _wraper_task
{
    struct list_head task_list_node;
    void * task;
}wraper_task;

typedef struct _device_console
{
    char * name;
    int (*read)(void * buf, size_t len, void * private_data);
    int (*write)(const void * buf, size_t len, void * private_data);
    int (*init)(void * private_data);
    int (*deinit)(void * private_data);
}device_console;

typedef struct _command_line_console
{
    struct list_head i_list;
    device_console * dev_console;
    char name[CLI_CONSOLE_MAX_NAME_LEN];
    int init_flag;
    int exit_flag;
    int alive;
    void * private_data;
    void * task;
#ifdef AW_AGENIE
    char cli_tag[CLI_CONSOLE_TAG_LEN];
    uint8_t cli_tag_len;
#endif
    sem_t sem;
    pthread_spinlock_t lock;
    command_callback finsh_callback;
    command_callback start_callback;
    struct list_head task_list;
}cli_console;

static int remove_cli_console_from_list(cli_console * console);

static int register_cli_console_to_list(cli_console * console);

static cli_console * lookup_cli_console(const char * name);

int get_alive_console_num(void);

int32_t create_default_console_task(uint32_t stack_size, uint32_t priority);

cli_console * get_current_console(void);

cli_console * get_default_console(void);

cli_console * set_default_console(void * console);

cli_console * get_global_console(void);

cli_console * set_global_console(void * console);

cli_console * get_clitask_console(void);

#ifdef AW_AGENIE
void cli_console_set_tag_len(uint8_t cli_tag_len, int * error);

uint8_t cli_console_get_tag_len(int * error);

void cli_console_set_tag(char cli_tag, uint32_t idx, int * error);

char cli_console_get_tag(uint32_t idx, int * error);

char *cli_console_get_all_tag(int * error);
#endif

//int cli_console_read(void * buf, size_t nbytes);
int cli_console_read(cli_console * console, void * buf, size_t nbytes);

//int cli_console_write(void * buf, size_t nbytes);
int cli_console_write(cli_console * console, const void * buf, size_t nbytes);

int cli_console_deinit(cli_console * console);

int cli_console_init(cli_console * console);

cli_console* cli_console_create(device_console * dev_console, const char * name, void * private_data);

int cli_console_destory(cli_console * console);

int cli_console_task_create(cli_console * console, pthread_t *tid, uint32_t stack_size, uint32_t priority);

int cli_console_task_destory(cli_console * console);

void cli_console_current_task_destory(void);

char * cli_task_get_console_name(void);

char * cli_console_get_name(cli_console * console);

int cli_console_task_check_exit_flag(void);

void check_console_task_exit(void);

void cli_console_remove_task_list_node(cli_console * console, void * task);

void cli_console_add_task_list_node(void * current_console, void * task);

int cli_console_check_invalid(cli_console * console);

void * cli_task_get_console(void * task_handle);
void *cli_task_set_console(void * task_handle, void * console);
int cli_task_clear_console(void * task_handle);
int32_t cli_task_create_add_console(const char *name, void (*fn)(void *), void *arg,
                        uint32_t stack, uint32_t priority, void ** task_handle);
int cli_uart_console_init(void);
cli_console * get_clidevice_console(void);
cli_console * cli_device_get_console(void * device_handle);

int uart_console_write(const void * buf, size_t len, void * privata_data);
int uart_console_read(void * buf, size_t len, void * privata_data);
#endif  /*CLI_CONSOLE_H*/
