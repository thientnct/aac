
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

/***************************  Fraunhofer IIS FDK Tools  **********************

   Author(s):
   Description:

******************************************************************************/

#ifndef __INCLUDE_SCALE_MIPS__
#define __INCLUDE_SCALE_MIPS__


#if defined(MIPS_DSP_R1_LE)
#ifndef FUNCTION_getScalefactor_DBL
#define FUNCTION_getScalefactor_DBL
#endif /* #if defined(MIPS_DSP_R1_LE) */

#if defined(MIPS_DSP_R2_LE)
#define FUNCTION_scaleValuesWithFactor_DBL
#endif /* #if defined(MIPS_DSP_R2_LE) */


/*!
 *
 *  \brief Calculate max possible scale factor for input vector
 *
 *  \return Maximum scale factor
 *
 *  This function can constitute a significant amount of computational complexity - very much depending on the
 *  bitrate. Since it is a rather small function, effective assembler optimization might be possible.
 *
 */
inline INT getScalefactor(const FIXP_DBL *vector, /*!< Pointer to input vector */
                          INT len)                /*!< Length of input vector */
{
  INT i;
  FIXP_DBL maxVal = FL2FX_DBL(0.0f);

  for(i=len;i!=0;i--){
    maxVal |= __builtin_mips_absq_s_w(*vector++);
  }

  return fixMax((INT)0,(CntLeadingZeros(maxVal) - 1));
}
#endif


#ifdef FUNCTION_scaleValuesWithFactor_DBL
/*!
 *
 *  \brief  Multiply input vector by \f$ 2^{scalefactor} \f$
 *  \param len must be larger than 4
 *  \return void
 *
 */
SCALE_INLINE
void scaleValuesWithFactor(
        FIXP_DBL *vector,
        FIXP_DBL factor,
        INT len,
        INT scalefactor
        )
{
  INT i;
  /* Compensate fMultDiv2 */
  scalefactor++;

  if(scalefactor > 0){
    scalefactor = fixmin_I(scalefactor,(INT)DFRACT_BITS-1);
    for (i = len&3; i--; )
    {
      *vector = fMultDiv2(*vector, factor) << scalefactor;
      vector++;
    }
    for (i = len>>2; i--; )
    {
        FIXP_DBL temp1,temp2,temp3,temp4;
        __asm__ volatile(
            "lw        %[temp1],   0(%[vector])                   \n\t"
            "lw        %[temp2],   4(%[vector])                   \n\t"
            "lw        %[temp3],   8(%[vector])                   \n\t"
            "lw        %[temp4],   12(%[vector])                  \n\t"
            "mult      $ac0,       %[temp1],      %[factor]       \n\t"
            "mult      $ac1,       %[temp2],      %[factor]       \n\t"
            "mult      $ac2,       %[temp3],      %[factor]       \n\t"
            "mult      $ac3,       %[temp4],      %[factor]       \n\t"
            "addiu     %[vector],  %[vector],     16              \n\t"
            "mfhi      %[temp1],   $ac0                           \n\t"
            "mfhi      %[temp2],   $ac1                           \n\t"
            "mfhi      %[temp3],   $ac2                           \n\t"
            "mfhi      %[temp4],   $ac3                           \n\t"
            "sllv      %[temp1],   %[temp1],      %[scalefactor]  \n\t"
            "sllv      %[temp2],   %[temp2],      %[scalefactor]  \n\t"
            "sllv      %[temp3],   %[temp3],      %[scalefactor]  \n\t"
            "sllv      %[temp4],   %[temp4],      %[scalefactor]  \n\t"
            "sw        %[temp1],   -16(%[vector])                 \n\t"
            "sw        %[temp2],   -12(%[vector])                 \n\t"
            "sw        %[temp3],   -8(%[vector])                  \n\t"
            "sw        %[temp4],   -4(%[vector])                  \n\t"

            : [temp1] "=&r" (temp1), [temp2] "=&r" (temp2),
              [temp3] "=&r" (temp3), [temp4] "=&r" (temp4),
              [vector] "+r" (vector)
            : [factor] "r" (factor), [scalefactor] "r" (scalefactor)
            : "hi", "lo", "$ac1hi", "$ac2lo", "$ac2hi", "$ac2lo",
              "$ac3hi", "$ac3lo", "memory"
        );
    }
  } else {
    INT negScalefactor = fixmin_I(-scalefactor,(INT)DFRACT_BITS-1);
    for (i = len&3; i--; )
    {
      *vector = fMultDiv2(*vector, factor) >> negScalefactor;
      vector++;
    }
    for (i = len>>2; i--; )
    {
      FIXP_DBL temp1,temp2,temp3,temp4;
      __asm__ volatile(
          "lw        %[temp1],   0(%[vector])                          \n\t"
          "lw        %[temp2],   4(%[vector])                          \n\t"
          "lw        %[temp3],   8(%[vector])                          \n\t"
          "lw        %[temp4],   12(%[vector])                         \n\t"
          "mult      $ac0,       %[temp1],      %[factor]              \n\t"
          "mult      $ac1,       %[temp2],      %[factor]              \n\t"
          "mult      $ac2,       %[temp3],      %[factor]              \n\t"
          "mult      $ac3,       %[temp4],      %[factor]              \n\t"
          "addiu     %[vector],  %[vector],     16                     \n\t"
          "mfhi      %[temp1],   $ac0                                  \n\t"
          "mfhi      %[temp2],   $ac1                                  \n\t"
          "mfhi      %[temp3],   $ac2                                  \n\t"
          "mfhi      %[temp4],   $ac3                                  \n\t"
          "srav      %[temp1],   %[temp1],      %[negScalefactor]      \n\t"
          "srav      %[temp2],   %[temp2],      %[negScalefactor]      \n\t"
          "srav      %[temp3],   %[temp3],      %[negScalefactor]      \n\t"
          "srav      %[temp4],   %[temp4],      %[negScalefactor]      \n\t"
          "sw        %[temp1],   -16(%[vector])                        \n\t"
          "sw        %[temp2],   -12(%[vector])                        \n\t"
          "sw        %[temp3],   -8(%[vector])                         \n\t"
          "sw        %[temp4],   -4(%[vector])                         \n\t"

          : [temp1] "=&r" (temp1), [temp2] "=&r" (temp2),
            [temp3] "=&r" (temp3), [temp4] "=&r" (temp4),
            [vector] "+r" (vector)
          : [factor] "r" (factor), [negScalefactor] "r" (negScalefactor)
          : "hi", "lo", "$ac1hi", "$ac2lo", "$ac2hi", "$ac2lo",
            "$ac3hi", "$ac3lo", "memory"
      );
    }
  }
}
#endif /* FUNCTION_scaleValuesWithFactor_DBL */

#endif /* #ifndef __INCLUDE_SCALE_MIPS__ */
