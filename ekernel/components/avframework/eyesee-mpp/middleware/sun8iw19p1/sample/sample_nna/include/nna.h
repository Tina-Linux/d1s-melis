#ifndef _NNA_H_
#define _NNA_H_

#include <ctype.h>
#include <stdio.h>
#include <inttypes.h>
#ifdef __cplusplus
extern "C"
{
#endif
// convolution + act + pooling
void nna_conv_act_pool(
    unsigned int output_addr,   // memory address of output data
    unsigned int input_addr,    // memory address of input data
    unsigned int weight_addr,   // memory address of weight data
    unsigned int bias_addr,     // memory address of bias data

    int output_shift,           // [alu_shift<<8|mul_trunc] (output_data<<alu_shift)>>mul_trunc
    int bias_shift,             // [alu_shift<<8|mul_trunc] (input_data+(bias data<<alu_shift input))>>mul_trunc
    int img_format,             // [pix_format<<8|img_mode]
    int img_mean_op,            // [A<<24|B<<16|G<<8|R_Y]
    int input_w,                // input data width
    int input_h,                // input data height
    int input_c,                // input data channel

    int num_output,             // number of output data channel
    int kernel,                 // kernel size
    int dilation,               // kernel dilation size
    int stride,                 // kernel stride size
    int pad,                    // pading size
    int bias_term,              // bias term 0:no bias 1: bias 2: batch normalization
    int act_type,               // 0: not act 1:ReLU 2:PreLU 3:sigmoid 4:swish
    int act_op,                 // prelu[scale<<8|trunc] coef = scale/(2^trunc)

    int pool_type,              // pooling type 0:avg 1:max, 1:min
    int pool_kernel,            // pooling kernel size
    int pool_stride,            // pooling stride
    int pool_pad                // pooling pad size
);

// convolution + act
void nna_conv_act(
    unsigned int output_addr,   // memory address of output data
    unsigned int input_addr,    // memory address of input data
    unsigned int weight_addr,   // memory address of weight data
    unsigned int bias_addr,     // memory address of bias data

    int output_shift,           // [alu_shift<<8|mul_trunc] (output_data<<alu_shift)>>mul_trunc
    int bias_shift,             // [alu_shift<<8|mul_trunc] (input_data+(bias_data<<alu_shift))>>mul_trunc
    int img_format,             // [pix_format<<8|img_mode]
    int img_mean_op,            // [A<<24|B<<16|G<<8|R_Y]
    int input_w,                // input data width
    int input_h,                // input data height
    int input_c,                // input data channel

    int num_output,             // number of output data channel
    int kernel,                 // kernel size
    int dilation,               // kernel dilation size
    int stride,                 // kernel stride size
    int pad,                    // pading size
    int bias_term,              // bias term 0:no bias 1: bias 2: batch normalization
    int act_type,               // 0: not act 1:ReLU 2:PreLU 3:sigmoid 4:swish
    int act_op                  // prelu[scale<<8|trunc] coef = scale/(2^trunc)
);

// convolution + act + eltwise
void nna_conv_act_eltwise(
    unsigned int output_addr,   // memory address of output data
    unsigned int input_addr,    // memory address of input data
    unsigned int input_b_addr,  // memory address of input data A  output_data = A*B or A+B or MAX(A,B)
    unsigned int weight_addr,   // memory address of weight data
    unsigned int bias_addr,     // memory address of bias data

    int output_shift,           // [alu_shift<<8|mul_trunc] (output_data<<alu_shift)>>mul_trunc
    int bias_shift,             // [alu_shift<<8|mul_trunc] (input_data+(bias_data<<alu_shift))>>mul_trunc
    int img_format,             // [pix_format<<8|img_mode]
    int img_mean_op,            // [A<<24|B<<16|G<<8|R_Y]
    int input_w,                // input data width
    int input_h,                // input data height
    int input_c,                // input data channel

    int num_output,             // number of output data channel
    int kernel,                 // kernel size
    int dilation,               // kernel dilation size
    int stride,                 // kernel stride size
    int pad,                    // pading size
    int bias_term,              // bias term 0:no bias 1: bias 2: batch normalization
    int act_type,               // 0: not act 1:ReLU 2:PreLU 3:sigmoid 4:swish
    int act_op,                 // prelu[scale<<8|trunc] coef = scale/(2^trunc)

    int operation,              // 0:PROD, 1:SUM, 2:MAX
    int a_coeff,                //  1  only is 1
    int b_coeff                 // -1:A-B 1:A+B
);

// inner product
void nna_inner_product(
    unsigned int output_addr,   // memory address of output data
    unsigned int input_addr,    // memory address of input data
    unsigned int weight_addr,   // memory address of weight data
    unsigned int bias_addr,     // memory address of bias data

    int output_shift,           // [alu_shift<<8|mul_trunc] (output_data<<alu_shift)>>mul_trunc
    int bias_shift,             // [alu_shift<<8|mul_trunc] (input_data+(bias data<<alu_shift input))>>mul_trunc
    int input_w,                // input data width
    int input_h,                // input data height
    int input_c,                // input data channel

    int num_output,             // number of output data channel
    int bias_term              // bias term 0:no bias 1: bias 2: batch normalization
);

// prelu
void nna_act(
    unsigned int output_addr,   // memory address of output data
    unsigned int input_addr,    // memory address of input data

    int output_shift,           // output data left shift bits & truncation bits [alu_shift<<8|mul_trunc]
    int input_w,                // input data width
    int input_h,                // input data height
    int input_c,                // input data channel

    int act_type,               // 0: not act 1:ReLU 2:PreLU 3:sigmoid 4:swish
    int act_op                  // prelu[scale<<8|trunc] coef = scale/(2^trunc)
);

// pooling
void nna_pool(
    unsigned int output_addr,   // memory address of output data
    unsigned int input_addr,    // memory address of input data

    int input_w,                // input data width
    int input_h,                // input data height
    int input_c,                // input data channel

    int pool_type,              // 0:avg 1:max, 1:min
    int kernel,                 // pooling kernel size
    int stride,                 // pooling setide size
    int pad,                    // pooling pading size
    int pad_value,              // pooling pading value
    int pad_mode                // 0:full, 1:valid, 2:same
);


// eltwise product or sum
void nna_eltwise(
    unsigned int output_addr,   // memory address of output data
    unsigned int input_a_addr,  // memory address of input data A  output_data = A*B or A+B or MAX(A,B)
    unsigned int input_b_addr,  // memory address of input data A  output_data = A*B or A+B or MAX(A,B)

    int output_shift,           // truncate
    int input_w,                // input data width
    int input_h,                // input data height
    int input_c,                // input data channel

    int operation,              // 0:PROD, 1:SUM, 2:MAX
    int a_coeff,                // 1:  only is 1
    int b_coeff                 // -1:A-B 1:A+B
);

// LRN
void nna_lrn(
    unsigned int output_addr,   // memory address of output data
    unsigned int input_addr,    // memory address of input data

    int input_w,                // input data width
    int input_h,                // input data height
    int input_c,                // input data channel

    int local_size,
    int alpha_scale,            // alpha=scale/(2^trunc)
    int alpha_trunc,            // alpha=scale/(2^trunc)
    int beta_scale,             // beta =scale/(2^trunc)
    int beta_trunc              // beta =scale/(2^trunc)
);

// detection box
struct nna_fd_bbox {
    int score;                  // bbox score
    int x1;                     // bbox left
    int y1;                     // bbox top
    int x2;                     // bbox right
    int y2;                     // bbox bottom
};


// NNA initiation
int nna_init(
    unsigned int nna_base       // nna hw physical address for mmap to user space address & init the NNA
    );

// NNA close
int nna_close();                // unmmap the user space address & close the NNA


//NNA Face Detection run
int nna_fd_run(                 // return the bbox number
    char * img,                 // input image data of rgb format
    int w,                      // input image width  w<=1920
    int h,                      // input image height h<=1080
    struct nna_fd_bbox  *bbox,  // output the bbox buffer
    int max_num_bbox             // bbox buffer max size
    );

//NNA Human Detection run
int nna_hd_run(                 // return the bbox number
    char * img,                 // input image data of rgb format
    int w,                      // input image width  w<=1920
    int h,                      // input image height h<=1080
    struct nna_fd_bbox  *bbox,  // output the bbox buffer
    int max_num_bbox            // bbox buffer max size
    );

int nna_diff_file(char *path, char *path2);
int nna_save_ram(unsigned int  target, int file_sz, char *path);
int nna_load_ram(unsigned int  target, char *path);

typedef void (*NNA_RESET_FUNC)(void);
typedef void (*NNA_RESIZE_FUNC)(unsigned int dst_addr, int dstw, int dsth, unsigned int src_addr, int srcw, int srch, unsigned int *box_list, int box_num);
typedef void (*NNA_WAIT_EVENT_FUNC)(void);

// physical address for NNA used input & output
void nna_setup_buffer(unsigned int buf_base_nna, uint64_t buf_base_cpu,int buf_size);
void nna_setup_reset_handler(NNA_RESET_FUNC func);
void nna_setup_resize_handler(NNA_RESIZE_FUNC func);
void nna_setup_wait_event_handler(NNA_WAIT_EVENT_FUNC func);
void nna_clean_interrupt(void);
int nna_get_status(void);

int nna_wait_event_done(void);

void nna_resize(
    unsigned int dst_addr,
    int dstw,
    int dsth,
    unsigned int src_addr,
    int srcw,
    int srch,
    unsigned int *box_list,      //x1 y1 x2 y2
    int box_num
    );

void nna_resize_cpu(
    unsigned int dst_addr,
    int dstw,
    int dsth,
    unsigned int src_addr,
    int srcw,
    int srch,
    unsigned int *box_list,     //x1 y1 x2 y2
    int box_num
    );


void nna_img_resize_cpu(
    unsigned int dst_addr,
    int dstw,
    int dsth,
    unsigned char * src_addr,
    int srcw,
    int srch
);


void nna_reset(void );


// NNA resize image of 3 channals
void nna_img_resize_c3(
    const unsigned char *src,   // source image buffer
    int srcw,                   // source image width
    int srch,                   // source image height
    unsigned char *dst,         // destination image buffer
    int w,                      // destination image width
    int h                       // destination image height
    );

// NNA resize image of 1 channals
void nna_img_resize_c1(
    const unsigned char *src,
    int srcw,
    int srch,
    unsigned char *dst,
    int w,
    int h);

void nna_img_cut_c1(
    const unsigned char *src,
    int srcw,
    int srch,
    unsigned char *dst,
    int x1,
    int y1,
    int x2,
    int y2);

int xramcpy(off_t target, off_t target2, int file_sz, int access_type);
unsigned int nna_get_buffer_viraddr(unsigned int buf_phyaddr);

#ifdef __cplusplus
}
#endif

#endif


