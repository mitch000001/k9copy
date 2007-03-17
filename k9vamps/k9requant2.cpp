// Adapted into transcode by Tilmann Bitterberg
// Code from libmpeg2 and mpeg2enc copyright by their respective owners
// New code and modifications copyright Antoine Missout
// Thanks to Sven Goethel for error resilience patches
// Released under GPL license, see gnu.org

// toggles:
// #define STAT // print stats on exit
#define NDEBUG // turns off asserts
#define REMOVE_BYTE_STUFFING	// removes 0x 00 00 00 00 00 00 used in cbr streams (look for 6 0x00 and remove 1 0x00)
/*	4 0x00 might be legit, for exemple:
00 00 01 b5 14 82 00 01 00 00 00 00 01 b8 .. ..
these two: -- -- are part of the seq. header ext.
AFAIK 5 0x00 should never happen except for byte stuffing but to be safe look for 6 */
// #define USE_FD // use 2 lasts args for input/output paths

#define REACT_DELAY (1024.0*128.0)
#define MAX_ERRORS 0

// notes:
//
// - intra block:
// 		- the quantiser is increment by one step
//
// - non intra block:
//		- in P_FRAME we keep the original quantiser but drop the last coefficient
//		  if there is more than one
//		- in B_FRAME we multiply the quantiser by a factor
//
// - I_FRAME is recoded when we're 5.0 * REACT_DELAY late
// - P_FRAME is recoded when we're 2.5 * REACT_DELAY late
// - B_FRAME are always recoded

// if we're getting *very* late (60 * REACT_DELAY)
//
// - intra blocks quantiser is incremented two step
// - drop a few coefficients but always keep the first one

#include "k9common.h"
#include "k9requant2.h"
#include "putvlc.h"
#include "getvlc.h"
#include <assert.h>
#include <math.h>
#include "ac.h"


// useful constants
#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3

// gcc
#ifdef HAVE_BUILTIN_EXPECT
#define likely(x) __builtin_expect ((x) != 0, 1)
#define unlikely(x) __builtin_expect ((x) != 0, 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#define verbose 0
#define DEB(msg) fprintf (stderr, "%s:%d " msg, __FILE__, __LINE__)
#define DEBF(format, args...) fprintf (stderr, "%s:%d " format, __FILE__, __LINE__, args)
#define LOG(msg) if (verbose > 1) fprintf (stderr, msg)
#define LOGF(format, args...) if (verbose > 1) fprintf (stderr, format, args)


#define BITS_IN_BUF (8)

#define BUF_SIZE (16*1024*1024)
#define MIN_READ (1*1024*1024)
#define MIN_WRITE (1*1024*1024)

/*
#define WRITE \
	mloka1 = wbuf - owbuf; \
	m_buffOut.writeBlock((char*)owbuf, mloka1); \
	outbytecnt += mloka1; \
	wbuf = owbuf; \
	mloka1 = rbuf - cbuf; if (mloka1) memmove(orbuf, cbuf, mloka1);\
	cbuf = rbuf = orbuf; rbuf += mloka1;

#define LOCK(x) \
	while (unlikely(x > (rbuf-cbuf))) \
{ \
	assert(rbuf + MIN_READ < orbuf + BUF_SIZE); \
	mloka1 =   m_buffIn.readBlock((char*)rbuf, MIN_READ); \
	if (mloka1 <= 0) { RETURN } \
	inbytecnt += mloka1; \
	rbuf += mloka1; \
}
*/
#define WRITE \
	mloka1 = wbuf - owbuf; \
	owbuf+=mloka1; \
	outbytecnt += mloka1; \
	wbuf = owbuf; \
	cbuf = rbuf; 

#define LOCK(x) \
	while (unlikely(x > (rbuf-cbuf))) \
{ \
	if (rbuf== (uint8_t*)(m_dataIn+m_sizeIn))  \
	   { RETURN } \
	rbuf=(uint8_t*)m_dataIn+m_sizeIn; \
	LOGF("rbuf:%d\n",m_sizeIn);\
	inbytecnt += m_sizeIn; \
}


/*
#define RETURN \
	assert(rbuf >= cbuf);\
	mloka1 = rbuf - cbuf;\
	if (mloka1) { COPY(mloka1); }\
	WRITE \
	free(orbuf); \
	free(owbuf); \
	return ;
*/

#define RETURN \
	assert(rbuf >= cbuf);\
	mloka1 = rbuf - cbuf;\
	if (mloka1) { COPY(mloka1); }\
	WRITE \
	return ;


#define COPY(x)\
	assert(x > 0); \
	assert(wbuf + x < owbuf + BUF_SIZE); \
	assert(cbuf + x < orbuf + BUF_SIZE); \
	assert(cbuf + x >= orbuf); \
	assert(wbuf + x >= orbuf); \
	tc_memcpy(wbuf, cbuf, x);\
	cbuf += x; \
	wbuf += x;

#define SEEKR(x)\
	cbuf += x; \
	assert (cbuf <= rbuf); \
	assert (cbuf < orbuf + BUF_SIZE); \
	assert (cbuf >= orbuf);

#define SEEKW(x)\
	wbuf += x; \
	assert (wbuf < owbuf + BUF_SIZE); \
	assert (wbuf >= owbuf);

#define MOTION_CALL(routine,direction) 						\
	do {														\
	if ((direction) & MACROBLOCK_MOTION_FORWARD)			\
	routine (f_code[0]);								\
	if ((direction) & MACROBLOCK_MOTION_BACKWARD)			\
	routine (f_code[1]);								\
	} while (0)

#define NEXT_MACROBLOCK											\
	do {															\
	h_offset += 16;												\
	if (h_offset == horizontal_size_value) 						\
{															\
	v_offset += 16;											\
	if (v_offset > (vertical_size_value - 16)) return;		\
	h_offset = 0;											\
}															\
	} while (0)

	const uint8_t non_linear_mquant_table[32] =
	{
		0, 1, 2, 3, 4, 5, 6, 7,
			8,10,12,14,16,18,20,22,
			24,28,32,36,40,44,48,52,
			56,64,72,80,88,96,104,112
	};
	const uint8_t map_non_linear_mquant[113] =
	{
		0,1,2,3,4,5,6,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,
			16,17,17,17,18,18,18,18,19,19,19,19,20,20,20,20,21,21,21,21,22,22,
			22,22,23,23,23,23,24,24,24,24,24,24,24,25,25,25,25,25,25,25,26,26,
			26,26,26,26,26,26,27,27,27,27,27,27,27,27,28,28,28,28,28,28,28,29,
			29,29,29,29,29,29,29,29,29,30,30,30,30,30,30,30,31,31,31,31,31
	};
	const int non_linear_quantizer_scale [] =
	{
		0,  1,  2,  3,  4,  5,   6,   7,
			8, 10, 12, 14, 16, 18,  20,  22,
			24, 28, 32, 36, 40, 44,  48,  52,
			56, 64, 72, 80, 88, 96, 104, 112
	};


