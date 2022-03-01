
#ifndef __ALARM_FUNCTION_H__
#define __ALARM_FUNCTION_H__
#include <kconfig.h>


extern __bool               alarm_is_exist(void);
extern reg_alarm_para_t*    alarm_get_happening(void);
extern __bool               alarm_check_run(void);
extern void                 alarm_settingTime_cleanHappen(__s16 alarmIndex);
extern __s32                standby_startup_alarm(void);
extern void                 alarm_enter_app(void);
extern void                 alarm_exit_app(void);


#endif//__ALARM_FUNCTION_H__
