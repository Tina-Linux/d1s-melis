
#ifndef __HAL_CSI_JPEG_H_
#define __HAL_CSI_JPEG_H_

#include <aw_list.h>
#include <hal_def.h>
enum pix_output_fmt_mode_t {
	PIX_FMT_OUT_NV12 = 0x1,
	PIX_FMT_OUT_JPEG = 0x2,
	PIX_FMT_OUT_MAX = 0x3,
};

enum line_mode_t {
	OFFLINE_MODE = 0,
	ONLINE_MODE,
};

typedef void (*CapStatusCb)(struct csi_ipeg_mem *jpeg_mem);

struct csi_jpeg_fmt {
	unsigned int width;
	unsigned int height;
	enum line_mode_t line_mode;
	enum pix_output_fmt_mode_t output_mode;
	CapStatusCb cb;
	unsigned char fps; //reserve
};

struct csi_ipeg_buf {
	unsigned int size;
	void *addr;
};

typedef struct {
	uint8_t buff_index; 	/* Indicate which buffer the currently encoded part jpeg is stored in */
	uint32_t buff_offset; 	/* Indicate the offset of the current part of jpeg in the buffer */
	uint8_t tail; 			/* Indicates whether it is the last part of a jpeg image */
	uint32_t size; 			/* Indicate the size of the current part of jpeg encoding */
} jpeg_mpartbuffinfo;

struct csi_ipeg_mem {
	unsigned char index;
	struct csi_ipeg_buf buf;
	jpeg_mpartbuffinfo mpart_info;
	struct list_head list;
};


void hal_csi_jpeg_set_fmt(struct csi_jpeg_fmt *intput_fmt);
int hal_csi_jpeg_reqbuf(unsigned int count);
int hal_csi_jpeg_freebuf(void);
unsigned int hal_csi_dqbuf(struct csi_ipeg_mem *csi_mem, unsigned int timeout_msec);
unsigned int hal_jpeg_dqbuf(struct csi_ipeg_mem *jpeg_mem, unsigned int timeout_msec);
void hal_csi_qbuf(void);
void hal_jpeg_qbuf(void);
void hal_csi_jpeg_s_stream(unsigned int on);
HAL_Status hal_csi_jpeg_probe(void);
HAL_Status hal_csi_jpeg_remove(void);

#endif
