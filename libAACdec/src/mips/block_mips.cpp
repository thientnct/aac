
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
   Description: long/short-block decoding

******************************************************************************/


#if defined(MIPS_DSP_R1_LE)
#define FUNCTION_InverseQuantizeBand
#define FUNCTION_CBlock_FrequencyToTime
#endif /* #if defined(MIPS_DSP_R1_LE) */

#if defined(MIPS32_LE)
#define FUNCTION_CBlock_ScaleSpectralData
#endif /* #if defined(MIPS32_LE) */

#if defined (FUNCTION_InverseQuantizeBand)
/* mso: provides a faster way to i-quantize a whole band in one go */

/**
 * \brief inverse quantize one sfb. Each value of the sfb is processed according to the
 *        formula: spectrum[i] = Sign(spectrum[i]) * Matissa(spectrum[i])^(4/3) * 2^(lsb/4).
 * \param spectrum pointer to first line of the sfb to be inverse quantized.
 * \param noLines number of lines belonging to the sfb.
 * \param lsb last 2 bits of the scale factor of the sfb.
 * \param scale max allowed shift scale for the sfb.
 */
void InverseQuantizeBand( FIXP_DBL * RESTRICT spectrum,
                          INT noLines,
                          INT lsb,
                          INT scale )
{
    const FIXP_DBL * RESTRICT InverseQuantTabler=(FIXP_DBL *)InverseQuantTable;
    const FIXP_DBL * RESTRICT MantissaTabler=(FIXP_DBL *)MantissaTable[lsb];
    const SCHAR* RESTRICT ExponentTabler=(SCHAR*)ExponentTable[lsb];

    FIXP_DBL *ptr = spectrum;
    FIXP_DBL signedValue;

    FDK_ASSERT(noLines>2);

    FIXP_DBL value;
    UINT freeBits;

    for (INT i=noLines; i--; )
    {
        if ((signedValue = *ptr++) != FL2FXCONST_DBL(0))
        {

          __asm__ volatile (
            "absq_s.w  %[value],       %[signedValue]                 \n\t"
            "clz       %[freeBits],    %[value]                       \n\t"
            "andi      %[freeBits],    %[freeBits],    0x1f           \n\t"

            : [value] "=&r" (value), [freeBits] "=&r" (freeBits)
            : [signedValue] "r" (signedValue)
            :
          );

          UINT exponent = 32 - freeBits;

          UINT x = (UINT) (LONG)value << (INT) freeBits;
          x <<= 1;                                  /* shift out sign bit to avoid masking later on */
          UINT tableIndex = x >> 24;
          x = (x >> 20) &  0x0F;

          UINT r0=(UINT)(LONG)InverseQuantTabler[tableIndex+0];
          UINT r1=(UINT)(LONG)InverseQuantTabler[tableIndex+1];
          UINT temp= (r1 - r0)*x + (r0 << 4);

          int expTabTemp = ExponentTabler[exponent];

          __asm__ volatile (
            ".set      push                                           \n\t"
            ".set      noreorder                                      \n\t"
            "mult      %[temp],        %[temp1]                       \n\t"
            "addu      %[expTabTemp],  %[expTabTemp],  %[scale]       \n\t"
            "addiu     %[expTabTemp],  %[expTabTemp],  1              \n\t"
            "bgez      %[expTabTemp],  1f                             \n\t"
            " mfhi     %[value]                                       \n\t"
            "negu      %[expTabTemp],  %[expTabTemp]                  \n\t"
            "b         2f                                             \n\t"
            " srav     %[value],       %[value],       %[expTabTemp]  \n\t"
          "1:                                                         \n\t"
            "sllv      %[value],       %[value],       %[expTabTemp]  \n\t"
          "2:                                                         \n\t"
            ".set      pop                                            \n\t"

            : [value] "=r" (value), [expTabTemp] "+r" (expTabTemp)
            : [temp] "r" (temp), [temp1] "r" (MantissaTabler[exponent]), [scale] "r" (scale)
            : "hi", "lo"
          );

          signedValue = (signedValue < (FIXP_DBL)0) ? -value : value;
          ptr[-1] = signedValue;
        }
    }
}
#endif /* #if defined (FUNCTION_InverseQuantizeBand) */


