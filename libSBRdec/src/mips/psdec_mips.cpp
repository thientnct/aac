
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
  \file
  \brief  parametric stereo decoder
*/

#if defined(MIPS_DSP_R2_LE)
#define FUNCTION_deCorrelateSlotBased
#define FUNCTION_applySlotBasedRotation
#endif /* #if defined(MIPS_DSP_R2_LE) */


/* slot based funcion prototypes */
#if defined (FUNCTION_deCorrelateSlotBased)
static void deCorrelateSlotBased( HANDLE_PS_DEC h_ps_d,

                                  FIXP_DBL    *mHybridRealLeft,
                                  FIXP_DBL    *mHybridImagLeft,
                                  SCHAR        sf_mHybridLeft,

                                  FIXP_DBL    *rIntBufferLeft,
                                  FIXP_DBL    *iIntBufferLeft,
                                  SCHAR        sf_IntBuffer,

                                  FIXP_DBL    *mHybridRealRight,
                                  FIXP_DBL    *mHybridImagRight,

                                  FIXP_DBL    *rIntBufferRight,
                                  FIXP_DBL    *iIntBufferRight );
#endif

#if defined (FUNCTION_applySlotBasedRotation)
static void applySlotBasedRotation( HANDLE_PS_DEC h_ps_d,

                                    FIXP_DBL  *mHybridRealLeft,
                                    FIXP_DBL  *mHybridImagLeft,

                                    FIXP_DBL  *QmfLeftReal,
                                    FIXP_DBL  *QmfLeftImag,

                                    FIXP_DBL  *mHybridRealRight,
                                    FIXP_DBL  *mHybridImagRight,

                                    FIXP_DBL  *QmfRightReal,
                                    FIXP_DBL  *QmfRightImag
                                  );
#endif


