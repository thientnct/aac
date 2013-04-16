
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
   Description: MDCT routines

******************************************************************************/

#if defined(MIPS_DSP_R1_LE)
#define FUNCTION_imdct_block
#endif /* #if defined(MIPS_DSP_R1_LE) */


#ifdef FUNCTION_imdct_block

INT  imdct_block(
        H_MDCT hMdct,
        FIXP_DBL *output,
        FIXP_DBL *spectrum,
        const SHORT scalefactor[],
        const INT nSpec,
        const INT noOutSamples,
        const INT tl,
        const FIXP_WTP *wls,
        INT fl,
        const FIXP_WTP *wrs,
        const INT fr,
        FIXP_DBL gain
        )
{
  FIXP_DBL *pOvl;
  FIXP_DBL *pOut0 = output, *pOut1;
  INT nl, nr;
  int w, i, nrSamples = 0, specShiftScale, transform_gain_e = 0;

  /* Derive NR and NL */
  nr = (tl - fr)>>1;
  nl = (tl - fl)>>1;

  /* Include 2/N IMDCT gain into gain factor and exponent. */
  imdct_gain(&gain, &transform_gain_e, tl);

  /* Detect FRprevious / FL mismatches and override parameters accordingly */
  if (hMdct->prev_fr != fl) {
    imdct_adapt_parameters(hMdct, &fl, &nl, tl, wls, noOutSamples);
  }

  pOvl = hMdct->overlap.freq + hMdct->ov_size - 1;

  if ( noOutSamples > nrSamples ) {
    /* Purge buffered output. */
    for (i=0; i<hMdct->ov_offset; i++) {
      *pOut0 = hMdct->overlap.time[i];
      pOut0 ++;
    }
    nrSamples = hMdct->ov_offset;
    hMdct->ov_offset = 0;
  }

  for (w=0; w<nSpec; w++)
  {
    FIXP_DBL *pSpec, *pCurr;
    const FIXP_WTP *pWindow;

    specShiftScale = transform_gain_e;

    /* Setup window pointers */
    pWindow = hMdct->prev_wrs;

    /* Current spectrum */
    pSpec = spectrum+w*tl;

    /* DCT IV of current spectrum. */
    dct_IV(pSpec, tl, &specShiftScale);

    /* Optional scaling of time domain - no yet windowed - of current spectrum */
    /* and de-scale current spectrum signal (time domain, no yet windowed) */
    if (gain != (FIXP_DBL)0) {
      scaleValuesWithFactor(pSpec, gain, tl, scalefactor[w] + specShiftScale);
    } else {
      scaleValues(pSpec, tl, scalefactor[w] + specShiftScale);
    }

    if ( noOutSamples <= nrSamples ) {
      /* Divert output first half to overlap buffer if we already got enough output samples. */
      pOut0 = hMdct->overlap.time + hMdct->ov_offset;
      hMdct->ov_offset += hMdct->prev_nr + fl/2;
    } else {
      /* Account output samples */
      nrSamples += hMdct->prev_nr + fl/2;
    }

    /* NR output samples 0 .. NR. -overlap[TL/2..TL/2-NR] */
    for (i=0; i<hMdct->prev_nr; i++) {
      FIXP_DBL x = - (*pOvl--);
      *pOut0 = IMDCT_SCALE_DBL(x);
      pOut0 ++;
    }

    if ( noOutSamples <= nrSamples ) {
      /* Divert output second half to overlap buffer if we already got enough output samples. */
      pOut1 = hMdct->overlap.time + hMdct->ov_offset + fl/2 - 1;
      hMdct->ov_offset += fl/2 + nl;
    } else {
      pOut1 = pOut0 + (fl - 1);
      nrSamples += fl/2 + nl;
    }

    /* output samples before window crossing point NR .. TL/2. -overlap[TL/2-NR..TL/2-NR-FL/2] + current[NR..TL/2] */
    /* output samples after window crossing point TL/2 .. TL/2+FL/2. -overlap[0..FL/2] - current[TL/2..FL/2] */
    pCurr = pSpec + tl - fl/2;
    {
        FIXP_WTP *tmp_pWin = (FIXP_WTP*)(&pWindow[0]);
        FIXP_WTP *tmp_pWin_end = (FIXP_WTP*)(&pWindow[fl/2-1]);
        FIXP_DBL x0, x1;
        FIXP_DBL tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;
        __asm__ volatile(
            ".set noreorder                                     \n\t"
            ".set push                                          \n\t"
            "lw        %[tmp1],         0*4(%[pCurr])           \n\t"
            "lw        %[tmp2],         0*4(%[pOvl])            \n\t"
            "lw        %[tmp3],         0(%[tmp_pWin])          \n\t"
            "lw        %[tmp4],         4(%[tmp_pWin])          \n\t"

          "1:                                                   \n\t"
            "mult      $ac0,            %[tmp1],        %[tmp3] \n\t"
            "madd      $ac0,            %[tmp2],        %[tmp4] \n\t"
            "mult      $ac1,            %[tmp1],        %[tmp4] \n\t"
            "msub      $ac1,            %[tmp2],        %[tmp3] \n\t"
            "addiu     %[pCurr],        %[pCurr],       1*4     \n\t"
            "addiu     %[pOvl],         %[pOvl],        -1*4    \n\t"
            "addiu     %[tmp_pWin],     %[tmp_pWin],    8       \n\t"
            "mfhi      %[tmp5],         $ac0                    \n\t"
            "mfhi      %[tmp6],         $ac1                    \n\t"
            "addiu     %[pOut1],        %[pOut1],       -1*4    \n\t"
            "addiu     %[pOut0],        %[pOut0],       1*4     \n\t"
            "lw        %[tmp1],         0*4(%[pCurr])           \n\t"
            "lw        %[tmp2],         0*4(%[pOvl])            \n\t"
            "lw        %[tmp3],         0(%[tmp_pWin])          \n\t"
            "lw        %[tmp4],         4(%[tmp_pWin])          \n\t"
            "sll       %[tmp5],         %[tmp5],        1       \n\t"
            "sll       %[tmp6],         %[tmp6],        1       \n\t"
            "subu      %[tmp5],         $zero,          %[tmp5] \n\t"
            "sw        %[tmp6],         -1*4(%[pOut0])          \n\t"
            "bne       %[tmp_pWin_end], %[tmp_pWin],    1b      \n\t"
            " sw       %[tmp5],         1*4(%[pOut1])           \n\t"
            //last loop iteration
            "mult      $ac0,            %[tmp1],        %[tmp3] \n\t"
            "madd      $ac0,            %[tmp2],        %[tmp4] \n\t"
            "mult      $ac1,            %[tmp1],        %[tmp4] \n\t"
            "msub      $ac1,            %[tmp2],        %[tmp3] \n\t"
            "mfhi      %[tmp5],         $ac0                    \n\t"
            "mfhi      %[tmp6],         $ac1                    \n\t"
            "addiu     %[pOut0],        %[pOut0],       1*4     \n\t"
            "addiu     %[pOut1],        %[pOut1],       -1*4    \n\t"
            "sll       %[tmp5],         %[tmp5],        1       \n\t"
            "subu      %[tmp5],         $zero,          %[tmp5] \n\t"
            "sll       %[tmp6],         %[tmp6],        1       \n\t"
            "sw        %[tmp6],         -1*4(%[pOut0])          \n\t"
            "sw        %[tmp5],         1*4(%[pOut1])           \n\t"
            ".set pop                                           \n\t"

            : [tmp_pWin] "+r" (tmp_pWin),
              [pCurr] "+r" (pCurr), [pOvl] "+r" (pOvl),
              [pOut0] "+r" (pOut0), [pOut1] "+r" (pOut1),
              [tmp3] "=&r" (tmp3), [tmp4] "=&r" (tmp4),
              [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2),
              [tmp5] "=&r" (tmp5), [tmp6] "=&r" (tmp6)
            : [tmp_pWin_end] "r" (tmp_pWin_end)
            : "hi", "lo", "$ac1hi", "$ac1lo", "memory"
        );
    }
    pOut0 += (fl/2);

    /* NL output samples TL/2+FL/2..TL. - current[FL/2..0] */
    pOut1 += (fl/2) + 1;
    pCurr = pSpec + tl - fl/2 - 1;
    for (i=0; i<nl; i++) {
      FIXP_DBL x = - (*pCurr--);
      *pOut1 = IMDCT_SCALE_DBL(x);
      pOut1 ++;
    }

    /* Set overlap source pointer for next window pOvl = pSpec + tl/2 - 1; */
    pOvl = pSpec + tl/2 - 1;

    /* Previous window values. */
    hMdct->prev_nr = nr;
    hMdct->prev_fr = fr;
    hMdct->prev_tl = tl;
    hMdct->prev_wrs = wrs;
  }

  /* Save overlap */

  pOvl = hMdct->overlap.freq + hMdct->ov_size - tl/2;
  FDK_ASSERT(pOvl >= hMdct->overlap.time + hMdct->ov_offset);
  FDK_ASSERT(tl/2 <= hMdct->ov_size);

  FIXP_DBL tmp_nc = (nSpec-1)*tl;
  FIXP_DBL *tmp_pOvl = &pOvl[0];
  FIXP_DBL *tmp_spectrum = &spectrum[tmp_nc+0];

  for (i=0; i<tl/2; i+=4) {
    FIXP_DBL tmp1,tmp2,tmp3,tmp4;
    __asm__ volatile(
        "lw        %[tmp1],         0(%[tmp_spectrum])       \n\t"
        "lw        %[tmp2],         4(%[tmp_spectrum])       \n\t"
        "lw        %[tmp3],         8(%[tmp_spectrum])       \n\t"
        "lw        %[tmp4],         12(%[tmp_spectrum])      \n\t"
        "sw        %[tmp1],         0(%[tmp_pOvl])           \n\t"
        "sw        %[tmp2],         4(%[tmp_pOvl])           \n\t"
        "sw        %[tmp3],         8(%[tmp_pOvl])           \n\t"
        "sw        %[tmp4],         12(%[tmp_pOvl])          \n\t"
        "addiu     %[tmp_spectrum], %[tmp_spectrum],     16  \n\t"
        "addiu     %[tmp_pOvl],     %[tmp_pOvl],         16  \n\t"

        : [tmp_pOvl] "+r" (tmp_pOvl), [tmp_spectrum] "+r" (tmp_spectrum),
          [tmp3] "=&r" (tmp3), [tmp4] "=&r" (tmp4),
          [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2)
        :
        : "memory"
    );

  }

  return nrSamples;
}
#endif /* FUNCTION_imdct_block */
