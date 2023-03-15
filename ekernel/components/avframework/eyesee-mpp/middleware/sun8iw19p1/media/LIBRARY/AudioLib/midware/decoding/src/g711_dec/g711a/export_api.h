#ifndef EXPORT_API_H
#define EXPORT_API_H

#define PREFIX		AudioG711a

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
