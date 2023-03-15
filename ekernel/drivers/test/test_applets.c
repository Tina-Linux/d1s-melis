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
#include <rtthread.h>
#include <finsh_api.h>
#include <mod_defs.h>
#include <aw_list.h>
#include "msh.h"
#include <finsh.h>
#include <finsh_api.h>
#include <shell.h>
extern int32_t console_LKeyDevEvent(__input_dev_t *dev, uint32_t type, uint32_t code, int32_t value);


#if 1 //auto test
static __krnl_event_t  *key_lock = NULL;
static int32_t is_running = 0;
static uint32_t task_id    = 0;
static int32_t all_test   = 0;

static int key_sim(uint32_t key_code,uint32_t delay_time)
{
    __u8 err;
    esKRNL_SemPend(key_lock, 0, &err);
    console_LKeyDevEvent(NULL,  EV_KEY,     key_code,  1); //1 is down;   0 is up;
    console_LKeyDevEvent(NULL,  EV_SYN,     0,      0   ); //1 is down;   0 is up;
    rt_thread_delay(2);
    console_LKeyDevEvent(NULL,  EV_KEY,     key_code,  0); //1 is down;   0 is up;
    console_LKeyDevEvent(NULL,  EV_SYN,     0,      0   ); //1 is down;   0 is up;
    esKRNL_SemPost(key_lock);
    
    if(delay_time<100 || delay_time>1000)
    {
        esKRNL_TimeDly(delay_time*100);//second
    }
    else
    {
        esKRNL_TimeDly(delay_time/10);//ms
    }
    return 0;
}


static void auto_movie_test(void*arg)
{
    int32_t test_cnt= *(unsigned int*)arg;
	uint8_t err;
    //@0
    while(test_cnt --)
    {
        key_sim(KPAD_ENTER,3);//进浏览器
        key_sim(KPAD_UP,1);//翻页
        key_sim(KPAD_UP,1);//翻页
        key_sim(KPAD_DOWN,1);//翻页
        key_sim(KPAD_DOWN,1);//翻页
        
		key_sim(KPAD_ENTER,60*60);//进电影,播放1h

        key_sim(KPAD_RETURN,4);//返回浏览器
        key_sim(KPAD_RETURN,3);//返回主界面
    }
	rt_kprintf("auto_test movie complete!\n");
	
	if(all_test == 1)
	{
		return ;//当做普通函数用
	}
	esKRNL_SemPend(key_lock, 0, &err);
	task_id = 0;
	is_running = 0;
    esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err);
	key_lock = NULL;
    esKRNL_TDel(EXEC_prioself);
}


