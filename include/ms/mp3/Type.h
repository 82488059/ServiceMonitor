#ifndef TYPE_H
#define TYPE_H

typedef struct 
{
    //FILE *file;
    int  type;                //类型
    int  channels;            //通道数
    int  bits;                //采样位数
    long samplerate;          //采样率
} struWave;
/* 压缩MP3参数 */
typedef struct 
{
    //FILE *file;
    int  type;
    int  layr;
    int  mode;
    int  bitr;
    int  psyc;
    int  emph;
    int  padding;
    long samples_per_frame;
    long bits_per_frame;
    long bits_per_slot;
    long total_frames;
    int  bitrate_index;
    int  samplerate_index;
    int  crc;
    int  ext;
    int  mode_ext;
    int  copyright;
    int  original;
    int  channels;
    int  granules;
    int  resv_limit;
	
} struMpeg;


#endif