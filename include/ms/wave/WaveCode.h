#pragma once

class CWaveCode
{
public:
	CWaveCode(void);
	~CWaveCode(void);
private:
	unsigned char *m_lpuLawTab;		// uLaw ±í

	bool IsQuiet( short * lpWav, int nLen );
	void Make_uLawTab();
public:
	void Encoder_uLaw(short int * lpWav, int nLen, unsigned char *uLaw_buf);
	void Decoder_uLaw(const unsigned char * pulaw, int nLen, short int * pWave);
};
