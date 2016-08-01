#include "StdAfx.h"
#include <ms/mp3/CodeTable.h>
#include <ms/mp3/Mp3Code.h>
#include <memory.h>

/* Scalefactor bands. */
static int sfBandIndex[4][3][23] =
{
  { /* MPEG-2.5 11.025 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
    /* MPEG-2.5 12 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
    /* MPEG-2.5 8 kHz */
    {0,12,24,36,48,60,72,88,108,132,160,192,232,280,336,400,476,566,568,570,572,574,576}
  },
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },
  { /* Table B.2.b: 22.05 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
    /* Table B.2.c: 24 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,114,136,162,194,232,278,332,394,464,540,576},
    /* Table B.2.a: 16 kHz */
    {0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576}
  },
  { /* Table B.8.b: 44.1 kHz */
    {0,4,8,12,16,20,24,30,36,44,52,62,74,90,110,134,162,196,238,288,342,418,576},
    /* Table B.8.c: 48 kHz */
    {0,4,8,12,16,20,24,30,36,42,50,60,72,88,106,128,156,190,230,276,330,384,576},
    /* Table B.8.a: 32 kHz */
    {0,4,8,12,16,20,24,30,36,44,54,66,82,102,126,156,194,240,296,364,448,550,576}
  }
};
static int cutoff_tab[3][2][15] =
  {
    { /* 44.1k, 22.05k, 11.025k */
      {100,104,131,157,183,209,261,313,365,418,418,418,418,418,418}, /* stereo */
      {183,209,261,313,365,418,418,418,418,418,418,418,418,418,418}  /* mono */
    },
    { /* 48k, 24k, 12k */
      {100,104,131,157,183,209,261,313,384,384,384,384,384,384,384}, /* stereo */
      {183,209,261,313,365,384,384,384,384,384,384,384,384,384,384}  /* mono */
    },
    { /* 32k, 16k, 8k */
      {100,104,131,157,183,209,261,313,365,418,522,576,576,576,576}, /* stereo */
      {183,209,261,313,365,418,522,576,576,576,576,576,576,576,576}  /* mono */
    }
  };
int Get_cutoff(int tSampleRateIndex,int tMode,int tBitRateIndex)
{
	return cutoff_tab[tSampleRateIndex]
                   [tMode == MODE_MONO]
                   [tBitRateIndex];
}
/************************************************************************************/
/*    MP3Code 函数                                                                  */
/************************************************************************************/
Mp3Code::Mp3Code(void)
{
    SetParameter(16,MODE_STEREO,16000,128);
	Reset();
}

Mp3Code::~Mp3Code(void)
{
}
void Mp3Code::Reset(void)
{
	int i,j,k,m;

	for(i=0;i<2;i++)
		for(j=0;j<2;j++)
			for(k=0;k<samp_per_frame2;k++)
			{
				l3_enc[i][j][k] = 0;
				mdct_freq[i][j][k] = 0;
			}


	for(i=0;i<2;i++)
		for(j=0;j<3;j++)
			for(k=0;k<18;k++)
				for(m=0;m<SBLIMIT;m++)
				    l3_sb_sample[i][j][k][m] = 0;

    bytes_per_frame = 0;

	memset(&side_info,0,sizeof(L3_side_info_t));

	scalefac_band_long = NULL;

	sideinfo_len = 0;
    
	for(i=0;i<2;i++)
		for(j=0;j<HAN_SIZE;j++)
	        x[i][j] = 0;

	for(i=0;i<512;i++) 
		z[i] = 0;
    for(i=0;i<64;i++)
	    y[i] = 0;

	off[0] = off[1] = 0;

	main_data_begin = 0;
	xr = NULL;
	for(i=0;i<samp_per_frame2;i++)
        xrabs[i] = 0;
	xrmax = 0;

	memset(&fifo,0,sizeof(fifo[FIFO_SIZE]));

    OutData.iBufIndex = 0;
	OutData.iBufCount = 0;
	OutData.iRetBufIndex = 0;
	wr = rd = 0; /* read and write index for side fifo */
    by = bi = 0; /* byte and bit counts within main or side stores */
    count   = 0; /* byte counter within current frame */
    bits    = 0; /* bit counter used when encoding side and main data */
}

