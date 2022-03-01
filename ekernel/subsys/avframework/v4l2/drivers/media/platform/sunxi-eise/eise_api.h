#ifndef EISE_API_H
#define EISE_API_H

/* io ctrl cmd */
#define EISE_MAGIC 'y'

#define EISE_WRITE_REGISTER      _IO(EISE_MAGIC, 0)
#define EISE_READ_REGISTER       _IO(EISE_MAGIC, 1)
#define ENABLE_EISE         		_IO(EISE_MAGIC, 2)
#define DISABLE_EISE         	_IO(EISE_MAGIC, 3)
#define WAIT_EISE_FINISH         _IO(EISE_MAGIC, 4)
#define SET_EISE_FREQ         	_IO(EISE_MAGIC, 5)

struct eise_register{
	unsigned int addr;
	unsigned int value;
};

#endif

MODULE_LICENSE("Dual BSD/GPL");