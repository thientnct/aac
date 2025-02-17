/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2018 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software
that implements the MPEG Advanced Audio Coding ("AAC") encoding and decoding
scheme for digital audio. This FDK AAC Codec software is intended to be used on
a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient
general perceptual audio codecs. AAC-ELD is considered the best-performing
full-bandwidth communications codec by independent studies and is widely
deployed. AAC has been standardized by ISO and IEC as part of the MPEG
specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including
those of Fraunhofer) may be obtained through Via Licensing
(www.vialicensing.com) or through the respective patent owners individually for
the purpose of encoding or decoding bit streams in products that are compliant
with the ISO/IEC MPEG audio standards. Please note that most manufacturers of
Android devices already license these patent claims through Via Licensing or
directly from the patent owners, and therefore FDK AAC Codec software may
already be covered under those patent licenses when it is used for those
licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions
with enhanced sound quality, are also available from Fraunhofer. Users are
encouraged to check the Fraunhofer website for additional applications
information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification,
are permitted without payment of copyright license fees provided that you
satisfy the following conditions:

You must retain the complete text of this software license in redistributions of
the FDK AAC Codec or your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation
and/or other materials provided with redistributions of the FDK AAC Codec or
your modifications thereto in binary form. You must make available free of
charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived
from this library without prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute
the FDK AAC Codec software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating
that you changed the software and the date of any change. For modified versions
of the FDK AAC Codec, the term "Fraunhofer FDK AAC Codec Library for Android"
must be replaced by the term "Third-Party Modified Version of the Fraunhofer FDK
AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without
limitation the patents of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE.
Fraunhofer provides no warranty of patent non-infringement with respect to this
software.

You may use this FDK AAC Codec software or modifications thereto only for
purposes that are authorized by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright
holders and contributors "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
including but not limited to the implied warranties of merchantability and
fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary,
or consequential damages, including but not limited to procurement of substitute
goods or services; loss of use, data, or profits, or business interruption,
however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of
this software, even if advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------- */

