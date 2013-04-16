
/* -----------------------------------------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2012 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
  All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software that implements
the MPEG Advanced Audio Coding ("AAC") encoding and decoding scheme for digital audio.
This FDK AAC Codec software is intended to be used on a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient general perceptual
audio codecs. AAC-ELD is considered the best-performing full-bandwidth communications codec by
independent studies and is widely deployed. AAC has been standardized by ISO and IEC as part
of the MPEG specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including those of Fraunhofer)
may be obtained through Via Licensing (www.vialicensing.com) or through the respective patent owners
individually for the purpose of encoding or decoding bit streams in products that are compliant with
the ISO/IEC MPEG audio standards. Please note that most manufacturers of Android devices already license
these patent claims through Via Licensing or directly from the patent owners, and therefore FDK AAC Codec
software may already be covered under those patent licenses when it is used for those licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions with enhanced sound quality,
are also available from Fraunhofer. Users are encouraged to check the Fraunhofer website for additional
applications information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification, are permitted without
payment of copyright license fees provided that you satisfy the following conditions:

You must retain the complete text of this software license in redistributions of the FDK AAC Codec or
your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation and/or other materials
provided with redistributions of the FDK AAC Codec or your modifications thereto in binary form.
You must make available free of charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived from this library without
prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute the FDK AAC Codec
software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating that you changed the software
and the date of any change. For modified versions of the FDK AAC Codec, the term
"Fraunhofer FDK AAC Codec Library for Android" must be replaced by the term
"Third-Party Modified Version of the Fraunhofer FDK AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without limitation the patents of Fraunhofer,
ARE GRANTED BY THIS SOFTWARE LICENSE. Fraunhofer provides no warranty of patent non-infringement with
respect to this software.

You may use this FDK AAC Codec software or modifications thereto only for purposes that are authorized
by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright holders and contributors
"AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, including but not limited to the implied warranties
of merchantability and fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary, or consequential damages,
including but not limited to procurement of substitute goods or services; loss of use, data, or profits,
or business interruption, however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of this software, even if
advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------------------------------------- */

/*!
  \file   dct.cpp
  \brief  DCT Implementations
  Library functions to calculate standard DCTs. This will most likely be replaced by hand-optimized
  functions for the specific target processor.

  Three different implementations of the dct type II and the dct type III transforms are provided.

  By default implementations which are based on a single, standard complex FFT-kernel are used (dctII_f() and dctIII_f()).
  These are specifically helpful in cases where optimized FFT libraries are already available. The FFT used in these
  implementation is FFT rad2 from FDK_tools.

  Of course, one might also use DCT-libraries should they be available. The DCT and DST
  type IV implementations are only available in a version based on a complex FFT kernel.
*/


#if defined(MIPS_DSP_R1_LE)
#define FUNCTION_dct_II
#define FUNCTION_dct_IV
#define FUNCTION_dst_IV
#endif /* #if defined(MIPS_DSP_R1_LE) */

#if defined(MIPS_DSP_R2_LE)
#define FUNCTION_dct_III
#endif /* #if defined(MIPS_DSP_R2_LE) */

#if defined (FUNCTION_dct_IV) || defined (FUNCTION_dst_IV)
static
void getTables(const FIXP_WTP **twiddle, const FIXP_STP **sin_twiddle, int *sin_step, int length);
#endif

