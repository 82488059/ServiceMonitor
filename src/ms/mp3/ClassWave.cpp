#include "StdAfx.h"
#include <ms/mp3/ClassWave.h>
#include <string.h>
CWave::CWave(void)
{
	m_strFileName = NULL;
}


CWave::~CWave(void)
{
	if(m_hFile)
		fclose(m_hFile);
}

bool CWave::Open(char *pWaveFileName)
{
	/* open input file */
//     if((m_hFile = fopen(pWaveFileName,"rb")) == NULL)
//        return false;
	errno_t err = fopen_s(&m_hFile, pWaveFileName, "rb");
	if (err != 0 || !m_hFile)
		return false;

	if (fread(&m_Header, sizeof(struct wave_header), 1, m_hFile) != 1)
        return false;
    if(strncmp(m_Header.riff,"RIFF",4) != 0) return false;
    if(strncmp(m_Header.wave,"WAVE",4) != 0) return false;
    if(strncmp(m_Header.fmt, "fmt ",4) != 0) return false;
    if(m_Header.tag != 1)                    return false;
    if(m_Header.channels > 2)                return false;
    if(m_Header.bit_samp != 16)              return false;
    //if(strncmp(m_Header.data,"data",4) != 0) return false;

	return true;
}

int CWave::WaveGet(unsigned long * pBuf, unsigned long iBufferSize)
{
	int iCount;
	if(!m_hFile)
		return 0;
	iCount = (int)fread(pBuf,sizeof(unsigned long),iBufferSize,m_hFile);
	return iCount;
}

unsigned long * CWave::GetAnFrame(long samples_per_frame)
{
  int iFrameLen,iReadCount;

  //iFrameLen = m_Header.channels == 1 ? samp_per_frame:samp_per_frame2;
  iFrameLen = samples_per_frame >> (2 - m_Header.channels);
  iReadCount = (int)fread(m_pBuf,sizeof(unsigned long),iFrameLen,m_hFile);
  if(!iReadCount)
    return NULL;
  else
  {
    for(; iReadCount < iFrameLen; iReadCount++)
      m_pBuf[iReadCount]=0;
    return m_pBuf;
  }
}