static void auto_music_test(void* arg)
{
    int32_t test_cnt= *(unsigned int*)arg;
	uint8_t err;
    key_sim(KPAD_RIGHT,300);//返回第一个元素
    while(test_cnt --)
    {
        key_sim(KPAD_ENTER,3);//进浏览器
        key_sim(KPAD_UP,6);//翻页
        key_sim(KPAD_UP,6);//翻页
        key_sim(KPAD_DOWN,6);//翻页
        key_sim(KPAD_DOWN,6);//翻页
        
        key_sim(KPAD_DOWN,60*60);//进音乐,播放1h

        key_sim(KPAD_RETURN,4);//返回浏览器
        key_sim(KPAD_RETURN,3);//返回主界面
    }
	key_sim(KPAD_LEFT,300);//返回第一个元素
	rt_kprintf("auto_test music complete!\n");
	if(all_test == 1)
	{
		return ;//当做普通函数用
	}
	
	esKRNL_SemPend(key_lock, 0, &err);
	task_id = 0;
	is_running = 0;
    esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err);
	key_lock = NULL;
    esKRNL_TDel(EXEC_prioself);
}
static void auto_power_test(void *arg)
{
    int32_t test_cnt= *(unsigned int*)arg;
	uint8_t err;
	
    key_sim(KPAD_RIGHT,300);//@2
    key_sim(KPAD_RIGHT,300);
    while(test_cnt --)
    {
        key_sim(KPAD_ENTER,3);//进入界面
        key_sim(KPAD_ENTER,1);//自动关屏
        key_sim(KPAD_ENTER,1);
        key_sim(KPAD_ENTER,1);
        key_sim(KPAD_ENTER,1);
        key_sim(KPAD_ENTER,1);
        key_sim(KPAD_ENTER,1);
        
        key_sim(KPAD_DOWN,1);
        key_sim(KPAD_ENTER,1);//定时关机
        key_sim(KPAD_ENTER,1);
        key_sim(KPAD_ENTER,1);
        key_sim(KPAD_ENTER,1);
        key_sim(KPAD_ENTER,1);
        
        key_sim(KPAD_RETURN,2);
    }
    key_sim(KPAD_LEFT,300);//返回第一个元素
    key_sim(KPAD_LEFT,300);
	rt_kprintf("auto_test power complete!\n");
	if(all_test == 1)
	{
		return ;//当做普通函数用
	}
	
	esKRNL_SemPend(key_lock, 0, &err);
	task_id = 0;
	is_running = 0;
    esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err);
	key_lock = NULL;
    esKRNL_TDel(EXEC_prioself);

}
static void auto_fm_test(void* arg)
{
    int32_t test_cnt= *(unsigned int*)arg;
	uint8_t err;
	
    key_sim(KPAD_RIGHT,300);//@3
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
    
    while(test_cnt --)
    {
        key_sim(KPAD_ENTER,4);//进入应用
        key_sim(KPAD_DOWN,2);//换台
        key_sim(KPAD_DOWN,2);//换台
        key_sim(KPAD_DOWN,2);//换台
        key_sim(KPAD_DOWN,2);//换台
        
        key_sim(KPAD_DOWN,60*60);//播放1h
        
        key_sim(KPAD_RETURN,4);//退回主界面
    }
	
    key_sim(KPAD_LEFT,300);//返回第一个元素
    key_sim(KPAD_LEFT,300);
    key_sim(KPAD_LEFT,300);
	rt_kprintf("auto_test fm complete!\n");
	if(all_test == 1)
	{
		return ;//当做普通函数用
	}
	
	esKRNL_SemPend(key_lock, 0, &err);
	task_id = 0;
	is_running = 0;
    esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err);
	key_lock = NULL;
    esKRNL_TDel(EXEC_prioself);
}


static void auto_photo_test(void* arg)
{
    int32_t test_cnt= *(unsigned int*)arg;
	uint8_t err;
	
    key_sim(KPAD_RIGHT,300);//@4
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);

    while(test_cnt --)
    {
        key_sim(KPAD_ENTER,3);//进浏览器
        key_sim(KPAD_DOWN,1);//上下翻页
        key_sim(KPAD_DOWN,1);//上下翻页
        key_sim(KPAD_UP,1);//上下翻页
        key_sim(KPAD_UP,1);//上下翻页
        
        key_sim(KPAD_ENTER,2);//进图片
        key_sim(KPAD_DOWN,4);//上下翻页
        key_sim(KPAD_DOWN,4);//上下翻页
        key_sim(KPAD_UP,4);//上下翻页
        key_sim(KPAD_UP,4);//上下翻页

        key_sim(KPAD_ENTER,60*60);//自动播放1h
        
        key_sim(KPAD_ENTER,5);//停止自动播放
        
        key_sim(KPAD_RETURN,2);//返回浏览器
        key_sim(KPAD_RETURN,3);//返回主界面
    }
	
    key_sim(KPAD_RIGHT,300);//返回第一个元素
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
	rt_kprintf("auto_test photo complete!\n");
	if(all_test == 1)
	{
		return ;//当做普通函数用
	}
	
	esKRNL_SemPend(key_lock, 0, &err);
	task_id = 0;
	is_running = 0;
    esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err);
	key_lock = NULL;
    esKRNL_TDel(EXEC_prioself);
}