#if defined FUNCTION_deCorrelateSlotBased
/***************************************************************************/
/*!
  \brief  Generate decorrelated side channel using allpass/delay

  \return

****************************************************************************/
static void
deCorrelateSlotBased( HANDLE_PS_DEC h_ps_d,            /*!< pointer to the module state */

                      FIXP_DBL    *mHybridRealLeft,    /*!< left (mono) hybrid values real */
                      FIXP_DBL    *mHybridImagLeft,    /*!< left (mono) hybrid values imag */
                      SCHAR        sf_mHybridLeft,     /*!< scalefactor for left (mono) hybrid bands */

                      FIXP_DBL    *rIntBufferLeft,     /*!< real qmf bands left (mono) (38x64) */
                      FIXP_DBL    *iIntBufferLeft,     /*!< real qmf bands left (mono) (38x64) */
                      SCHAR        sf_IntBuffer,       /*!< scalefactor for all left and right qmf bands   */

                      FIXP_DBL    *mHybridRealRight,   /*!< right (decorrelated) hybrid values real */
                      FIXP_DBL    *mHybridImagRight,   /*!< right (decorrelated) hybrid values imag */

                      FIXP_DBL    *rIntBufferRight,    /*!< real qmf bands right (decorrelated) (38x64) */
                      FIXP_DBL    *iIntBufferRight )   /*!< real qmf bands right (decorrelated) (38x64) */
{

  INT  i, m, sb, gr, bin;

  FIXP_DBL peakDiff, nrg, transRatio;

  FIXP_DBL *RESTRICT aaLeftReal;
  FIXP_DBL *RESTRICT aaLeftImag;

  FIXP_DBL *RESTRICT aaRightReal;
  FIXP_DBL *RESTRICT aaRightImag;

  FIXP_DBL *RESTRICT pRealDelayBuffer;
  FIXP_DBL *RESTRICT pImagDelayBuffer;

  C_ALLOC_SCRATCH_START(aaPowerSlot, FIXP_DBL, NO_MID_RES_BINS);
  C_ALLOC_SCRATCH_START(aaTransRatioSlot, FIXP_DBL, NO_MID_RES_BINS);

/*!
<pre>
   parameter index       qmf bands             hybrid bands
  ----------------------------------------------------------------------------
         0                   0                      0,7
         1                   0                      1,6
         2                   0                      2
         3                   0                      3           HYBRID BANDS
         4                   1                      9
         5                   1                      8
         6                   2                     10
         7                   2                     11
  ----------------------------------------------------------------------------
         8                   3
         9                   4
        10                   5
        11                   6
        12                   7
        13                   8
        14                   9,10      (2 )                      QMF BANDS
        15                   11 - 13   (3 )
        16                   14 - 17   (4 )
        17                   18 - 22   (5 )
        18                   23 - 34   (12)
        19                   35 - 63   (29)
  ----------------------------------------------------------------------------
</pre>
*/

  #define FLTR_SCALE 3

  /* hybrid bands (parameter index 0 - 7) */
  aaLeftReal  = mHybridRealLeft;
  aaLeftImag  = mHybridImagLeft;

  int temp0, temp1, temp2, temp3, temp4, tmp0, tmp1, tmp2, tmp3, tmp4;

  __asm__ volatile (
      "lw        %[temp0],  0(%[mHybridRealLeft])             \n\t"
      "lw        %[temp1],  0(%[mHybridImagLeft])             \n\t"
      "lw        %[temp2],  28(%[mHybridRealLeft])            \n\t"
      "lw        %[temp3],  28(%[mHybridImagLeft])            \n\t"
      "lw        %[tmp0],   4(%[mHybridRealLeft])             \n\t"
      "lw        %[tmp1],   4(%[mHybridImagLeft])             \n\t"
      "lw        %[tmp2],   24(%[mHybridRealLeft])            \n\t"
      "lw        %[tmp3],   24(%[mHybridImagLeft])            \n\t"
      "mulq_s.w  %[temp0],  %[temp0],              %[temp0]   \n\t"
      "mulq_s.w  %[temp1],  %[temp1],              %[temp1]   \n\t"
      "mulq_s.w  %[temp2],  %[temp2],              %[temp2]   \n\t"
      "mulq_s.w  %[temp3],  %[temp3],              %[temp3]   \n\t"
      "mulq_s.w  %[tmp0],   %[tmp0],               %[tmp0]    \n\t"
      "mulq_s.w  %[tmp1],   %[tmp1],               %[tmp1]    \n\t"
      "mulq_s.w  %[tmp2],   %[tmp2],               %[tmp2]    \n\t"
      "mulq_s.w  %[tmp3],   %[tmp3],               %[tmp3]    \n\t"
      "srl       %[temp0],  %[temp0],              1          \n\t"
      "srl       %[temp1],  %[temp1],              1          \n\t"
      "srl       %[temp2],  %[temp2],              1          \n\t"
      "srl       %[temp3],  %[temp3],              1          \n\t"
      "srl       %[tmp0],   %[tmp0],               1          \n\t"
      "srl       %[tmp1],   %[tmp1],               1          \n\t"
      "srl       %[tmp2],   %[tmp2],               1          \n\t"
      "srl       %[tmp3],   %[tmp3],               1          \n\t"
      "addu      %[temp0],  %[temp0],              %[temp1]   \n\t"
      "addu      %[temp2],  %[temp2],              %[temp3]   \n\t"
      "addu      %[tmp0],   %[tmp0],               %[tmp1]    \n\t"
      "addu      %[tmp2],   %[tmp2],               %[tmp3]    \n\t"
      "lw        %[temp1],  8(%[mHybridImagLeft])             \n\t"
      "lw        %[temp3],  12(%[mHybridImagLeft])            \n\t"
      "lw        %[tmp1],   36(%[mHybridImagLeft])            \n\t"
      "lw        %[tmp3],   32(%[mHybridImagLeft])            \n\t"
      "srl       %[temp0],  %[temp0],              3          \n\t"
      "srl       %[temp2],  %[temp2],              3          \n\t"
      "srl       %[tmp0],   %[tmp0],               3          \n\t"
      "srl       %[tmp2],   %[tmp2],               3          \n\t"
      "addu      %[temp0],  %[temp0],              %[temp2]   \n\t"
      "addu      %[tmp0],   %[tmp0],               %[tmp2]    \n\t"
      "sw        %[temp0],  0(%[aaPowerSlot])                 \n\t"
      "sw        %[tmp0],   4(%[aaPowerSlot])                 \n\t"
      "mulq_s.w  %[temp1],  %[temp1],              %[temp1]   \n\t"
      "mulq_s.w  %[temp3],  %[temp3],              %[temp3]   \n\t"
      "mulq_s.w  %[tmp1],   %[tmp1],               %[tmp1]    \n\t"
      "mulq_s.w  %[tmp3],   %[tmp3],               %[tmp3]    \n\t"
      "lw        %[temp0],  8(%[mHybridRealLeft])             \n\t"
      "lw        %[temp2],  12(%[mHybridRealLeft])            \n\t"
      "lw        %[tmp0],   36(%[mHybridRealLeft])            \n\t"
      "lw        %[tmp2],   32(%[mHybridRealLeft])            \n\t"
      "mulq_s.w  %[temp0],  %[temp0],              %[temp0]   \n\t"
      "mulq_s.w  %[temp2],  %[temp2],              %[temp2]   \n\t"
      "mulq_s.w  %[tmp0],   %[tmp0],               %[tmp0]    \n\t"
      "mulq_s.w  %[tmp2],   %[tmp2],               %[tmp2]    \n\t"
      "srl       %[temp1],  %[temp1],              1          \n\t"
      "srl       %[temp3],  %[temp3],              1          \n\t"
      "srl       %[tmp1],   %[tmp1],               1          \n\t"
      "srl       %[tmp3],   %[tmp3],               1          \n\t"
      "srl       %[temp0],  %[temp0],              1          \n\t"
      "srl       %[temp2],  %[temp2],              1          \n\t"
      "srl       %[tmp0],   %[tmp0],               1          \n\t"
      "srl       %[tmp2],   %[tmp2],               1          \n\t"
      "addu      %[temp0],  %[temp0],              %[temp1]   \n\t"
      "addu      %[temp2],  %[temp2],              %[temp3]   \n\t"
      "lw        %[temp1],  40(%[mHybridImagLeft])            \n\t"
      "lw        %[temp3],  44(%[mHybridImagLeft])            \n\t"
      "addu      %[tmp0],   %[tmp0],               %[tmp1]    \n\t"
      "addu      %[tmp2],   %[tmp2],               %[tmp3]    \n\t"
      "lw        %[tmp1],   40(%[mHybridRealLeft])            \n\t"
      "lw        %[tmp3],   44(%[mHybridRealLeft])            \n\t"
      "mulq_s.w  %[tmp1],   %[tmp1],               %[tmp1]    \n\t"
      "mulq_s.w  %[temp1],  %[temp1],              %[temp1]   \n\t"
      "mulq_s.w  %[tmp3],   %[tmp3],               %[tmp3]    \n\t"
      "mulq_s.w  %[temp3],  %[temp3],              %[temp3]   \n\t"
      "srl       %[temp0],  %[temp0],              3          \n\t"
      "srl       %[temp2],  %[temp2],              3          \n\t"
      "srl       %[tmp0],   %[tmp0],               3          \n\t"
      "srl       %[tmp2],   %[tmp2],               3          \n\t"
      "sw        %[temp0],  8(%[aaPowerSlot])                 \n\t"
      "sw        %[temp2],  12(%[aaPowerSlot])                \n\t"
      "sw        %[tmp0],   16(%[aaPowerSlot])                \n\t"
      "sw        %[tmp2],   20(%[aaPowerSlot])                \n\t"
      "srl       %[tmp1],   %[tmp1],               1          \n\t"
      "srl       %[temp1],  %[temp1],              1          \n\t"
      "srl       %[tmp3],   %[tmp3],               1          \n\t"
      "srl       %[temp3],  %[temp3],              1          \n\t"
      "addu      %[tmp1],   %[tmp1],               %[temp1]   \n\t"
      "addu      %[tmp3],   %[tmp3],               %[temp3]   \n\t"
      "srl       %[tmp1],   %[tmp1],               3          \n\t"
      "srl       %[tmp3],   %[tmp3],               3          \n\t"
      "sw        %[tmp1],   24(%[aaPowerSlot])                \n\t"
      "sw        %[tmp3],   28(%[aaPowerSlot])                \n\t"

      : [temp0] "=&r" (temp0), [temp1] "=&r" (temp1),
        [temp2] "=&r" (temp2), [temp3] "=&r" (temp3),
        [tmp0] "=&r" (tmp0), [tmp1] "=&r" (tmp1),
        [tmp2] "=&r" (tmp2), [tmp3] "=&r" (tmp3)
      : [mHybridRealLeft] "r" (mHybridRealLeft),
        [mHybridImagLeft] "r" (mHybridImagLeft),
        [aaPowerSlot] "r" (aaPowerSlot)
      : "memory", "hi", "lo"
  );

  {
      FIXP_DBL slotNrg;
      int loop_end1, loop_end2;

      __asm__ volatile (
          "lw         %[temp0],   12(%[rIntBufferLeft])            \n\t"
          "lw         %[temp1],   12(%[iIntBufferLeft])            \n\t"
          "lw         %[temp2],   16(%[rIntBufferLeft])            \n\t"
          "lw         %[temp3],   16(%[iIntBufferLeft])            \n\t"
          "lw         %[tmp0],    20(%[rIntBufferLeft])            \n\t"
          "lw         %[tmp1],    20(%[iIntBufferLeft])            \n\t"
          "lw         %[tmp2],    24(%[rIntBufferLeft])            \n\t"
          "lw         %[tmp3],    24(%[iIntBufferLeft])            \n\t"
          "mulq_s.w   %[temp0],   %[temp0],             %[temp0]   \n\t"
          "mulq_s.w   %[temp1],   %[temp1],             %[temp1]   \n\t"
          "mulq_s.w   %[temp2],   %[temp2],             %[temp2]   \n\t"
          "mulq_s.w   %[temp3],   %[temp3],             %[temp3]   \n\t"
          "mulq_s.w   %[tmp0],    %[tmp0],              %[tmp0]    \n\t"
          "mulq_s.w   %[tmp1],    %[tmp1],              %[tmp1]    \n\t"
          "mulq_s.w   %[tmp2],    %[tmp2],              %[tmp2]    \n\t"
          "mulq_s.w   %[tmp3],    %[tmp3],              %[tmp3]    \n\t"
          "srl        %[temp0],   %[temp0],             1          \n\t"
          "srl        %[temp1],   %[temp1],             1          \n\t"
          "srl        %[temp2],   %[temp2],             1          \n\t"
          "srl        %[temp3],   %[temp3],             1          \n\t"
          "srl        %[tmp0],    %[tmp0],              1          \n\t"
          "srl        %[tmp1],    %[tmp1],              1          \n\t"
          "srl        %[tmp2],    %[tmp2],              1          \n\t"
          "srl        %[tmp3],    %[tmp3],              1          \n\t"
          "addu       %[temp0],   %[temp0],             %[temp1]   \n\t"
          "addu       %[temp2],   %[temp2],             %[temp3]   \n\t"
          "addu       %[tmp0],    %[tmp0],              %[tmp1]    \n\t"
          "addu       %[tmp2],    %[tmp2],              %[tmp3]    \n\t"
          "lw         %[temp1],   28(%[iIntBufferLeft])            \n\t"
          "lw         %[temp3],   32(%[iIntBufferLeft])            \n\t"
          "lw         %[tmp1],    36(%[iIntBufferLeft])            \n\t"
          "lw         %[tmp3],    40(%[iIntBufferLeft])            \n\t"
          "srl        %[temp0],   %[temp0],             3          \n\t"
          "srl        %[temp2],   %[temp2],             3          \n\t"
          "srl        %[tmp0],    %[tmp0],              3          \n\t"
          "srl        %[tmp2],    %[tmp2],              3          \n\t"
          "sw         %[temp0],   32(%[aaPowerSlot])               \n\t"
          "sw         %[temp2],   36(%[aaPowerSlot])               \n\t"
          "sw         %[tmp0],    40(%[aaPowerSlot])               \n\t"
          "sw         %[tmp2],    44(%[aaPowerSlot])               \n\t"
          "mulq_s.w   %[temp1],   %[temp1],             %[temp1]   \n\t"
          "mulq_s.w   %[temp3],   %[temp3],             %[temp3]   \n\t"
          "mulq_s.w   %[tmp1],    %[tmp1],              %[tmp1]    \n\t"
          "mulq_s.w   %[tmp3],    %[tmp3],              %[tmp3]    \n\t"
          "lw         %[temp0],   28(%[rIntBufferLeft])            \n\t"
          "lw         %[temp2],   32(%[rIntBufferLeft])            \n\t"
          "lw         %[tmp0],    36(%[rIntBufferLeft])            \n\t"
          "lw         %[tmp2],    40(%[rIntBufferLeft])            \n\t"
          "mulq_s.w   %[temp0],   %[temp0],             %[temp0]   \n\t"
          "mulq_s.w   %[temp2],   %[temp2],             %[temp2]   \n\t"
          "mulq_s.w   %[tmp0],    %[tmp0],              %[tmp0]    \n\t"
          "mulq_s.w   %[tmp2],    %[tmp2],              %[tmp2]    \n\t"
          "srl        %[temp1],   %[temp1],             1          \n\t"
          "srl        %[temp3],   %[temp3],             1          \n\t"
          "srl        %[tmp1],    %[tmp1],              1          \n\t"
          "srl        %[tmp3],    %[tmp3],              1          \n\t"
          "srl        %[temp0],   %[temp0],             1          \n\t"
          "srl        %[temp2],   %[temp2],             1          \n\t"
          "srl        %[tmp0],    %[tmp0],              1          \n\t"
          "srl        %[tmp2],    %[tmp2],              1          \n\t"
          "addu       %[temp0],   %[temp0],             %[temp1]   \n\t"
          "addu       %[temp2],   %[temp2],             %[temp3]   \n\t"
          "addu       %[tmp0],    %[tmp0],              %[tmp1]    \n\t"
          "addu       %[tmp2],    %[tmp2],              %[tmp3]    \n\t"
          "lw         %[temp1],   44(%[iIntBufferLeft])            \n\t"
          "lw         %[temp3],   48(%[iIntBufferLeft])            \n\t"
          "lw         %[tmp1],    52(%[iIntBufferLeft])            \n\t"
          "srl        %[temp0],   %[temp0],             3          \n\t"
          "srl        %[temp2],   %[temp2],             3          \n\t"
          "srl        %[tmp0],    %[tmp0],              3          \n\t"
          "srl        %[tmp2],    %[tmp2],              3          \n\t"
          "addu       %[tmp0],    %[tmp2],              %[tmp0]    \n\t"
          "sw         %[temp0],   48(%[aaPowerSlot])               \n\t"
          "sw         %[temp2],   52(%[aaPowerSlot])               \n\t"
          "sw         %[tmp0],    56(%[aaPowerSlot])               \n\t"
          "mulq_s.w   %[temp1],   %[temp1],             %[temp1]   \n\t"
          "mulq_s.w   %[temp3],   %[temp3],             %[temp3]   \n\t"
          "mulq_s.w   %[tmp1],    %[tmp1],              %[tmp1]    \n\t"
          "lw         %[temp0],   44(%[rIntBufferLeft])            \n\t"
          "lw         %[temp2],   48(%[rIntBufferLeft])            \n\t"
          "lw         %[tmp0],    52(%[rIntBufferLeft])            \n\t"
          "mulq_s.w   %[temp0],   %[temp0],             %[temp0]   \n\t"
          "mulq_s.w   %[temp2],   %[temp2],             %[temp2]   \n\t"
          "mulq_s.w   %[tmp0],    %[tmp0],              %[tmp0]    \n\t"
          "srl        %[temp1],   %[temp1],             1          \n\t"
          "srl        %[temp3],   %[temp3],             1          \n\t"
          "srl        %[tmp1],    %[tmp1],              1          \n\t"
          "lw         %[tmp3],    68(%[iIntBufferLeft])            \n\t"
          "srl        %[temp0],   %[temp0],             1          \n\t"
          "srl        %[temp2],   %[temp2],             1          \n\t"
          "srl        %[tmp0],    %[tmp0],              1          \n\t"
          "addu       %[temp0],   %[temp0],             %[temp1]   \n\t"
          "addu       %[temp2],   %[temp2],             %[temp3]   \n\t"
          "addu       %[tmp0],    %[tmp0],              %[tmp1]    \n\t"
          "lw         %[temp1],   56(%[iIntBufferLeft])            \n\t"
          "lw         %[temp3],   60(%[iIntBufferLeft])            \n\t"
          "lw         %[tmp1],    64(%[iIntBufferLeft])            \n\t"
          "srl        %[temp0],   %[temp0],             3          \n\t"
          "srl        %[temp2],   %[temp2],             3          \n\t"
          "srl        %[tmp0],    %[tmp0],              3          \n\t"
          "addu       %[temp0],   %[temp0],             %[temp2]   \n\t"
          "addu       %[temp0],   %[temp0],             %[tmp0]    \n\t"
          "sw         %[temp0],   60(%[aaPowerSlot])               \n\t"
          "mulq_s.w   %[temp1],   %[temp1],             %[temp1]   \n\t"
          "mulq_s.w   %[temp3],   %[temp3],             %[temp3]   \n\t"
          "mulq_s.w   %[tmp1],    %[tmp1],              %[tmp1]    \n\t"
          "mulq_s.w   %[tmp3],    %[tmp3],              %[tmp3]    \n\t"
          "lw         %[temp0],   56(%[rIntBufferLeft])            \n\t"
          "lw         %[temp2],   60(%[rIntBufferLeft])            \n\t"
          "lw         %[tmp0],    64(%[rIntBufferLeft])            \n\t"
          "lw         %[tmp2],    68(%[rIntBufferLeft])            \n\t"
          "mulq_s.w   %[temp0],   %[temp0],             %[temp0]   \n\t"
          "mulq_s.w   %[temp2],   %[temp2],             %[temp2]   \n\t"
          "mulq_s.w   %[tmp0],    %[tmp0],              %[tmp0]    \n\t"
          "mulq_s.w   %[tmp2],    %[tmp2],              %[tmp2]    \n\t"
          "srl        %[temp1],   %[temp1],             1          \n\t"
          "srl        %[temp3],   %[temp3],             1          \n\t"
          "srl        %[tmp1],    %[tmp1],              1          \n\t"
          "srl        %[tmp3],    %[tmp3],              1          \n\t"
          "srl        %[temp0],   %[temp0],             1          \n\t"
          "srl        %[temp2],   %[temp2],             1          \n\t"
          "srl        %[tmp0],    %[tmp0],              1          \n\t"
          "srl        %[tmp2],    %[tmp2],              1          \n\t"
          "addu       %[temp0],   %[temp0],             %[temp1]   \n\t"
          "addu       %[temp2],   %[temp2],             %[temp3]   \n\t"
          "addu       %[tmp0],    %[tmp0],              %[tmp1]    \n\t"
          "addu       %[tmp2],    %[tmp2],              %[tmp3]    \n\t"
          "srl        %[temp0],   %[temp0],             3          \n\t"
          "srl        %[temp2],   %[temp2],             3          \n\t"
          "srl        %[tmp0],    %[tmp0],              3          \n\t"
          "srl        %[tmp2],    %[tmp2],              3          \n\t"
          "addu       %[temp0],   %[temp0],             %[temp2]   \n\t"
          "addu       %[temp0],   %[temp0],             %[tmp0]    \n\t"
          "addu       %[temp0],   %[temp0],             %[tmp2]    \n\t"
          "sw         %[temp0],   64(%[aaPowerSlot])               \n\t"

          : [temp0] "=&r" (temp0), [temp1] "=&r" (temp1),
            [temp2] "=&r" (temp2), [temp3] "=&r" (temp3),
            [tmp0] "=&r" (tmp0), [tmp1] "=&r" (tmp1),
            [tmp2] "=&r" (tmp2), [tmp3] "=&r" (tmp3)
          : [rIntBufferLeft] "r" (rIntBufferLeft),
            [iIntBufferLeft] "r" (iIntBufferLeft),
            [aaPowerSlot] "r" (aaPowerSlot)
          : "memory", "hi", "lo"
      );
      loop_end1 = (int)&rIntBufferLeft[12];
      loop_end2 = (int)&rIntBufferLeft[40];

      __asm__ volatile (
          "lw         %[temp0],          72(%[rIntBufferLeft])           \n\t"
          "lw         %[temp1],          72(%[iIntBufferLeft])           \n\t"
          "lw         %[temp2],          76(%[rIntBufferLeft])           \n\t"
          "lw         %[temp3],          76(%[iIntBufferLeft])           \n\t"
          "lw         %[tmp0],           80(%[rIntBufferLeft])           \n\t"
          "lw         %[tmp1],           80(%[iIntBufferLeft])           \n\t"
          "lw         %[tmp2],           84(%[rIntBufferLeft])           \n\t"
          "lw         %[tmp3],           84(%[iIntBufferLeft])           \n\t"
          "lw         %[temp4],          88(%[rIntBufferLeft])           \n\t"
          "lw         %[tmp4],           88(%[iIntBufferLeft])           \n\t"
          "mulq_s.w   %[temp0],          %[temp0],             %[temp0]  \n\t"
          "mulq_s.w   %[temp1],          %[temp1],             %[temp1]  \n\t"
          "mulq_s.w   %[temp2],          %[temp2],             %[temp2]  \n\t"
          "mulq_s.w   %[temp3],          %[temp3],             %[temp3]  \n\t"
          "mulq_s.w   %[tmp0],           %[tmp0],              %[tmp0]   \n\t"
          "mulq_s.w   %[tmp1],           %[tmp1],              %[tmp1]   \n\t"
          "mulq_s.w   %[tmp2],           %[tmp2],              %[tmp2]   \n\t"
          "mulq_s.w   %[tmp3],           %[tmp3],              %[tmp3]   \n\t"
          "mulq_s.w   %[temp4],          %[temp4],             %[temp4]  \n\t"
          "mulq_s.w   %[tmp4],           %[tmp4],              %[tmp4]   \n\t"
          "srl        %[temp0],          %[temp0],             1         \n\t"
          "srl        %[temp1],          %[temp1],             1         \n\t"
          "srl        %[temp2],          %[temp2],             1         \n\t"
          "srl        %[temp3],          %[temp3],             1         \n\t"
          "srl        %[tmp0],           %[tmp0],              1         \n\t"
          "srl        %[tmp1],           %[tmp1],              1         \n\t"
          "srl        %[tmp2],           %[tmp2],              1         \n\t"
          "srl        %[tmp3],           %[tmp3],              1         \n\t"
          "srl        %[temp4],          %[temp4],             1         \n\t"
          "srl        %[tmp4],           %[tmp4],              1         \n\t"
          "addu       %[temp0],          %[temp0],             %[temp1]  \n\t"
          "addu       %[temp2],          %[temp2],             %[temp3]  \n\t"
          "addu       %[tmp0],           %[tmp0],              %[tmp1]   \n\t"
          "addu       %[tmp2],           %[tmp2],              %[tmp3]   \n\t"
          "addu       %[tmp4],           %[temp4],             %[tmp4]   \n\t"
          "srl        %[temp0],          %[temp0],             3         \n\t"
          "srl        %[temp2],          %[temp2],             3         \n\t"
          "srl        %[tmp0],           %[tmp0],              3         \n\t"
          "srl        %[tmp2],           %[tmp2],              3         \n\t"
          "srl        %[tmp4],           %[tmp4],              3         \n\t"
          "addu       %[temp0],          %[temp0],             %[temp2]  \n\t"
          "addu       %[temp0],          %[temp0],             %[tmp0]   \n\t"
          "addu       %[temp0],          %[temp0],             %[tmp2]   \n\t"
          "addu       %[temp0],          %[temp0],             %[tmp4]   \n\t"
          "sw         %[temp0],          68(%[aaPowerSlot])              \n\t"
          "xor        %[temp4],          %[temp4],             %[temp4]  \n\t"
       "1:                                                               \n\t"
          "lw         %[temp0],          92(%[rIntBufferLeft])           \n\t"
          "lw         %[temp1],          92(%[iIntBufferLeft])           \n\t"
          "lw         %[temp2],          96(%[rIntBufferLeft])           \n\t"
          "lw         %[temp3],          96(%[iIntBufferLeft])           \n\t"
          "lw         %[tmp0],           100(%[rIntBufferLeft])          \n\t"
          "lw         %[tmp1],           100(%[iIntBufferLeft])          \n\t"
          "lw         %[tmp2],           104(%[rIntBufferLeft])          \n\t"
          "lw         %[tmp3],           104(%[iIntBufferLeft])          \n\t"
          "mulq_s.w   %[temp0],          %[temp0],             %[temp0]  \n\t"
          "mulq_s.w   %[temp1],          %[temp1],             %[temp1]  \n\t"
          "mulq_s.w   %[temp2],          %[temp2],             %[temp2]  \n\t"
          "mulq_s.w   %[temp3],          %[temp3],             %[temp3]  \n\t"
          "mulq_s.w   %[tmp0],           %[tmp0],              %[tmp0]   \n\t"
          "mulq_s.w   %[tmp1],           %[tmp1],              %[tmp1]   \n\t"
          "mulq_s.w   %[tmp2],           %[tmp2],              %[tmp2]   \n\t"
          "mulq_s.w   %[tmp3],           %[tmp3],              %[tmp3]   \n\t"
          "srl        %[temp0],          %[temp0],             1         \n\t"
          "srl        %[temp1],          %[temp1],             1         \n\t"
          "srl        %[temp2],          %[temp2],             1         \n\t"
          "srl        %[temp3],          %[temp3],             1         \n\t"
          "srl        %[tmp0],           %[tmp0],              1         \n\t"
          "srl        %[tmp1],           %[tmp1],              1         \n\t"
          "srl        %[tmp2],           %[tmp2],              1         \n\t"
          "srl        %[tmp3],           %[tmp3],              1         \n\t"
          "addu       %[temp0],          %[temp0],             %[temp1]  \n\t"
          "addu       %[temp2],          %[temp2],             %[temp3]  \n\t"
          "addu       %[tmp0],           %[tmp0],              %[tmp1]   \n\t"
          "addu       %[tmp2],           %[tmp2],              %[tmp3]   \n\t"
          "srl        %[temp0],          %[temp0],             3         \n\t"
          "srl        %[temp2],          %[temp2],             3         \n\t"
          "srl        %[tmp0],           %[tmp0],              3         \n\t"
          "srl        %[tmp2],           %[tmp2],              3         \n\t"
          "addu       %[temp0],          %[temp0],             %[temp2]  \n\t"
          "addu       %[temp0],          %[temp0],             %[tmp0]   \n\t"
          "addu       %[temp0],          %[temp0],             %[tmp2]   \n\t"
          "addu       %[temp4],          %[temp4],             %[temp0]  \n\t"
          "addiu      %[rIntBufferLeft], %[rIntBufferLeft],    16        \n\t"
          "addiu      %[iIntBufferLeft], %[iIntBufferLeft],    16        \n\t"
          "bne        %[loop_end1],      %[rIntBufferLeft],    1b        \n\t"
          "lw         %[temp0],          92(%[rIntBufferLeft])           \n\t"
          "lw         %[temp1],          92(%[iIntBufferLeft])           \n\t"
          "sw         %[temp4],          72(%[aaPowerSlot])              \n\t"
          "xor        %[tmp4],           %[tmp4],              %[tmp4]   \n\t"
       "2:                                                               \n\t"
          "lw         %[temp2],          96(%[rIntBufferLeft])           \n\t"
          "lw         %[temp3],          96(%[iIntBufferLeft])           \n\t"
          "lw         %[tmp0],           100(%[rIntBufferLeft])          \n\t"
          "lw         %[tmp1],           100(%[iIntBufferLeft])          \n\t"
          "lw         %[tmp2],           104(%[rIntBufferLeft])          \n\t"
          "lw         %[tmp3],           104(%[iIntBufferLeft])          \n\t"
          "mulq_s.w   %[temp0],          %[temp0],             %[temp0]  \n\t"
          "mulq_s.w   %[temp1],          %[temp1],             %[temp1]  \n\t"
          "mulq_s.w   %[temp2],          %[temp2],             %[temp2]  \n\t"
          "mulq_s.w   %[temp3],          %[temp3],             %[temp3]  \n\t"
          "mulq_s.w   %[tmp0],           %[tmp0],              %[tmp0]   \n\t"
          "mulq_s.w   %[tmp1],           %[tmp1],              %[tmp1]   \n\t"
          "mulq_s.w   %[tmp2],           %[tmp2],              %[tmp2]   \n\t"
          "mulq_s.w   %[tmp3],           %[tmp3],              %[tmp3]   \n\t"
          "srl        %[temp0],          %[temp0],             1         \n\t"
          "srl        %[temp1],          %[temp1],             1         \n\t"
          "srl        %[temp2],          %[temp2],             1         \n\t"
          "srl        %[temp3],          %[temp3],             1         \n\t"
          "srl        %[tmp0],           %[tmp0],              1         \n\t"
          "srl        %[tmp1],           %[tmp1],              1         \n\t"
          "srl        %[tmp2],           %[tmp2],              1         \n\t"
          "srl        %[tmp3],           %[tmp3],              1         \n\t"
          "addu       %[temp0],          %[temp0],             %[temp1]  \n\t"
          "addu       %[temp2],          %[temp2],             %[temp3]  \n\t"
          "addu       %[tmp0],           %[tmp0],              %[tmp1]   \n\t"
          "addu       %[tmp2],           %[tmp2],              %[tmp3]   \n\t"
          "srl        %[temp0],          %[temp0],             3         \n\t"
          "srl        %[temp2],          %[temp2],             3         \n\t"
          "srl        %[tmp0],           %[tmp0],              3         \n\t"
          "srl        %[tmp2],           %[tmp2],              3         \n\t"
          "addu       %[temp0],          %[temp0],             %[temp2]  \n\t"
          "addu       %[temp0],          %[temp0],             %[tmp0]   \n\t"
          "addu       %[temp0],          %[temp0],             %[tmp2]   \n\t"
          "addu       %[tmp4],           %[tmp4],              %[temp0]  \n\t"
          "lw         %[temp0],          108(%[rIntBufferLeft])          \n\t"
          "lw         %[temp1],          108(%[iIntBufferLeft])          \n\t"
          "addiu      %[rIntBufferLeft], %[rIntBufferLeft],    16        \n\t"
          "addiu      %[iIntBufferLeft], %[iIntBufferLeft],    16        \n\t"
          "bne        %[loop_end2],      %[rIntBufferLeft],    2b        \n\t"
          "mulq_s.w   %[temp0],          %[temp0],             %[temp0]  \n\t"
          "mulq_s.w   %[temp1],          %[temp1],             %[temp1]  \n\t"
          "srl        %[temp0],          %[temp0],             1         \n\t"
          "srl        %[temp1],          %[temp1],             1         \n\t"
          "addu       %[temp0],          %[temp0],             %[temp1]  \n\t"
          "srl        %[temp0],          %[temp0],             3         \n\t"
          "addu       %[tmp4],           %[tmp4],              %[temp0]  \n\t"
          "sw         %[tmp4],           76(%[aaPowerSlot])              \n\t"

          : [temp0] "=&r" (temp0), [temp1] "=&r" (temp1),
            [temp2] "=&r" (temp2), [temp3] "=&r" (temp3),
            [temp4] "=&r" (temp4),
            [tmp0] "=&r" (tmp0), [tmp1] "=&r" (tmp1),
            [tmp2] "=&r" (tmp2), [tmp3] "=&r" (tmp3),
            [tmp4] "=&r" (tmp4),
            [rIntBufferLeft] "+r" (rIntBufferLeft),
            [iIntBufferLeft] "+r" (iIntBufferLeft)
          : [aaPowerSlot] "r" (aaPowerSlot),
            [loop_end1] "r" (loop_end1),
            [loop_end2] "r" (loop_end2)
          : "memory", "hi", "lo"
      );
      rIntBufferLeft-=40;
      iIntBufferLeft-=40;
  }
  /* calculation of transient ratio */
  for (bin=0; bin < NO_MID_RES_BINS; bin++) {   /* noBins = 20 ( BASELINE_PS ) */

    h_ps_d->specificTo.mpeg.aPeakDecayFastBin[bin] = fMult( h_ps_d->specificTo.mpeg.aPeakDecayFastBin[bin], PEAK_DECAY_FACTOR );

    if (h_ps_d->specificTo.mpeg.aPeakDecayFastBin[bin] < aaPowerSlot[bin]) {
      h_ps_d->specificTo.mpeg.aPeakDecayFastBin[bin] = aaPowerSlot[bin];
    }

    /* calculate PSmoothPeakDecayDiffNrg */
    peakDiff = fMultAdd ( (h_ps_d->specificTo.mpeg.aPrevPeakDiffBin[bin]>>1),
                 INT_FILTER_COEFF, h_ps_d->specificTo.mpeg.aPeakDecayFastBin[bin] - aaPowerSlot[bin] - h_ps_d->specificTo.mpeg.aPrevPeakDiffBin[bin]);

    /* save peakDiff for the next frame */
    h_ps_d->specificTo.mpeg.aPrevPeakDiffBin[bin] = peakDiff;

    nrg = h_ps_d->specificTo.mpeg.aPrevNrgBin[bin] + fMult( INT_FILTER_COEFF, aaPowerSlot[bin] - h_ps_d->specificTo.mpeg.aPrevNrgBin[bin] );

    /* Negative energies don't exist. But sometimes they appear due to rounding. */

    nrg = fixMax(nrg,FL2FXCONST_DBL(0.f));

    /* save nrg for the next frame */
    h_ps_d->specificTo.mpeg.aPrevNrgBin[bin] = nrg;

    nrg = fMult( nrg, TRANSIENT_IMPACT_FACTOR );

    /* save transient impact factor */
    if ( peakDiff <= nrg || peakDiff == FL2FXCONST_DBL(0.0) ) {
      aaTransRatioSlot[bin] = (FIXP_DBL)MAXVAL_DBL /* FL2FXCONST_DBL(1.0f)*/;
    }
    else if ( nrg <= FL2FXCONST_DBL(0.0f) ) {
        aaTransRatioSlot[bin] = FL2FXCONST_DBL(0.f);
      }
    else {
      /* scale to denominator */
      INT scale_left = fixMax(0, CntLeadingZeros(peakDiff) - 1);
      aaTransRatioSlot[bin] = schur_div( nrg<<scale_left, peakDiff<<scale_left, 16);
    }
  } /* bin */




  #define DELAY_GROUP_OFFSET    20
  #define NR_OF_DELAY_GROUPS     2

  FIXP_DBL rTmp, iTmp, rTmp0, iTmp0, rR0, iR0;

  INT TempDelay     = h_ps_d->specificTo.mpeg.delayBufIndex;  /* set delay indices */

  pRealDelayBuffer = h_ps_d->specificTo.mpeg.aaRealDelayBufferSubQmf[TempDelay];
  pImagDelayBuffer = h_ps_d->specificTo.mpeg.aaImagDelayBufferSubQmf[TempDelay];

  aaLeftReal  = mHybridRealLeft;
  aaLeftImag  = mHybridImagLeft;
  aaRightReal = mHybridRealRight;
  aaRightImag = mHybridImagRight;

  /************************/
  /* ICC groups :  0 -  9 */
  /************************/

  /* gr = ICC groups */
  for (gr=0; gr < SUBQMF_GROUPS; gr++) {

    transRatio = aaTransRatioSlot[bins2groupMap20[gr]];

    /* sb = subQMF/QMF subband */
    sb = groupBorders20[gr];

    /* Update delay buffers, sample delay allpass = 2 */
    rTmp0 = pRealDelayBuffer[sb];
    iTmp0 = pImagDelayBuffer[sb];

    pRealDelayBuffer[sb] = aaLeftReal[sb];
    pImagDelayBuffer[sb] = aaLeftImag[sb];

    /* delay by fraction */
    cplxMultDiv2(&rR0, &iR0, rTmp0, iTmp0, aaFractDelayPhaseFactorReSubQmf20[sb], aaFractDelayPhaseFactorImSubQmf20[sb]);
    rR0<<=1;
    iR0<<=1;

    FIXP_DBL *pAaaRealDelayRBufferSerSubQmf = h_ps_d->specificTo.mpeg.aaaRealDelayRBufferSerSubQmf[sb];
    FIXP_DBL *pAaaImagDelayRBufferSerSubQmf = h_ps_d->specificTo.mpeg.aaaImagDelayRBufferSerSubQmf[sb];
    FIXP_DBL *a, *b, *cc, c;

    a = (FIXP_DBL*)&aaFractDelayPhaseFactorSerReSubQmf20[sb][0];
    b = (FIXP_DBL*)&aaFractDelayPhaseFactorSerImSubQmf20[sb][0];
    cc = (FIXP_DBL*)&aAllpassLinkDecaySer[0];

    for (m=0; m<NO_SERIAL_ALLPASS_LINKS ; m++) {

      INT tmpDelayRSer = h_ps_d->specificTo.mpeg.aDelayRBufIndexSer[m];
      FIXP_DBL result, result1, temp0, temp1;

      /* get delayed values from according buffer : m(0)=3; m(1)=4; m(2)=5; */
      rTmp0 = pAaaRealDelayRBufferSerSubQmf[tmpDelayRSer];
      iTmp0 = pAaaImagDelayRBufferSerSubQmf[tmpDelayRSer];

      /* delay by fraction */
      __asm__ volatile (
          "lw    %[c],       0(%[cc])                \n\t"
          "lw    %[temp0],   0(%[a])                 \n\t"
          "lw    %[temp1],   0(%[b])                 \n\t"
          "mult  $ac2,       %[c],       %[rR0]      \n\t"
          "mult  $ac3,       %[c],       %[iR0]      \n\t"
          "mult  $ac0,       %[rTmp0],   %[temp0]    \n\t"
          "mult  $ac1,       %[rTmp0],   %[temp1]    \n\t"
          "msub  $ac0,       %[iTmp0],   %[temp1]    \n\t"
          "madd  $ac1,       %[iTmp0],   %[temp0]    \n\t"
          "mfhi  %[temp0],   $ac2                    \n\t"
          "mfhi  %[temp1],   $ac3                    \n\t"
          "mfhi  %[result],  $ac0                    \n\t"
          "mfhi  %[result1], $ac1                    \n\t"
          "addiu %[a],       %[a],       4           \n\t"
          "addiu %[b],       %[b],       4           \n\t"
          "addiu %[cc],      %[cc],      4           \n\t"
          "subu  %[temp0],   %[result],  %[temp0]    \n\t"
          "subu  %[temp1],   %[result1], %[temp1]    \n\t"
          "sll   %[result],  %[temp0],   1           \n\t"
          "sll   %[result1], %[temp1],   1           \n\t"
          "mult  $ac0,       %[c],       %[result]   \n\t"
          "mult  $ac1,       %[c],       %[result1]  \n\t"
          "mfhi  %[temp0],   $ac0                    \n\t"
          "mfhi  %[temp1],   $ac1                    \n\t"
          "sll   %[temp0],   %[temp0],   1           \n\t"
          "sll   %[temp1],   %[temp1],   1           \n\t"
          "addu  %[temp0],   %[rR0],     %[temp0]    \n\t"
          "addu  %[temp1],   %[iR0],     %[temp1]    \n\t"

          : [result] "=&r" (result), [result1] "=&r" (result1),
            [temp0] "=&r" (temp0), [temp1] "=&r" (temp1),
            [a] "+r" (a), [b] "+r" (b), [c] "=&r" (c), [cc] "+r" (cc)
          : [rTmp0] "r" (rTmp0), [iTmp0] "r" (iTmp0), [rR0] "r" (rR0), [iR0] "r" (iR0)
          : "memory", "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo"
      );

      pAaaRealDelayRBufferSerSubQmf[tmpDelayRSer] = temp0;
      pAaaImagDelayRBufferSerSubQmf[tmpDelayRSer] = temp1;

      rR0 = result;
      iR0 = result1;

      pAaaRealDelayRBufferSerSubQmf += aAllpassLinkDelaySer[m];
      pAaaImagDelayRBufferSerSubQmf += aAllpassLinkDelaySer[m];

    } /* m */

    /* duck if a past transient is found */
    __asm__ volatile (
        "mult   $ac0,      %[transRatio],   %[rR0]    \n\t"
        "mult   $ac1,      %[transRatio],   %[iR0]    \n\t"
        "mfhi   %[temp2],  $ac0                       \n\t"
        "mfhi   %[temp3],  $ac1                       \n\t"
        "sll    %[temp2],  %[temp2],        1         \n\t"
        "sll    %[temp3],  %[temp3],        1         \n\t"

        : [temp2] "=&r" (temp2), [temp3] "=&r" (temp3)
        : [transRatio] "r" (transRatio), [rR0] "r" (rR0), [iR0] "r" (iR0)
        : "hi", "lo", "$ac1hi", "$ac1lo"
    );
    aaRightReal[sb] = temp2;
    aaRightImag[sb] = temp3;

  } /* gr */


  scaleValues( mHybridRealLeft,  NO_SUB_QMF_CHANNELS, -SCAL_HEADROOM );
  scaleValues( mHybridImagLeft,  NO_SUB_QMF_CHANNELS, -SCAL_HEADROOM );
  scaleValues( mHybridRealRight, NO_SUB_QMF_CHANNELS, -SCAL_HEADROOM );
  scaleValues( mHybridImagRight, NO_SUB_QMF_CHANNELS, -SCAL_HEADROOM );


  /************************/

  aaLeftReal  = rIntBufferLeft;
  aaLeftImag  = iIntBufferLeft;
  aaRightReal = rIntBufferRight;
  aaRightImag = iIntBufferRight;

  pRealDelayBuffer = h_ps_d->specificTo.mpeg.aaRealDelayBufferQmf[TempDelay];
  pImagDelayBuffer = h_ps_d->specificTo.mpeg.aaImagDelayBufferQmf[TempDelay];

  /************************/
  /* ICC groups : 10 - 19 */
  /************************/


  /* gr = ICC groups */
  for (gr=SUBQMF_GROUPS; gr < NO_IID_GROUPS - NR_OF_DELAY_GROUPS; gr++) {

    transRatio = aaTransRatioSlot[bins2groupMap20[gr]];

    /* sb = subQMF/QMF subband */
    for (sb = groupBorders20[gr]; sb < groupBorders20[gr+1]; sb++) {
      FIXP_DBL resR, resI;

      /* decayScaleFactor = 1.0f + decay_cutoff * DECAY_SLOPE - DECAY_SLOPE * sb; DECAY_SLOPE = 0.05 */
      FIXP_DBL decayScaleFactor = decayScaleFactTable[sb];

      /* Update delay buffers, sample delay allpass = 2 */
      rTmp0 = pRealDelayBuffer[sb];
      iTmp0 = pImagDelayBuffer[sb];

      pRealDelayBuffer[sb] = aaLeftReal[sb];
      pImagDelayBuffer[sb] = aaLeftImag[sb];

      /* delay by fraction */
      cplxMultDiv2(&rR0, &iR0, rTmp0, iTmp0, aaFractDelayPhaseFactorReQmf[sb], aaFractDelayPhaseFactorImQmf[sb]);
      rR0<<=1;
      iR0<<=1;

      resR = fMult(decayScaleFactor, rR0);
      resI = fMult(decayScaleFactor, iR0);

      FIXP_DBL *pAaaRealDelayRBufferSerQmf = h_ps_d->specificTo.mpeg.aaaRealDelayRBufferSerQmf[sb];
      FIXP_DBL *pAaaImagDelayRBufferSerQmf = h_ps_d->specificTo.mpeg.aaaImagDelayRBufferSerQmf[sb];
      FIXP_DBL *a, *b, *cc, c;
      int mask = 0xfffffffe;

      a = (FIXP_DBL*)&aaFractDelayPhaseFactorSerReQmf[sb][0];
      b = (FIXP_DBL*)&aaFractDelayPhaseFactorSerImQmf[sb][0];
      cc = (FIXP_DBL*)&aAllpassLinkDecaySer[0];

      for (m=0; m<NO_SERIAL_ALLPASS_LINKS ; m++) {

        INT tmpDelayRSer = h_ps_d->specificTo.mpeg.aDelayRBufIndexSer[m];
        FIXP_DBL result, result1, tmp0, tmp1, temp0, temp1;

        /* get delayed values from according buffer : m(0)=3; m(1)=4; m(2)=5; */
        rTmp0 = pAaaRealDelayRBufferSerQmf[tmpDelayRSer];
        iTmp0 = pAaaImagDelayRBufferSerQmf[tmpDelayRSer];

        /* delay by fraction */
        __asm__ volatile (
            "lw        %[c],       0(%[cc])                        \n\t"
            "lw        %[tmp0],    0(%[a])                         \n\t"
            "lw        %[tmp1],    0(%[b])                         \n\t"
            "mult      $ac2,       %[c],                %[resR]    \n\t"
            "mult      $ac3,       %[c],                %[resI]    \n\t"
            "mult      $ac0,       %[rTmp0],            %[tmp0]    \n\t"
            "msub      $ac0,       %[iTmp0],            %[tmp1]    \n\t"
            "mult      $ac1,       %[rTmp0],            %[tmp1]    \n\t"
            "madd      $ac1,       %[iTmp0],            %[tmp0]    \n\t"
            "mfhi      %[tmp0],    $ac2                            \n\t"
            "mfhi      %[tmp1],    $ac3                            \n\t"
            "mfhi      %[result],  $ac0                            \n\t"
            "addiu     %[a],       %[a],                4          \n\t"
            "mfhi      %[result1], $ac1                            \n\t"
            "addiu     %[b],       %[b],                4          \n\t"
            "addiu     %[cc],      %[cc],               4          \n\t"
            "subu      %[tmp0],    %[result],           %[tmp0]    \n\t"
            "subu      %[tmp1],    %[result1],          %[tmp1]    \n\t"
            "sll       %[result],  %[tmp0],             1          \n\t"
            "sll       %[result1], %[tmp1],             1          \n\t"
            "mulq_s.w  %[resR],    %[decayScaleFactor], %[result]  \n\t"
            "mulq_s.w  %[resI],    %[decayScaleFactor], %[result1] \n\t"
            "and       %[resR],    %[resR],             %[mask]    \n\t"
            "and       %[resI],    %[resI],             %[mask]    \n\t"
            "mulq_s.w  %[tmp0],    %[c],                %[resR]    \n\t"
            "mulq_s.w  %[tmp1],    %[c],                %[resI]    \n\t"
            "and       %[tmp0],    %[tmp0],             %[mask]    \n\t"
            "and       %[tmp1],    %[tmp1],             %[mask]    \n\t"
            "addu      %[tmp0],    %[rR0],              %[tmp0]    \n\t"
            "addu      %[tmp1],    %[iR0],              %[tmp1]    \n\t"

            : [result] "=&r" (result), [result1] "=&r" (result1),
              [tmp0] "=&r" (tmp0), [tmp1] "=&r" (tmp1),
              [a] "+r" (a), [b] "+r" (b), [cc] "+r" (cc), [c] "=&r" (c),
              [resR] "+r" (resR), [resI] "+r" (resI)
            : [rTmp0] "r" (rTmp0), [iTmp0] "r" (iTmp0),
              [decayScaleFactor] "r" (decayScaleFactor),
              [rR0] "r" (rR0), [iR0] "r" (iR0), [mask] "r" (mask)
            : "memory", "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo"
        );

        pAaaRealDelayRBufferSerQmf[tmpDelayRSer] = tmp0;
        pAaaImagDelayRBufferSerQmf[tmpDelayRSer] = tmp1;

        rR0 = result;
        iR0 = result1;

        pAaaRealDelayRBufferSerQmf += aAllpassLinkDelaySer[m];
        pAaaImagDelayRBufferSerQmf += aAllpassLinkDelaySer[m];

      } /* m */

      /* duck if a past transient is found */
      __asm__ volatile (
          "mult   $ac0,       %[transRatio],   %[rR0]    \n\t"
          "mult   $ac1,       %[transRatio],   %[iR0]    \n\t"
          "mfhi   %[temp2],   $ac0                       \n\t"
          "mfhi   %[temp3],   $ac1                       \n\t"
          "sll    %[temp2],   %[temp2],        1         \n\t"
          "sll    %[temp3],   %[temp3],        1         \n\t"

          : [temp2] "=&r" (temp2), [temp3] "=&r" (temp3)
          : [transRatio] "r" (transRatio), [rR0] "r" (rR0), [iR0] "r" (iR0)
          : "hi", "lo", "$ac1hi", "$ac1lo"
      );
      aaRightReal[sb] = temp2;
      aaRightImag[sb] = temp3;
    } /* sb */
  } /* gr */

  /************************/
  /* ICC groups : 20,  21 */
  /************************/


  /* gr = ICC groups */
  for (gr=DELAY_GROUP_OFFSET; gr < NO_IID_GROUPS; gr++) {

    INT sbStart = groupBorders20[gr];
    INT sbStop  = groupBorders20[gr+1];

    UCHAR *pDelayBufIdx = &h_ps_d->specificTo.mpeg.aDelayBufIndexDelayQmf[sbStart-FIRST_DELAY_SB];

    transRatio = aaTransRatioSlot[bins2groupMap20[gr]];

    /* sb = subQMF/QMF subband */
    for (sb = sbStart; sb < sbStop; sb++) {

      /* Update delay buffers */
      rR0 = h_ps_d->specificTo.mpeg.pAaRealDelayBufferQmf[*pDelayBufIdx][sb-FIRST_DELAY_SB];
      iR0 = h_ps_d->specificTo.mpeg.pAaImagDelayBufferQmf[*pDelayBufIdx][sb-FIRST_DELAY_SB];

      h_ps_d->specificTo.mpeg.pAaRealDelayBufferQmf[*pDelayBufIdx][sb-FIRST_DELAY_SB] = aaLeftReal[sb];
      h_ps_d->specificTo.mpeg.pAaImagDelayBufferQmf[*pDelayBufIdx][sb-FIRST_DELAY_SB] = aaLeftImag[sb];

      /* duck if a past transient is found */
      aaRightReal[sb] = fMult(transRatio, rR0);
      aaRightImag[sb] = fMult(transRatio, iR0);

      if (++(*pDelayBufIdx) >= delayIndexQmf[sb]) {
        *pDelayBufIdx = 0;
      }
      pDelayBufIdx++;

    } /* sb */
  } /* gr */


  /* Update delay buffer index */
  if (++h_ps_d->specificTo.mpeg.delayBufIndex >= NO_SAMPLE_DELAY_ALLPASS)
    h_ps_d->specificTo.mpeg.delayBufIndex = 0;

  for (m=0; m<NO_SERIAL_ALLPASS_LINKS ; m++) {
    if (++h_ps_d->specificTo.mpeg.aDelayRBufIndexSer[m] >= aAllpassLinkDelaySer[m])
      h_ps_d->specificTo.mpeg.aDelayRBufIndexSer[m] = 0;
  }


  scaleValues( &rIntBufferLeft[NO_QMF_BANDS_HYBRID20],  NO_QMF_CHANNELS-NO_QMF_BANDS_HYBRID20, -SCAL_HEADROOM );
  scaleValues( &iIntBufferLeft[NO_QMF_BANDS_HYBRID20],  NO_QMF_CHANNELS-NO_QMF_BANDS_HYBRID20, -SCAL_HEADROOM );
  scaleValues( &rIntBufferRight[NO_QMF_BANDS_HYBRID20], NO_QMF_CHANNELS-NO_QMF_BANDS_HYBRID20, -SCAL_HEADROOM );
  scaleValues( &iIntBufferRight[NO_QMF_BANDS_HYBRID20], NO_QMF_CHANNELS-NO_QMF_BANDS_HYBRID20, -SCAL_HEADROOM );

  /* free memory on scratch */
  C_ALLOC_SCRATCH_END(aaTransRatioSlot, FIXP_DBL, NO_MID_RES_BINS);
  C_ALLOC_SCRATCH_END(aaPowerSlot, FIXP_DBL, NO_MID_RES_BINS);
}
#endif /* #if defined FUNCTION_deCorrelateSlotBased */


