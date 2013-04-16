
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

/********************************  Fraunhofer IIS  ***************************

   Author(s):
   Description: QMF filterbank

******************************************************************************/


#if defined(MIPS_DSP_R2_LE)
#define FUNCTION_qmfSynPrototypeFirSlot
#define FUNCTION_qmfAnaPrototypeFirSlot
#endif /* #if defined(MIPS_DSP_R2_LE) */

#if defined(FUNCTION_qmfSynPrototypeFirSlot)

/*!
  \brief Perform Synthesis Prototype Filtering on a single slot of input data.

  The filter takes 2 * qmf->no_channels of input data and
  generates qmf->no_channels time domain output samples.
*/
static
void qmfSynPrototypeFirSlot(
                             HANDLE_QMF_FILTER_BANK qmf,
                             FIXP_QMF *RESTRICT realSlot,            /*!< Input: Pointer to real Slot */
                             FIXP_QMF *RESTRICT imagSlot,            /*!< Input: Pointer to imag Slot */
                             INT_PCM  *RESTRICT timeOut,             /*!< Time domain data */
                             int       stride
                            )
{
  FIXP_QSS* FilterStates = (FIXP_QSS*)qmf->FilterStates;
  int       no_channels = qmf->no_channels;
  const FIXP_PFT *p_Filter = qmf->p_filter;
  int p_stride = qmf->p_stride;
  int j;
  FIXP_QSS *RESTRICT sta = FilterStates;
  const FIXP_PFT *RESTRICT p_flt, *RESTRICT p_fltm;
  int scale = ((DFRACT_BITS-SAMPLE_BITS)-1-qmf->outScalefactor);

  p_flt  = p_Filter+p_stride*QMF_NO_POLY;          /*                     5-ter von 330 */
  p_fltm = p_Filter+(qmf->FilterSize/2)-p_stride*QMF_NO_POLY;  /* 5 + (320 - 2*5) = 315-ter von 330 */
  int p_str_QMF = p_stride * 10;

  FDK_ASSERT(SAMPLE_BITS-1-qmf->outScalefactor >= 0); //   (DFRACT_BITS-SAMPLE_BITS)-1-qmf->outScalefactor >= 0);

  for (j = no_channels-1; j >= 0; j--) {  /* ---- läuft ueber alle Linien eines Slots ---- */
    FIXP_QMF imag  =  imagSlot[j];  // no_channels-1 .. 0
    FIXP_QMF real  =  realSlot[j];  // ~~"~~
    {
      INT_PCM tmp;
      FIXP_QSS  temp1,temp2;
      FIXP_DBL Are = FX_QSS2FX_DBL(sta[0]) + fMultDiv2( p_fltm[0] , real);

      if (qmf->outGain!=(FIXP_DBL)0x80000000) {
        Are = fMult(Are,qmf->outGain);
      }

#if SAMPLE_BITS > 16
      tmp = (INT_PCM)(SATURATE_SHIFT(fAbs(Are), scale, SAMPLE_BITS));
#else
      int temp_S_B = SAMPLE_BITS;
      __asm__ volatile(
        "absq_s.w  %[temp1], %[Are]                     \n\t"
        "subu      %[temp2], %[temp_S_B], %[scale]      \n\t"
        "shllv_s.w %[temp2], %[temp1],    %[temp2]      \n\t"
        "srl       %[temp2], %[temp2],    16            \n\t"

        : [temp1] "=&r" (temp1), [temp2] "=&r" (temp2)
        : [Are] "r" (Are), [scale] "r" (scale), [temp_S_B] "r" (temp_S_B)
      );

      tmp = (INT_PCM)(temp2);
#endif
      if (Are < (FIXP_QMF)0) {
        tmp = -tmp;
      }
      timeOut[ (j)*stride ] = tmp;
    }

     FIXP_QSS  temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8;
     FIXP_QSS  temp9,temp10,temp11,temp12;
    __asm__ volatile(

        "lw      %[temp1],   4(%[sta])                    \n\t"
        "lh      %[temp2],   8(%[p_flt])                  \n\t"
        "lw      %[temp3],   8(%[sta])                    \n\t"
        "lh      %[temp4],   2(%[p_fltm])                 \n\t"
        "lw      %[temp5],   12(%[sta])                   \n\t"
        "lh      %[temp6],   6(%[p_flt])                  \n\t"
        "lw      %[temp7],   16(%[sta])                   \n\t"
        "lh      %[temp8],   4(%[p_fltm])                 \n\t"

        "sll     %[temp2],   %[temp2],      16            \n\t"
        "sll     %[temp4],   %[temp4],      16            \n\t"
        "sll     %[temp6],   %[temp6],      16            \n\t"
        "sll     %[temp8],   %[temp8],      16            \n\t"

        "mult    $ac0,       %[temp2],      %[imag]       \n\t"
        "mult    $ac1,       %[temp4],      %[real]       \n\t"
        "mult    $ac2,       %[temp6],      %[imag]       \n\t"
        "mult    $ac3,       %[temp8],      %[real]       \n\t"

        "mfhi    %[temp2],   $ac0                         \n\t"
        "mfhi    %[temp4],   $ac1                         \n\t"
        "mfhi    %[temp6],   $ac2                         \n\t"
        "mfhi    %[temp8],   $ac3                         \n\t"

        "lw      %[temp9],   20(%[sta])                   \n\t"
        "lh      %[temp10],  4(%[p_flt])                  \n\t"
        "lw      %[temp11],  24(%[sta])                   \n\t"
        "lh      %[temp12],  6(%[p_fltm])                 \n\t"

        "addu    %[temp1],   %[temp1],      %[temp2]      \n\t"
        "addu    %[temp3],   %[temp3],      %[temp4]      \n\t"
        "addu    %[temp5],   %[temp5],      %[temp6]      \n\t"
        "addu    %[temp7],   %[temp7],      %[temp8]      \n\t"

        "lw      %[temp2],   28(%[sta])                   \n\t"
        "lh      %[temp4],   2(%[p_flt])                  \n\t"
        "lw      %[temp6],   32(%[sta])                   \n\t"
        "lh      %[temp8],   8(%[p_fltm])                 \n\t"

        "sll     %[temp10],  %[temp10],     16            \n\t"
        "sll     %[temp12],  %[temp12],     16            \n\t"
        "sll     %[temp4],   %[temp4],      16            \n\t"
        "sll     %[temp8],   %[temp8],      16            \n\t"

        "mult    $ac0,       %[temp10],     %[imag]       \n\t"
        "mult    $ac1,       %[temp12],     %[real]       \n\t"
        "mult    $ac2,       %[temp4],      %[imag]       \n\t"
        "mult    $ac3,       %[temp8],      %[real]       \n\t"

        "mfhi    %[temp10],  $ac0                         \n\t"
        "mfhi    %[temp12],  $ac1                         \n\t"
        "mfhi    %[temp4],   $ac2                         \n\t"
        "mfhi    %[temp8],   $ac3                         \n\t"

        "sw      %[temp1],   0(%[sta])                    \n\t"
        "lh      %[temp1],   0(%[p_flt])                  \n\t"
        "sw      %[temp3],   4(%[sta])                    \n\t"
        "sw      %[temp5],   8(%[sta])                    \n\t"
        "sll     %[temp1],   %[temp1],      16            \n\t"
        "mult    $ac0,       %[temp1],      %[imag]       \n\t"
        "sw      %[temp7],   12(%[sta])                   \n\t"
        "addu    %[temp10],  %[temp10],     %[temp9]      \n\t"

        "addu    %[temp12],  %[temp12],     %[temp11]     \n\t"
        "addu    %[temp4],   %[temp4],      %[temp2]      \n\t"
        "mfhi    %[temp1],   $ac0                         \n\t"
        "addu    %[temp8],   %[temp8],      %[temp6]      \n\t"

        "addu    %[p_flt],   %[p_flt],      %[p_str_QMF]  \n\t"
        "subu    %[p_fltm],  %[p_fltm],     %[p_str_QMF]  \n\t"

        "sw      %[temp10],  16(%[sta])                   \n\t"
        "sw      %[temp12],  20(%[sta])                   \n\t"
        "sw      %[temp4],   24(%[sta])                   \n\t"
        "sw      %[temp8],   28(%[sta])                   \n\t"
        "sw      %[temp1],   32(%[sta])                   \n\t"

        "addiu   %[sta],     %[sta],        36            \n\t"

        : [temp1] "=&r" (temp1), [temp2] "=&r" (temp2), [temp3] "=&r" (temp3),
          [temp4] "=&r" (temp4), [temp5] "=&r" (temp5), [temp6] "=&r" (temp6),
          [temp7] "=&r" (temp7), [temp8] "=&r" (temp8), [temp9] "=&r" (temp9),
          [temp10] "=&r" (temp10), [temp11] "=&r" (temp11), [temp12] "=&r" (temp12),
          [sta] "+r" (sta), [p_flt] "+r" (p_flt), [p_fltm] "+r" (p_fltm)
        : [imag] "r" (imag), [real] "r" (real),  [p_str_QMF] "r" (p_str_QMF)
        : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo",
          "$ac3hi", "$ac3lo", "memory"
    );
  }
}
#endif /* #if defined(FUNCTION_qmfSynPrototypeFirSlot) */