#if defined (FUNCTION_dct_II)
void dct_II(FIXP_DBL *pDat, /*!< pointer to input/output */
            FIXP_DBL *tmp,  /*!< pointer to temporal working buffer */
            int L,          /*!< lenght of transform */
            int *pDat_e
            )
{
    FDK_ASSERT(L == 64 || L == 32);
    FIXP_DBL accu1,accu2;
    FIXP_DBL *pTmp_0, *pTmp_1;

    int i;
    int inc;
    int M =  L>>1;
    int ld_M;

    FDK_ASSERT(L == 64 || L == 32);
    ld_M = 4 + (L >> 6);  /* L=64: 5,  L=32: 4 */

    inc = (64/2) >> ld_M; /* L=64: 1,  L=32: 2 */

    FIXP_DBL *pdat  = &pDat[0];
    FIXP_DBL accu3, accu4;
    pTmp_0 = &tmp[0];
    pTmp_1 = &tmp[L-1];
    for (i = M>>1; i--; )
    {
      accu1 = *pdat++;
      accu2 = *pdat++;
      accu3 = *pdat++;
      accu4 = *pdat++;
      accu1 >>= 1;
      accu2 >>= 1;
      accu3 >>= 1;
      accu4 >>= 1;
      *pTmp_0++ = accu1;
      *pTmp_0++ = accu3;
      *pTmp_1-- = accu2;
      *pTmp_1-- = accu4;
    }

    fft(M, tmp, pDat_e);

    pTmp_0 = &tmp[2];
    pTmp_1 = &tmp[(M-1)*2];

    for (i=1; i<M>>1; i+=2,pTmp_0+=4,pTmp_1-=4) {

      FIXP_DBL a1,a2,a11,a22;
      FIXP_DBL accu3, accu4,accu33, accu44,accu11, accu22;
      FIXP_DBL temp_1,temp_2,temp_3,temp_4,temp_5,temp_6,temp_7,temp_8;
      FIXP_DBL *tmp_sin_4_inc = (FIXP_DBL*)(&sin_twiddle_L64[0]);

      __asm__ volatile(
        "lw     %[temp_1], 4(%[pTmp_0])                        \n\t"
        "lw     %[temp_2], 4(%[pTmp_1])                        \n\t"
        "lw     %[temp_3], 0(%[pTmp_1])                        \n\t"
        "lw     %[temp_4], 0(%[pTmp_0])                        \n\t"
        "lw     %[temp_5], 12(%[pTmp_0])                       \n\t"
        "lw     %[temp_6], -4(%[pTmp_1])                       \n\t"
        "lw     %[temp_7], -8(%[pTmp_1])                       \n\t"
        "lw     %[temp_8], 8(%[pTmp_0])                        \n\t"

        "mul    %[accu1],  %[i],              %[inc]           \n\t"

        "sra    %[temp_1], %[temp_1],         1                \n\t"
        "sra    %[temp_2], %[temp_2],         1                \n\t"
        "sra    %[temp_3], %[temp_3],         1                \n\t"
        "sra    %[temp_4], %[temp_4],         1                \n\t"
        "sra    %[temp_5], %[temp_5],         1                \n\t"
        "sra    %[temp_6], %[temp_6],         1                \n\t"
        "sra    %[temp_7], %[temp_7],         1                \n\t"
        "sra    %[temp_8], %[temp_8],         1                \n\t"
        "addu   %[a1],     %[temp_1],         %[temp_2]        \n\t"

        "subu   %[a2],     %[temp_3],         %[temp_4]        \n\t"
        "addu   %[a11],    %[temp_5],         %[temp_6]        \n\t"
        "subu   %[a22],    %[temp_7],         %[temp_8]        \n\t"
        "sll    %[accu1],  %[accu1],          5                \n\t"
        "addu   %[accu1],  %[accu1],          %[tmp_sin_4_inc] \n\t"
        "sll    %[accu11], %[inc],            5                \n\t"
        "addu   %[accu11], %[accu11],         %[accu1]         \n\t"

        "lw     %[accu2],  0(%[accu1])                         \n\t"
        "lw     %[accu1],  4(%[accu1])                         \n\t"
        "lw     %[accu22], 0(%[accu11])                        \n\t"
        "lw     %[accu11], 4(%[accu11])                        \n\t"

        "mult   $ac0,      %[a2],             %[accu2]         \n\t"
        "mult   $ac1,      %[a2],             %[accu1]         \n\t"
        "mult   $ac2,      %[a22],            %[accu22]        \n\t"
        "mult   $ac3,      %[a22],            %[accu11]        \n\t"

        "msub   $ac0,      %[accu1],          %[a1]            \n\t"
        "madd   $ac1,      %[accu2],          %[a1]            \n\t"
        "msub   $ac2,      %[accu11],         %[a11]           \n\t"
        "madd   $ac3,      %[accu22],         %[a11]           \n\t"

        "addu   %[a1],     %[temp_4],         %[temp_3]        \n\t"
        "mul    %[temp_3], %[i],              %[inc]           \n\t"

        "mfhi   %[accu1],  $ac0                                \n\t"
        "mfhi   %[accu2],  $ac1                                \n\t"
        "mfhi   %[accu11], $ac2                                \n\t"
        "mfhi   %[accu22], $ac3                                \n\t"

        "subu   %[a2],     %[temp_1],         %[temp_2]        \n\t"
        "addu   %[a11],    %[temp_8],         %[temp_7]        \n\t"
        "subu   %[a22],    %[temp_5],         %[temp_6]        \n\t"

        "addu   %[temp_4], %[temp_3],         %[inc]           \n\t"
        "sll    %[temp_3], %[temp_3],         3                \n\t"
        "sll    %[temp_4], %[temp_4],         3                \n\t"
        "addu   %[temp_3], %[temp_3],         %[tmp_sin_4_inc] \n\t"
        "addu   %[temp_4], %[temp_4],         %[tmp_sin_4_inc] \n\t"

        "sll    %[accu1],  %[accu1],          1                \n\t"
        "sll    %[accu2],  %[accu2],          1                \n\t"
        "sll    %[accu11], %[accu11],         1                \n\t"
        "sll    %[accu22], %[accu22],         1                \n\t"

        "lw     %[temp_1], 0(%[temp_3])                        \n\t"
        "lw     %[temp_3], 4(%[temp_3])                        \n\t"
        "lw     %[temp_2], 0(%[temp_4])                        \n\t"
        "lw     %[temp_4], 4(%[temp_4])                        \n\t"

        "addu   %[temp_5], %[accu2],          %[a1]            \n\t"
        "addu   %[temp_6], %[accu1],          %[a2]            \n\t"
        "addu   %[temp_7], %[accu22],         %[a11]           \n\t"
        "addu   %[temp_8], %[accu11],         %[a22]           \n\t"

        "mult   $ac0,      %[temp_5],         %[temp_1]        \n\t"
        "mult   $ac1,      %[temp_5],         %[temp_3]        \n\t"
        "mult   $ac2,      %[temp_7],         %[temp_2]        \n\t"
        "mult   $ac3,      %[temp_7],         %[temp_4]        \n\t"

        "madd   $ac0,      %[temp_6],         %[temp_3]        \n\t"
        "msub   $ac1,      %[temp_6],         %[temp_1]        \n\t"
        "madd   $ac2,      %[temp_8],         %[temp_4]        \n\t"
        "msub   $ac3,      %[temp_8],         %[temp_2]        \n\t"
        "subu   %[temp_5], %[L],              %[i]             \n\t"
        "subu   %[temp_7], %[M],              %[i]             \n\t"
        "mul    %[temp_7], %[temp_7],         %[inc]           \n\t"
        "mfhi   %[temp_1], $ac0                                \n\t"
        "mfhi   %[temp_2], $ac1                                \n\t"
        "mfhi   %[temp_3], $ac2                                \n\t"
        "mfhi   %[temp_4], $ac3                                \n\t"

        "sll    %[temp_6], %[i],              2                \n\t"
        "sll    %[temp_5], %[temp_5],         2                \n\t"
        "addu   %[temp_6], %[temp_6],         %[pDat]          \n\t"
        "addu   %[temp_5], %[temp_5],         %[pDat]          \n\t"

        "subu   %[temp_8], %[temp_7],         %[inc]           \n\t"
        "sll    %[temp_7], %[temp_7],         3                \n\t"
        "sll    %[temp_8], %[temp_8],         3                \n\t"

        "addu   %[temp_7], %[temp_7],         %[tmp_sin_4_inc] \n\t"
        "addu   %[temp_8], %[temp_8],         %[tmp_sin_4_inc] \n\t"
        "sw     %[temp_1], 0(%[temp_6])                        \n\t"
        "sw     %[temp_2], 0(%[temp_5])                        \n\t"
        "sw     %[temp_3], 4(%[temp_6])                        \n\t"
        "sw     %[temp_4], -4(%[temp_5])                       \n\t"

        "lw     %[temp_1], 0(%[temp_7])                        \n\t"
        "lw     %[temp_3], 4(%[temp_7])                        \n\t"
        "lw     %[temp_2], 0(%[temp_8])                        \n\t"
        "lw     %[temp_4], 4(%[temp_8])                        \n\t"

        "subu   %[temp_5], %[a1],             %[accu2]         \n\t"
        "subu   %[temp_6], %[accu1],          %[a2]            \n\t"
        "subu   %[temp_7], %[a11],            %[accu22]        \n\t"
        "subu   %[temp_8], %[accu11],         %[a22]           \n\t"

        "mult   $ac0,      %[temp_5],         %[temp_1]        \n\t"
        "mult   $ac1,      %[temp_5],         %[temp_3]        \n\t"
        "mult   $ac2,      %[temp_7],         %[temp_2]        \n\t"
        "mult   $ac3,      %[temp_7],         %[temp_4]        \n\t"

        "madd   $ac0,      %[temp_6],         %[temp_3]        \n\t"
        "msub   $ac1,      %[temp_6],         %[temp_1]        \n\t"
        "madd   $ac2,      %[temp_8],         %[temp_4]        \n\t"
        "msub   $ac3,      %[temp_8],         %[temp_2]        \n\t"

        "addu   %[temp_7], %[M],              %[i]             \n\t"
        "subu   %[temp_8], %[M],              %[i]             \n\t"

        "mfhi   %[temp_1], $ac0                                \n\t"
        "mfhi   %[temp_2], $ac1                                \n\t"
        "mfhi   %[temp_3], $ac2                                \n\t"
        "mfhi   %[temp_4], $ac3                                \n\t"

        "sll    %[temp_8], %[temp_8],         2                \n\t"
        "sll    %[temp_7], %[temp_7],         2                \n\t"
        "addu   %[temp_8], %[temp_8],         %[pDat]          \n\t"
        "addu   %[temp_7], %[temp_7],         %[pDat]          \n\t"

        "sw     %[temp_1], 0(%[temp_8])                        \n\t"
        "sw     %[temp_2], 0(%[temp_7])                        \n\t"
        "sw     %[temp_3], -4(%[temp_8])                       \n\t"
        "sw     %[temp_4], 4(%[temp_7])                        \n\t"

        : [temp_1] "=&r" (temp_1), [temp_2] "=&r" (temp_2),
          [temp_3] "=&r" (temp_3), [temp_4] "=&r" (temp_4),
          [temp_5] "=&r" (temp_5), [temp_6] "=&r" (temp_6),
          [temp_7] "=&r" (temp_7), [temp_8] "=&r" (temp_8),
          [accu1] "=&r" (accu1), [accu2] "=&r" (accu2),
          [accu11] "=&r" (accu11), [accu22] "=&r" (accu22),
          [a1] "=&r" (a1), [a2] "=&r" (a2),
          [a11] "=&r" (a11), [a22] "=&r" (a22)
        : [pTmp_0] "r" (pTmp_0), [pTmp_1] "r" (pTmp_1), [i] "r" (i), [inc] "r" (inc),
          [tmp_sin_4_inc] "r" (tmp_sin_4_inc), [pDat] "r" (pDat), [L] "r" (L), [M] "r" (M)
        : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo", "memory"
      );
   }

    cplxMultDiv2(&accu1, &accu2, tmp[M], tmp[M+1], sin_twiddle_L64[(M/2)*inc]);
    pDat[L - (M/2)] = accu2;
    pDat[M/2]       = accu1;

    pDat[0] = (tmp[0]>>1)+(tmp[1]>>1);
    pDat[M] =  fMult(((tmp[0]>>1)-(tmp[1]>>1)), sin_twiddle_L64[64/2].v.re);/* cos((PI/(2*L))*M); */

    *pDat_e += 2;
}
#endif /* #if defined (FUNCTION_dct_II) */

