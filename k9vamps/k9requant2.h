#include "k9fifo.h"

#ifndef K9REQUANT2_H
#define K9REQUANT2_H


class k9Requant2 
{
public:
	void setInput(char *data,int size);
	void setOutput(char*data,int size);
	uint64_t getOutByteCnt() { return outbytecnt-outbytecnt1;};
	void setFactor(float fact);
	void run();	
	k9Requant2();
	~k9Requant2();
private:

	char *m_dataIn,*m_dataOut;
	int m_sizeIn,m_sizeOut;

	// mpeg2 state
	// seq header
	uint horizontal_size_value;
	uint vertical_size_value;

	// pic header
	uint picture_coding_type;

	// pic code ext
	uint f_code[2][2];
	uint intra_dc_precision;
	uint picture_structure;
	uint frame_pred_frame_dct;
	uint concealment_motion_vectors;
	uint q_scale_type;
	uint intra_vlc_format;
	uint alternate_scan;

	// error
	int validPicHeader;
	int validSeqHeader;
	int validExtHeader;
	int sliceError;

	// slice or mb
	uint quantizer_scale;
	uint new_quantizer_scale;
	uint last_coded_scale;
	int h_offset, v_offset;

	// rate
	double quant_corr;

	// block data
	typedef struct
	{
		uint8_t run;
		short level;
	} RunLevel;
	RunLevel block[6][65]; // terminated by level = 0, so we need 64+1
	// end mpeg2 state
	// global variables
	uint8_t	*cbuf, *rbuf, *wbuf, *orbuf, *owbuf;
	int		inbitcnt, outbitcnt;
	uint32_t	inbitbuf, outbitbuf;
	uint64_t	inbytecnt, outbytecnt,outbytecnt1;
	float	fact_x;
	int		mloka1;

	/////---- begin ext mpeg code

private:
	void putbits(uint val, int n);
	void Refill_bits(void);
	uint Show_Bits(uint n);
	uint Get_Bits(uint n);
	uint Copy_Bits(uint n);
	void flush_read_buffer();
	void flush_write_buffer();
	int scale_quant(double quant);
	int increment_quant(int quant);
	int intmax( register int x, register int y );
	int intmin( register int x, register int y );
	int getNewQuant(int curQuant);
	int isNotEmpty(RunLevel *blk);
	int putAC(int run, int signed_level, int vlcformat);
	int putACfirst(int run, int val);
	void putnonintrablk(RunLevel *blk);
	void putcbp(int cbp);
	void putmbtype(int mb_type);
	int get_macroblock_modes ();
	int get_quantizer_scale ();
	int get_motion_delta (const int f_code);
	int get_dmv ();
	int get_coded_block_pattern ();
	int get_luma_dc_dct_diff ();
	int get_chroma_dc_dct_diff ();
	void get_intra_block_B14 ();
	void get_intra_block_B15 ();
	int get_non_intra_block_drop (RunLevel *blk);
	int get_non_intra_block_rq (RunLevel *blk);
	void slice_intra_DCT (const int cc);
	void slice_non_intra_DCT (int cur_block);
	void motion_fr_frame ( uint f_code[2] );
	void motion_fr_field ( uint f_code[2] );
	void motion_fr_dmv ( uint f_code[2] );
	void motion_fr_conceal ( );
	void motion_fi_field ( uint f_code[2] );
	void motion_fi_16x8 ( uint f_code[2] );
	void motion_fi_dmv ( uint f_code[2] );
	void motion_fi_conceal ();

	void putmbdata(int macroblock_modes);
	void put_quantiser(int quantiser);
	int slice_init (int code);
	void mpeg2_slice ( const int code );
	void Flush_Bits(uint);
	void init();
protected:
};

#endif