void k9Requant2::putbits(uint val, int n)
{
	assert(n < 32);
	assert(!(val & (0xffffffffU << n)));

	while (unlikely(n >= outbitcnt))
	{
		wbuf[0] = (outbitbuf << outbitcnt ) | (val >> (n - outbitcnt));
		SEEKW(1);
		n -= outbitcnt;
		outbitbuf = 0;
		val &= ~(0xffffffffU << n);
		outbitcnt = BITS_IN_BUF;
	}

	if (likely(n))
	{
		outbitbuf = (outbitbuf << n) | val;
		outbitcnt -= n;
	}

	assert(outbitcnt > 0);
	assert(outbitcnt <= BITS_IN_BUF);
}

void k9Requant2::Refill_bits(void)
{
	assert((rbuf - cbuf) >= 1);
	inbitbuf |= cbuf[0] << (24 - inbitcnt);
	inbitcnt += 8;
	SEEKR(1)
}

void k9Requant2::Flush_Bits(uint n)
{
	assert(inbitcnt >= n);

	inbitbuf <<= n;
	inbitcnt -= n;

	assert( (!n) || ((n>0) && !(inbitbuf & 0x1)) );

	while (unlikely(inbitcnt < 24)) Refill_bits();
}

uint k9Requant2::Show_Bits(uint n)
{
	return ((unsigned int)inbitbuf) >> (32 - n);
}

uint k9Requant2::Get_Bits(uint n)
{
	uint Val = Show_Bits(n);
	Flush_Bits(n);
	return Val;
}

uint k9Requant2::Copy_Bits(uint n)
{
	uint Val = Get_Bits(n);
	putbits(Val, n);
	return Val;
}

void k9Requant2::flush_read_buffer()
{
	int i = inbitcnt & 0x7;
	if (i)
	{
		if (inbitbuf >> (32 - i))
		{
			DEBF("illegal inbitbuf: 0x%08X, %i, 0x%02X, %i\n", inbitbuf, inbitcnt, (inbitbuf >> (32 - i)), i);
			sliceError++;
		}

		inbitbuf <<= i;
		inbitcnt -= i;
	}
	SEEKR(-1 * (inbitcnt >> 3));
	inbitcnt = 0;
}

void k9Requant2::flush_write_buffer()
{
	if (outbitcnt != 8) putbits(0, outbitcnt);
}


int k9Requant2::scale_quant(double quant )
{
	int iquant;
	if (q_scale_type)
	{
		iquant = (int) floor(quant+0.5);

		/* clip mquant to legal (linear) range */
		if (iquant<1) iquant = 1;
		if (iquant>112) iquant = 112;

		iquant = non_linear_mquant_table[map_non_linear_mquant[iquant]];
	}
	else
	{
		/* clip mquant to legal (linear) range */
		iquant = (int)floor(quant+0.5);
		if (iquant<2) iquant = 2;
		if (iquant>62) iquant = 62;
		iquant = (iquant/2)*2; // Must be *even*
	}
	return iquant;
}

int k9Requant2::increment_quant(int quant)
{
	if (q_scale_type)
	{
		//assert(quant >= 1 && quant <= 112);
		if (quant < 1 || quant > 112)
		{
			DEBF("illegal quant: %d\n", quant);
			if (quant > 112) quant = 112;
			else if (quant < 1) quant = 1;
			DEBF("illegal quant changed to : %d\n", quant);
			sliceError++;
		}
		quant = map_non_linear_mquant[quant] + 1;
		if (quant_corr < -60.0f) quant++;
		if (quant > 31) quant = 31;
		quant = non_linear_mquant_table[quant];
	}
	else
	{
		// assert(!(quant & 1));
		if ((quant & 1) || (quant < 2) || (quant > 62))
		{
			DEBF("illegal quant: %d\n", quant);
			if (quant & 1) quant--;
			if (quant > 62) quant = 62;
			else if (quant < 2) quant = 2;
			DEBF("illegal quant changed to : %d\n", quant);
			sliceError++;
		}
		quant += 2;
		if (quant_corr < -60.0f) quant += 2;
		if (quant > 62) quant = 62;
	}
	return quant;
}

int k9Requant2::intmax( register int x, register int y )
{ return x < y ? y : x; }

int k9Requant2::intmin( register int x, register int y )
{ return x < y ? x : y; }


int k9Requant2::getNewQuant(int curQuant)
{
	double calc_quant, quant_to_use;
	int mquant = 0;

	calc_quant = curQuant * fact_x;
	quant_corr = (((inbytecnt - (rbuf - cbuf)) / fact_x) - (outbytecnt + (wbuf - owbuf))) / REACT_DELAY;
	quant_to_use = calc_quant - quant_corr;

	switch (picture_coding_type)
	{
	case I_TYPE:
	case P_TYPE:
		mquant = increment_quant(curQuant);
		break;

	case B_TYPE:
		mquant = intmax(scale_quant(quant_to_use), increment_quant(curQuant));
		break;

	default:
		assert(0);
		break;
	}

	/*
	LOGF("type: %s orig_quant: %3i calc_quant: %7.1f quant_corr: %7.1f using_quant: %3i\n",
	(picture_coding_type == I_TYPE ? "I_TYPE" : (picture_coding_type == P_TYPE ? "P_TYPE" : "B_TYPE")),
	(int)curQuant, (float)calc_quant, (float)quant_corr, (int)mquant);
	*/

	assert(mquant >= curQuant);

	return mquant;
}

int k9Requant2::isNotEmpty(RunLevel *blk)
{
	return (blk->level);
}


// return != 0 if error
int k9Requant2::putAC(int run, int signed_level, int vlcformat)
{
	int level, len;
	const VLCtable *ptab = NULL;

	level = (signed_level<0) ? -signed_level : signed_level; /* abs(signed_level) */

	// assert(!(run<0 || run>63 || level==0 || level>2047));
	if(run<0 || run>63)
	{
		DEBF("illegal run: %d\n", run);
		sliceError++;
		return 1;
	}
	if(level==0 || level>2047)
	{
		DEBF("illegal level: %d\n", level);
		sliceError++;
		return 1;
	}

	len = 0;

	if (run<2 && level<41)
	{
		if (vlcformat)  ptab = &dct_code_tab1a[run][level-1];
		else ptab = &dct_code_tab1[run][level-1];
		len = ptab->len;
	}
	else if (run<32 && level<6)
	{
		if (vlcformat) ptab = &dct_code_tab2a[run-2][level-1];
		else ptab = &dct_code_tab2[run-2][level-1];
		len = ptab->len;
	}

	if (len) /* a VLC code exists */
	{
		putbits(ptab->code, len);
		putbits(signed_level<0, 1); /* sign */
	}
	else
	{
		putbits(1l, 6); /* Escape */
		putbits(run, 6); /* 6 bit code for run */
		putbits(((uint)signed_level) & 0xFFF, 12);
	}

	return 0;
}

// return != 0 if error
int k9Requant2::putACfirst(int run, int val)
{
	if (run==0 && (val==1 || val==-1))
	{
		putbits(2|(val<0),2);
		return 0;
	}
	else return putAC(run,val,0);
}

void k9Requant2::putnonintrablk(RunLevel *blk)
{
	assert(blk->level);

	if (putACfirst(blk->run, blk->level)) return;
	blk++;

	while(blk->level)
	{
		if (putAC(blk->run, blk->level, 0)) return;
		blk++;
	}

	putbits(2,2);
}