#if defined(FUNCTION_dct_III)
void dct_III(FIXP_DBL *pDat, /*!< pointer to input/output */
             FIXP_DBL *tmp,  /*!< pointer to temporal working buffer */
             int L,          /*!< lenght of transform */
             int *pDat_e
             )
{
  int  i;
  FIXP_DBL xr, accu1, accu2;
  int M = L>>1;
  FIXP_DBL *pTmp_0 = &tmp[2];
  FIXP_DBL *pTmp_1 = &tmp[(M-1)*2];
  long *ptr1, *ptr2, *ptr3, twiddle_1, twiddle_2, twiddle_3, loop_end;
  long *pDat_1, *pDat_2;
  int temp0, temp1, temp2, temp3, temp4, temp5, temp6;

  if (L==32) {
    pDat_1 = &pDat[-1];
    pDat_2 = &pDat[1];
    ptr1 = (long*)&sin_twiddle_L64[2];
    ptr2 = (long*)&sin_twiddle_L64[30];
    ptr3 = (long*)&sin_twiddle_L64[8];
    loop_end = (long)&pDat[8];

    __asm__ volatile (
        "lw     %[temp0],   128(%[pDat_1])            \n\t"
        "lw     %[temp1],   0(%[ptr1])                \n\t"
        "lw     %[temp2],   0(%[pDat_2])              \n\t"
        "lw     %[temp3],   4(%[ptr1])                \n\t"
        "lw     %[temp4],   64(%[pDat_2])             \n\t"
        "lw     %[temp5],   0(%[ptr2])                \n\t"
        "lw     %[temp6],   64(%[pDat_1])             \n\t"
        "lw     %[xr],      4(%[ptr2])                \n\t"
    "1:                                               \n\t"
        "mult   $ac0,       %[temp0],     %[temp1]    \n\t"
        "mult   $ac1,       %[temp0],     %[temp3]    \n\t"
        "mult   $ac2,       %[temp4],     %[temp5]    \n\t"
        "mult   $ac3,       %[temp4],     %[xr]       \n\t"
        "msub   $ac0,       %[temp2],     %[temp3]    \n\t"
        "madd   $ac1,       %[temp2],     %[temp1]    \n\t"
        "msub   $ac2,       %[temp6],     %[xr]       \n\t"
        "madd   $ac3,       %[temp6],     %[temp5]    \n\t"
        "lw     %[temp0],   0(%[ptr3])                \n\t"
        "mfhi   %[accu2],   $ac0                      \n\t"
        "mfhi   %[accu1],   $ac1                      \n\t"
        "mfhi   %[temp5],   $ac2                      \n\t"
        "mfhi   %[temp6],   $ac3                      \n\t"
        "addiu  %[pDat_1],  %[pDat_1],    -4          \n\t"
        "addiu  %[pDat_2],  %[pDat_2],    4           \n\t"
        "lw     %[temp1],   4(%[ptr3])                \n\t"
        "sra    %[temp4],   %[accu2],     1           \n\t"
        "sra    %[temp3],   %[accu1],     1           \n\t"
        "sra    %[temp5],   %[temp5],     1           \n\t"
        "sra    %[temp6],   %[temp6],     1           \n\t"
        "subu   %[temp2],   %[temp6],     %[temp3]    \n\t"
        "addu   %[xr],      %[temp4],     %[temp5]    \n\t"
        "mult   $ac2,       %[temp2],     %[temp1]    \n\t"
        "madd   $ac2,       %[xr],        %[temp0]    \n\t"
        "mult   $ac1,       %[temp2],     %[temp0]    \n\t"
        "msub   $ac1,       %[xr],        %[temp1]    \n\t"
        "addu   %[temp2],   %[temp6],     %[temp3]    \n\t"
        "subu   %[xr],      %[temp4],     %[temp5]    \n\t"
        "mfhi   %[accu1],   $ac2                      \n\t"
        "mfhi   %[accu2],   $ac1                      \n\t"
        "addiu  %[ptr1],    %[ptr1],      16          \n\t"
        "addiu  %[ptr2],    %[ptr2],      -16         \n\t"
        "addiu  %[ptr3],    %[ptr3],      64          \n\t"
        "sra    %[temp2],   %[temp2],     1           \n\t"
        "sra    %[temp3],   %[xr],        1           \n\t"
        "lw     %[temp0],   128(%[pDat_1])            \n\t"
        "lw     %[temp5],   0(%[ptr2])                \n\t"
        "lw     %[temp6],   64(%[pDat_1])             \n\t"
        "lw     %[xr],      4(%[ptr2])                \n\t"
        "subu   %[temp1],   %[temp2],     %[accu1]    \n\t"
        "addu   %[temp2],   %[temp2],     %[accu1]    \n\t"
        "subu   %[temp4],   %[temp3],     %[accu2]    \n\t"
        "addu   %[temp3],   %[temp3],     %[accu2]    \n\t"
        "negu   %[temp3],   %[temp3]                  \n\t"
        "sw     %[temp1],   0(%[pTmp_0])              \n\t"
        "lw     %[temp1],   0(%[ptr1])                \n\t"
        "sw     %[temp2],   0(%[pTmp_1])              \n\t"
        "lw     %[temp2],   0(%[pDat_2])              \n\t"
        "sw     %[temp4],   4(%[pTmp_0])              \n\t"
        "lw     %[temp4],   64(%[pDat_2])             \n\t"
        "sw     %[temp3],   4(%[pTmp_1])              \n\t"
        "lw     %[temp3],   4(%[ptr1])                \n\t"
        "addiu  %[pTmp_0],  %[pTmp_0],    8           \n\t"
        "addiu  %[pTmp_1],  %[pTmp_1],    -8          \n\t"
        "bne    %[loop_end],%[pDat_2],    1b          \n\t"

        : [accu2] "=&r" (accu2), [accu1] "=&r" (accu1), [pTmp_0] "+r" (pTmp_0),
          [pTmp_1] "+r" (pTmp_1), [temp0] "=&r" (temp0), [temp1] "=&r" (temp1),
          [temp2] "=&r" (temp2), [temp3] "=&r" (temp3), [temp4] "=&r" (temp4),
          [temp5] "=&r" (temp5), [temp6] "=&r" (temp6), [xr] "=&r" (xr),
          [pDat_1] "+r" (pDat_1), [pDat_2] "+r" (pDat_2), [ptr1] "+r" (ptr1),
          [ptr2] "+r" (ptr2), [ptr3] "+r" (ptr3)
        : [loop_end] "r" (loop_end)
        : "memory", "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo"
    );

    twiddle_1 = sin_twiddle_L64[64/2].v.re;
    twiddle_2 = sin_twiddle_L64[64/4].v.re;
    twiddle_3 = sin_twiddle_L64[64/4].v.im;

    __asm__ volatile (
        "lw       %[temp0],   64(%[pDat])                 \n\t"
        "lw       %[temp2],   96(%[pDat])                 \n\t"
        "lw       %[temp3],   32(%[pDat])                 \n\t"
        "mult     $ac0,       %[temp0],    %[twiddle_1]   \n\t"
        "mult     $ac1,       %[temp2],    %[twiddle_2]   \n\t"
        "msub     $ac1,       %[temp3],    %[twiddle_3]   \n\t"
        "mult     $ac2,       %[temp2],    %[twiddle_3]   \n\t"
        "madd     $ac2,       %[temp3],    %[twiddle_2]   \n\t"
        "mfhi     %[temp4],   $ac0                        \n\t"
        "lw       %[temp1],   0(%[pDat])                  \n\t"
        "mfhi     %[accu2],   $ac1                        \n\t"
        "mfhi     %[accu1],   $ac2                        \n\t"
        "sra      %[temp1],   %[temp1],    1              \n\t"
        "addqh.w  %[temp2],   %[temp1],    %[temp4]       \n\t"
        "subqh.w  %[temp1],   %[temp1],    %[temp4]       \n\t"
        "sw       %[temp2],   0(%[tmp])                   \n\t"
        "sw       %[temp1],   4(%[tmp])                   \n\t"
        "sra      %[accu2],   %[accu2],    1              \n\t"
        "sra      %[accu1],   %[accu1],    1              \n\t"

        : [temp0] "=&r" (temp0), [temp1] "=&r" (temp1), [temp2] "=&r" (temp2),
          [temp3] "=&r" (temp3), [temp4] "=&r" (temp4), [tmp] "+r" (tmp),
          [accu1] "=&r" (accu1), [accu2] "=&r" (accu2)
        : [twiddle_1] "r" (twiddle_1), [twiddle_2] "r" (twiddle_2),
          [twiddle_3] "r" (twiddle_3), [pDat] "r" (pDat)
        : "memory", "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo"
    );
  } else {
    pDat_1 = &pDat[-1];
    pDat_2 = &pDat[1];
    ptr1 = (long*)&sin_twiddle_L64[1];
    ptr2 = (long*)&sin_twiddle_L64[31];
    ptr3 = (long*)&sin_twiddle_L64[4];
    loop_end = (long)&pDat[16];

    __asm__ volatile (
        "lw     %[temp0],   256(%[pDat_1])            \n\t"
        "lw     %[temp1],   0(%[ptr1])                \n\t"
        "lw     %[temp2],   0(%[pDat_2])              \n\t"
        "lw     %[temp3],   4(%[ptr1])                \n\t"
        "lw     %[temp4],   128(%[pDat_2])            \n\t"
        "lw     %[temp5],   0(%[ptr2])                \n\t"
        "lw     %[temp6],   128(%[pDat_1])            \n\t"
        "lw     %[xr],      4(%[ptr2])                \n\t"
    "1:                                               \n\t"
        "mult   $ac0,       %[temp0],     %[temp1]    \n\t"
        "mult   $ac1,       %[temp0],     %[temp3]    \n\t"
        "mult   $ac2,       %[temp4],     %[temp5]    \n\t"
        "mult   $ac3,       %[temp4],     %[xr]       \n\t"
        "msub   $ac0,       %[temp2],     %[temp3]    \n\t"
        "madd   $ac1,       %[temp2],     %[temp1]    \n\t"
        "msub   $ac2,       %[temp6],     %[xr]       \n\t"
        "madd   $ac3,       %[temp6],     %[temp5]    \n\t"
        "lw     %[temp0],   0(%[ptr3])                \n\t"
        "mfhi   %[accu2],   $ac0                      \n\t"
        "mfhi   %[accu1],   $ac1                      \n\t"
        "mfhi   %[temp5],   $ac2                      \n\t"
        "mfhi   %[temp6],   $ac3                      \n\t"
        "addiu  %[pDat_1],  %[pDat_1],    -4          \n\t"
        "addiu  %[pDat_2],  %[pDat_2],    4           \n\t"
        "lw     %[temp1],   4(%[ptr3])                \n\t"
        "sra    %[temp4],   %[accu2],     1           \n\t"
        "sra    %[temp3],   %[accu1],     1           \n\t"
        "sra    %[temp5],   %[temp5],     1           \n\t"
        "sra    %[temp6],   %[temp6],     1           \n\t"
        "subu   %[temp2],   %[temp6],     %[temp3]    \n\t"
        "addu   %[xr],      %[temp4],     %[temp5]    \n\t"
        "mult   $ac2,       %[temp2],     %[temp1]    \n\t"
        "madd   $ac2,       %[xr],        %[temp0]    \n\t"
        "mult   $ac1,       %[temp2],     %[temp0]    \n\t"
        "msub   $ac1,       %[xr],        %[temp1]    \n\t"
        "addu   %[temp2],   %[temp6],     %[temp3]    \n\t"
        "subu   %[xr],      %[temp4],     %[temp5]    \n\t"
        "mfhi   %[accu1],   $ac2                      \n\t"
        "mfhi   %[accu2],   $ac1                      \n\t"
        "addiu  %[ptr1],    %[ptr1],      8           \n\t"
        "addiu  %[ptr2],    %[ptr2],      -8          \n\t"
        "addiu  %[ptr3],    %[ptr3],      32          \n\t"
        "sra    %[temp2],   %[temp2],     1           \n\t"
        "sra    %[temp3],   %[xr],        1           \n\t"
        "lw     %[temp0],   256(%[pDat_1])            \n\t"
        "lw     %[temp5],   0(%[ptr2])                \n\t"
        "lw     %[temp6],   128(%[pDat_1])            \n\t"
        "lw     %[xr],      4(%[ptr2])                \n\t"
        "subu   %[temp1],   %[temp2],     %[accu1]    \n\t"
        "addu   %[temp2],   %[temp2],     %[accu1]    \n\t"
        "subu   %[temp4],   %[temp3],     %[accu2]    \n\t"
        "addu   %[temp3],   %[temp3],     %[accu2]    \n\t"
        "negu   %[temp3],   %[temp3]                  \n\t"
        "sw     %[temp1],   0(%[pTmp_0])              \n\t"
        "lw     %[temp1],   0(%[ptr1])                \n\t"
        "sw     %[temp2],   0(%[pTmp_1])              \n\t"
        "lw     %[temp2],   0(%[pDat_2])              \n\t"
        "sw     %[temp4],   4(%[pTmp_0])              \n\t"
        "lw     %[temp4],   128(%[pDat_2])            \n\t"
        "sw     %[temp3],   4(%[pTmp_1])              \n\t"
        "lw     %[temp3],   4(%[ptr1])                \n\t"
        "addiu  %[pTmp_0],  %[pTmp_0],    8           \n\t"
        "addiu  %[pTmp_1],  %[pTmp_1],    -8          \n\t"
        "bne    %[loop_end],%[pDat_2],    1b          \n\t"

        : [accu2] "=&r" (accu2), [accu1] "=&r" (accu1), [pTmp_0] "+r" (pTmp_0),
          [pTmp_1] "+r" (pTmp_1), [temp0] "=&r" (temp0), [temp1] "=&r" (temp1),
          [temp2] "=&r" (temp2), [temp3] "=&r" (temp3), [temp4] "=&r" (temp4),
          [temp5] "=&r" (temp5), [temp6] "=&r" (temp6), [xr] "=&r" (xr),
          [pDat_1] "+r" (pDat_1), [pDat_2] "+r" (pDat_2), [ptr1] "+r" (ptr1),
          [ptr2] "+r" (ptr2), [ptr3] "+r" (ptr3)
        : [loop_end] "r" (loop_end)
        : "memory", "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo"
    );

    twiddle_1 = sin_twiddle_L64[64/2].v.re;
    twiddle_2 = sin_twiddle_L64[64/4].v.re;
    twiddle_3 = sin_twiddle_L64[64/4].v.im;

    __asm__ volatile (
        "lw       %[temp0],   128(%[pDat])                \n\t"
        "lw       %[temp2],   192(%[pDat])                \n\t"
        "lw       %[temp3],   64(%[pDat])                 \n\t"
        "mult     $ac0,       %[temp0],    %[twiddle_1]   \n\t"
        "mult     $ac1,       %[temp2],    %[twiddle_2]   \n\t"
        "msub     $ac1,       %[temp3],    %[twiddle_3]   \n\t"
        "mult     $ac2,       %[temp2],    %[twiddle_3]   \n\t"
        "madd     $ac2,       %[temp3],    %[twiddle_2]   \n\t"
        "mfhi     %[temp6],   $ac0                        \n\t"
        "lw       %[temp1],   0(%[pDat])                  \n\t"
        "mfhi     %[accu2],   $ac1                        \n\t"
        "mfhi     %[accu1],   $ac2                        \n\t"
        "sra      %[temp1],   %[temp1],    1              \n\t"
        "addqh.w  %[temp2],   %[temp1],    %[temp6]       \n\t"
        "subqh.w  %[temp1],   %[temp1],    %[temp6]       \n\t"
        "sw       %[temp2],   0(%[tmp])                   \n\t"
        "sw       %[temp1],   4(%[tmp])                   \n\t"
        "sra      %[accu2],   %[accu2],    1              \n\t"
        "sra      %[accu1],   %[accu1],    1              \n\t"

        : [temp0] "=&r" (temp0), [temp1] "=&r" (temp1), [temp2] "=&r" (temp2),
          [temp3] "=&r" (temp3), [temp6] "=&r" (temp6), [tmp] "+r" (tmp),
          [accu1] "=&r" (accu1), [accu2] "=&r" (accu2)
        : [twiddle_1] "r" (twiddle_1), [twiddle_2] "r" (twiddle_2),
          [twiddle_3] "r" (twiddle_3), [pDat] "r" (pDat)
        : "memory", "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo"
    );
  }
  tmp[M]   = accu1;
  tmp[M+1] = accu2;

  /* dit_fft expects 1 bit scaled input values */
  fft(M, tmp, pDat_e);

  pTmp_1 = &tmp[L];
  for (i = M>>1; i--;)
  {
    FIXP_DBL tmp1, tmp2, tmp3, tmp4;
    tmp1 = *tmp++;
    tmp2 = *tmp++;
    tmp3 = *--pTmp_1;
    tmp4 = *--pTmp_1;
    *pDat++ = tmp1;
    *pDat++ = tmp3;
    *pDat++ = tmp2;
    *pDat++ = tmp4;
  }

  *pDat_e += 2;
}
#endif /* #if defined(FUNCTION_dct_III) */