static void auto_explorer_test(void* arg)
{
    int32_t test_cnt= *(unsigned int*)arg;
	uint8_t err;
	
    key_sim(KPAD_RIGHT,300);//@5
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
    
    while(test_cnt --)
    {
        key_sim(KPAD_ENTER,5);// SD/USB
        key_sim(KPAD_ENTER,3);//SD list
        key_sim(KPAD_ENTER,2);//enter node
        key_sim(KPAD_DOWN,500);//next
        key_sim(KPAD_RETURN,2);
        
        key_sim(KPAD_DOWN,800);//next
        key_sim(KPAD_DOWN,800);//next
        key_sim(KPAD_UP,800);//next
        key_sim(KPAD_UP,800);//next
        key_sim(KPAD_RETURN,3);//返回SD list
        key_sim(KPAD_RETURN,3);//返回主界面
    }
	
    key_sim(KPAD_RIGHT,300);//返回第一个元素
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
	rt_kprintf("auto_test explorer complete!\n");
	if(all_test == 1)
	{
		return ;//当做普通函数用
	}
	esKRNL_SemPend(key_lock, 0, &err);
	task_id = 0;
	is_running = 0;
    esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err);
	key_lock = NULL;
    esKRNL_TDel(EXEC_prioself);
}


static void auto_ebook_test(void*arg)
{
    int32_t test_cnt= *(unsigned int*)arg;
	uint8_t err;
	
    key_sim(KPAD_RIGHT,300);//@6
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);
    key_sim(KPAD_RIGHT,300);

    while(test_cnt --)
    {
        key_sim(KPAD_ENTER,3);//进浏览器
        key_sim(KPAD_ENTER,3);//进电子书
        key_sim(KPAD_RIGHT,1);//翻页
        key_sim(KPAD_RIGHT,1);//翻页
        key_sim(KPAD_LEFT,1);//翻页
        key_sim(KPAD_LEFT,1);//翻页
        
        key_sim(KPAD_MENU,2);//进入菜单
        key_sim(KPAD_ENTER,2);//进入颜色选择
        key_sim(KPAD_DOWN,2);//移动到绿色
        key_sim(KPAD_ENTER,2);//选择绿色
        key_sim(KPAD_DOWN,2);//移动到蓝色
        key_sim(KPAD_RETURN,2);//退出

        key_sim(KPAD_MENU,2);//进入菜单
        key_sim(KPAD_DOWN,2);
        key_sim(KPAD_ENTER,2);//进入速度选择
        key_sim(KPAD_DOWN,2);//选择关
        key_sim(KPAD_DOWN,2);//选择快
        key_sim(KPAD_DOWN,2);//选择中
        key_sim(KPAD_DOWN,2);//选择慢
        key_sim(KPAD_RETURN,2);//退出

		key_sim(KPAD_MENU,2);//进入菜单
        key_sim(KPAD_DOWN,2);
        key_sim(KPAD_DOWN,2);
        key_sim(KPAD_ENTER,2);//进入跳转页面
        key_sim(KPAD_ENTER,2);
		key_sim(KPAD_ENTER,2);
		key_sim(KPAD_ENTER,2);
        key_sim(KPAD_ENTER,2);//跳转到第1111页
        key_sim(KPAD_UP,2);
        key_sim(KPAD_UP,2);//移动到确定
        key_sim(KPAD_ENTER,2);//点击确定
        
        
        key_sim(KPAD_RETURN,3);//返回浏览器
        key_sim(KPAD_RETURN,2);//返回主界面
    }

    key_sim(KPAD_RIGHT,300);//返回第一个元素
    key_sim(KPAD_RIGHT,300);
	rt_kprintf("auto_test ebook complete!\n");
	if(all_test == 1)
	{
		return ;//当做普通函数用
	}
	
	esKRNL_SemPend(key_lock, 0, &err);
	task_id = 0;
	is_running = 0;
    esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err);
	key_lock = NULL;
    esKRNL_TDel(EXEC_prioself);
}


