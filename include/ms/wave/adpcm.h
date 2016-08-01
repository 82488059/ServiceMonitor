
#pragma pack( push, 1 )
struct adpcm_stream_head	// ADPCM 数据流头
{
	char flag[5];						// ADPCM
	char channel;						// 声道数
	unsigned short int samples_per_sec;	// 采样率
	unsigned short int frame_size;		// 单声道帧长
	short int preval_l;					// 左声道前一采样值
	short int preval_r;					// 右声道前一采样值
	char index_l;						// 左声道步长索引值
	char index_r;						// 右声道步长索引值
};
#pragma pack( pop )

struct adpcm_state			// ADPCM 压缩索引
{
	short valprev;	// Previous output value
	char index;		// Index into stepsize table
};

void adpcm_coder( short indata[], char outdata[], int len, struct adpcm_state *state );
void adpcm_decoder( char indata[], short outdata[], int len, struct adpcm_state * state );
