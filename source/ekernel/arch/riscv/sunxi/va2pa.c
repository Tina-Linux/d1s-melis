/*
 * ===========================================================================================
 *
 *       Filename:  va2pa.c
 *
 *    Description:  va-pa address transform.
 *
 *        Version:  Melis3.0 
 *         Create:  2020-08-17 12:07:45
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-08-17 12:08:12
 *
 * ===========================================================================================
 */

unsigned long awos_arch_virt_to_phys(unsigned long virtaddr)
{
    return virtaddr;
}

unsigned long awos_arch_phys_to_virt(unsigned long phyaddr)
{
    return phyaddr;
}

