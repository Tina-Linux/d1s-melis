#include <rtthread.h>
#include <finsh_api.h>
#include <mod_defs.h>
#include <list.h>

typedef struct _modlist {
    struct list_head list;
    __mp *mp;
    __u32 mid;
    char modname[32];
} modlist;

struct list_head list_mod_head = {NULL, NULL};

int cmd_insmod(int argc, char** argv)
{
    int i = 0;
    modlist *plistmod = NULL;
    struct list_head *plist = NULL;

    rt_kprintf("\t install mod para num [%d]: ", argc);
    for(i = 0; i < argc; i ++)
    {
        rt_kprintf("%s ", argv[i]);
    }
    rt_kprintf("\r\n", argv[i]);

    if(argc != 2)
    {
        rt_kprintf("please check install only one mod\r\n");
        return 0;
    }

    if(NULL == list_mod_head.next)
    {
        INIT_LIST_HEAD(&list_mod_head);
    }

    list_for_each(plist, &list_mod_head)
    {
        plistmod = list_entry(plist, modlist, list);
        if(0 == rt_strcmp(argv[1], plistmod->modname))
        {
            rt_kprintf("%s L%d %s has been installed!\r\n", __FUNCTION__, __LINE__, argv[1]);
            return plistmod->mid;
        }
    }

    plistmod = (modlist*)rt_malloc(sizeof(modlist));
    if(NULL == plistmod)
    {
        rt_kprintf("%s L%d request mem failed !\r\n", __FUNCTION__, __LINE__);
        return 0;
    }
    rt_memset(plistmod, 0, sizeof(modlist));

    rt_memcpy(plistmod->modname, argv[1], rt_strlen(argv[1]));

    plistmod->mid = esMODS_MInstall(argv[1], 0);

    if (!plistmod->mid)
    {
        rt_kprintf("%s install fail!\r\n", argv[1]);
        rt_free(plistmod);
        return 0;
    }

    plistmod->mp = esMODS_MOpen(plistmod->mid, 0);

    if (plistmod->mp == 0)
    {
        rt_kprintf("%s start fail!\r\n", argv[1]);
        esMODS_MUninstall(plistmod->mid);
        rt_free(plistmod);
        return 0;
    }

    list_add_tail(&plistmod->list, &list_mod_head);

    rt_kprintf("Mod:%s Installed!, mod id=%d\r\n", argv[1], plistmod->mid);
    return plistmod->mid;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_insmod, __cmd_insmod, install a mod);

int cmd_uninsmod(int argc, char** argv)
{
    int i = 0, ret = 0;
    modlist *plistmod = NULL;
    struct list_head *plist = NULL;

    rt_kprintf("\t uninsmod mod para num [%d]: ", argc);
    for(i = 0; i < argc; i ++)
    {
        rt_kprintf("%s ", argv[i]);
    }
    rt_kprintf("\r\n", argv[i]);

    if(argc != 2)
    {
        rt_kprintf("please check, only uninstall one mod once\r\n");
        return -1;
    }

    if(NULL == list_mod_head.next)
    {
        INIT_LIST_HEAD(&list_mod_head);
    }

    list_for_each(plist, &list_mod_head)
    {
        plistmod = list_entry(plist, modlist, list);
        if(0 == rt_strcmp(argv[1], plistmod->modname))
        {
            rt_kprintf("%s L%d %s find the module!\r\n", __FUNCTION__, __LINE__, argv[1]);
            break;
        }
        plistmod = NULL;
    }

    if(NULL == plistmod)
    {
        rt_kprintf("%s L%d %s module do not be found!\r\n", __FUNCTION__, __LINE__, argv[1]);
        return -1;
    }

    ret = esMODS_MClose(plistmod->mp);

    ret = esMODS_MUninstall(plistmod->mid);

    list_del(&(plistmod->list));
    rt_free(plistmod);

    return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_uninsmod, __cmd_uninsmod, uninstall a mod);



extern int32_t console_LKeyDevEvent(__input_dev_t *dev, uint32_t type, uint32_t code, int32_t value);

typedef struct _keyboard_map
{
    char *key_str;
    int  keycode;
}keyboard_map;

keyboard_map keymap[] = {
    {"menu"     ,       KPAD_MENU       }, //
    {"left"     ,       KPAD_LEFT       }, //
    {"right"    ,       KPAD_RIGHT      }, // 
    {"up"       ,       KPAD_UP         }, //
    {"down"     ,       KPAD_DOWN       }, 
    {"return"   ,       KPAD_RETURN     },
    {"volu"     ,       KPAD_VOICEUP    },
    {"vold"     ,       KPAD_VOICEDOWN  },
    {"enter"    ,       KPAD_ENTER      },
    {"poweroff" ,       KPAD_POWEROFF   },
    {NULL       ,       0               },
};

#define isdigit(c)  ((unsigned)((c) - '0') < 10)
int skip_atoi(const char **s)
{
    register int i = 0;

    while (isdigit(**s))
    {
        i = i * 10 + *((*s)++) - '0';
    }

    return i;
}

int cmd_send_key(int argc, char** argv)
{
    int32_t i = 0;
    //int32_t value = 0;

    rt_kprintf("%d parameters, and %s key_string is %s !\r\n", argc, argv[0], argv[1]);

    if(argc == 1)
    {
        rt_kprintf("send_key [keystring] [value]\r\n"
                    "keystring: menu|left|right|up|down|return|enter|volu|vold\r\n");
    }

    if(argc == 2)
    {
      //  value   = skip_atoi(&argv[2]);
        for(i = 0; i < 32; i ++)
        {
            if(NULL == keymap[i].key_str)
            {
                rt_kprintf("the key is not match anyone key in the keymap!\r\n");
                return EPDK_FAIL;
            }

            if(0 == rt_strncmp(argv[1], keymap[i].key_str, rt_strlen(keymap[i].key_str)))
            {
                rt_kprintf("send key '%s' keycode 0x%X\r\n", keymap[i].key_str, keymap[i].keycode);
                console_LKeyDevEvent(NULL,  EV_KEY,     keymap[i].keycode,  1); //1 is down;   0 is up;
                console_LKeyDevEvent(NULL,  EV_SYN,     0,                  0   ); //1 is down;   0 is up;
                rt_thread_delay(2);
                console_LKeyDevEvent(NULL,  EV_KEY,     keymap[i].keycode,  0); //1 is down;   0 is up;
                console_LKeyDevEvent(NULL,  EV_SYN,     0,                  0   ); //1 is down;   0 is up;
                return EPDK_OK;
            }
        }
    }
}
FINSH_FUNCTION_EXPORT_CMD(cmd_send_key, __cmd_send_key, send a to keyboar of input system);