void k9Requant2::putcbp(int cbp)
{
	putbits(cbptable[cbp].code,cbptable[cbp].len);
}

void k9Requant2::putmbtype(int mb_type)
{
	putbits(mbtypetab[picture_coding_type-1][mb_type].code,
		mbtypetab[picture_coding_type-1][mb_type].len);
}




int k9Requant2::get_macroblock_modes ()
{
	int macroblock_modes;
	const MBtab * tab;

	switch (picture_coding_type)
	{
	case I_TYPE:

		tab = MB_I + UBITS (bit_buf, 1);
		DUMPBITS (bit_buf, bits, tab->len);
		macroblock_modes = tab->modes;

		if ((! (frame_pred_frame_dct)) && (picture_structure == FRAME_PICTURE))
		{
			macroblock_modes |= UBITS (bit_buf, 1) * DCT_TYPE_INTERLACED;
			DUMPBITS (bit_buf, bits, 1);
		}

		return macroblock_modes;

	case P_TYPE:

		tab = MB_P + UBITS (bit_buf, 5);
		DUMPBITS (bit_buf, bits, tab->len);
		macroblock_modes = tab->modes;

		if (picture_structure != FRAME_PICTURE)
		{
			if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
			{
				macroblock_modes |= UBITS (bit_buf, 2) * MOTION_TYPE_BASE;
				DUMPBITS (bit_buf, bits, 2);
			}
			return macroblock_modes;
		}
		else if (frame_pred_frame_dct)
		{
			if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
				macroblock_modes |= MC_FRAME;
			return macroblock_modes;
		}
		else
		{
			if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
			{
				macroblock_modes |= UBITS (bit_buf, 2) * MOTION_TYPE_BASE;
				DUMPBITS (bit_buf, bits, 2);
			}
			if (macroblock_modes & (MACROBLOCK_INTRA | MACROBLOCK_PATTERN))
			{
				macroblock_modes |= UBITS (bit_buf, 1) * DCT_TYPE_INTERLACED;
				DUMPBITS (bit_buf, bits, 1);
			}
			return macroblock_modes;
		}

	case B_TYPE:

		tab = MB_B + UBITS (bit_buf, 6);
		DUMPBITS (bit_buf, bits, tab->len);
		macroblock_modes = tab->modes;

		if (picture_structure != FRAME_PICTURE)
		{
			if (! (macroblock_modes & MACROBLOCK_INTRA))
			{
				macroblock_modes |= UBITS (bit_buf, 2) * MOTION_TYPE_BASE;
				DUMPBITS (bit_buf, bits, 2);
			}
			return macroblock_modes;
		}
		else if (frame_pred_frame_dct)
		{
			/* if (! (macroblock_modes & MACROBLOCK_INTRA)) */
			macroblock_modes |= MC_FRAME;
			return macroblock_modes;
		}
		else
		{
			if (macroblock_modes & MACROBLOCK_INTRA) goto intra;
			macroblock_modes |= UBITS (bit_buf, 2) * MOTION_TYPE_BASE;
			DUMPBITS (bit_buf, bits, 2);
			if (macroblock_modes & (MACROBLOCK_INTRA | MACROBLOCK_PATTERN))
			{
intra:
				macroblock_modes |= UBITS (bit_buf, 1) * DCT_TYPE_INTERLACED;
				DUMPBITS (bit_buf, bits, 1);
			}
			return macroblock_modes;
		}

	default:
		return 0;
	}

}

int k9Requant2::get_quantizer_scale ()
{
	int quantizer_scale_code;

	quantizer_scale_code = UBITS (bit_buf, 5);
	DUMPBITS (bit_buf, bits, 5); 

	if (!quantizer_scale_code)
	{
		DEBF("illegal quant scale code: %d\n", quantizer_scale_code);
		sliceError++;
		quantizer_scale_code++;
	}

	if (q_scale_type) return non_linear_quantizer_scale[quantizer_scale_code];
	else return quantizer_scale_code << 1;
}

int k9Requant2::get_motion_delta (const int f_code)
{
#define bit_buf (inbitbuf)

	int delta;
	int sign;
	const MVtab * tab;

	if (bit_buf & 0x80000000)
	{
		COPYBITS (bit_buf, bits, 1);
		return 0;
	}
	else if (bit_buf >= 0x0c000000)
	{

		tab = MV_4 + UBITS (bit_buf, 4);
		delta = (tab->delta << f_code) + 1;
		COPYBITS (bit_buf, bits, tab->len);

		sign = SBITS (bit_buf, 1);
		COPYBITS (bit_buf, bits, 1);

		if (f_code) delta += UBITS (bit_buf, f_code);
		COPYBITS (bit_buf, bits, f_code);

		return (delta ^ sign) - sign;
	}
	else
	{

		tab = MV_10 + UBITS (bit_buf, 10);
		delta = (tab->delta << f_code) + 1;
		COPYBITS (bit_buf, bits, tab->len);

		sign = SBITS (bit_buf, 1);
		COPYBITS (bit_buf, bits, 1);

		if (f_code)
		{
			delta += UBITS (bit_buf, f_code);
			COPYBITS (bit_buf, bits, f_code);
		}

		return (delta ^ sign) - sign;
	}
}


int k9Requant2::get_dmv ()
{
	const DMVtab * tab;

	tab = DMV_2 + UBITS (bit_buf, 2);
	COPYBITS (bit_buf, bits, tab->len);
	return tab->dmv;
}

int k9Requant2::get_coded_block_pattern ()
{
#define bit_buf (inbitbuf)
	const CBPtab * tab;

	if (bit_buf >= 0x20000000)
	{
		tab = CBP_7 + (UBITS (bit_buf, 7) - 16);
		DUMPBITS (bit_buf, bits, tab->len);
		return tab->cbp;
	}
	else
	{
		tab = CBP_9 + UBITS (bit_buf, 9);
		DUMPBITS (bit_buf, bits, tab->len);
		return tab->cbp;
	}
}

int k9Requant2::get_luma_dc_dct_diff ()
{
#define bit_buf (inbitbuf)
	const DCtab * tab;
	int size;
	int dc_diff;

	if (bit_buf < 0xf8000000)
	{
		tab = DC_lum_5 + UBITS (bit_buf, 5);
		size = tab->size;
		if (size)
		{
			COPYBITS (bit_buf, bits, tab->len);
			//dc_diff = UBITS (bit_buf, size) - UBITS (SBITS (~bit_buf, 1), size);
			dc_diff = UBITS (bit_buf, size); if (!(dc_diff >> (size - 1))) dc_diff = (dc_diff + 1) - (1 << size);
			COPYBITS (bit_buf, bits, size);
			return dc_diff;
		}
		else
		{
			COPYBITS (bit_buf, bits, 3);
			return 0;
		}
	}
	else
	{
		tab = DC_long + (UBITS (bit_buf, 9) - 0x1e0);
		size = tab->size;
		COPYBITS (bit_buf, bits, tab->len);
		//dc_diff = UBITS (bit_buf, size) - UBITS (SBITS (~bit_buf, 1), size);
		dc_diff = UBITS (bit_buf, size); if (!(dc_diff >> (size - 1))) dc_diff = (dc_diff + 1) - (1 << size);
		COPYBITS (bit_buf, bits, size);
		return dc_diff;
	}
}