#if defined FUNCTION_qmfAnaPrototypeFirSlot
/*!
  \brief Perform Analysis Prototype Filtering on a single slot of input data.
*/
static
void qmfAnaPrototypeFirSlot( FIXP_QMF *analysisBuffer,
                             int       no_channels,             /*!< Number channels of analysis filter */
                             const FIXP_PFT *p_filter,
                             int       p_stride,                /*!< Stide of analysis filter    */
                             FIXP_QAS *RESTRICT pFilterStates
                            )
{
    int k;

    FIXP_DBL accu_0, accu_1;
    const FIXP_PFT *RESTRICT p_flt = p_filter;
    FIXP_QMF *RESTRICT pData_0 = analysisBuffer + 2*no_channels - 1;
    FIXP_QMF *RESTRICT pData_1 = analysisBuffer;

    FIXP_QAS *RESTRICT sta_0 = (FIXP_QAS *)pFilterStates;
    FIXP_QAS *RESTRICT sta_1 = (FIXP_QAS *)pFilterStates + (2*QMF_NO_POLY*no_channels) - 1;
    int pfltStep = (QMF_NO_POLY * (p_stride)) << 1;
    int staStep0 = no_channels<<1;
    int staStep1 = no_channels<<2;
    int staStep2 = ((no_channels<<3) - 1) << 1; /* Rewind one less */
    int temp_0, temp_1, temp_2, temp_3;

    __asm__ volatile (
        "lh      %[temp_0],    0(%[p_flt])                   \n\t"
        "lh      %[temp_1],    0(%[sta_1])                   \n\t"
        "subu    %[sta_1],     %[sta_1],      %[staStep1]    \n\t"
        "lh      %[temp_2],    2(%[p_flt])                   \n\t"
        "lh      %[temp_3],    0(%[sta_1])                   \n\t"
        "subu    %[sta_1],     %[sta_1],      %[staStep1]    \n\t"
        "mult    %[temp_0],    %[temp_1]                     \n\t"
        "madd    %[temp_2],    %[temp_3]                     \n\t"

        "lh      %[temp_0],    4(%[p_flt])                   \n\t"
        "lh      %[temp_1],    0(%[sta_1])                   \n\t"
        "subu    %[sta_1],     %[sta_1],      %[staStep1]    \n\t"
        "lh      %[temp_2],    6(%[p_flt])                   \n\t"
        "lh      %[temp_3],    0(%[sta_1])                   \n\t"
        "subu    %[sta_1],     %[sta_1],      %[staStep1]    \n\t"
        "madd    %[temp_0],    %[temp_1]                     \n\t"
        "lh      %[temp_0],    8(%[p_flt])                   \n\t"
        "lh      %[temp_1],    0(%[sta_1])                   \n\t"
        "addu    %[sta_1],     %[sta_1],      %[staStep2]    \n\t"
        "madd    %[temp_2],    %[temp_3]                     \n\t"
        "madd    %[temp_0],    %[temp_1]                     \n\t"
        "mflo    %[accu_1]                                   \n\t"

        "addu    %[p_flt],     %[p_flt],      %[pfltStep]    \n\t"
        "lh      %[temp_1],    0(%[sta_0])                   \n\t"
        "lh      %[temp_2],    0(%[sta_1])                   \n\t"
        "lh      %[temp_0],    0(%[p_flt])                   \n\t"
        "subu    %[sta_1],     %[sta_1],      %[staStep1]    \n\t"
        "addu    %[sta_0],     %[sta_0],      %[staStep1]    \n\t"

        : [p_flt] "+r" (p_flt), [accu_1] "=&r" (accu_1),
          [sta_1] "+r" (sta_1), [sta_0] "+r" (sta_0),
          [temp_0] "=&r" (temp_0), [temp_1] "=&r" (temp_1),
          [temp_2] "=&r" (temp_2), [temp_3] "=&r" (temp_3)
        : [pfltStep] "r" (pfltStep), [staStep1] "r" (staStep1), [staStep2] "r" (staStep2)
        : "hi", "lo", "memory"
    );

    *pData_1++ = FX_DBL2FX_QMF(accu_1<<1);

    /* FIR filters 1..63 127..65 */
    for (k=0; k<no_channels-1; k++)
    {
      __asm__ volatile (
          ".set    push                                        \n\t"
          ".set    noreorder                                   \n\t"

          "mult    %[temp_0],    %[temp_1]                     \n\t"
          "mult    $ac1,         %[temp_0],     %[temp_2]      \n\t"

          "lh      %[temp_0],    2(%[p_flt])                   \n\t"
          "lh      %[temp_1],    0(%[sta_0])                   \n\t"
          "lh      %[temp_2],    0(%[sta_1])                   \n\t"
          "subu    %[sta_1],     %[sta_1],      %[staStep1]    \n\t"
          "addu    %[sta_0],     %[sta_0],      %[staStep1]    \n\t"
          "madd    %[temp_0],    %[temp_1]                     \n\t"
          "madd    $ac1,         %[temp_0],     %[temp_2]      \n\t"

          "lh      %[temp_0],    4(%[p_flt])                   \n\t"
          "lh      %[temp_1],    0(%[sta_0])                   \n\t"
          "lh      %[temp_2],    0(%[sta_1])                   \n\t"
          "subu    %[sta_1],     %[sta_1],      %[staStep1]    \n\t"
          "addu    %[sta_0],     %[sta_0],      %[staStep1]    \n\t"
          "madd    %[temp_0],    %[temp_1]                     \n\t"
          "madd    $ac1,         %[temp_0],     %[temp_2]      \n\t"

          "lh      %[temp_0],    6(%[p_flt])                   \n\t"
          "lh      %[temp_1],    0(%[sta_0])                   \n\t"
          "lh      %[temp_2],    0(%[sta_1])                   \n\t"
          "subu    %[sta_1],     %[sta_1],      %[staStep1]    \n\t"
          "addu    %[sta_0],     %[sta_0],      %[staStep1]    \n\t"
          "madd    %[temp_0],    %[temp_1]                     \n\t"
          "madd    $ac1,         %[temp_0],     %[temp_2]      \n\t"

          "lh      %[temp_0],    8(%[p_flt])                   \n\t"
          "lh      %[temp_1],    0(%[sta_0])                   \n\t"
          "lh      %[temp_2],    0(%[sta_1])                   \n\t"

          "madd    %[temp_0],    %[temp_1]                     \n\t"
          "madd    $ac1,         %[temp_0],     %[temp_2]      \n\t"

          "addu    %[p_flt],     %[p_flt],      %[pfltStep]    \n\t"
          "addu    %[sta_1],     %[sta_1],      %[staStep2]    \n\t"
          "subu    %[sta_0],     %[sta_0],      %[staStep2]    \n\t"

          "mflo    %[accu_0]                                   \n\t"
          "mflo    %[accu_1],    $ac1                          \n\t"

          "lh      %[temp_1],    0(%[sta_0])                   \n\t"
          "lh      %[temp_2],    0(%[sta_1])                   \n\t"
          "lh      %[temp_0],    0(%[p_flt])                   \n\t"
          "subu    %[sta_1],     %[sta_1],      %[staStep1]    \n\t"
          "addu    %[sta_0],     %[sta_0],      %[staStep1]    \n\t"
          ".set    pop                                         \n\t"

          : [p_flt] "+r" (p_flt),
            [sta_1] "+r" (sta_1), [sta_0] "+r" (sta_0),
            [accu_0] "=r" (accu_0), [accu_1] "=r" (accu_1),
            [temp_0] "+r" (temp_0), [temp_1] "+r" (temp_1), [temp_2] "+r" (temp_2)
          : [pfltStep] "r" (pfltStep), [staStep1] "r" (staStep1), [staStep2] "r" (staStep2)
          : "hi", "lo", "memory", "$ac1hi", "$ac1lo"
      );

      *pData_0-- = FX_DBL2FX_QMF(accu_0<<1);
      *pData_1++ = FX_DBL2FX_QMF(accu_1<<1);
    }

    sta_0 -= staStep0;
    sta_1 += staStep0;

    /* FIR filter 64 */
    __asm__ volatile (
        "lh      %[temp_0],    0(%[p_flt])                   \n\t"
        "lh      %[temp_1],    0(%[sta_0])                   \n\t"
        "addu    %[sta_0],     %[sta_0],      %[staStep1]    \n\t"
        "lh      %[temp_2],    2(%[p_flt])                   \n\t"
        "lh      %[temp_3],    0(%[sta_0])                   \n\t"
        "addu    %[sta_0],     %[sta_0],      %[staStep1]    \n\t"
        "mult    %[temp_0],    %[temp_1]                     \n\t"
        "lh      %[temp_0],    4(%[p_flt])                   \n\t"
        "lh      %[temp_1],    0(%[sta_0])                   \n\t"
        "addu    %[sta_0],     %[sta_0],      %[staStep1]    \n\t"
        "madd    %[temp_2],    %[temp_3]                     \n\t"
        "madd    %[temp_0],    %[temp_1]                     \n\t"
        "lh      %[temp_2],    6(%[p_flt])                   \n\t"
        "lh      %[temp_3],    0(%[sta_0])                   \n\t"
        "addu    %[sta_0],     %[sta_0],      %[staStep1]    \n\t"
        "lh      %[temp_0],    8(%[p_flt])                   \n\t"
        "lh      %[temp_1],    0(%[sta_0])                   \n\t"
        "subu    %[sta_0],     %[sta_0],      %[staStep2]    \n\t"
        "madd    %[temp_2],    %[temp_3]                     \n\t"
        "madd    %[temp_0],    %[temp_1]                     \n\t"
        "mflo    %[accu_0]                                   \n\t"

        : [p_flt] "+r" (p_flt), [sta_0] "+r" (sta_0), [accu_0] "=&r" (accu_0),
          [temp_0] "=&r" (temp_0), [temp_1] "=&r" (temp_1),
          [temp_2] "=&r" (temp_2), [temp_3] "=&r" (temp_3)
        : [staStep1] "r" (staStep1), [staStep2] "r" (staStep2)
        : "hi", "lo", "memory"
    );
    *pData_0-- = FX_DBL2FX_QMF(accu_0<<1);

}
#endif /* !defined(FUNCTION_qmfAnaPrototypeFirSlot) */


