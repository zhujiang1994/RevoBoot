/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Portions Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 2.0 (the "License").  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON- INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/**************************************************************
 LZSS.C -- A Data Compression Program
***************************************************************
    4/6/1989 Haruhiko Okumura
    Use, distribute, and modify this program freely.
    Please send me your improved versions.
        PC-VAN      SCIENCE
        NIFTY-Serve PAF01022
        CompuServe  74050,1022

**************************************************************/
/*
 *  lzss.c - Package for decompressing lzss compressed objects
 *
 *  Copyright (c) 2003 Apple Computer, Inc.
 *
 *  DRI: Josh de Cesare
 */

#include <sl.h>

#define N			4096	// Size of ring buffer - must be power of 2.
#define N_MIN_1		4095
#define F			18		// Upper limit for match_length.
#define R			N - F
#define THRESHOLD	2		// Encode string into position and length, 
							// if match_length is greater than this.
#define NIL			N		// Index for root of binary search trees.


//==============================================================================
// Refactoring and bug fix Copyright (c) 2010 by DHP.

int decompressLZSS(u_int8_t * dst, u_int8_t * src, u_int32_t srclen)
{
	// Four KB ring buffer with 17 extra bytes added to aid string comparisons.
	u_int8_t text_buf[N_MIN_1 + F];
	u_int8_t * dststart = dst;
	const u_int8_t * srcend = (src + srclen);
	
	int r = R;
	int  i, j, k, c;
	unsigned int flags = 0;
	
	for (i = 0; i < R; i++)
	{
		text_buf[i] = ' ';
	}
	
	while (src < srcend)
	{
		if (((flags >>= 1) & 0x100) == 0)
		{
			c = *src++;
			flags = c | 0xFF00;  // Clever use of the high byte.
        }
		
        if ((src < srcend) && (flags & 1))
		{
			c = *src++;
			*dst++ = c;
			text_buf[r++] = c;
			r &= N_MIN_1;
		}
		else if ((src + 2) <= srcend)
		{
			i = *src++;
			j = *src++;

			i |= ((j & 0xF0) << 4);
			j = (j & 0x0F) + THRESHOLD;
				
			for (k = 0; k <= j; k++)
			{
				c = text_buf[(i + k) & N_MIN_1];
				*dst++ = c;
				text_buf[r++] = c;
				r &= N_MIN_1;
			}
		}
	}
    
	return dst - dststart;
}
