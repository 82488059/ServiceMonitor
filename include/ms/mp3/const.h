#ifndef CONST_H
#define CONST_H
/***************************************************************/
/*        常量定义                                             */
/***************************************************************/
#define MAX_SAMPLE_CHANEL 4                      //最大音频压缩路数

#define MPEG2_5           0                      //MPEG2.5
#define MPEG2             2                      //MPEG2
#define MPEG1             3                      //MPEG1    

#define MODE_STEREO       0                      //立体声
#define MODE_MS_STEREO    1                      //MS立体声
#define MODE_DUAL_CHANNEL 2                      //双声道
#define MODE_MONO         3                      //单声道

#define LAYER_3 1
#define LAYER_2 2
#define LAYER_1 3
#define samp_per_frame    1152                   //双声道每帧数据长度
#define samp_per_frame2   576                    //单声道每帧数据长度
#define PI                3.14159265358979
#define HAN_SIZE          512
#define SBLIMIT           32

#define HUFFBITS          unsigned long int
#define HTN               34
#define MXOFF             250

#endif