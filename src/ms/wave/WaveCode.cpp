#include "StdAfx.h"
#include <ms/wave/WaveCode.h>
#include <windows.h>

CWaveCode::CWaveCode(void)
{
	m_lpuLawTab = (unsigned char *)::GlobalAlloc( GPTR, 0x10000 );
	Make_uLawTab();
}

CWaveCode::~CWaveCode(void)
{
	if( m_lpuLawTab )	::GlobalFree( m_lpuLawTab );
}
static short int seg_end[8] = { 0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF };
#define	BIAS		(0x84)		// Bias for linear code.
#define	QUANT_MASK	0x0f
#define	SEG_MASK	0x70
#define	SEG_SHIFT	0x04
#define	SIGN_BIT	0x80

// 查表压缩
void CWaveCode::Encoder_uLaw(short int * lpWav, int nLen, unsigned char *uLaw_buf)
{
	for( int i=0; i<nLen/2; i++ )
	{
		unsigned short int pcm_val = lpWav[i];
		uLaw_buf[i] = m_lpuLawTab[ pcm_val ];
	}
}
// uLaw 压缩
inline short int s_search( short int val, short int * table, short int size )
{
	for( short int i = 0; i < size; i++ )
	{
		if( val <= *table++ )	return i;
	}
	return size;
}
void CWaveCode::Make_uLawTab()
{
	for( int i=0; i<0x10000; i++ )	// 2个字节一压缩
	{
		short int pcm_val = (short)i;
		int		mask;
		int		seg;
		unsigned char	uval;

		// Get the sign and the magnitude of the value.
		if( pcm_val < 0)
		{
			pcm_val = BIAS - pcm_val;
			mask = 0x7F;
		}
		else
		{
			pcm_val += BIAS;
			mask = 0xFF;
		}

		// Convert the scaled magnitude to segment number.
		seg = s_search( pcm_val, seg_end, 8 );

		// Combine the sign, segment, quantization bits;
		// and complement the code word.

		if( seg >= 8 )		// out of range, return maximum value.
			uval = 0x7F ^ mask;
		else
		{
			uval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0xF);
			uval = uval ^ mask;
		}
		m_lpuLawTab[i] = uval;
	}
}
// uLaw 解压缩
void CWaveCode::Decoder_uLaw(const unsigned char * pulaw, int nLen, short int * pWave)
{
	for( int i=0; i<nLen; i++ )
	{
		unsigned char u_val = pulaw[i];
		int		t;

		// Complement to obtain normal u-law value.
		u_val = ~u_val;

		// Extract and bias the quantization bits. Then
		// shift up by the segment number and subtract out the bias.

		t = ((u_val & QUANT_MASK) << 3) + BIAS;
		t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;

		pWave[i] = ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
	}
}
bool CWaveCode::IsQuiet( short * lpWav, int nLen )
{
	for( int i=0; i<nLen; i++ )
	{
		if( (lpWav[i] > 600) || (lpWav[i] < -600) )	return false;
	}

	return true;
}
