// sim_mod_orange.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                     )
{
    return TRUE;
}

extern "C"
{
#include "typedef.h"
#include "ePDK.h"
    __mods_mgsec_t g_modinfo;
    int g_nModualID = 0;
    ES_FILE *keyfile;
    extern const __mods_mgsec_t modinfo;
    void *GetMagicInfo()
    {
        memcpy(&g_modinfo, &modinfo, sizeof(__mods_mgsec_t));
        return &g_modinfo;
    }
}