#if defined (FUNCTION_CBlock_FrequencyToTime)

int getWindow2Nr(int length, int shape);

void CBlock_FrequencyToTime(CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
                            CAacDecoderChannelInfo *pAacDecoderChannelInfo,
                            INT_PCM outSamples[],
                            const SHORT frameLen,
                            const int stride,
                            const int frameOk,
                            FIXP_DBL *pWorkBuffer1 )
{
  int fr, fl, tl, nSamples, nSpec;

  /* Determine left slope length (fl), right slope length (fr) and transform length (tl).
     USAC: The slope length may mismatch with the previous frame in case of LPD / FD
           transitions. The adjustment is handled by the imdct implementation.
  */
  tl = frameLen;
  nSpec = 1;

  switch( pAacDecoderChannelInfo->icsInfo.WindowSequence ) {
    default:
    case OnlyLongSequence:
      fl = frameLen;
      fr = frameLen - getWindow2Nr(frameLen, GetWindowShape(&pAacDecoderChannelInfo->icsInfo));
      break;
    case LongStopSequence:
      fl = frameLen >> 3;
      fr = frameLen;
      break;
    case LongStartSequence: /* or StopStartSequence */
      fl = frameLen;
      fr = frameLen >> 3;
      break;
    case EightShortSequence:
      fl = fr = frameLen >> 3;
      tl >>= 3;
      nSpec = 8;
      break;
  }

  {
    int i;

    {
      FIXP_DBL *tmp = pAacDecoderChannelInfo->pComData->workBufferCore1->mdctOutTemp;

      nSamples = imdct_block(
             &pAacDecoderStaticChannelInfo->IMdct,
              tmp,
              SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient),
              pAacDecoderChannelInfo->specScale,
              nSpec,
              frameLen,
              tl,
              FDKgetWindowSlope(fl, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)),
              fl,
              FDKgetWindowSlope(fr, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)),
              fr,
              (FIXP_DBL)0 );

      FIXP_DBL temp_scale = DFRACT_BITS-SAMPLE_BITS-MDCT_OUT_HEADROOM;
      FIXP_DBL temp_S_B = SAMPLE_BITS;
      FIXP_DBL temp1,temp2,temp3,temp4,temp5;
      FIXP_DBL temp6,temp7,temp8;
      FIXP_DBL *tmp_pok = &tmp[0];
      INT_PCM  *tmp_pok1 = &outSamples[0];
      FIXP_DBL tmp_stride = stride*2;

      for (i=0; i<frameLen; i+=4) {

        __asm__ volatile(
          "lw        %[temp1],   0(%[tmp_pok])                   \n\t"
          "lw        %[temp3],   4(%[tmp_pok])                   \n\t"
          "lw        %[temp4],   8(%[tmp_pok])                   \n\t"
          "lw        %[temp5],   12(%[tmp_pok])                  \n\t"
          "subu      %[temp2],   %[temp_S_B],    %[temp_scale]   \n\t"
          "addu      %[temp6],   %[tmp_pok1],    %[tmp_stride]   \n\t"
          "addu      %[temp7],   %[temp6],       %[tmp_stride]   \n\t"
          "addu      %[temp8],   %[temp7],       %[tmp_stride]   \n\t"

          "shllv_s.w %[temp1],   %[temp1],       %[temp2]        \n\t"
          "shllv_s.w %[temp3],   %[temp3],       %[temp2]        \n\t"
          "shllv_s.w %[temp4],   %[temp4],       %[temp2]        \n\t"
          "shllv_s.w %[temp5],   %[temp5],       %[temp2]        \n\t"

          "srl       %[temp1],   %[temp1],       16              \n\t"
          "srl       %[temp3],   %[temp3],       16              \n\t"
          "srl       %[temp4],   %[temp4],       16              \n\t"
          "srl       %[temp5],   %[temp5],       16              \n\t"

          "sh        %[temp1],   0(%[tmp_pok1])                  \n\t"
          "sh        %[temp3],   0(%[temp6])                     \n\t"
          "sh        %[temp4],   0(%[temp7])                     \n\t"
          "sh        %[temp5],   0(%[temp8])                     \n\t"

          "addiu     %[tmp_pok], %[tmp_pok],     16              \n\t"
          "addu      %[tmp_pok1],%[temp8],       %[tmp_stride]   \n\t"

          : [temp1] "=&r" (temp1), [temp2] "=&r" (temp2),
            [temp3] "=&r" (temp3), [temp4] "=&r" (temp4),
            [temp5] "=&r" (temp5), [temp6] "=&r" (temp6),
            [temp7] "=&r" (temp7), [temp8] "=&r" (temp8),
            [tmp_pok] "+r" (tmp_pok), [tmp_pok1] "+r" (tmp_pok1)
          : [temp_scale] "r" (temp_scale), [temp_S_B] "r" (temp_S_B),
            [tmp_stride] "r" (tmp_stride)
          : "memory"
        );
      }
    }
  }

  FDK_ASSERT(nSamples == frameLen);

}
#endif /* #if defined (FUNCTION_CBlock_FrequencyToTime) */


