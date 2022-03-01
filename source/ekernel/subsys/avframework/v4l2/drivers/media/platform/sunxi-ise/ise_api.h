#ifndef ISE_API_H
#define ISE_API_H

/* io ctrl cmd */
#define ISE_MAGIC 'x'

#define ISE_WRITE_REGISTER      _IO(ISE_MAGIC, 0)
#define ISE_READ_REGISTER       _IO(ISE_MAGIC, 1)
#define ENABLE_ISE         		_IO(ISE_MAGIC, 2)
#define DISABLE_ISE         	_IO(ISE_MAGIC, 3)
#define WAIT_ISE_FINISH         _IO(ISE_MAGIC, 4)
#define SET_ISE_FREQ         	_IO(ISE_MAGIC, 5)

struct ise_register{
	unsigned int addr;
	unsigned int value;
};

#endif

MODULE_LICENSE("Dual BSD/GPL");