/*
 * find_samplerate_index:
 * ----------------------
 */
int Mp3Code::find_samplerate_index(long freq)
{
  static long sr[4][3] = {{11025, 12000,  8000},   /* mpeg 2.5 */
                          {    0,     0,     0},   /* reserved */
                          {22050, 24000, 16000},   /* mpeg 2 */
                          {44100, 48000, 32000}};  /* mpeg 1 */
  int i, j;

  for(j=0; j<4; j++)
    for(i=0; i<3; i++)
      if((freq == sr[j][i]) && (j != 1))
      {
          m_Mpeg.type = j;
          return i;
      }

  return -1;
}
/*
 * find_bitrate_index:
 * -------------------
 */
int Mp3Code::find_bitrate_index(int bitr)
{
  static long br[2][15] =
    {{0, 8,16,24,32,40,48,56, 64, 80, 96,112,128,144,160},   /* mpeg 2/2.5 */
     {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320}};  /* mpeg 1 */
  int i;

  for(i=1; i<15; i++)
    if(bitr==br[m_Mpeg.type & 1][i]) 
		return i;

  return -1;
}

int Mp3Code::SetParameter(int tWaveBits, int tWaveType, int tWaveSampleRate, int tMp3BitRate)
{
	memset(&m_Mpeg,0,sizeof(struMpeg));
	/*初始化Wave参数*/
	m_Wave.bits = tWaveBits;
	m_Wave.channels = 2;
	m_Wave.samplerate = tWaveSampleRate;
	m_Wave.type = tWaveType;
	
	/*初始化MP3压缩参数*/
	m_Mpeg.type = MPEG1;
    m_Mpeg.layr = LAYER_3;
    m_Mpeg.mode = MODE_DUAL_CHANNEL;
    m_Mpeg.bitr = tMp3BitRate;
    m_Mpeg.psyc = 0;
    m_Mpeg.emph = 0;
    m_Mpeg.crc  = 0;
    m_Mpeg.ext  = 0;
    m_Mpeg.mode_ext  = 0;
    m_Mpeg.copyright = 0;
    m_Mpeg.original  = 1;
    m_Mpeg.channels = 2;
    m_Mpeg.granules = 2;

	m_Mpeg.samplerate_index = find_samplerate_index(m_Wave.samplerate);
    m_Mpeg.bitrate_index    = find_bitrate_index(m_Mpeg.bitr);
	if(m_Mpeg.samplerate_index == -1 || m_Mpeg.bitrate_index == -1)
		return -1;
	m_iCutOff = Get_cutoff(m_Mpeg.samplerate_index,m_Wave.type,m_Mpeg.bitrate_index);

	scalefac_band_long = sfBandIndex[m_Mpeg.type][m_Mpeg.samplerate_index];


	if(m_Mpeg.type == MPEG1)
    {
       m_Mpeg.granules = 2;
       m_Mpeg.samples_per_frame = samp_per_frame;
       m_Mpeg.resv_limit = ((1<<9)-1)<<3;
       sideinfo_len = (m_Mpeg.channels == 1) ? 168 : 288;
    }
    else /* mpeg 2/2.5 */
    {
       m_Mpeg.granules = 1;
       m_Mpeg.samples_per_frame = samp_per_frame2;
       m_Mpeg.resv_limit = ((1<<8)-1)<<3;
       sideinfo_len = (m_Mpeg.channels == 1) ? 104 : 168;
    }

	{ /* find number of whole bytes per frame and the remainder */
       long x = m_Mpeg.samples_per_frame * m_Mpeg.bitr * (1000/8);
       bytes_per_frame = x / m_Wave.samplerate;
       remainder  = x % m_Wave.samplerate;
	   lag = 0;
    }

	header[0] = 0xff;

    header[1] = 0xe0 |
                (m_Mpeg.type << 3) |
                (m_Mpeg.layr << 1) |
                !m_Mpeg.crc;
 
    header[2] = (m_Mpeg.bitrate_index << 4) |
                (m_Mpeg.samplerate_index << 2) |
                /*m_Mpeg.padding inserted later */
                 m_Mpeg.ext;

    header[3] = (m_Mpeg.mode << 6) |
                (m_Mpeg.mode_ext << 4) |
                (m_Mpeg.copyright << 3) |
                (m_Mpeg.original << 2) |
                 m_Mpeg.emph;

	return 0;
}