static void auto_setting_test(void*arg)
{
    int32_t test_cnt= *(unsigned int*)arg;
	uint8_t err;
	
    key_sim(KPAD_LEFT,300);//@7
    while(test_cnt --)
    {
        key_sim(KPAD_ENTER,3);//进界面
        key_sim(KPAD_ENTER,1);//语言
        key_sim(KPAD_ENTER,1);//
        
        key_sim(KPAD_DOWN,500);
        key_sim(KPAD_ENTER,500);//背光亮度
        key_sim(KPAD_ENTER,500);//
        
        key_sim(KPAD_DOWN,500);
        key_sim(KPAD_ENTER,500);//音量
        key_sim(KPAD_ENTER,500);//
        
        key_sim(KPAD_DOWN,500);
        key_sim(KPAD_ENTER,500);//输出
        key_sim(KPAD_ENTER,500);//
        
        key_sim(KPAD_DOWN,500);
        key_sim(KPAD_ENTER,2);//恢复出厂
        key_sim(KPAD_ENTER,3);//是
        
        key_sim(KPAD_DOWN,500);//软件版本
        key_sim(KPAD_RETURN,2);
    }

    key_sim(KPAD_RIGHT,300);//返回第一个元素
	rt_kprintf("auto_test setting complete!\n");
	if(all_test == 1)
	{
		return ;//当做普通函数用
	}
	
	esKRNL_SemPend(key_lock, 0, &err);
	task_id = 0;
	is_running = 0;
    esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err);
	key_lock = NULL;
    esKRNL_TDel(EXEC_prioself);
}

static void auto_all_test(void* arg);

void(*pfun[9])(void*arg) = 
{
    auto_movie_test,
    auto_music_test,
    auto_power_test,
    auto_fm_test,
    auto_photo_test,
    auto_explorer_test,
    auto_ebook_test,
    auto_setting_test,
    auto_all_test,
};
	
static void auto_all_test(void* arg)
{
    int32_t test_cnt= *(unsigned int*)arg;
	uint8_t err;
    int32_t id = 0;
	int32_t temp_cnt = 1;
	all_test = 1;
    while(test_cnt -- )
    {
        id = esKSRV_Random(7);
        pfun[id](&temp_cnt);
    }
	
	rt_kprintf("auto_test all complete! test time = %d\n",test_cnt);
	
	esKRNL_SemPend(key_lock, 0, &err);
	task_id = 0;
	is_running = 0;
	all_test = 0;
    esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err);
	key_lock = NULL;
    esKRNL_TDel(EXEC_prioself);

}

static int auto_test(int argc, const char **argv)
{
    static int32_t id = 0,cnt = 0;
    char*err;
    __u8 err1;
    long args[2];
    
    if(argc == 1 || argc == 2)
    {
        rt_kprintf("auto_test [id] [cnt]\r\n");
        rt_kprintf("\t   id: 0 : movie\n");
        rt_kprintf("\t   id: 1 : music\n");
        rt_kprintf("\t   id: 2 : power\n");
        rt_kprintf("\t   id: 3 : fm\n");
        rt_kprintf("\t   id: 4 : photo\n");
        rt_kprintf("\t   id: 5 : explorer\n");
        rt_kprintf("\t   id: 6 : ebook\n");
        rt_kprintf("\t   id: 7 : setting\n");
        rt_kprintf("\t   id: 8 : all test\n");
        rt_kprintf("\t   id: -1：stop test(cnt = -1 too)\n");
        rt_kprintf("\t   cnt: n time\n");
        return 0;
    }
    else if(argc == 3)
    {
        id  = strtol(argv[1], &err, 0);
        cnt = strtol(argv[2], &err, 0);
        rt_kprintf("id = %d cnt = %d\n",id,cnt);
        if(is_running == 0)
        {
            if(id >= 0 && id <=8 && cnt >0)
            {
                key_lock = esKRNL_SemCreate(1);
                task_id = esKRNL_TCreate(pfun[id],&cnt,0x1000,KRNL_priolevel1);
                is_running = 1;
            }
            else
            {
                rt_kprintf("please check para!!\n");
            }
        }
        else
        {
            if((id == -1 ) && (cnt == -1))
            {
            	if(key_lock != NULL && task_id != 0)
            	{
                	esKRNL_SemPend(key_lock, 0, &err1);
                	esKRNL_TDel(task_id);
					task_id = 0;
					is_running = 0;
                	esKRNL_SemDel(key_lock, OS_DEL_ALWAYS, &err1);
					key_lock = NULL;
            	}
            }
            else
            {
                rt_kprintf("please check para!!\n");
            }
        }
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(auto_test, __cmd_auto_test, auto test);

#endif
