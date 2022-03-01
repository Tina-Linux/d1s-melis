#ifndef __MISA_H_DEF__
#define __MISA_H_DEF__

#define RV(x) ((unsigned long)1 << (x - 'A'))
#define RVI RV('I')
#define RVE RV('E') /* E and I are mutually exclusive */
#define RVM RV('M')
#define RVA RV('A')
#define RVF RV('F')
#define RVD RV('D')
#define RVC RV('C')
#define RVS RV('S')
#define RVU RV('U')
#define RVH RV('H')

#endif
