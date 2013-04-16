
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

/*****************************  MPEG-4 AAC Decoder  **************************

   Author(s):
   Description: independent channel concealment

******************************************************************************/

/*!
  \page concealment AAC core concealment

  This AAC core implementation includes a concealment function, which can be enabled
  using the several defines during compilation.

  There are various tests inside the core, starting with simple CRC tests and ending in
  a variety of plausibility checks. If such a check indicates an invalid bitstream, then
  concealment is applied.

  Concealment is also applied when the calling main program indicates a distorted or missing
  data frame using the frameOK flag. This is used for error detection on the transport layer.
  (See below)

  There are three concealment-modes:

  1) Muting: The spectral data is simply set to zero in case of an detected error.

  2) Noise substitution: In case of an detected error, concealment copies the last frame and adds
     attenuates the spectral data. For this mode you have to set the #CONCEAL_NOISE define.
     Noise substitution adds no additional delay.

  3) Interpolation: The interpolation routine swaps the spectral data from the previous and the
     current frame just before the final frequency to time conversion. In case a single frame is
     corrupted, concealmant interpolates between the last good and the first good frame to create
     the spectral data for the missing frame. If multiple frames are corrupted, concealment
     implements first a fade out based on slightly modified spectral values from the last good
     frame. As soon as good frames are available, concealmant fades in the new spectral data.
     For this mode you have to set the #CONCEAL_INTER define. Note that in this case, you also
     need to set #SBR_BS_DELAY_ENABLE, which basically adds approriate delay in the SBR decoder.
     Note that the Interpolating-Concealment increases the delay of your decoder by one frame
     and that it does require additional resources such as memory and computational complexity.

  <h2>How concealment can be used with errors on the transport layer</h2>

  Many errors can or have to be detected on the transport layer. For example in IP based systems
  packet loss can occur. The transport protocol used should indicate such packet loss by inserting
  an empty frame with frameOK=0.
*/


#define FUNCTION_CConcealment_Store

#if defined (FUNCTION_CConcealment_Store)
/*!
  \brief Store data for concealment techniques applied later

  Interface function to store data for different concealment strategies

   \return  none
 */