#if defined (FUNCTION_dct_IV)
void dct_IV(FIXP_DBL *pDat,
            int L,
            int *pDat_e)
{
  int sin_step = 0;
  int M = L >> 1;

  const FIXP_WTP *twiddle;
  const FIXP_STP *sin_twiddle;

  FDK_ASSERT(L >= 4);

  getTables(&twiddle, &sin_twiddle, &sin_step, L);

  {
    FIXP_DBL *RESTRICT pDat_0 = &pDat[0];
    FIXP_DBL *RESTRICT pDat_1 = &pDat[L - 2];
    register int i;

    FIXP_DBL tmp_1_re = twiddle[0].v.re;
    FIXP_DBL tmp_1_im = twiddle[0].v.im;
    FIXP_DBL tmp_2_re = twiddle[1].v.re;
    FIXP_DBL tmp_2_im = twiddle[1].v.im;

    for (i = 0; i < M-1; i+=2,pDat_0+=2,pDat_1-=2)//Branja M%4=0,raspetljati kod
    {
      register FIXP_DBL accu1,accu2,accu3,accu4;
      register FIXP_DPK* temp_pok = (FIXP_DPK*)(&(twiddle[i+2]));

      __asm__ volatile(
        "lw    %[accu1],    4(%[pDat_1])            \n\t"
        "lw    %[accu4],    0(%[pDat_1])            \n\t"
        "lw    %[accu2],    0(%[pDat_0])            \n\t"
        "lw    %[accu3],    4(%[pDat_0])            \n\t"

        "mult  $ac0,        %[accu1],   %[tmp_1_re] \n\t"
        "mult  $ac1,        %[accu1],   %[tmp_1_im] \n\t"
        "mult  $ac2,        %[accu4],   %[tmp_2_re] \n\t"
        "mult  $ac3,        %[accu4],   %[tmp_2_im] \n\t"

        "msub  $ac0,        %[accu2],   %[tmp_1_im] \n\t"
        "madd  $ac1,        %[accu2],   %[tmp_1_re] \n\t"
        "msub  $ac2,        %[accu3],   %[tmp_2_im] \n\t"
        "madd  $ac3,        %[accu3],   %[tmp_2_re] \n\t"

        "lw    %[tmp_1_re], 0(%[temp_pok])          \n\t"

        "mfhi  %[accu1],    $ac0                    \n\t"
        "mfhi  %[accu2],    $ac1                    \n\t"
        "mfhi  %[accu3],    $ac2                    \n\t"
        "mfhi  %[accu4],    $ac3                    \n\t"

        "lw    %[tmp_1_im], 4(%[temp_pok])          \n\t"
        "lw    %[tmp_2_re], 8(%[temp_pok])          \n\t"
        "lw    %[tmp_2_im], 12(%[temp_pok])         \n\t"

        "sw    %[accu1],    4(%[pDat_0])            \n\t"
        "sw    %[accu2],    0(%[pDat_0])            \n\t"
        "subu  %[accu3],    $zero,       %[accu3]   \n\t"
        "sw    %[accu4],    0(%[pDat_1])            \n\t"
        "sw    %[accu3],    4(%[pDat_1])            \n\t"

         : [accu1] "=&r" (accu1), [accu2] "=&r" (accu2), [accu3] "=&r" (accu3),
           [accu4] "=&r" (accu4), [tmp_1_re] "+r" (tmp_1_re), [tmp_2_re] "+r" (tmp_2_re),
           [tmp_1_im] "+r" (tmp_1_im), [tmp_2_im] "+r" (tmp_2_im)
         : [temp_pok] "r" (temp_pok),
           [pDat_1] "r" (pDat_1), [pDat_0] "r" (pDat_0)
         : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo", "memory"
        );

    }
    if (M&1)
    {
      register FIXP_DBL accu1,accu2;
      register FIXP_DPK* temp_pok = (FIXP_DPK*)(&(twiddle[i]));
      register FIXP_DBL tmp_1_re = twiddle[i].v.re;
      register FIXP_DBL tmp_1_im = twiddle[i].v.im;


      __asm__ volatile(
        "lw     %[accu1],   4(%[pDat_1])                \n\t"
        "lw     %[tmp_1_re],0(%[temp_pok])              \n\t"
        "lw     %[tmp_1_im],4(%[temp_pok])              \n\t"
        "lw     %[accu2],   0(%[pDat_0])                \n\t"

        "mult   $ac0,       %[accu1],     %[tmp_1_re]   \n\t"
        "mult   $ac1,       %[accu1],     %[tmp_1_im]   \n\t"
        "msub   $ac0,       %[accu2],     %[tmp_1_im]   \n\t"
        "madd   $ac1,       %[accu2],     %[tmp_1_re]   \n\t"
        "mfhi   %[accu1],   $ac0                        \n\t"
        "mfhi   %[accu2],   $ac1                        \n\t"

        "sw     %[accu1],   4(%[pDat_0])                \n\t"
        "sw     %[accu2],   0(%[pDat_0])                \n\t"

         : [accu1] "=&r" (accu1), [accu2] "=&r" (accu2)
         : [tmp_1_re] "r" (tmp_1_re), [tmp_1_im] "r" (tmp_1_im),
           [temp_pok] "r" (temp_pok),
           [pDat_1] "r" (pDat_1), [pDat_0] "r" (pDat_0)
         : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo", "memory"
        );

    }
  }

  fft(M, pDat, pDat_e);

  {
    FIXP_DBL *RESTRICT pDat_0 = &pDat[0];
    FIXP_DBL *RESTRICT pDat_1 = &pDat[L - 2];
    register FIXP_DBL accu1,accu2,accu3,accu4;
    int idx, i;

    /* Sin and Cos values are 0.0f and 1.0f */
    accu1 = pDat_1[0];
    accu2 = pDat_1[1];

    pDat_1[1] = -(pDat_0[1]>>1);
    pDat_0[0] = (pDat_0[0]>>1);

    for (idx = sin_step,i=1; i<(M+1)>>1; i++, idx+=sin_step)
    {
      FIXP_STP twd = sin_twiddle[idx];

      register FIXP_DBL accu3,accu4;
      register FIXP_DBL tmp_1_re = twd.v.re;
      register FIXP_DBL tmp_1_im = twd.v.im;
      register FIXP_DBL tmp_1, tmp_2;


      __asm__ volatile(
        "lw      %[tmp_1],  12(%[pDat_0])             \n\t"
        "lw      %[tmp_2],  8(%[pDat_0])              \n\t"

        "mult    $ac0,      %[accu1],     %[tmp_1_re] \n\t"
        "mult    $ac1,      %[accu1],     %[tmp_1_im] \n\t"
        "mult    $ac2,      %[tmp_1],     %[tmp_1_re] \n\t"
        "mult    $ac3,      %[tmp_1],     %[tmp_1_im] \n\t"

        "msub    $ac0,      %[accu2],     %[tmp_1_im] \n\t"
        "madd    $ac1,      %[accu2],     %[tmp_1_re] \n\t"
        "msub    $ac2,      %[tmp_2],     %[tmp_1_im] \n\t"
        "madd    $ac3,      %[tmp_2],     %[tmp_1_re] \n\t"

        "addiu   %[pDat_0], %[pDat_0],    8           \n\t"
        "subu    %[pDat_1], %[pDat_1],    8           \n\t"

        "mfhi    %[accu3],  $ac0                      \n\t"
        "mfhi    %[accu4],  $ac1                      \n\t"
        "mfhi    %[tmp_1],  $ac2                      \n\t"
        "mfhi    %[tmp_2],  $ac3                      \n\t"

        "lw      %[accu1],  0(%[pDat_1])              \n\t"
        "lw      %[accu2],  4(%[pDat_1])              \n\t"

        "sw      %[accu3],  -4(%[pDat_0])             \n\t"
        "sw      %[accu4],  8(%[pDat_1])              \n\t"
        "subu    %[tmp_1],  $zero,        %[tmp_1]    \n\t"
        "sw      %[tmp_2],  0(%[pDat_0])              \n\t"
        "sw      %[tmp_1],  4(%[pDat_1])              \n\t"

         : [accu1] "+r" (accu1), [accu2] "+r" (accu2), [accu3] "=&r" (accu3),
           [accu4] "=&r" (accu4), [pDat_1] "+r" (pDat_1), [pDat_0] "+r" (pDat_0),
           [tmp_1] "=&r" (tmp_1), [tmp_2] "=&r" (tmp_2)
         : [tmp_1_re] "r" (tmp_1_re), [tmp_1_im] "r" (tmp_1_im)
         : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo", "memory"
        );
    }

    if ( (M&1) == 0 )
    {
      /* Last Sin and Cos value pair are the same */
      accu1 = fMultDiv2(accu1, WTC(0x5a82799a));
      accu2 = fMultDiv2(accu2, WTC(0x5a82799a));

      pDat_1[0] = accu1 + accu2;
      pDat_0[1] = accu1 - accu2;
    }
  }

  /* Add twiddeling scale. */
  *pDat_e += 2;
}
#endif /* #if defined (FUNCTION_dct_IV) */

