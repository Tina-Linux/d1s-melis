#ifndef __G726_EXPORT_API_H__
#define __G726_EXPORT_API_H__

#define PREFIX		AudioG726

#define STAT(x,y)	  STAT_1(x,y)
#define STAT_1(x,y)   x##y

#define EXPORT(x)	  STAT(PREFIX, x)

#if (defined __OS_UCOS) || (defined __OS_DS5)

#define	LibLayerDecInit       DecInit
#define	LibLayerDecExit       DecExit

#else

#define	LibLayerDecInit       EXPORT(DecInit)
#define	LibLayerDecExit       EXPORT(DecExit)

#endif

#endif