void
  CConcealment_Store (
    CConcealmentInfo *hConcealmentInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo )
{
  if ( !(pAacDecoderChannelInfo->renderMode == AACDEC_RENDER_LPD
      ) )
  {
    FIXP_DBL *pSpectralCoefficient =  SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient);
    SHORT    *pSpecScale           =  pAacDecoderChannelInfo->specScale;
    CIcsInfo *pIcsInfo             = &pAacDecoderChannelInfo->icsInfo;

    int  tSpecScale[4];
    UCHAR  tWindowShape, tWindowSequence;
    short *tmp_pok;
    FIXP_DBL tSpec, tSpec1, tSpec2, tSpec3, tSpec4, tSpec5, tSpec6, tSpec7;

    /* store old window infos for swapping */
    tWindowSequence = hConcealmentInfo->windowSequence;
    tWindowShape    = hConcealmentInfo->windowShape;

    /* store old scale factors for swapping */
    tmp_pok = &hConcealmentInfo->specScale[0];
    __asm__ volatile (
      "lw    %[tSpec],  0(%[tmp_pok])           \n\t"
      "lw    %[tSpec1], 4(%[tmp_pok])           \n\t"
      "lw    %[tSpec2], 8(%[tmp_pok])           \n\t"
      "lw    %[tSpec3], 12(%[tmp_pok])          \n\t"

      "sw    %[tSpec],  0(%[tSpecScale])        \n\t"
      "sw    %[tSpec1], 4(%[tSpecScale])        \n\t"
      "sw    %[tSpec2], 8(%[tSpecScale])        \n\t"
      "sw    %[tSpec3], 12(%[tSpecScale])       \n\t"

      : [tSpec] "=&r" (tSpec), [tSpec1] "=&r" (tSpec1),
        [tSpec2] "=&r" (tSpec2), [tSpec3] "=&r" (tSpec3)
      : [tSpecScale] "r" (tSpecScale), [tmp_pok] "r" (tmp_pok)
      : "memory"
    );

    /* store new window infos */
    hConcealmentInfo->windowSequence = GetWindowSequence(pIcsInfo);
    hConcealmentInfo->windowShape    = GetWindowShape(pIcsInfo);
    hConcealmentInfo->lastWinGrpLen  = *(GetWindowGroupLengthTable(pIcsInfo)+GetWindowGroups(pIcsInfo)-1);

    /* store new scale factors */
    __asm__ volatile (
      "lw    %[tSpec],  0(%[pSpecScale])     \n\t"
      "lw    %[tSpec1], 4(%[pSpecScale])     \n\t"
      "lw    %[tSpec2], 8(%[pSpecScale])     \n\t"
      "lw    %[tSpec3], 12(%[pSpecScale])    \n\t"

      "sw    %[tSpec],  0(%[tmp_pok])        \n\t"
      "sw    %[tSpec1], 4(%[tmp_pok])        \n\t"
      "sw    %[tSpec2], 8(%[tmp_pok])        \n\t"
      "sw    %[tSpec3], 12(%[tmp_pok])       \n\t"

      : [tSpec] "=&r" (tSpec), [tSpec1] "=&r" (tSpec1),
        [tSpec2] "=&r" (tSpec2), [tSpec3] "=&r" (tSpec3)
      : [pSpecScale] "r" (pSpecScale), [tmp_pok] "r" (tmp_pok)
      : "memory"
    );

    if (CConcealment_GetDelay(hConcealmentInfo->pConcealParams) == 0)
    {
      /* store new spectral bins */
#if (CNCL_FRACT_BITS == DFRACT_BITS)
      FIXP_DBL *tmp_pok1 = &hConcealmentInfo->spectralCoefficient[0];
      FIXP_DBL *tmp_pok2 = &pSpectralCoefficient[0];
      for (int i=0; i<1024; i+=8)
      {

        __asm__ volatile (
          "lw    %[tSpec],    0(%[tmp_pok2])        \n\t"
          "lw    %[tSpec1],   4(%[tmp_pok2])        \n\t"
          "lw    %[tSpec2],   8(%[tmp_pok2])        \n\t"
          "lw    %[tSpec3],   12(%[tmp_pok2])       \n\t"

          "sw    %[tSpec],    0(%[tmp_pok1])        \n\t"
          "sw    %[tSpec1],   4(%[tmp_pok1])        \n\t"
          "sw    %[tSpec2],   8(%[tmp_pok1])        \n\t"
          "sw    %[tSpec3],   12(%[tmp_pok1])       \n\t"

          "lw    %[tSpec],    16(%[tmp_pok2])       \n\t"
          "lw    %[tSpec1],   20(%[tmp_pok2])       \n\t"
          "lw    %[tSpec2],   24(%[tmp_pok2])       \n\t"
          "lw    %[tSpec3],   28(%[tmp_pok2])       \n\t"

          "sw    %[tSpec],    16(%[tmp_pok1])       \n\t"
          "sw    %[tSpec1],   20(%[tmp_pok1])       \n\t"
          "sw    %[tSpec2],   24(%[tmp_pok1])       \n\t"
          "sw    %[tSpec3],   28(%[tmp_pok1])       \n\t"

          "addiu %[tmp_pok1], %[tmp_pok1],    32    \n\t"
          "addiu %[tmp_pok2], %[tmp_pok2],    32    \n\t"

          : [tSpec] "=&r" (tSpec), [tSpec1] "=&r" (tSpec1),
            [tSpec2] "=&r" (tSpec2), [tSpec3] "=&r" (tSpec3),
            [tmp_pok1] "+r" (tmp_pok1), [tmp_pok2] "+r" (tmp_pok2)
          :
          : "memory"
        );
      }
#else
      FIXP_CNCL *RESTRICT pCncl = &hConcealmentInfo->spectralCoefficient[1024-1];
      FIXP_DBL  *RESTRICT pSpec = &pSpectralCoefficient[1024-1];
      int i;

      for (i = 1024; i != 0; i--) {
        *pCncl-- = FX_DBL2FX_CNCL(*pSpec--);
      }
#endif
    }
    else
    {
      FIXP_CNCL *RESTRICT pCncl = &hConcealmentInfo->spectralCoefficient[1024-1];
      FIXP_DBL  *RESTRICT pSpec = &pSpectralCoefficient[1024-1];
      int i;

      /* swap spectral data */
      for (i = 1024; i != 0; i-=8) {

        __asm__ volatile (
          "lw    %[tSpec],  0(%[pSpec])          \n\t"
          "lw    %[tSpec1], -4(%[pSpec])         \n\t"
          "lw    %[tSpec2], -8(%[pSpec])         \n\t"
          "lw    %[tSpec3], -12(%[pSpec])        \n\t"

          "lw    %[tSpec4],  0(%[pCncl])         \n\t"
          "lw    %[tSpec5], -4(%[pCncl])         \n\t"
          "lw    %[tSpec6], -8(%[pCncl])         \n\t"
          "lw    %[tSpec7], -12(%[pCncl])        \n\t"

          "sw    %[tSpec],  0(%[pCncl])          \n\t"
          "sw    %[tSpec1], -4(%[pCncl])         \n\t"
          "sw    %[tSpec2], -8(%[pCncl])         \n\t"
          "sw    %[tSpec3], -12(%[pCncl])        \n\t"

          "sw    %[tSpec4], 0(%[pSpec])          \n\t"
          "sw    %[tSpec5], -4(%[pSpec])         \n\t"
          "sw    %[tSpec6], -8(%[pSpec])         \n\t"
          "sw    %[tSpec7], -12(%[pSpec])        \n\t"

          "lw    %[tSpec],  -16(%[pSpec])        \n\t"
          "lw    %[tSpec1], -20(%[pSpec])        \n\t"
          "lw    %[tSpec2], -24(%[pSpec])        \n\t"
          "lw    %[tSpec3], -28(%[pSpec])        \n\t"

          "lw    %[tSpec4], -16(%[pCncl])        \n\t"
          "lw    %[tSpec5], -20(%[pCncl])        \n\t"
          "lw    %[tSpec6], -24(%[pCncl])        \n\t"
          "lw    %[tSpec7], -28(%[pCncl])        \n\t"

          "sw    %[tSpec],  -16(%[pCncl])        \n\t"
          "sw    %[tSpec1], -20(%[pCncl])        \n\t"
          "sw    %[tSpec2], -24(%[pCncl])        \n\t"
          "sw    %[tSpec3], -28(%[pCncl])        \n\t"

          "sw    %[tSpec4], -16(%[pSpec])        \n\t"
          "sw    %[tSpec5], -20(%[pSpec])        \n\t"
          "sw    %[tSpec6], -24(%[pSpec])        \n\t"
          "sw    %[tSpec7], -28(%[pSpec])        \n\t"

          "subu  %[pSpec],  %[pSpec],       32   \n\t"
          "subu  %[pCncl],  %[pCncl],       32   \n\t"

          : [tSpec] "=&r" (tSpec), [tSpec1] "=&r" (tSpec1), [tSpec2] "=&r" (tSpec2),
            [tSpec3] "=&r" (tSpec3), [tSpec4] "=&r" (tSpec4), [tSpec5] "=&r" (tSpec5),
            [tSpec6] "=&r" (tSpec6), [tSpec7] "=&r" (tSpec7), [pSpec] "+r" (pSpec),
            [pCncl] "+r" (pCncl)
          :
          : "memory"
        );
      }

      /* complete swapping of window infos */
      pIcsInfo->WindowSequence = tWindowSequence;
      pIcsInfo->WindowShape    = tWindowShape;

      /* complete swapping of scale factors */
      __asm__ volatile (
        "lw    %[tSpec],  0(%[tSpecScale])   \n\t"
        "lw    %[tSpec1], 4(%[tSpecScale])   \n\t"
        "lw    %[tSpec2], 8(%[tSpecScale])   \n\t"
        "lw    %[tSpec3], 12(%[tSpecScale])  \n\t"

        "sw    %[tSpec],  0(%[pSpecScale])   \n\t"
        "sw    %[tSpec1], 4(%[pSpecScale])   \n\t"
        "sw    %[tSpec2], 8(%[pSpecScale])   \n\t"
        "sw    %[tSpec3], 12(%[pSpecScale])  \n\t"

        : [tSpec] "=&r" (tSpec), [tSpec1] "=&r" (tSpec1),
          [tSpec2] "=&r" (tSpec2), [tSpec3] "=&r" (tSpec3)
        : [pSpecScale] "r" (pSpecScale), [tSpecScale] "r" (tSpecScale)
        : "memory"
      );
    }
  }
}
#endif /* #if defined (FUNCTION_CConcealment_Store) */