#if defined (FUNCTION_dst_IV)
void dst_IV(FIXP_DBL *pDat,
            int L,
            int *pDat_e )
{
  int sin_step = 0;
  int M = L >> 1;

  const FIXP_WTP *twiddle;
  const FIXP_STP *sin_twiddle;

#ifdef DSTIV2_ENABLE
  if (L == 2) {
    const FIXP_STP tab = STCP(0x7641AF3D, 0x30FB9452);
    FIXP_DBL tmp1, tmp2;

    cplxMultDiv2(&tmp2, &tmp1, pDat[0], pDat[1], tab);

    pDat[0] = tmp1;
    pDat[1] = tmp2;

    *pDat_e += 1;

    return;
  }
#else
  FDK_ASSERT(L >= 4);
#endif

  getTables(&twiddle, &sin_twiddle, &sin_step, L);

  {
    FIXP_DBL *RESTRICT pDat_0 = &pDat[0];
    FIXP_DBL *RESTRICT pDat_1 = &pDat[L - 2];

    register int i;

    for (i = 0; i < M-1; i+=2,pDat_0+=2,pDat_1-=2)
    {
      FIXP_DBL temp_1,temp_2,temp_3,temp_4;
      FIXP_DBL temp_twidd_re_1 = twiddle[i].v.re;
      FIXP_DBL temp_twidd_im_1 = twiddle[i].v.im;
      FIXP_DBL temp_twidd_re_2 = twiddle[i+1].v.re;
      FIXP_DBL temp_twidd_im_2 = twiddle[i+1].v.im;

      __asm__ volatile(
        "lw     %[temp_1], 4(%[pDat_1])                    \n\t"
        "lw     %[temp_4], 0(%[pDat_1])                    \n\t"
        "lw     %[temp_2], 0(%[pDat_0])                    \n\t"
        "lw     %[temp_3], 4(%[pDat_0])                    \n\t"
        "subu   %[temp_4], $zero,       %[temp_4]          \n\t"

        "mult   $ac0,      %[temp_1],   %[temp_twidd_re_1] \n\t"
        "mult   $ac1,      %[temp_1],   %[temp_twidd_im_1] \n\t"
        "mult   $ac2,      %[temp_4],   %[temp_twidd_re_2] \n\t"
        "mult   $ac3,      %[temp_4],   %[temp_twidd_im_2] \n\t"

        "madd   $ac0,      %[temp_2],   %[temp_twidd_im_1] \n\t"
        "msub   $ac1,      %[temp_2],   %[temp_twidd_re_1] \n\t"
        "msub   $ac2,      %[temp_3],   %[temp_twidd_im_2] \n\t"
        "madd   $ac3,      %[temp_3],   %[temp_twidd_re_2] \n\t"

        "mfhi   %[temp_1], $ac0                            \n\t"
        "mfhi   %[temp_2], $ac1                            \n\t"
        "mfhi   %[temp_3], $ac2                            \n\t"
        "mfhi   %[temp_4], $ac3                            \n\t"

        "sw     %[temp_1], 4(%[pDat_0])                   \n\t"
        "sw     %[temp_2], 0(%[pDat_0])                   \n\t"
        "subu   %[temp_3], $zero,       %[temp_3]         \n\t"
        "sw     %[temp_4], 0(%[pDat_1])                   \n\t"
        "sw     %[temp_3], 4(%[pDat_1])                   \n\t"

        : [temp_1] "=&r" (temp_1), [temp_2] "=&r" (temp_2),
          [temp_3] "=&r" (temp_3), [temp_4] "=&r" (temp_4)
        : [pDat_0] "r" (pDat_0), [pDat_1] "r" (pDat_1),
          [temp_twidd_re_1] "r" (temp_twidd_re_1), [temp_twidd_im_1] "r" (temp_twidd_im_1),
          [temp_twidd_re_2] "r" (temp_twidd_re_2), [temp_twidd_im_2] "r" (temp_twidd_im_2)
        : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo", "memory"
      );
    }
    if (M&1)
    {
      register FIXP_DBL temp_1,temp_2;
      FIXP_DBL temp_twidd_re_1 = twiddle[i].v.re;
      FIXP_DBL temp_twidd_im_1 = twiddle[i].v.im;
      __asm__ volatile(
        "lw     %[temp_1], 4(%[pDat_1])                    \n\t"
        "lw     %[temp_2], 0(%[pDat_0])                    \n\t"

        "mult   $ac0,      %[temp_1],   %[temp_twidd_re_1] \n\t"
        "mult   $ac1,      %[temp_1],   %[temp_twidd_im_1] \n\t"

        "madd   $ac0,      %[temp_2],   %[temp_twidd_im_1] \n\t"
        "msub   $ac1,      %[temp_2],   %[temp_twidd_re_1] \n\t"

        "mfhi   %[temp_1], $ac0                            \n\t"
        "mfhi   %[temp_2], $ac1                            \n\t"

        "sw     %[temp_1], 4(%[pDat_0])                    \n\t"
        "sw     %[temp_2], 0(%[pDat_0])                    \n\t"

        : [temp_1] "=&r" (temp_1), [temp_2] "=&r" (temp_2)
        : [pDat_0] "r" (pDat_0), [pDat_1] "r" (pDat_1),
          [temp_twidd_re_1] "r" (temp_twidd_re_1), [temp_twidd_im_1] "r" (temp_twidd_im_1)
        : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo", "memory"
      );
    }
  }

  fft(M, pDat, pDat_e);

  {
    FIXP_DBL *RESTRICT pDat_0;
    FIXP_DBL *RESTRICT pDat_1;
    register FIXP_DBL accu1,accu2,accu3,accu4;
    int idx, i;

    pDat_0 = &pDat[0];
    pDat_1 = &pDat[L - 2];

    /* Sin and Cos values are 0.0f and 1.0f */
    accu1 = pDat_1[0];
    accu2 = pDat_1[1];

    pDat_1[1] = -(pDat_0[0]>>1);
    pDat_0[0] = (pDat_0[1]>>1);

    for (idx = sin_step,i=1; i<(M+1)>>1; i++, idx+=sin_step)
    {
      FIXP_DBL temp_1,temp_2,temp_3,temp_4;
      FIXP_DBL temp_twidd_re_1 = sin_twiddle[idx].v.re;
      FIXP_DBL temp_twidd_im_1 = sin_twiddle[idx].v.im;

      __asm__ volatile(
        "lw     %[temp_4], 12(%[pDat_0])                    \n\t"
        "lw     %[temp_3], 8(%[pDat_0])                     \n\t"

        "mult   $ac0,      %[accu1],     %[temp_twidd_re_1] \n\t"
        "mult   $ac1,      %[accu1],     %[temp_twidd_im_1] \n\t"
        "mult   $ac2,      %[temp_4],    %[temp_twidd_re_1] \n\t"
        "mult   $ac3,      %[temp_4],    %[temp_twidd_im_1] \n\t"

        "msub   $ac0,      %[accu2],     %[temp_twidd_im_1] \n\t"
        "madd   $ac1,      %[accu2],     %[temp_twidd_re_1] \n\t"
        "msub   $ac2,      %[temp_3],    %[temp_twidd_im_1] \n\t"
        "madd   $ac3,      %[temp_3],    %[temp_twidd_re_1] \n\t"

        "lw     %[accu1],  -8(%[pDat_1])                    \n\t"

        "mfhi   %[temp_1], $ac0                             \n\t"
        "mfhi   %[temp_2], $ac1                             \n\t"
        "mfhi   %[temp_3], $ac2                             \n\t"
        "mfhi   %[temp_4], $ac3                             \n\t"

        "lw     %[accu2],  -4(%[pDat_1])                    \n\t"
        "addiu  %[pDat_0], %[pDat_0],    8                  \n\t"
        "addiu   %[pDat_1],%[pDat_1],    -8                 \n\t"

        "subu   %[temp_1], $zero,        %[temp_1]          \n\t"
        "subu   %[temp_2], $zero,        %[temp_2]          \n\t"
        "sw     %[temp_3], 0(%[pDat_0])                     \n\t"
        "subu   %[temp_4], $zero,        %[temp_4]          \n\t"

        "sw     %[temp_1], 8(%[pDat_1])                     \n\t"
        "sw     %[temp_2], -4(%[pDat_0])                    \n\t"
        "sw     %[temp_4], 4(%[pDat_1])                     \n\t"

        : [accu1] "+r" (accu1), [accu2] "+r" (accu2),
          [temp_1] "=&r" (temp_1), [temp_2] "=&r" (temp_2),
          [temp_3] "=&r" (temp_3), [temp_4] "=&r" (temp_4),
          [pDat_0] "+r" (pDat_0), [pDat_1] "+r" (pDat_1)
        : [temp_twidd_re_1] "r" (temp_twidd_re_1), [temp_twidd_im_1] "r" (temp_twidd_im_1)
        : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo", "memory"
      );
    }

    if ( (M&1) == 0 )
    {
      /* Last Sin and Cos value pair are the same */
      accu1 = fMultDiv2(accu1, WTC(0x5a82799a));
      accu2 = fMultDiv2(accu2, WTC(0x5a82799a));

      pDat_0[1] = - accu1 - accu2;
      pDat_1[0] =   accu2 - accu1;
    }
  }

  /* Add twiddeling scale. */
  *pDat_e += 2;
}
#endif /* #if defined (FUNCTION_dst_IV) */