int k9Requant2::get_chroma_dc_dct_diff ()
{
#define bit_buf (inbitbuf)

	const DCtab * tab;
	int size;
	int dc_diff;

	if (bit_buf < 0xf8000000)
	{
		tab = DC_chrom_5 + UBITS (bit_buf, 5);
		size = tab->size;
		if (size)
		{
			COPYBITS (bit_buf, bits, tab->len);
			//dc_diff = UBITS (bit_buf, size) - UBITS (SBITS (~bit_buf, 1), size);
			dc_diff = UBITS (bit_buf, size); if (!(dc_diff >> (size - 1))) dc_diff = (dc_diff + 1) - (1 << size);
			COPYBITS (bit_buf, bits, size);
			return dc_diff;
		} else
		{
			COPYBITS (bit_buf, bits, 2);
			return 0;
		}
	}
	else
	{
		tab = DC_long + (UBITS (bit_buf, 10) - 0x3e0);
		size = tab->size;
		COPYBITS (bit_buf, bits, tab->len + 1);
		//dc_diff = UBITS (bit_buf, size) - UBITS (SBITS (~bit_buf, 1), size);
		dc_diff = UBITS (bit_buf, size); if (!(dc_diff >> (size - 1))) dc_diff = (dc_diff + 1) - (1 << size);
		COPYBITS (bit_buf, bits, size);
		return dc_diff;
	}
}

