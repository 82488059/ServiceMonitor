
#pragma pack( push, 1 )
struct adpcm_stream_head	// ADPCM ������ͷ
{
	char flag[5];						// ADPCM
	char channel;						// ������
	unsigned short int samples_per_sec;	// ������
	unsigned short int frame_size;		// ������֡��
	short int preval_l;					// ������ǰһ����ֵ
	short int preval_r;					// ������ǰһ����ֵ
	char index_l;						// ��������������ֵ
	char index_r;						// ��������������ֵ
};
#pragma pack( pop )

struct adpcm_state			// ADPCM ѹ������
{
	short valprev;	// Previous output value
	char index;		// Index into stepsize table
};

void adpcm_coder( short indata[], char outdata[], int len, struct adpcm_state *state );
void adpcm_decoder( char indata[], short outdata[], int len, struct adpcm_state * state );