int Mp3Code::EncodeAnFrame(unsigned long * pInBuf, unsigned char **pOutBuf)
{
	int  ch,i,gr;
	unsigned long *buffer[2];
	int  iRetIndex = -1;

    buffer[0] = buffer[1] = pInBuf;

	/* sort out padding */
    m_Mpeg.padding = (lag += remainder) >= m_Wave.samplerate;
    if (m_Mpeg.padding)
      lag -= m_Wave.samplerate;
    m_Mpeg.bits_per_frame = 8*(bytes_per_frame + m_Mpeg.padding);

    /* bits per channel per granule */
    mean_bits = (m_Mpeg.bits_per_frame - sideinfo_len) >>
                      (m_Mpeg.granules + m_Mpeg.channels - 2);


	/* polyphase filtering */
    for(gr=0; gr<m_Mpeg.granules; gr++)
       for(ch=0; ch<m_Mpeg.channels; ch++)
          for(i=0;i<18;i++)
		      L3_window_filter_subband(&(buffer[ch]), &l3_sb_sample[ch][gr+1][i][0] ,ch);

	/* apply mdct to the polyphase output */
    L3_mdct_sub(l3_sb_sample, mdct_freq);

	/* bit and noise allocation */
    L3_iteration_loop(mdct_freq, &side_info, l3_enc, mean_bits);

    /* write the frame to the bitstream */
    L3_format_bitstream(l3_enc, &side_info);

	if(OutData.iRetBufIndex != OutData.iBufIndex)
	{
       iRetIndex = OutData.iRetBufIndex;
	   OutData.iRetBufIndex = OutData.iBufIndex;
	   *pOutBuf = OutData.pOutBuf[iRetIndex];
	}
	
	return iRetIndex;
}

void Mp3Code::L3_window_filter_subband(unsigned long ** pBuffer, long s[SBLIMIT], int k)
{
   //static int off[2];
   long s1,s2;
   int i,j;
   unsigned long *pBuf = * pBuffer;

   /* replace 32 oldest samples with 32 new samples */

   /* data format depends on mode */
   if(m_Mpeg.channels == 1)
   {
      if(m_Wave.channels == 2)
      { /* mono from stereo, sum upper and lower */
         for (i=31;i>=0;i--)
         {
            x[k][i+off[k]] = (((*pBuf) >> 16) + (((*pBuf) << 16)>>16)) << 15;
            pBuf++;

         }
      }
      else
      {  /* mono data, use upper then lower */
         for (i=15;i>=0;i--)
         {
            x[k][(2*i)+off[k]+1] = (*pBuf) << 16;
            x[k][(2*i)+off[k]] = ((*pBuf) >> 16) << 16;
			pBuf++;
         }
      }
   }
   else if(k)
   {  /* stereo left, use upper */
      for (i=31;i>=0;i--)
	  {
	     x[k][i+off[k]] = (*pBuf) & 0xffff0000;
		 pBuf++;
	  }
   }
   else
   {  /* stereo right, use lower */
      for (i=31;i>=0;i--)
	  {
         //x[k][i+off[k]] = (*(*buffer)++) << 16;
		  x[k][i+off[k]] = (*pBuf) << 16;
		  pBuf++;
	  }
   }

   /* shift samples into proper window positions, and window data */
   for (i=HAN_SIZE; i--; )
      z[i] = mul(x[k][(i+off[k])&(HAN_SIZE-1)],ew[i]);

   off[k] = (off[k] + 480) & (HAN_SIZE-1); /* offset is modulo (HAN_SIZE)*/

   /* sub sample the windowed data */
   for (i=64; i--; )
      for (j=8, y[i] = 0; j--; )
         y[i] += z[i+(j<<6)];

   /* combine sub samples for the simplified matrix calculation */
   for (i=0; i<16; i++)
      y[i+17] += y[15-i];
   for (i=0; i<15; i++)
      y[i+33] -= y[63-i];

   /* simlplified polyphase filter matrix multiplication */
   for (i=16; i--; )
   {
     for (j=0, s[i]= 0, s[31-i]=0; j<32; j += 2)
     {
        s1 = mul(fl[i][j],y[j+16]);
        s2 = mul(fl[i][j+1],y[j+17]);
        s[i] += s1 + s2;
        s[31-i] += s1 - s2;
     }
   }
   * pBuffer = pBuf;
}