void k9Requant2::get_intra_block_B14 ()
{
#define bit_buf (inbitbuf)
	int q = quantizer_scale, nq = new_quantizer_scale, tst = (nq / q) + ((nq % q) ? 1 : 0);
	int i, li;
	int val;
	const DCTtab * tab;

	li = i = 0;

	while (1)
	{
		if (bit_buf >= 0x28000000)
		{
			tab = DCT_B14AC_5 + (UBITS (bit_buf, 5) - 5);

			i += tab->run;
			if (i >= 64) break;	/* end of block */

normal_code:
			DUMPBITS (bit_buf, bits, tab->len);
			val = tab->level;
			if (val >= tst)
			{
				val = (val ^ SBITS (bit_buf, 1)) - SBITS (bit_buf, 1);
				if (putAC(i - li - 1, (val * q) / nq, 0)) break;
				li = i;
			}

			DUMPBITS (bit_buf, bits, 1);

			continue;
		}
		else if (bit_buf >= 0x04000000)
		{
			tab = DCT_B14_8 + (UBITS (bit_buf, 8) - 4);

			i += tab->run;
			if (i < 64) goto normal_code;

			/* escape code */
			i += (UBITS (bit_buf, 12) & 0x3F) - 64;
			if (i >= 64) break;	/* illegal, check needed to avoid buffer overflow */

			DUMPBITS (bit_buf, bits, 12);
			val = SBITS (bit_buf, 12);
			if (abs(val) >= tst)
			{
				if (putAC(i - li - 1, (val * q) / nq, 0)) break;
				li = i;
			}

			DUMPBITS (bit_buf, bits, 12);

			continue;
		}
		else if (bit_buf >= 0x02000000)
		{
			tab = DCT_B14_10 + (UBITS (bit_buf, 10) - 8);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else if (bit_buf >= 0x00800000)
		{
			tab = DCT_13 + (UBITS (bit_buf, 13) - 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else if (bit_buf >= 0x00200000)
		{
			tab = DCT_15 + (UBITS (bit_buf, 15) - 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else
		{
			tab = DCT_16 + UBITS (bit_buf, 16);
			DUMPBITS (bit_buf, bits, 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		break;	/* illegal, check needed to avoid buffer overflow */
	}

	COPYBITS (bit_buf, bits, 2);	/* end of block code */
}

void k9Requant2::get_intra_block_B15 ()
{
#define bit_buf (inbitbuf)
	int q = quantizer_scale, nq = new_quantizer_scale, tst = (nq / q) + ((nq % q) ? 1 : 0);
	int i, li;
	int val;
	const DCTtab * tab;

	li = i = 0;

	while (1)
	{
		if (bit_buf >= 0x04000000)
		{
			tab = DCT_B15_8 + (UBITS (bit_buf, 8) - 4);

			i += tab->run;
			if (i < 64)
			{
normal_code:
				DUMPBITS (bit_buf, bits, tab->len);

				val = tab->level;
				if (val >= tst)
				{
					val = (val ^ SBITS (bit_buf, 1)) - SBITS (bit_buf, 1);
					if (putAC(i - li - 1, (val * q) / nq, 1)) break;
					li = i;
				}

				DUMPBITS (bit_buf, bits, 1);

				continue;
			}
			else
			{
				i += (UBITS (bit_buf, 12) & 0x3F) - 64;

				if (i >= 64) break;	/* illegal, check against buffer overflow */

				DUMPBITS (bit_buf, bits, 12);
				val = SBITS (bit_buf, 12);
				if (abs(val) >= tst)
				{
					if (putAC(i - li - 1, (val * q) / nq, 1)) break;
					li = i;
				}

				DUMPBITS (bit_buf, bits, 12);

				continue;
			}
		}
		else if (bit_buf >= 0x02000000)
		{
			tab = DCT_B15_10 + (UBITS (bit_buf, 10) - 8);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else if (bit_buf >= 0x00800000)
		{
			tab = DCT_13 + (UBITS (bit_buf, 13) - 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else if (bit_buf >= 0x00200000)
		{
			tab = DCT_15 + (UBITS (bit_buf, 15) - 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else
		{
			tab = DCT_16 + UBITS (bit_buf, 16);
			DUMPBITS (bit_buf, bits, 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		break;	/* illegal, check needed to avoid buffer overflow */
	}

	COPYBITS (bit_buf, bits, 4);	/* end of block code */
}


int k9Requant2::get_non_intra_block_drop (RunLevel *blk)
{
#define bit_buf (inbitbuf)

	int i, li;
	int val;
	const DCTtab * tab;
	RunLevel *sblk = blk + 1;

	li = i = -1;

	if (bit_buf >= 0x28000000)
	{
		tab = DCT_B14DC_5 + (UBITS (bit_buf, 5) - 5);
		goto entry_1;
	}
	else goto entry_2;

	while (1)
	{
		if (bit_buf >= 0x28000000)
		{
			tab = DCT_B14AC_5 + (UBITS (bit_buf, 5) - 5);

entry_1:
			i += tab->run;
			if (i >= 64) break;	/* end of block */

normal_code:

			DUMPBITS (bit_buf, bits, tab->len);
			val = tab->level;
			val = (val ^ SBITS (bit_buf, 1)) - SBITS (bit_buf, 1); /* if (bitstream_get (1)) val = -val; */

			blk->level = val;
			blk->run = i - li - 1;
			li = i;
			blk++;

			DUMPBITS (bit_buf, bits, 1);

			continue;
		}

entry_2:

		if (bit_buf >= 0x04000000)
		{
			tab = DCT_B14_8 + (UBITS (bit_buf, 8) - 4);

			i += tab->run;
			if (i < 64) goto normal_code;

			/* escape code */

			i += (UBITS (bit_buf, 12) & 0x3F) - 64;

			if (i >= 64) break;	/* illegal, check needed to avoid buffer overflow */

			DUMPBITS (bit_buf, bits, 12);
			val = SBITS (bit_buf, 12);

			blk->level = val;
			blk->run = i - li - 1;
			li = i;
			blk++;

			DUMPBITS (bit_buf, bits, 12);

			continue;
		}
		else if (bit_buf >= 0x02000000)
		{
			tab = DCT_B14_10 + (UBITS (bit_buf, 10) - 8);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else if (bit_buf >= 0x00800000)
		{
			tab = DCT_13 + (UBITS (bit_buf, 13) - 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else if (bit_buf >= 0x00200000)
		{
			tab = DCT_15 + (UBITS (bit_buf, 15) - 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else
		{
			tab = DCT_16 + UBITS (bit_buf, 16);
			DUMPBITS (bit_buf, bits, 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		break;	/* illegal, check needed to avoid buffer overflow */
	}
	DUMPBITS (bit_buf, bits, 2);	/* dump end of block code */

	// remove last coeff
	if (blk != sblk) 
	{
		blk--;
		// remove more coeffs if very late
		if ((quant_corr < -60.0f) && (blk != sblk))
		{
			blk--;
			if ((quant_corr < -80.0f) && (blk != sblk))
			{
				blk--;
				if ((quant_corr < -100.0f) && (blk != sblk))
				{
					blk--;
					if ((quant_corr < -120.0f) && (blk != sblk))
						blk--;
				}
			}
		}
	}

	blk->level = 0;

	return i;
}

int k9Requant2::get_non_intra_block_rq (RunLevel *blk)
{
#define bit_buf (inbitbuf)
	int q = quantizer_scale, nq = new_quantizer_scale, tst = (nq / q) + ((nq % q) ? 1 : 0);
	int i, li;
	int val;
	const DCTtab * tab;

	li = i = -1;

	if (bit_buf >= 0x28000000)
	{
		tab = DCT_B14DC_5 + (UBITS (bit_buf, 5) - 5);
		goto entry_1;
	}
	else goto entry_2;

	while (1)
	{
		if (bit_buf >= 0x28000000)
		{
			tab = DCT_B14AC_5 + (UBITS (bit_buf, 5) - 5);

entry_1:
			i += tab->run;
			if (i >= 64)
				break;	/* end of block */

normal_code:

			DUMPBITS (bit_buf, bits, tab->len);
			val = tab->level;
			if (val >= tst)
			{
				val = (val ^ SBITS (bit_buf, 1)) - SBITS (bit_buf, 1);
				blk->level = (val * q) / nq;
				blk->run = i - li - 1;
				li = i;
				blk++;
			}

			//if ( ((val) && (tab->level < tst)) || ((!val) && (tab->level >= tst)) )
			//	LOGF("level: %i val: %i tst : %i q: %i nq : %i\n", tab->level, val, tst, q, nq);

			DUMPBITS (bit_buf, bits, 1);

			continue;
		}

entry_2:
		if (bit_buf >= 0x04000000)
		{
			tab = DCT_B14_8 + (UBITS (bit_buf, 8) - 4);

			i += tab->run;
			if (i < 64) goto normal_code;

			/* escape code */

			i += (UBITS (bit_buf, 12) & 0x3F) - 64;

			if (i >= 64) break;	/* illegal, check needed to avoid buffer overflow */

			DUMPBITS (bit_buf, bits, 12);
			val = SBITS (bit_buf, 12);
			if (abs(val) >= tst)
			{
				blk->level = (val * q) / nq;
				blk->run = i - li - 1;
				li = i;
				blk++;
			}

			DUMPBITS (bit_buf, bits, 12);

			continue;
		}
		else if (bit_buf >= 0x02000000)
		{
			tab = DCT_B14_10 + (UBITS (bit_buf, 10) - 8);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else if (bit_buf >= 0x00800000)
		{
			tab = DCT_13 + (UBITS (bit_buf, 13) - 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else if (bit_buf >= 0x00200000)
		{
			tab = DCT_15 + (UBITS (bit_buf, 15) - 16);
			i += tab->run;
			if (i < 64) goto normal_code;
		}
		else
		{
			tab = DCT_16 + UBITS (bit_buf, 16);
			DUMPBITS (bit_buf, bits, 16);

			i += tab->run;
			if (i < 64) goto normal_code;
		}
		break;	/* illegal, check needed to avoid buffer overflow */
	}
	DUMPBITS (bit_buf, bits, 2);	/* dump end of block code */

	blk->level = 0;

	return i;
}

void k9Requant2::slice_intra_DCT (const int cc)
{
	if (cc == 0)	get_luma_dc_dct_diff ();
	else			get_chroma_dc_dct_diff ();

	if (intra_vlc_format) get_intra_block_B15 ();
	else get_intra_block_B14 ();
}

void k9Requant2::slice_non_intra_DCT (int cur_block)
{
	if (picture_coding_type == P_TYPE) get_non_intra_block_drop(block[cur_block]);
	else get_non_intra_block_rq(block[cur_block]);
}

void k9Requant2::motion_fr_frame ( uint f_code[2] )
{
	get_motion_delta (f_code[0]);
	get_motion_delta (f_code[1]);
}

void k9Requant2::motion_fr_field ( uint f_code[2] )
{
	COPYBITS (bit_buf, bits, 1);

	get_motion_delta (f_code[0]);
	get_motion_delta (f_code[1]);

	COPYBITS (bit_buf, bits, 1);

	get_motion_delta (f_code[0]);
	get_motion_delta (f_code[1]);
}

void k9Requant2::motion_fr_dmv ( uint f_code[2] )
{
	get_motion_delta (f_code[0]);
	get_dmv ();

	get_motion_delta (f_code[1]);
	get_dmv ();
}

/* like motion_frame, but parsing without actual motion compensation */
void k9Requant2::motion_fr_conceal ( )
{
	get_motion_delta (f_code[0][0]);
	get_motion_delta (f_code[0][1]);

	COPYBITS (bit_buf, bits, 1); /* remove marker_bit */
}

void k9Requant2::motion_fi_field ( uint f_code[2] )
{
	COPYBITS (bit_buf, bits, 1);

	get_motion_delta (f_code[0]);
	get_motion_delta (f_code[1]);
}

void k9Requant2::motion_fi_16x8 ( uint f_code[2] )
{
	COPYBITS (bit_buf, bits, 1);

	get_motion_delta (f_code[0]);
	get_motion_delta (f_code[1]);

	COPYBITS (bit_buf, bits, 1);

	get_motion_delta (f_code[0]);
	get_motion_delta (f_code[1]);
}

void k9Requant2::motion_fi_dmv ( uint f_code[2] )
{
	get_motion_delta (f_code[0]);
	get_dmv ();

	get_motion_delta (f_code[1]);
	get_dmv ();
}

void k9Requant2::motion_fi_conceal ()
{
	COPYBITS (bit_buf, bits, 1); /* remove field_select */

	get_motion_delta (f_code[0][0]);
	get_motion_delta (f_code[0][1]);

	COPYBITS (bit_buf, bits, 1); /* remove marker_bit */
}


void k9Requant2::putmbdata(int macroblock_modes)
{
	putmbtype(macroblock_modes & 0x1F);

	switch (picture_coding_type)
	{
	case I_TYPE:
		if ((! (frame_pred_frame_dct)) && (picture_structure == FRAME_PICTURE))
			putbits(macroblock_modes & DCT_TYPE_INTERLACED ? 1 : 0, 1);
		break;

	case P_TYPE:
		if (picture_structure != FRAME_PICTURE)
		{
			if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
				putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
			break;
		}
		else if (frame_pred_frame_dct) break;
		else
		{
			if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
				putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
			if (macroblock_modes & (MACROBLOCK_INTRA | MACROBLOCK_PATTERN))
				putbits(macroblock_modes & DCT_TYPE_INTERLACED ? 1 : 0, 1);
			break;
		}

	case B_TYPE:
		if (picture_structure != FRAME_PICTURE)
		{
			if (! (macroblock_modes & MACROBLOCK_INTRA))
				putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
			break;
		}
		else if (frame_pred_frame_dct) break;
		else
		{
			if (macroblock_modes & MACROBLOCK_INTRA) goto intra;
			putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
			if (macroblock_modes & (MACROBLOCK_INTRA | MACROBLOCK_PATTERN))
			{
intra:
				putbits(macroblock_modes & DCT_TYPE_INTERLACED ? 1 : 0, 1);
			}
			break;
		}
	}

}

void k9Requant2::put_quantiser(int quantiser)
{
	putbits(q_scale_type ? map_non_linear_mquant[quantiser] : quantiser >> 1, 5);
	last_coded_scale = quantiser;
}

int k9Requant2::slice_init (int code)
{
#define bit_buf (inbitbuf)

	int offset;
	const MBAtab * mba;

	v_offset = (code - 1) * 16;

	quantizer_scale = get_quantizer_scale ();
	if (picture_coding_type == P_TYPE) new_quantizer_scale = quantizer_scale;
	else new_quantizer_scale = getNewQuant(quantizer_scale);
	put_quantiser(new_quantizer_scale);

	/*LOGF("************************\nstart of slice %i in %s picture. ori quant: %i new quant: %i\n", code,
	(picture_coding_type == I_TYPE ? "I_TYPE" : (picture_coding_type == P_TYPE ? "P_TYPE" : "B_TYPE")),
	quantizer_scale, new_quantizer_scale);*/

	/* ignore intra_slice and all the extra data */
	while (bit_buf & 0x80000000)
	{
		DUMPBITS (bit_buf, bits, 9);
	}

	/* decode initial macroblock address increment */
	offset = 0;
	while (1)
	{
		if (bit_buf >= 0x08000000)
		{
			mba = MBA_5 + (UBITS (bit_buf, 6) - 2);
			break;
		}
		else if (bit_buf >= 0x01800000)
		{
			mba = MBA_11 + (UBITS (bit_buf, 12) - 24);
			break;
		}
		else switch (UBITS (bit_buf, 12))
		{
			case 8:		/* macroblock_escape */
				offset += 33;
				COPYBITS (bit_buf, bits, 11);
				continue;
			default:	/* error */
				return 1;
		}
	}

	COPYBITS (bit_buf, bits, mba->len + 1);
	h_offset = (offset + mba->mba) << 4;

	while (h_offset - (int)horizontal_size_value >= 0)
	{
		h_offset -= horizontal_size_value;
		v_offset += 16;
	}

	if (v_offset > (vertical_size_value - 16)) return 1;

	return 0;

}

void k9Requant2::mpeg2_slice ( const int code )
{
#define bit_buf (inbitbuf)

	if (slice_init (code)) return;

	while (1)
	{
		int macroblock_modes;
		int mba_inc;
		const MBAtab * mba;

		macroblock_modes = get_macroblock_modes ();
		if (macroblock_modes & MACROBLOCK_QUANT) quantizer_scale = get_quantizer_scale ();

		//LOGF("blk %i : ", h_offset >> 4);

		if (macroblock_modes & MACROBLOCK_INTRA)
		{
			//LOG("intra "); if (macroblock_modes & MACROBLOCK_QUANT) LOGF("got new quant: %i ", quantizer_scale);

			new_quantizer_scale = increment_quant(quantizer_scale);
			if (last_coded_scale == new_quantizer_scale) macroblock_modes &= 0xFFFFFFEF; // remove MACROBLOCK_QUANT
			else macroblock_modes |= MACROBLOCK_QUANT; //add MACROBLOCK_QUANT
			putmbdata(macroblock_modes);
			if (macroblock_modes & MACROBLOCK_QUANT) put_quantiser(new_quantizer_scale);

			//if (macroblock_modes & MACROBLOCK_QUANT) LOGF("put new quant: %i ", new_quantizer_scale);

			if (concealment_motion_vectors)
			{
				if (picture_structure == FRAME_PICTURE) motion_fr_conceal ();
				else motion_fi_conceal ();
			}

			slice_intra_DCT ( 0);
			slice_intra_DCT ( 0);
			slice_intra_DCT ( 0);
			slice_intra_DCT ( 0);
			slice_intra_DCT ( 1);
			slice_intra_DCT ( 2);
		}
		else
		{
			int new_coded_block_pattern = 0;

			// begin saving data
			int batb;
			uint8_t	n_owbuf[32], *n_wbuf,
				*o_owbuf = owbuf, *o_wbuf = wbuf;
			uint32_t	n_outbitcnt, n_outbitbuf,
				o_outbitcnt = outbitcnt, o_outbitbuf = outbitbuf;

			outbitbuf = 0; outbitcnt = BITS_IN_BUF;
			owbuf = wbuf = n_owbuf;

			if (picture_structure == FRAME_PICTURE)
				switch (macroblock_modes & MOTION_TYPE_MASK)
			{
				case MC_FRAME: MOTION_CALL (motion_fr_frame, macroblock_modes); break;
				case MC_FIELD: MOTION_CALL (motion_fr_field, macroblock_modes); break;
				case MC_DMV: MOTION_CALL (motion_fr_dmv, MACROBLOCK_MOTION_FORWARD); break;
			}
			else
				switch (macroblock_modes & MOTION_TYPE_MASK)
			{
				case MC_FIELD: MOTION_CALL (motion_fi_field, macroblock_modes); break;
				case MC_16X8: MOTION_CALL (motion_fi_16x8, macroblock_modes); break;
				case MC_DMV: MOTION_CALL (motion_fi_dmv, MACROBLOCK_MOTION_FORWARD); break;
			}

			assert(wbuf - owbuf < 32);

			n_wbuf = wbuf;
			n_outbitcnt = outbitcnt;
			n_outbitbuf = outbitbuf;
			assert(owbuf == n_owbuf);

			outbitcnt = o_outbitcnt;
			outbitbuf = o_outbitbuf;
			owbuf = o_owbuf;
			wbuf = o_wbuf;
			// end saving data


			if (picture_coding_type == P_TYPE) new_quantizer_scale = quantizer_scale;
			else new_quantizer_scale = getNewQuant(quantizer_scale);

			//LOG("non intra "); if (macroblock_modes & MACROBLOCK_QUANT) LOGF("got new quant: %i ", quantizer_scale);

			if (macroblock_modes & MACROBLOCK_PATTERN)
			{
				int coded_block_pattern = get_coded_block_pattern ();

				if (coded_block_pattern & 0x20) slice_non_intra_DCT(0);
				if (coded_block_pattern & 0x10) slice_non_intra_DCT(1);
				if (coded_block_pattern & 0x08) slice_non_intra_DCT(2);
				if (coded_block_pattern & 0x04) slice_non_intra_DCT(3);
				if (coded_block_pattern & 0x02) slice_non_intra_DCT(4);
				if (coded_block_pattern & 0x01) slice_non_intra_DCT(5);

				if (picture_coding_type == B_TYPE)
				{
					if (coded_block_pattern & 0x20) if (isNotEmpty(block[0])) new_coded_block_pattern |= 0x20;
					if (coded_block_pattern & 0x10) if (isNotEmpty(block[1])) new_coded_block_pattern |= 0x10;
					if (coded_block_pattern & 0x08) if (isNotEmpty(block[2])) new_coded_block_pattern |= 0x08;
					if (coded_block_pattern & 0x04) if (isNotEmpty(block[3])) new_coded_block_pattern |= 0x04;
					if (coded_block_pattern & 0x02) if (isNotEmpty(block[4])) new_coded_block_pattern |= 0x02;
					if (coded_block_pattern & 0x01) if (isNotEmpty(block[5])) new_coded_block_pattern |= 0x01;
					if (!new_coded_block_pattern) macroblock_modes &= 0xFFFFFFED; // remove MACROBLOCK_PATTERN and MACROBLOCK_QUANT flag
				}
				else new_coded_block_pattern = coded_block_pattern;
			}

			if (last_coded_scale == new_quantizer_scale) macroblock_modes &= 0xFFFFFFEF; // remove MACROBLOCK_QUANT
			else if (macroblock_modes & MACROBLOCK_PATTERN) macroblock_modes |= MACROBLOCK_QUANT; //add MACROBLOCK_QUANT
			assert( (macroblock_modes & MACROBLOCK_PATTERN) || !(macroblock_modes & MACROBLOCK_QUANT) );

			putmbdata(macroblock_modes);
			if (macroblock_modes & MACROBLOCK_QUANT) put_quantiser(new_quantizer_scale);

			//if (macroblock_modes & MACROBLOCK_PATTERN) LOG("coded ");
			//if (macroblock_modes & MACROBLOCK_QUANT) LOGF("put new quant: %i ", new_quantizer_scale);

			// put saved motion data...
			for (batb = 0; batb < (n_wbuf - n_owbuf); batb++) putbits(n_owbuf[batb], 8);
			putbits(n_outbitbuf, BITS_IN_BUF - n_outbitcnt);
			// end saved motion data...

			if (macroblock_modes & MACROBLOCK_PATTERN)
			{
				putcbp(new_coded_block_pattern);

				if (new_coded_block_pattern & 0x20) putnonintrablk(block[0]);
				if (new_coded_block_pattern & 0x10) putnonintrablk(block[1]);
				if (new_coded_block_pattern & 0x08) putnonintrablk(block[2]);
				if (new_coded_block_pattern & 0x04) putnonintrablk(block[3]);
				if (new_coded_block_pattern & 0x02) putnonintrablk(block[4]);
				if (new_coded_block_pattern & 0x01) putnonintrablk(block[5]);
			}
		}

		//LOGF("\n\to: %i c: %i n: %i\n", quantizer_scale, last_coded_scale, new_quantizer_scale);

		NEXT_MACROBLOCK;

		mba_inc = 0;
		while (1)
		{
			if (bit_buf >= 0x10000000)
			{
				mba = MBA_5 + (UBITS (bit_buf, 5) - 2);
				break;
			}
			else if (bit_buf >= 0x03000000)
			{
				mba = MBA_11 + (UBITS (bit_buf, 11) - 24);
				break;
			}
			else
				switch (UBITS (bit_buf, 11))
			{
				case 8:		/* macroblock_escape */
					mba_inc += 33;
					COPYBITS (bit_buf, bits, 11);
					continue;
				default:	/* end of slice, or error */
					return;
			}
		}
		COPYBITS (bit_buf, bits, mba->len);
		mba_inc += mba->mba;

		if (mba_inc) do { NEXT_MACROBLOCK; } while (--mba_inc);
	}

}

/////---- end ext mpeg code


void k9Requant2::init() {
	// mpeg2 state
	// seq header
	horizontal_size_value=vertical_size_value=0;

	// pic header
	picture_coding_type=0;

	// pic code ext
	intra_dc_precision=picture_structure=frame_pred_frame_dct=0;
	concealment_motion_vectors=q_scale_type=intra_vlc_format=alternate_scan=0;

	// error
	validPicHeader=validSeqHeader=validExtHeader=sliceError=0;

	// slice or mb
	quantizer_scale=new_quantizer_scale=last_coded_scale=0;
	h_offset= v_offset=0;

	// rate
	quant_corr=0;

	// end mpeg2 state
	// global variables
	inbitcnt=outbitcnt=0;
	inbitbuf=outbitbuf=0;
	mloka1=0;
}

k9Requant2::k9Requant2(){
	inbytecnt=outbytecnt=0;
	m_dataIn=m_dataOut=NULL;
	cbuf=rbuf=wbuf=orbuf=owbuf=NULL;

	init();
}

void k9Requant2::run ()
{
	outbytecnt1=outbytecnt;
	uint8_t ID, found;
	int byte_stuff;
	init();

	// default
	//fact_x = 1.25;
	byte_stuff = 1;

//	rbuf = cbuf = orbuf = (uint8_t*)malloc(BUF_SIZE);
//	wbuf = owbuf = (uint8_t*)malloc(BUF_SIZE);
	if (!orbuf || !owbuf) {
		fprintf(stderr, "malloc() failed at %s:%d\n", __FILE__, __LINE__);
		return;
	}
	//inbytecnt = outbytecnt = 0;

	validPicHeader = 0;
	validSeqHeader = 0;
	validExtHeader = 0;

	if (fact_x < 1.0) fact_x = 1.0;
	else if (fact_x > 900.0) fact_x = 900.0;
	byte_stuff = !!byte_stuff;

	LOG("MPEG2 Requantiser by Makira.\n");
	LOGF("Using %f as factor.\n",fact_x);

	// recoding
	while(1)
	{
		// get next start code prefix
		found = 0;
		while (!found)
		{
			if (!byte_stuff) {
				LOCK(3)
			} else {
				LOCK(6)
					if ( (cbuf[0] == 0) && (cbuf[1] == 0) && (cbuf[2] == 0) && (cbuf[3] == 0) && (cbuf[4] == 0) && (cbuf[5] == 0) ) { SEEKR(1) }
			}
			if ( (cbuf[0] == 0) && (cbuf[1] == 0) && (cbuf[2] == 1) ) found = 1; // start code !
			else { COPY(1) } // continue search
		}
		COPY(3)

			// get start code
			LOCK(1)
			ID = cbuf[0];
		COPY(1)

			if (ID == 0x00) // pic header
			{
				LOCK(4)
					picture_coding_type = (cbuf[1] >> 3) & 0x7;
				if (picture_coding_type < 1 || picture_coding_type > 3)
				{
					DEBF("illegal picture_coding_type: %i\n", picture_coding_type);
					validPicHeader = 0;
				}
				else
				{
					validPicHeader = 1;
					cbuf[1] |= 0x7; cbuf[2] = 0xFF; cbuf[3] |= 0xF8; // vbv_delay is now 0xFFFF
				}
				COPY(4)
			}
			else if (ID == 0xB3) // seq header
			{
				LOCK(8)
					horizontal_size_value = (cbuf[0] << 4) | (cbuf[1] >> 4);
				vertical_size_value = ((cbuf[1] & 0xF) << 8) | cbuf[2];
				if (	horizontal_size_value > 720 || horizontal_size_value < 352
					||  vertical_size_value > 576 || vertical_size_value < 480
					|| (horizontal_size_value & 0xF) || (vertical_size_value & 0xF))
				{
					DEBF("illegal size, hori: %i verti: %i\n", horizontal_size_value, vertical_size_value);
					validSeqHeader = 0;
				}
				else
					validSeqHeader = 1;
				COPY(8)
			}
			else if (ID == 0xB5) // extension
			{
				LOCK(1)
					if ((cbuf[0] >> 4) == 0x8) // pic coding ext
					{
						LOCK(5)

							f_code[0][0] = (cbuf[0] & 0xF) - 1;
						f_code[0][1] = (cbuf[1] >> 4) - 1;
						f_code[1][0] = (cbuf[1] & 0xF) - 1;
						f_code[1][1] = (cbuf[2] >> 4) - 1;

						intra_dc_precision = (cbuf[2] >> 2) & 0x3;
						picture_structure = cbuf[2] & 0x3;
						frame_pred_frame_dct = (cbuf[3] >> 6) & 0x1;
						concealment_motion_vectors = (cbuf[3] >> 5) & 0x1;
						q_scale_type = (cbuf[3] >> 4) & 0x1;
						intra_vlc_format = (cbuf[3] >> 3) & 0x1;
						alternate_scan = (cbuf[3] >> 2) & 0x1;

						if (	(f_code[0][0] > 8 && f_code[0][0] < 14)
							||  (f_code[0][1] > 8 && f_code[0][1] < 14)
							||  (f_code[1][0] > 8 && f_code[1][0] < 14)
							||  (f_code[1][1] > 8 && f_code[1][1] < 14)
							||  picture_structure == 0)
						{
							DEBF("illegal ext, f_code[0][0]: %i f_code[0][1]: %i f_code[1][0]: %i f_code[1][1]: %i picture_structure:%i\n",
								f_code[0][0], f_code[0][1], f_code[1][0], f_code[1][1], picture_structure);
							validExtHeader = 0;
						}
						else
							validExtHeader = 1;
						COPY(5)
					}
					else
					{
						COPY(1)
					}
			}
			else if (ID == 0xB8) // gop header
			{
				LOCK(4)
					COPY(4)
			}
			else if ((ID >= 0x01) && (ID <= 0xAF) && validPicHeader && validSeqHeader && validExtHeader) // slice
			{
				uint8_t *outTemp = wbuf, *inTemp = cbuf;

				quant_corr = (((inbytecnt - (rbuf - cbuf)) / fact_x) - (outbytecnt + (wbuf - owbuf))) / REACT_DELAY;

				if 	(		((picture_coding_type == B_TYPE) && (quant_corr < 2.5f)) // don't recompress if we're in advance!
					||	((picture_coding_type == P_TYPE) && (quant_corr < -2.5f))
					||	((picture_coding_type == I_TYPE) && (quant_corr < -5.0f))
					)
				{
					uint8_t *nsc = cbuf;
					int fsc = 0, toLock;

					// lock all the slice
					while (!fsc)
					{
						toLock = nsc - cbuf + 3;
						LOCK(toLock)

							if ( (nsc[0] == 0) && (nsc[1] == 0) && (nsc[2] == 1) ) fsc = 1; // start code !
							else nsc++; // continue search
					}

					// init error
					sliceError = 0;

					// init bit buffer
					inbitbuf = 0; inbitcnt = 0;
					outbitbuf = 0; outbitcnt = BITS_IN_BUF;

					// get 32 bits
					Refill_bits();
					Refill_bits();
					Refill_bits();
					Refill_bits();

					// begin bit level recoding
					mpeg2_slice(ID);
					flush_read_buffer();
					flush_write_buffer();
					// end bit level recoding

					/*LOGF("type: %s code: %02i in : %6i out : %6i diff : %6i fact: %2.2f\n",
					(picture_coding_type == I_TYPE ? "I_TYPE" : (picture_coding_type == P_TYPE ? "P_TYPE" : "B_TYPE")),
					ID,  cbuf - inTemp, wbuf - outTemp, (wbuf - outTemp) - (cbuf - inTemp), (float)(cbuf - inTemp) / (float)(wbuf - outTemp));*/

					if ((wbuf - outTemp > cbuf - inTemp) || (sliceError > MAX_ERRORS)) // yes that might happen, rarely
					{
#ifndef NDEBUG
						if (sliceError > MAX_ERRORS)
						{
							DEBF("sliceError (%i) > MAX_ERRORS (%i)\n", sliceError, MAX_ERRORS);
						}
#endif

						/*LOGF("*** slice bigger than before !! (type: %s code: %i in : %i out : %i diff : %i)\n",
						(picture_coding_type == I_TYPE ? "I_TYPE" : (picture_coding_type == P_TYPE ? "P_TYPE" : "B_TYPE")),
						ID, cbuf - inTemp, wbuf - outTemp, (wbuf - outTemp) - (cbuf - inTemp));*/

						// in this case, we'll just use the original slice !
						tc_memcpy(outTemp, inTemp, cbuf - inTemp);
						wbuf = outTemp + (cbuf - inTemp);

						// adjust outbytecnt
						outbytecnt -= (wbuf - outTemp) - (cbuf - inTemp);
					}

				}
			}

#ifndef NDEBUG
			if ((ID >= 0x01) && (ID <= 0xAF) && (!validPicHeader || !validSeqHeader || !validExtHeader))
			{
				if (!validPicHeader) DEBF("missing pic header (%02X)\n", ID);
				if (!validSeqHeader) DEBF("missing seq header (%02X)\n", ID);
				if (!validExtHeader) DEBF("missing ext header (%02X)\n", ID);
			}
#endif

			if (wbuf - owbuf > MIN_WRITE) { WRITE }
	}

	// keeps gcc happy
	return;
}


k9Requant2::~k9Requant2() {
}

void k9Requant2::setInput(char * data,int size) {
	m_dataIn=data;
	m_sizeIn=size;
	rbuf = cbuf = orbuf = (uint8_t*) m_dataIn;
}

void k9Requant2::setOutput(char *data,int size) {
	m_dataOut=data;
	m_sizeOut=size;

	wbuf = owbuf =(uint8_t*) m_dataOut;

}

void k9Requant2::setFactor(float fact) {
	fact_x=fact;
}