/******************* Library for basic calculation routines ********************

   Author(s):   M. Lohwasser, M. Gayer

   Description:

********************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <sys/ifunc.h>
#include <common_fix.h>
#include "scramble.h"

#include <iostream>
#include <x86intrin.h>  // Include required headers even AVX headers if needed.
#include "fft_rad2.h"

#ifndef __FFT_RAD2_CPP__
#error \
    "Do not compile this file separately. It is included on demand from fft_rad2.cpp"
#endif

#ifndef FUNCTION_dit_fft

#define FUNCTION_dit_fft
#if defined FUNCTION_dit_fft

/*****************************************************************************

    functionname: dit_fft (analysis)-- sse2 function
    description:  dit-tukey-algorithm
                  scrambles data at entry
                  i.e. loop is made with scrambled data
    returns:
    input:
    output:

*****************************************************************************/
__attribute__ ((__target__ ("sse2")))
 static  void dit_fft_generic(FIXP_DBL *x, const INT ldn, const FIXP_STP *trigdata,
             const INT trigDataSize) {
  const INT n = 1 << ldn;
  INT trigstep, i, ldm;

  C_ALLOC_ALIGNED_CHECK(x);

  scramble(x, n);
  /*
   * 1+2 stage radix 4
   */
  int k = n*2;
  for (i = 0; i < k; i += 8) {
    FIXP_DBL a00, a10, a20, a30;
    a00 = (x[i + 0] + x[i + 2]) >> 1; /* Re A + Re B */
    a10 = (x[i + 4] + x[i + 6]) >> 1; /* Re C + Re D */
    a20 = (x[i + 1] + x[i + 3]) >> 1; /* Im A + Im B */
    a30 = (x[i + 5] + x[i + 7]) >> 1; /* Im C + Im D */

    x[i + 0] = a00 + a10; /* Re A' = Re A + Re B + Re C + Re D */
    x[i + 4] = a00 - a10; /* Re C' = Re A + Re B - Re C - Re D */
    x[i + 1] = a20 + a30; /* Im A' = Im A + Im B + Im C + Im D */
    x[i + 5] = a20 - a30; /* Im C' = Im A + Im B - Im C - Im D */

    a00 = a00 - x[i + 2]; /* Re A - Re B */
    a10 = a10 - x[i + 6]; /* Re C - Re D */
    a20 = a20 - x[i + 3]; /* Im A - Im B */
    a30 = a30 - x[i + 7]; /* Im C - Im D */

    x[i + 2] = a00 + a30; /* Re B' = Re A - Re B + Im C - Im D */
    x[i + 6] = a00 - a30; /* Re D' = Re A - Re B - Im C + Im D */
    x[i + 3] = a20 - a10; /* Im B' = Im A - Im B - Re C + Re D */
    x[i + 7] = a20 + a10; /* Im D' = Im A - Im B + Re C - Re D */
  }

  for (ldm = 3; ldm <= ldn; ++ldm) {
    INT m = (1 << ldm);
    INT mh = (m >> 1);
    INT j, r;

    trigstep = ((trigDataSize << 2) >> ldm);

    FDK_ASSERT(trigstep > 0);

    /* Do first iteration with c=1.0 and s=0.0 separately to avoid loosing to
       much precision. Beware: The impact on the overal FFT precision is rather
       large. */
    { /* block 1 */

      j = 0;

      for (r = 0; r < n; r += m) {
        INT t1 = (r + j) << 1;
        INT t2 = t1 + (mh << 1);
        FIXP_DBL vr, vi, ur, ui;

        // cplxMultDiv2(&vi, &vr, x[t2+1], x[t2], (FIXP_SGL)1.0, (FIXP_SGL)0.0);
        vi = x[t2 + 1] >> 1;
        vr = x[t2] >> 1;

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui + vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui - vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        // cplxMultDiv2(&vr, &vi, x[t2+1], x[t2], (FIXP_SGL)1.0, (FIXP_SGL)0.0);
        vr = x[t2 + 1] >> 1;
        vi = x[t2] >> 1;

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;
      }

    } /* end of  block 1 */

    for (j = 1; j < mh / 4; ++j) {
      FIXP_STP cs;

      cs = trigdata[j * trigstep];

      for (r = 0; r < n; r += m) {
        INT t1 = (r + j) << 1;
        INT t2 = t1 + (mh << 1);
        FIXP_DBL vr, vi, ur, ui;

        cplxMultDiv2(&vi, &vr, x[t2 + 1], x[t2], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui + vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui - vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vr, &vi, x[t2 + 1], x[t2], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;

        /* Same as above but for t1,t2 with j>mh/4 and thus cs swapped */
        t1 = (r + mh / 2 - j) << 1;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vi, &vr, x[t2], x[t2 + 1], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vr, &vi, x[t2], x[t2 + 1], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur - vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur + vr;
        x[t2 + 1] = ui + vi;
      }
    }

    { /* block 2 */
      j = mh / 4;

      for (r = 0; r < n; r += m) {
        INT t1 = (r + j) << 1;
        INT t2 = t1 + (mh << 1);
        FIXP_DBL vr, vi, ur, ui;

        cplxMultDiv2(&vi, &vr, x[t2 + 1], x[t2], STC(0x5a82799a),
                     STC(0x5a82799a));

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui + vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui - vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vr, &vi, x[t2 + 1], x[t2], STC(0x5a82799a),
                     STC(0x5a82799a));

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;
      }
    } /* end of block 2 */
  }
}




