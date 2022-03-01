/*
 **********************************************************************************
 *                             name: srfmain.h
 *
 *                             function:gbktoutf8 & srf
 *
 *                             version:3.0
 *
 *                             Author: CX@AL2, 28-02-2011
 ************************************************************************************
 */

#define SRF_PATH  BEETLES_APP_ROOT"apps\\srf\\"
#define SRFINSIZE 512//请勿改动512并保证输入接口的拼音的字母长度小于32
#define SRFOUTSIZE 10240//请勿改动10240并保证输入接口的拼音的字母长度小于32


typedef struct
{
    int   srfmode; //0表示拼音输入法，1表示T9常规输入法，2表示T9高效输入法，3表示笔画输入法，默认为0.
    int   jiantiorfanti; //简体还是繁体，0表示简体，1表示繁体。默认为0。
    int   cikuuse;        //是否使用用户词库，0表示使用，1表示不使用。默认为0,假若不适用也将不保存用户输入到用户词库中。
    int   wordsassociation; //是否支持联想功能，0表示支持，1表示不支持。默认为0.
    int   otherset ;//未来可能会附加上的设置，现在为0，为0表示没有其他设置。
} SRF_SET_T;


typedef struct
{
    SRF_SET_T    *srf_set; //用户设置
    unsigned char    inputstring[SRFINSIZE]; //输入法输入的字符串
    unsigned char    inputstringlen;//输入实际长度
    int   inputstringmore[SRFINSIZE]; //额外的输入内容，比如T9的拼音串。T9的数字串在inputstring中存放。  大小为32.
    unsigned char    inputstringmorelen;
} SRFINPUT_T;


typedef struct
{
    unsigned char    outputstring[SRFOUTSIZE];//包含输出的信息，每个拼音串对应一个汉字串。在每组拼音汉字组合之前有一个值表示这个词是由多少个拼音得出的。比如shenmesh 则会有8shenmeshi什么事  6shenme 什么，当然数字值是一个CHAR值，小于输入长度32。SRFOUTSIZE大小为5000.
    int    outputstringlen;   //输出长度
    unsigned char    outputstringmore[SRFOUTSIZE];//另外一些其他的输出信息，假如与outputstring有所区别，将在这里输出。比如汉字串在T9中对应的数字串。
    int    outputstringmorelen;
} SRFOUTPUT_T;



//输入法接口
int srf_main(SRFINPUT_T  *srf_input, SRFOUTPUT_T  *srf_output);

void srf_gbk2utf8(unsigned char *gbktxt, unsigned char *utf8txt); //gbk转utf8
//gb2utf.txt文件路径在BEETLES_APP_ROOTapps\\gb2utf.txt