#if defined (FUNCTION_CBlock_ScaleSpectralData)
void CBlock_ScaleSpectralData(CAacDecoderChannelInfo *pAacDecoderChannelInfo, SamplingRateInfo *pSamplingRateInfo)
{
  int band;
  int window;
  const SHORT * RESTRICT pSfbScale  = pAacDecoderChannelInfo->pDynData->aSfbScale;
  SHORT * RESTRICT pSpecScale = pAacDecoderChannelInfo->specScale;
  int groupwin,group;
  const SHORT * RESTRICT BandOffsets = GetScaleFactorBandOffsets(&pAacDecoderChannelInfo->icsInfo, pSamplingRateInfo);
  SPECTRAL_PTR RESTRICT pSpectralCoefficient = pAacDecoderChannelInfo->pSpectralCoefficient;


  FDKmemclear(pSpecScale, 8*sizeof(SHORT));

  int max_band = GetScaleFactorBandsTransmitted(&pAacDecoderChannelInfo->icsInfo);
  if (pAacDecoderChannelInfo->pDynData->TnsData.Active) {
    for (window=0, group=0; group < GetWindowGroups(&pAacDecoderChannelInfo->icsInfo); group++)
    {
      for (groupwin=0; groupwin < GetWindowGroupLength(&pAacDecoderChannelInfo->icsInfo,group); groupwin++, window++)
      {
        int SpecScale_window = pSpecScale[window];
        FIXP_DBL *pSpectrum = SPEC(pSpectralCoefficient, window,  pAacDecoderChannelInfo->granuleLength);
        int tmp_window =window*16;

        /* find scaling for current window */
        for (band=0; band < max_band; band++)
        {
          SpecScale_window = fMax(SpecScale_window, (int)pSfbScale[tmp_window+band]);
        }

        SpecScale_window += TNS_SCALE;

        /* store scaling of current window */
        pSpecScale[window] = SpecScale_window;

        for (band=0; band < max_band; band++)
        {
          int scale = SpecScale_window - pSfbScale[tmp_window+band];
          if (scale)
          {
            /* following relation can be used for optimizations: (BandOffsets[i]%4) == 0 for all i */
            int max_index = BandOffsets[band+1];
            FIXP_DBL *tmp_pSpec = &pSpectrum[BandOffsets[band]];
              for (int index = BandOffsets[band]; index < max_index; index+=4)
              {
                FIXP_DBL temp1, temp2, temp3, temp4;
                __asm__ volatile(
                    "lw        %[temp1],     0(%[tmp_pSpec])           \n\t"
                    "lw        %[temp2],     4(%[tmp_pSpec])           \n\t"
                    "lw        %[temp3],     8(%[tmp_pSpec])           \n\t"
                    "lw        %[temp4],     12(%[tmp_pSpec])          \n\t"

                    "srav      %[temp1],     %[temp1],       %[scale]  \n\t"
                    "srav      %[temp2],     %[temp2],       %[scale]  \n\t"
                    "srav      %[temp3],     %[temp3],       %[scale]  \n\t"
                    "srav      %[temp4],     %[temp4],       %[scale]  \n\t"

                    "sw        %[temp1],     0(%[tmp_pSpec])           \n\t"
                    "sw        %[temp2],     4(%[tmp_pSpec])           \n\t"
                    "sw        %[temp3],     8(%[tmp_pSpec])           \n\t"
                    "sw        %[temp4],     12(%[tmp_pSpec])          \n\t"

                    "addiu     %[tmp_pSpec], %[tmp_pSpec],   16        \n\t"

                    : [temp1] "=&r" (temp1), [temp2] "=&r" (temp2),
                      [temp3] "=&r" (temp3), [temp4] "=&r" (temp4),
                      [tmp_pSpec] "+r" (tmp_pSpec)
                    : [scale] "r" (scale)
                    : "memory"
                );
              }
          }
        }
      }
    }
  }
  else{
    for (window=0, group=0; group < GetWindowGroups(&pAacDecoderChannelInfo->icsInfo); group++)
    {
      for (groupwin=0; groupwin < GetWindowGroupLength(&pAacDecoderChannelInfo->icsInfo,group); groupwin++, window++)
      {
        int SpecScale_window = pSpecScale[window];
        FIXP_DBL *pSpectrum = SPEC(pSpectralCoefficient, window,  pAacDecoderChannelInfo->granuleLength);
        int tmp_window =window*16;

        /* find scaling for current window */
        for (band=0; band < max_band; band++)
        {
          SpecScale_window = fMax(SpecScale_window, (int)pSfbScale[tmp_window+band]);
        }

        /* store scaling of current window */
        pSpecScale[window] = SpecScale_window;

        for (band=0; band < max_band; band++)
        {
          int scale = SpecScale_window - pSfbScale[tmp_window+band];
          if (scale)
          {
            /* following relation can be used for optimizations: (BandOffsets[i]%4) == 0 for all i */
            int max_index = BandOffsets[band+1];
            FIXP_DBL *tmp_pSpec = &pSpectrum[BandOffsets[band]];
              for (int index = BandOffsets[band]; index < max_index; index+=4)
              {
                FIXP_DBL temp1, temp2, temp3, temp4;
                __asm__ volatile(
                    "lw        %[temp1],     0(%[tmp_pSpec])           \n\t"
                    "lw        %[temp2],     4(%[tmp_pSpec])           \n\t"
                    "lw        %[temp3],     8(%[tmp_pSpec])           \n\t"
                    "lw        %[temp4],     12(%[tmp_pSpec])          \n\t"

                    "srav      %[temp1],     %[temp1],       %[scale]  \n\t"
                    "srav      %[temp2],     %[temp2],       %[scale]  \n\t"
                    "srav      %[temp3],     %[temp3],       %[scale]  \n\t"
                    "srav      %[temp4],     %[temp4],       %[scale]  \n\t"

                    "sw        %[temp1],     0(%[tmp_pSpec])           \n\t"
                    "sw        %[temp2],     4(%[tmp_pSpec])           \n\t"
                    "sw        %[temp3],     8(%[tmp_pSpec])           \n\t"
                    "sw        %[temp4],     12(%[tmp_pSpec])          \n\t"

                    "addiu     %[tmp_pSpec], %[tmp_pSpec],   16        \n\t"

                    : [temp1] "=&r" (temp1), [temp2] "=&r" (temp2),
                      [temp3] "=&r" (temp3), [temp4] "=&r" (temp4),
                      [tmp_pSpec] "+r" (tmp_pSpec)
                    : [scale] "r" (scale)
                    : "memory"
                );
              }
          }
        }
      }
    }
  }
}
#endif /* FUNCTION_CBlock_ScaleSpectralData */
