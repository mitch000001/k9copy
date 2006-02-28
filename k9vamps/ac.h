/*
 *  ac.h
 *
 *  Copyright (C) Thomas Östreich - November 2002
 *
 *  This file is part of transcode, a video stream processing tool
 *      
 *  transcode is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  transcode is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#ifndef _AC_H
#define _AC_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "k9common.h"

#include <unistd.h>

// #include "libtc/libtc.h"

#ifdef __bsdi__
typedef unsigned int uint32_t;
#endif

//mm_support
#define MM_C        0x0000 //plain C (default)
#define MM_IA32ASM  0x0001 //32-bit assembler optimized code (non-MMX)
#define MM_AMD64ASM 0x0002 //64-bit assembler optimized code (non-MMX)
#define MM_CMOVE    0x0004 //CMOVcc instruction
#define MM_MMX      0x0008 //standard MMX 
#define MM_MMXEXT   0x0010 //SSE integer functions or AMD MMX ext
#define MM_3DNOW    0x0020 //AMD 3DNOW 
#define MM_SSE      0x0040 //SSE functions 
#define MM_SSE2     0x0080 //PIV SSE2 functions 
#define MM_3DNOWEXT 0x0100 //AMD 3DNow! ext.
#define MM_SSE3     0x0200 //Prescott SSE3

extern void * (*tc_memcpy)(void *, const void *, size_t);
extern void tc_memcpy_init(int verbose, int mmflags);

extern int mm_flags;
int ac_mmflag();
void ac_mmtest();
char *ac_mmstr(int flag, int mode);

//ac_memcpy
void * ac_memcpy_mmx(void *dest, const void *src, size_t bytes);
void * ac_memcpy_amdmmx(void *dest, const void *src, size_t bytes);
void * ac_memcpy_amd64(void *dest, const void *src, size_t bytes);

//average (simple average over 2 rows)
int ac_average_mmx(char *row1, char *row2, char *out, int bytes);
int ac_average_sse(char *row1, char *row2, char *out, int bytes);
int ac_average_sse2(char *row1, char *row2, char *out, int bytes);

//swap
int ac_swap_rgb2bgr_asm(char *im, int bytes);
int ac_swap_rgb2bgr_asm64(char *im, int pixels);

//rescale
int ac_rescale_mmxext(char *row1, char *row2, char *out, int bytes, 
		      unsigned long weight1, unsigned long weight2);
int ac_rescale_sse(char *row1, char *row2, char *out, int bytes, 
		   unsigned long weight1, unsigned long weight2);
int ac_rescale_sse2(char *row1, char *row2, char *out, int bytes, 
		    unsigned long weight1, unsigned long weight2);

#endif