#if defined (FUNCTION_applySlotBasedRotation)
static void applySlotBasedRotation( HANDLE_PS_DEC h_ps_d,        /*!< pointer to the module state */

                                    FIXP_DBL  *mHybridRealLeft,  /*!< hybrid values real left  */
                                    FIXP_DBL  *mHybridImagLeft,  /*!< hybrid values imag left  */

                                    FIXP_DBL  *QmfLeftReal,      /*!< real bands left qmf channel */
                                    FIXP_DBL  *QmfLeftImag,      /*!< imag bands left qmf channel */

                                    FIXP_DBL  *mHybridRealRight, /*!< hybrid values real right  */
                                    FIXP_DBL  *mHybridImagRight, /*!< hybrid values imag right  */

                                    FIXP_DBL  *QmfRightReal,     /*!< real bands right qmf channel */
                                    FIXP_DBL  *QmfRightImag      /*!< imag bands right qmf channel */
                                   )
{
  INT     group;
  INT     subband;

  FIXP_DBL *RESTRICT HybrLeftReal;
  FIXP_DBL *RESTRICT HybrLeftImag;
  FIXP_DBL *RESTRICT HybrRightReal;
  FIXP_DBL *RESTRICT HybrRightImag;

  FIXP_DBL tmpLeft, tmpRight;


  /**********************************************************************************************/
  /*!
  <h2> Mapping </h2>

  The number of stereo bands that is actually used depends on the number of availble
  parameters for IID and ICC:
  <pre>
   nr. of IID para.| nr. of ICC para. | nr. of Stereo bands
   ----------------|------------------|-------------------
     10,20         |     10,20        |        20
     10,20         |     34           |        34
     34            |     10,20        |        34
     34            |     34           |        34
  </pre>
  In the case the number of parameters for IIS and ICC differs from the number of stereo
  bands, a mapping from the lower number to the higher number of parameters is applied.
  Index mapping of IID and ICC parameters is already done in psbitdec.cpp. Further mapping is
  not needed here in baseline version.
  **********************************************************************************************/

  /************************************************************************************************/
  /*!
  <h2> Mixing </h2>

  To generate the QMF subband signals for the subband samples n = n[e]+1 ,,, n_[e+1] the
  parameters at position n[e] and n[e+1] are required as well as the subband domain signals
  s_k(n) and d_k(n) for n = n[e]+1... n_[e+1]. n[e] represents the start position for
  envelope e. The border positions n[e] are handled in DecodePS().

  The stereo sub subband signals are constructed as:
  <pre>
  l_k(n) = H11(k,n) s_k(n) + H21(k,n) d_k(n)
  r_k(n) = H21(k,n) s_k(n) + H22(k,n) d_k(n)
  </pre>
  In order to obtain the matrices H11(k,n)... H22 (k,n), the vectors h11(b)... h22(b) need to
  be calculated first (b: parameter index). Depending on ICC mode either mixing procedure R_a
  or R_b is used for that. For both procedures, the parameters for parameter position n[e+1]
  is used.
  ************************************************************************************************/


  /************************************************************************************************/
  /*!
  <h2>Phase parameters </h2>
  With disabled phase parameters (which is the case in baseline version), the H-matrices are
  just calculated by:

  <pre>
  H11(k,n[e+1] = h11(b(k))
  (...)
  b(k): parameter index according to mapping table
  </pre>

  <h2>Processing of the samples in the sub subbands </h2>
  this loop includes the interpolation of the coefficients Hxx
  ************************************************************************************************/


  /* loop thru all groups ... */
  HybrLeftReal  = mHybridRealLeft;
  HybrLeftImag  = mHybridImagLeft;
  HybrRightReal = mHybridRealRight;
  HybrRightImag = mHybridImagRight;

  FIXP_DBL *H11r, *H12r, *H21r, *H22r;
  FIXP_DBL *H11r_delta, *H12r_delta, *H21r_delta, *H22r_delta;
  FIXP_DBL *HybLeftRe, *HybRightRe, *HybLeftIm, *HybRightIm;
  FIXP_DBL temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;


  /******************************************************/
  /* construct stereo sub subband signals according to: */
  /*                                                    */
  /* l_k(n) = H11(k,n) s_k(n) + H21(k,n) d_k(n)         */
  /* r_k(n) = H12(k,n) s_k(n) + H22(k,n) d_k(n)         */
  /******************************************************/

    H11r = &(h_ps_d->specificTo.mpeg.coef.H11r[0]);
    H12r = &(h_ps_d->specificTo.mpeg.coef.H12r[0]);
    H21r = &(h_ps_d->specificTo.mpeg.coef.H21r[0]);
    H22r = &(h_ps_d->specificTo.mpeg.coef.H22r[0]);

    H11r_delta = &(h_ps_d->specificTo.mpeg.coef.DeltaH11r[0]);
    H12r_delta = &(h_ps_d->specificTo.mpeg.coef.DeltaH12r[0]);
    H21r_delta = &(h_ps_d->specificTo.mpeg.coef.DeltaH21r[0]);
    H22r_delta = &(h_ps_d->specificTo.mpeg.coef.DeltaH22r[0]);



  for ( group = 0; group < SUBQMF_GROUPS; group++ ) {

    subband = groupBorders20[group];

    HybLeftRe = &HybrLeftReal[subband];
    HybRightRe = &HybrRightReal[subband];
    HybLeftIm = &HybrLeftImag[subband];
    HybRightIm = &HybrRightImag[subband];

    __asm__ volatile (
        "lw      %[temp1],  0(%[H11r])                 \n\t"
        "lw      %[temp5],  0(%[H11r_delta])           \n\t"
        "lw      %[temp2],  0(%[H12r])                 \n\t"
        "lw      %[temp6],  0(%[H12r_delta])           \n\t"
        "lw      %[temp3],  0(%[H21r])                 \n\t"
        "lw      %[temp7],  0(%[H21r_delta])           \n\t"
        "lw      %[temp4],  0(%[H22r])                 \n\t"
        "lw      %[temp8],  0(%[H22r_delta])           \n\t"
        "addu    %[temp1],  %[temp1],        %[temp5]  \n\t"
        "addu    %[temp2],  %[temp2],        %[temp6]  \n\t"
        "lw      %[temp5],  0(%[HybLeftRe])            \n\t"
        "lw      %[temp6],  0(%[HybRightRe])           \n\t"
        "addu    %[temp3],  %[temp3],        %[temp7]  \n\t"
        "addu    %[temp4],  %[temp4],        %[temp8]  \n\t"
        "mult    $ac0,      %[temp1],        %[temp5]  \n\t"
        "mult    $ac1,      %[temp3],        %[temp6]  \n\t"
        "mult    $ac2,      %[temp2],        %[temp5]  \n\t"
        "mult    $ac3,      %[temp4],        %[temp6]  \n\t"
        "mfhi    %[temp5],  $ac0                       \n\t"
        "mfhi    %[temp6],  $ac1                       \n\t"
        "mfhi    %[temp7],  $ac2                       \n\t"
        "mfhi    %[temp8],  $ac3                       \n\t"
        "sw      %[temp1],  0(%[H11r])                 \n\t"
        "sw      %[temp2],  0(%[H12r])                 \n\t"
        "sw      %[temp3],  0(%[H21r])                 \n\t"
        "addu    %[temp5],  %[temp5],        %[temp6]  \n\t"
        "addu    %[temp7],  %[temp7],        %[temp8]  \n\t"
        "lw      %[temp6],  0(%[HybLeftIm])            \n\t"
        "lw      %[temp8],  0(%[HybRightIm])           \n\t"
        "sw      %[temp4],  0(%[H22r])                 \n\t"
        "mult    $ac0,      %[temp1],        %[temp6]  \n\t"
        "mult    $ac1,      %[temp3],        %[temp8]  \n\t"
        "mult    $ac2,      %[temp2],        %[temp6]  \n\t"
        "mult    $ac3,      %[temp4],        %[temp8]  \n\t"
        "mfhi    %[temp1],  $ac0                       \n\t"
        "mfhi    %[temp2],  $ac1                       \n\t"
        "mfhi    %[temp3],  $ac2                       \n\t"
        "mfhi    %[temp4],  $ac3                       \n\t"
        "sll     %[temp5],  %[temp5],        1         \n\t"
        "sll     %[temp7],  %[temp7],        1         \n\t"
        "sw      %[temp5],  0(%[HybLeftRe])            \n\t"
        "sw      %[temp7],  0(%[HybRightRe])           \n\t"
        "addu    %[temp1],  %[temp1],        %[temp2]  \n\t"
        "addu    %[temp3],  %[temp3],        %[temp4]  \n\t"
        "sll     %[temp1],  %[temp1],        1         \n\t"
        "sll     %[temp3],  %[temp3],        1         \n\t"
        "sw      %[temp1],  0(%[HybLeftIm])            \n\t"
        "sw      %[temp3],  0(%[HybRightIm])           \n\t"

        : [temp1] "=&r" (temp1), [temp2] "=&r" (temp2), [temp3] "=&r" (temp3),
          [temp4] "=&r" (temp4), [temp5] "=&r" (temp5), [temp6] "=&r" (temp6),
          [temp7] "=&r" (temp7), [temp8] "=&r" (temp8),
          [HybLeftRe] "+r" (HybLeftRe), [HybRightRe] "+r" (HybRightRe),
          [HybLeftIm] "+r" (HybLeftIm), [HybRightIm] "+r" (HybRightIm)
        : [H11r_delta] "r" (H11r_delta), [H12r_delta] "r" (H12r_delta),
          [H21r_delta] "r" (H21r_delta), [H22r_delta] "r" (H22r_delta),
          [H11r] "r" (H11r), [H21r] "r" (H21r), [H12r] "r" (H12r), [H22r] "r" (H22r)
        : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo", "memory"
    );

    H11r++;
    H21r++;
    H12r++;
    H22r++;
    H11r_delta++;
    H21r_delta++;
    H12r_delta++;
    H22r_delta++;

  }

  /* continue in the qmf buffers */
  HybrLeftReal  = QmfLeftReal;
  HybrLeftImag  = QmfLeftImag;
  HybrRightReal = QmfRightReal;
  HybrRightImag = QmfRightImag;

  H11r = &(h_ps_d->specificTo.mpeg.coef.H11r[SUBQMF_GROUPS]);
  H12r = &(h_ps_d->specificTo.mpeg.coef.H12r[SUBQMF_GROUPS]);
  H21r = &(h_ps_d->specificTo.mpeg.coef.H21r[SUBQMF_GROUPS]);
  H22r = &(h_ps_d->specificTo.mpeg.coef.H22r[SUBQMF_GROUPS]);

  H11r_delta = &(h_ps_d->specificTo.mpeg.coef.DeltaH11r[SUBQMF_GROUPS]);
  H12r_delta = &(h_ps_d->specificTo.mpeg.coef.DeltaH12r[SUBQMF_GROUPS]);
  H21r_delta = &(h_ps_d->specificTo.mpeg.coef.DeltaH21r[SUBQMF_GROUPS]);
  H22r_delta = &(h_ps_d->specificTo.mpeg.coef.DeltaH22r[SUBQMF_GROUPS]);


  for (; group < NO_IID_GROUPS; group++ ) {

    subband = groupBorders20[group];
    int subband1 = groupBorders20[group + 1];

    HybLeftRe = &HybrLeftReal[subband];
    HybRightRe = &HybrRightReal[subband];
    HybLeftIm = &HybrLeftImag[subband];
    HybRightIm = &HybrRightImag[subband];

    __asm__ volatile (
        "lw     %[temp1],       0(%[H11r])                 \n\t"
        "lw     %[temp5],       0(%[H11r_delta])           \n\t"
        "lw     %[temp2],       0(%[H12r])                 \n\t"
        "lw     %[temp6],       0(%[H12r_delta])           \n\t"
        "lw     %[temp3],       0(%[H21r])                 \n\t"
        "lw     %[temp7],       0(%[H21r_delta])           \n\t"
        "lw     %[temp4],       0(%[H22r])                 \n\t"
        "lw     %[temp8],       0(%[H22r_delta])           \n\t"
        "addu   %[temp1],       %[temp1],        %[temp5]  \n\t"
        "addu   %[temp2],       %[temp2],        %[temp6]  \n\t"
        "addu   %[temp3],       %[temp3],        %[temp7]  \n\t"
        "addu   %[temp4],       %[temp4],        %[temp8]  \n\t"
        "sw     %[temp1],       0(%[H11r])                 \n\t"
        "sw     %[temp2],       0(%[H12r])                 \n\t"
        "lw     %[temp5],       0(%[HybLeftRe])            \n\t"
        "lw     %[temp6],       0(%[HybRightRe])           \n\t"
        "sw     %[temp3],       0(%[H21r])                 \n\t"
        "sw     %[temp4],       0(%[H22r])                 \n\t"
      "1:                                                  \n\t"
        "mult    $ac0,          %[temp1],        %[temp5]  \n\t"
        "mult    $ac1,          %[temp3],        %[temp6]  \n\t"
        "mult    $ac2,          %[temp2],        %[temp5]  \n\t"
        "mult    $ac3,          %[temp4],        %[temp6]  \n\t"
        "mfhi    %[temp5],      $ac0                       \n\t"
        "mfhi    %[temp6],      $ac1                       \n\t"
        "mfhi    %[temp7],      $ac2                       \n\t"
        "mfhi    %[temp8],      $ac3                       \n\t"
        "addiu   %[HybLeftRe],  %[HybLeftRe],    4         \n\t"
        "addiu   %[HybRightRe], %[HybRightRe],   4         \n\t"
        "addiu   %[HybLeftIm],  %[HybLeftIm],    4         \n\t"
        "addiu   %[HybRightIm], %[HybRightIm],   4         \n\t"
        "addu    %[temp5],      %[temp5],        %[temp6]  \n\t"
        "addu    %[temp7],      %[temp7],        %[temp8]  \n\t"
        "sll     %[temp5],      %[temp5],        1         \n\t"
        "sll     %[temp7],      %[temp7],        1         \n\t"
        "lw      %[temp6],      -4(%[HybLeftIm])           \n\t"
        "lw      %[temp8],      -4(%[HybRightIm])          \n\t"
        "sw      %[temp5],      -4(%[HybLeftRe])           \n\t"
        "sw      %[temp7],      -4(%[HybRightRe])          \n\t"
        "mult    $ac0,          %[temp1],        %[temp6]  \n\t"
        "mult    $ac1,          %[temp3],        %[temp8]  \n\t"
        "mult    $ac2,          %[temp2],        %[temp6]  \n\t"
        "mult    $ac3,          %[temp4],        %[temp8]  \n\t"
        "mfhi    %[temp5],      $ac0                       \n\t"
        "mfhi    %[temp6],      $ac1                       \n\t"
        "mfhi    %[temp7],      $ac2                       \n\t"
        "mfhi    %[temp8],      $ac3                       \n\t"
        "addiu   %[subband],    %[subband],      1         \n\t"
        "addu    %[temp5],      %[temp5],        %[temp6]  \n\t"
        "addu    %[temp7],      %[temp7],        %[temp8]  \n\t"
        "sll     %[temp5],      %[temp5],        1         \n\t"
        "sll     %[temp7],      %[temp7],        1         \n\t"
        "sw      %[temp5],      -4(%[HybLeftIm])           \n\t"
        "sw      %[temp7],      -4(%[HybRightIm])          \n\t"
        "lw      %[temp5],      0(%[HybLeftRe])            \n\t"
        "lw      %[temp6],      0(%[HybRightRe])           \n\t"
        "bne     %[subband],    %[subband1],     1b        \n\t"

        : [temp1] "=&r" (temp1), [temp2] "=&r" (temp2), [temp3] "=&r" (temp3),
          [temp4] "=&r" (temp4), [temp5] "=&r" (temp5), [temp6] "=&r" (temp6),
          [temp7] "=&r" (temp7), [temp8] "=&r" (temp8),
          [HybLeftRe] "+r" (HybLeftRe), [HybRightRe] "+r" (HybRightRe),
          [HybLeftIm] "+r" (HybLeftIm), [HybRightIm] "+r" (HybRightIm), [subband] "+r" (subband)
        : [H11r_delta] "r" (H11r_delta), [H12r_delta] "r" (H12r_delta),
          [H21r_delta] "r" (H21r_delta), [H22r_delta] "r" (H22r_delta),
          [subband1] "r" (subband1), [H11r] "r" (H11r), [H21r] "r" (H21r),
          [H12r] "r" (H12r), [H22r] "r" (H22r)
        : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo", "$ac3hi", "$ac3lo", "memory"

    );

    H11r++;
    H21r++;
    H12r++;
    H22r++;
    H11r_delta++;
    H21r_delta++;
    H12r_delta++;
    H22r_delta++;

  }
}
#endif