void Mp3Code::L3_mdct_sub(long sb_sample[2][3][18][SBLIMIT], long mdct_freq[2][2][samp_per_frame2])
{
	/* note. we wish to access the array 'mdct_freq[2][2][576]' as
   * [2][2][32][18]. (32*18=576),
   */
  long (*mdct_enc)[18];

  int  ch,gr,band,j,k;
  long mdct_in[36];
  long bu,bd,*m;

  for(gr=0; gr<m_Mpeg.granules; gr++)
    for(ch=m_Mpeg.channels; ch--; )
    {
      /* set up pointer to the part of mdct_freq we're using */
      mdct_enc = (long (*)[18]) mdct_freq[gr][ch];

      /* Compensate for inversion in the analysis filter
       * (every odd index of band AND k)
       */
      for(band=1; band<=31; band+=2 )
        for(k=1; k<=17; k+=2 )
          sb_sample[ch][gr+1][k][band] *= -1;

      /* Perform imdct of 18 previous subband samples + 18 current subband samples */
      for(band=32; band--; )
      {
        for(k=18; k--; )
        {
          mdct_in[k]    = sb_sample[ch][ gr ][k][band];
          mdct_in[k+18] = sb_sample[ch][gr+1][k][band];
        }

        /* Calculation of the MDCT
         * In the case of long blocks ( block_type 0,1,3 ) there are
         * 36 coefficients in the time domain and 18 in the frequency
         * domain.
         */
        for(k=18; k--; )
        {
          m = &mdct_enc[band][k];
          for(j=36, *m=0; j--; )
            *m += mul(mdct_in[j],cos_l[k][j]);
        }
      }

      /* Perform aliasing reduction butterfly */
      for(band=31; band--; )
        for(k=8; k--; )
        {
          /* must left justify result of multiplication here because the centre
           * two values in each block are not touched.
           */
          bu = muls(mdct_enc[band][17-k],cs[k]) + muls(mdct_enc[band+1][k],ca[k]);
          bd = muls(mdct_enc[band+1][k],cs[k]) - muls(mdct_enc[band][17-k],ca[k]);
          mdct_enc[band][17-k] = bu;
          mdct_enc[band+1][k]  = bd;
        }
    }

  /* Save latest granule's subband samples to be used in the next mdct call */
  for(ch=m_Mpeg.channels ;ch--; )
    for(j=18; j--; )
      for(band=32; band--; )
        sb_sample[ch][0][j][band] = sb_sample[ch][m_Mpeg.granules][j][band];
}


void Mp3Code::SetOutBufAddr(unsigned int iBufNo, unsigned long iBufSize, char * pBufAddr)
{
	if(iBufNo<2)
	{
		OutData.pOutBuf[iBufNo] = (unsigned char *)pBufAddr;
		OutData.iBufSize = iBufSize;
	}
}