/*****************************************************************************

    functionname: dit_fft (analysis)--avx2 function
    description:  dit-tukey-algorithm
                  scrambles data at entry
                  i.e. loop is made with scrambled data
    returns:
    input:
    output:

*****************************************************************************/
__attribute__ ((__target__ ("avx2,fma")))
static void dit_fft_avx2(FIXP_DBL *x, const INT ldn, const FIXP_STP *trigdata,
             const INT trigDataSize) {
  const INT n = 1 << ldn;
  INT trigstep, i, ldm,p;

  C_ALLOC_ALIGNED_CHECK(x);

  scramble(x, n);
  /*
   * 1+2 stage radix 4
   */
  int k = n*2;
  if (( k%64) == 0 ) {
     for ( i = 0, p=0; i < k; i += 64,p+=64) {
        /* Allocating 8(32-bit) packed integers in a 256-bit variable,thereby performing vector operations for
         * 64(32-bit) elements in parallel 
         * */
        __m256i xdash_1 = _mm256_set_epi32 ( x[56+i],x[48+i],x[40+i], x[32+i],x[24+i],  x[16+i],  x[8+i],  x[0+i]);
        __m256i ydash_1 = _mm256_set_epi32 ( x[58+i],x[50+i],x[42+i], x[34+i],x[26+i],  x[18+i],  x[10+i], x[2+i]);
        __m256i xdash_2 = _mm256_set_epi32 ( x[60+i],x[52+i],x[44+i], x[36+i],x[28+i],  x[20+i],  x[12+i], x[4+i]);
        __m256i ydash_2 = _mm256_set_epi32 ( x[62+i],x[54+i],x[46+i], x[38+i],x[30+i],  x[22+i],  x[14+i], x[6+i]);
        __m256i xdash_3 = _mm256_set_epi32 ( x[57+i],x[49+i],x[41+i], x[33+i],x[25+i],  x[17+i],  x[9+i],  x[1+i]);
        __m256i ydash_3 = _mm256_set_epi32 ( x[59+i],x[51+i],x[43+i], x[35+i],x[27+i],  x[19+i],  x[11+i], x[3+i]);
        __m256i xdash_4 = _mm256_set_epi32 ( x[61+i],x[53+i],x[45+i], x[37+i],x[29+i],  x[21+i],  x[13+i], x[5+i]);
        __m256i ydash_4 = _mm256_set_epi32 ( x[63+i],x[55+i],x[47+i], x[39+i],x[31+i],  x[23+i],  x[15+i], x[7+i]);

        __m256i a00,a10,a20,a30,xdash_11,xdash_22,xdash_33,xdash_44,xx1,xx2,xx3,xx4,yy1,yy2,yy3,yy4,
        a00_1,a10_1, a20_1, a30_1,xx11,xx22,xx33,xx44,yy11,yy22,yy33,yy44;

        a00=  _mm256_add_epi32(xdash_1, ydash_1);
        a00= _mm256_srai_epi32(a00, 1);

        a10=  _mm256_add_epi32(xdash_2, ydash_2);
        a10= _mm256_srai_epi32(a10, 1);

        a20=  _mm256_add_epi32(xdash_3, ydash_3);
        a20= _mm256_srai_epi32(a20, 1);
        
	       a30=  _mm256_add_epi32(xdash_4, ydash_4);
        a30= _mm256_srai_epi32(a30, 1);    

        xdash_11 =  _mm256_add_epi32(a00, a10); /* Re A' = Re A + Re B + Re C + Re D */
        xdash_22 =  _mm256_sub_epi32(a00, a10); /* Re C' = Re A + Re B - Re C - Re D */
        xdash_33 =  _mm256_add_epi32(a20, a30); /* Im A' = Im A + Im B + Im C + Im D */
        xdash_44 =  _mm256_sub_epi32(a20, a30); /* Im C' = Im A + Im B + Im C + Im D */

        xx1 = _mm256_unpacklo_epi32(xdash_11,xdash_33);
        yy1 = _mm256_unpackhi_epi32(xdash_11,xdash_33);
        xx2 = _mm256_unpacklo_epi32(xdash_22,xdash_44);
        yy2 = _mm256_unpackhi_epi32(xdash_22,xdash_44);

        a00_1 = _mm256_sub_epi32(a00, ydash_1); /* Re A - Re B */
        a10_1 = _mm256_sub_epi32(a10, ydash_2); /* Re C - Re D */
        a20_1 = _mm256_sub_epi32(a20, ydash_3); /* Im A - Im B */
        a30_1 = _mm256_sub_epi32(a30, ydash_4); /* Im C - Im D */

        xdash_11 = _mm256_add_epi32(a00_1,a30_1); /* Re B' = Re A - Re B + Im C - Im D */
        xdash_22 = _mm256_sub_epi32(a00_1,a30_1); /* Re D' = Re A - Re B - Im C + Im D */
        xdash_33 = _mm256_sub_epi32(a20_1,a10_1); /* Im B' = Im A - Im B - Re C + Re D */
        xdash_44 = _mm256_add_epi32(a20_1,a10_1); /* Im D' = Im A - Im B + Re C - Re D */

        xx3 = _mm256_unpacklo_epi32(xdash_11,xdash_33);
        yy3 = _mm256_unpackhi_epi32(xdash_11,xdash_33);
        xx4 = _mm256_unpacklo_epi32(xdash_22,xdash_44);
        yy4 = _mm256_unpackhi_epi32(xdash_22,xdash_44);

        /* Unpacking so as to reaarange the 4(32-bit) elements in each half of 128-bit lane so that
         * they can be stored in an aligned fashion.
         * */
        xx11 = _mm256_unpacklo_epi64(xx1,xx3);
        xx22 = _mm256_unpacklo_epi64(xx2,xx4);
        yy11 = _mm256_unpacklo_epi64(yy1,yy3);
        yy22 = _mm256_unpacklo_epi64(yy2,yy4);

        xx33 = _mm256_unpackhi_epi64(xx1,xx3);
        xx44 = _mm256_unpackhi_epi64(xx2,xx4);
        yy33 = _mm256_unpackhi_epi64(yy1,yy3);
        yy44 = _mm256_unpackhi_epi64(yy2,yy4);

        /* Storing high and low 128-bit halves (each composed of integer data) into memory in
         * two different 128-bit locations, each half has aligned data of 4(32-bit) elements
         * */
        _mm256_storeu2_m128i((__m128i*)&x[p+32],(__m128i*)&x[p+0],xx11);
        _mm256_storeu2_m128i((__m128i*)&x[p+36],(__m128i*)&x[p+4],xx22);
        _mm256_storeu2_m128i((__m128i*)&x[p+40],(__m128i*)&x[p+8],xx33);
        _mm256_storeu2_m128i((__m128i*)&x[p+44],(__m128i*)&x[p+12],xx44);

        _mm256_storeu2_m128i((__m128i*)&x[p+48],(__m128i*)&x[p+16],yy11);
        _mm256_storeu2_m128i((__m128i*)&x[p+52],(__m128i*)&x[p+20],yy22);
        _mm256_storeu2_m128i((__m128i*)&x[p+56],(__m128i*)&x[p+24],yy33);
        _mm256_storeu2_m128i((__m128i*)&x[p+60],(__m128i*)&x[p+28],yy44);
     }
   } else {
     for (i = 0; i < k; i += 8) {
        FIXP_DBL a00, a10, a20, a30;
        a00 = (x[i + 0] + x[i + 2]) >> 1; /* Re A + Re B */
        a10 = (x[i + 4] + x[i + 6]) >> 1; /* Re C + Re D */
        a20 = (x[i + 1] + x[i + 3]) >> 1; /* Im A + Im B */
        a30 = (x[i + 5] + x[i + 7]) >> 1; /* Im C + Im D */

        x[i + 0] = a00 + a10; /* Re A' = Re A + Re B + Re C + Re D */
        x[i + 4] = a00 - a10; /* Re C' = Re A + Re B - Re C - Re D */
        x[i + 1] = a20 + a30; /* Im A' = Im A + Im B + Im C + Im D */
        x[i + 5] = a20 - a30; /* Im C' = Im A + Im B - Im C - Im D */

        a00 = a00 - x[i + 2]; /* Re A - Re B */
        a10 = a10 - x[i + 6]; /* Re C - Re D */
        a20 = a20 - x[i + 3]; /* Im A - Im B */
        a30 = a30 - x[i + 7]; /* Im C - Im D */

        x[i + 2] = a00 + a30; /* Re B' = Re A - Re B + Im C - Im D */
        x[i + 6] = a00 - a30; /* Re D' = Re A - Re B - Im C + Im D */
        x[i + 3] = a20 - a10; /* Im B' = Im A - Im B - Re C + Re D */
        x[i + 7] = a20 + a10; /* Im D' = Im A - Im B + Re C - Re D */
     }
  }

   for (ldm = 3; ldm <= ldn; ++ldm) {
    INT m = (1 << ldm);
    INT mh = (m >> 1);
    INT j, r;

    trigstep = ((trigDataSize << 2) >> ldm);

    FDK_ASSERT(trigstep > 0);

    /* Do first iteration with c=1.0 and s=0.0 separately to avoid loosing to
       much precision. Beware: The impact on the overal FFT precision is rather
       large. */
    { /* block 1 */

      j = 0;

      for (r = 0; r < n; r += m) {
        INT t1 = (r + j) << 1;
        INT t2 = t1 + (mh << 1);
        FIXP_DBL vr, vi, ur, ui;

        // cplxMultDiv2(&vi, &vr, x[t2+1], x[t2], (FIXP_SGL)1.0, (FIXP_SGL)0.0);
        vi = x[t2 + 1] >> 1;
        vr = x[t2] >> 1;

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui + vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui - vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        // cplxMultDiv2(&vr, &vi, x[t2+1], x[t2], (FIXP_SGL)1.0, (FIXP_SGL)0.0);
        vr = x[t2 + 1] >> 1;
        vi = x[t2] >> 1;

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;
      }

    } /* end of  block 1 */

    for (j = 1; j < mh / 4; ++j) {
      FIXP_STP cs;

      cs = trigdata[j * trigstep];

      for (r = 0; r < n; r += m) {
        INT t1 = (r + j) << 1;
        INT t2 = t1 + (mh << 1);
        FIXP_DBL vr, vi, ur, ui;

        cplxMultDiv2(&vi, &vr, x[t2 + 1], x[t2], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui + vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui - vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vr, &vi, x[t2 + 1], x[t2], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;

        /* Same as above but for t1,t2 with j>mh/4 and thus cs swapped */
        t1 = (r + mh / 2 - j) << 1;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vi, &vr, x[t2], x[t2 + 1], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vr, &vi, x[t2], x[t2 + 1], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur - vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur + vr;
        x[t2 + 1] = ui + vi;
      }
    }

    { /* block 2 */
      j = mh / 4;

      for (r = 0; r < n; r += m) {
        INT t1 = (r + j) << 1;
        INT t2 = t1 + (mh << 1);
        FIXP_DBL vr, vi, ur, ui;

        cplxMultDiv2(&vi, &vr, x[t2 + 1], x[t2], STC(0x5a82799a),
                     STC(0x5a82799a));

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui + vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui - vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vr, &vi, x[t2 + 1], x[t2], STC(0x5a82799a),
                     STC(0x5a82799a));

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;
      }
    } /* end of block 2 */
  }
}

/* Dynamic dispatcher ifunc to invoke the required dit_fft implementation based on sse2 and avx2 support */ 
extern "C" {
   typedef void dit_fft_func(FIXP_DBL *x, const INT ldn, const FIXP_STP *trigdata,
                           const INT trigDataSize);
   dit_fft_func* dit_fft_resolver() {
      __builtin_cpu_init();
      if (__builtin_cpu_supports("avx2"))
	    return dit_fft_avx2;
      else
	    return dit_fft_generic;
   }
   dit_fft_func dit_fft __attribute__((ifunc("dit_fft_resolver")));
}
#endif

#endif
