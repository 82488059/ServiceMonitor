#ifndef MP3CODE_H
#define MP3CODE_H

#include <ms/mp3/Type.h>
#include <ms/mp3/const.h>

#define FIFO_SIZE 128


#pragma once

struct huffcodetab 
{
    unsigned int xlen;    /*max. x-index+                         */
    unsigned int ylen;    /*max. y-index+                         */
    unsigned int linbits; /*number of linbits                     */
    unsigned int linmax;  /*max number to be stored in linbits    */
    HUFFBITS *table;      /*pointer to array[xlen][ylen]          */
    unsigned char *hlen;  /*pointer to array[xlen][ylen]          */
};
typedef struct 
{
    unsigned part2_3_length;
    unsigned big_values;
    unsigned count1;
    unsigned global_gain;
    unsigned table_select[3];
    unsigned region0_count;
    unsigned region1_count;
    unsigned count1table_select;
    unsigned address1;
    unsigned address2;
    unsigned address3;
    int quantizerStepSize;
} gr_info;

typedef struct 
{
  int main_data_begin;
  struct
  {
    struct
    {
        gr_info tt;
    } ch[2];
  } gr[2];
  int resv_drain;
} L3_side_info_t;

/* utils.c */
long mul(int x, int y);
long muls(int x, int y);
long mulr(int x, int y);
long mulsr(int x, int y);

class Mp3Code
{
//数据成员
public:
	struMpeg m_Mpeg;
	struWave m_Wave;
public:
	struct
	{
		unsigned char iBufIndex,iRetBufIndex;
		unsigned long iBufCount;
		unsigned long iBufSize;
		unsigned char *pOutBuf[2];
	}OutData;
private:
	int      l3_enc[2][2][samp_per_frame2];
	long     l3_sb_sample[2][3][18][SBLIMIT];
    long     mdct_freq[2][2][samp_per_frame2];
	unsigned long *buffer[2];
    long     bytes_per_frame;
    L3_side_info_t side_info;
	int      *scalefac_band_long;
	int      m_iCutOff;

	long     x[2][HAN_SIZE],z[512],y[64];
	int      off[2];

	long     sideinfo_len;
    long     lag;
	long     remainder;
    int      mean_bits;
	//loop.c
	int      main_data_begin;
	long     *xr,                    /* magnitudes of the spectral values */
             xrabs[samp_per_frame2], /* xr absolute */
             xrmax;                  /* maximum of xrabs array */
	//bitfarmat
	unsigned char header[4];
	struct
    {
       int si_len; /* number of bytes in side info */
       int fr_len; /* number of data bytes in frame */
       unsigned char side[40]; /* side info max len = (288 bits / 8) + 1 which gets cleared */
    } fifo[FIFO_SIZE];
	unsigned char main_[4096];  /* main data buffer (length always less than this) */
    int wr, rd; /* read and write index for side fifo */
    int by, bi; /* byte and bit counts within main or side stores */
    int count;  /* byte counter within current frame */
    int bits;   /* bit counter used when encoding side and main data */

//方法
public:
	Mp3Code(void);
	virtual ~Mp3Code(void);
	int SetParameter(int tWaveBits, int tWaveType, int tWaveSampleRate, int tMp3BitRate);
	int EncodeAnFrame(unsigned long * pInBuf, unsigned char ** pOutBuf);
	int EndData(void);
	void Reset(void);
	void SetOutBufAddr(unsigned int iBufNo, unsigned long iBufSize, char * pBufAddr);
private:
	void L3_window_filter_subband(unsigned long ** pBuffer, long s[SBLIMIT], int k);
	//Loop.cpp
	void L3_iteration_loop(long            mdct_freq_org[2][2][samp_per_frame2],
                       L3_side_info_t *side_info,
                       int             l3_enc[2][2][samp_per_frame2],
                       int             mean_bits);
	int quantize(int ix[samp_per_frame2], int stepsize );
	int inner_loop(int ix[samp_per_frame2],int max_bits,gr_info *gi);
	int ix_max( int ix[samp_per_frame2], unsigned int begin, unsigned int end );
	void calc_runlen( int ix[samp_per_frame2], gr_info *gi );
	int count1_bitcount(int ix[samp_per_frame2], gr_info *gi);
	void bigv_tab_select( int ix[samp_per_frame2], gr_info *gi );
	int new_choose_table( int ix[samp_per_frame2], unsigned int begin, unsigned int end );
	int bigv_bitcount(int ix[samp_per_frame2], gr_info *gi);
	void subdivide(gr_info *gi);
	int count_bit(int ix[samp_per_frame2],
              unsigned int start,
              unsigned int end,
              unsigned int table );
	int encodeMainData(int l3_enc[2][2][samp_per_frame2],L3_side_info_t  *si);
	void encodeSideInfo( L3_side_info_t  *si );
	void L3_format_bitstream( int l3_enc[2][2][samp_per_frame2], L3_side_info_t  *l3_side);
	void L3_mdct_sub(long sb_sample[2][3][18][SBLIMIT], long mdct_freq[2][2][samp_per_frame2]);
	void Huffmancodebits( int *ix, gr_info *gi );
	int putbytes(unsigned char *data, int n);
	int L3_huffman_coder_count1( struct huffcodetab *h, int v, int w, int x, int y );
	int HuffmanCode(int table_select, int x, int y, unsigned long *code,
                unsigned long *ext, int *cbits, int *xbits );
	void putbits (unsigned char *buf, unsigned long val, unsigned int n);
	int setbytes(unsigned char data, int n);
	int find_samplerate_index(long freq);
	int find_bitrate_index(int bitr);
};

#endif