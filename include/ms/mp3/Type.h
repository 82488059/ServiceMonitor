#ifndef TYPE_H
#define TYPE_H

typedef struct 
{
    //FILE *file;
    int  type;                //����
    int  channels;            //ͨ����
    int  bits;                //����λ��
    long samplerate;          //������
} struWave;
/* ѹ��MP3���� */
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