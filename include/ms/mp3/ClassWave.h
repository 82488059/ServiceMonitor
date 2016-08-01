#ifndef ClASSWAVE_H
#define ClASSWAVE_H
#include "const.h"
#pragma once
struct wave_header
{
    char riff[4];             /* "RIFF" */
    unsigned long size;       /* length of rest of file = size of rest of header(36) + data length */
    char wave[4];             /* "WAVE" */
    char fmt[4];              /* "fmt " */
    unsigned long fmt_len;    /* length of rest of fmt chunk = 16 */
    unsigned short tag;       /* MS PCM = 1 */
    unsigned short channels;  /* channels, mono = 1, stereo = 2 */
    unsigned long samp_rate;  /* samples per second = 44100 */
    unsigned long byte_rate;  /* bytes per second = samp_rate * byte_samp = 176400 */
    unsigned short byte_samp; /* block align (bytes per sample) = channels * bits_per_sample / 8 = 4 */
    unsigned short bit_samp;  /* bits per sample = 16 for MS PCM (format specific) */
    char data[4];             /* "data" */
    unsigned long length;     /* data length (bytes) */
};

class CWave
{
//数据成员
public:
    FILE *m_hFile;
	char *m_strFileName;
    struct wave_header m_Header;
	unsigned long m_pBuf[samp_per_frame];
//方法
public:
	CWave(void);
	~CWave(void);
	bool Open(char *pWaveFileName);
	int WaveGet(unsigned long * pBuf, unsigned long iBufferSize);
	unsigned long * GetAnFrame(long samples_per_frame);
	
};
#endif