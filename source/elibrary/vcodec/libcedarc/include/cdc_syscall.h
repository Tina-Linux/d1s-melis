#ifndef CDC_SYSCALL_H
#define CDC_SYSCALL_H

#include <libc.h>

#define fopen(name, mode)               eLIBs_fopen((name),(mode))
#define fclose(fptr)                    eLIBs_fclose((fptr))
#define fread(buf,unit,size,fptr)       eLIBs_fread((buf),(unit),(size),(fptr))
#define fwrite(buf,unit,size,fptr)      eLIBs_fwrite((buf),(unit),(size),(fptr))


#define printf                          eLIBs_printf
#define assert(x)                       NULL
#define fprintf(...)

#endif
