/* SIMULATION & FPGA PALTFORM & IC PLATFORM */

#ifndef _SCR_TEST_H_
#define _SCR_TEST_H_

//#ifdef CONFIG_SCR_TEST

extern const uint32_t SCR_REG_DEFAULT[];
extern const uint32_t SCR_REG_RW[];


const unsigned int SCR_REG_DEFAULT[] = {12             //reg Count
	                           ,0x00000000 ,0x000 ,0x01ff0f07   //reg 0: (default, addr, mask)
	                           ,0x00000000 ,0x004 ,0x00ff1e1f
	                           ,0x00000000 ,0x008 ,0x00ff1e1f
	                           ,0x00000101 ,0x00c ,0x00000707
	                           ,0x00000000 ,0x010 ,0x1f1f1f1f //0x1f001f1f //
	                           ,0x00000000 ,0x014 ,0x000000ff
	                           ,0x00000000 ,0x018 ,0xffffffff
	                           ,0x00000000 ,0x01c ,0x00ffffff
	                           ,0x00000000 ,0x020 ,0xffff00ff
	                           ,0x00000000 ,0x030 ,0x000000ff
	                           ,0x00000000 ,0x03c ,0xffffffff
	                           ,0x000003ff ,0x040 ,0xffffffff
	                           };

const unsigned int SCR_REG_RW[]    =   {12               //reg Count
	                           ,0x01  //Byte|Hword|Word
	                           ,0x00000000 ,0x000 ,0x01ff0007   //reg 0: (default, addr, mask)
	                           ,0x00000000 ,0x004 ,0x00ff1e1f
	                           ,0x00000000 ,0x008 ,0x00000000
	                           ,0x00000101 ,0x00c ,0x00000000
	                           ,0x00000000 ,0x010 ,0x1f1f0000 //0x1f000000 //
	                           ,0x00000000 ,0x014 ,0x000000ff
	                           ,0x00000000 ,0x018 ,0xffffffff
	                           ,0x00000000 ,0x01c ,0x00ffffff
	                           ,0x00000000 ,0x020 ,0xffff00ff
	                           ,0x00000000 ,0x030 ,0x000000fd
	                           ,0x00000000 ,0x03c ,0x00000000
	                           ,0x000003ff ,0x040 ,0xffffffff
	                           };


extern void scr_params_init(void);
extern uint32_t scr_reg_test(void);
//extern hal_irqreturn_t scr0_irq_handler(void *dev);
extern void reg_scr0_irq_handler(void);
extern uint32_t scr_insert_remove_test(void);
extern uint32_t scr_test_process(pscr_struct pscr);
extern void scr_pio_setup(void);

extern void scr_system_setup(u32 sel);
extern void scr_rest(void);

//#endif

#endif
