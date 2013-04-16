
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

/***************************************************************************\
*
 *   filename: ldfiltbank.c
 *   project : MPEG-4 Audio Decoder
 *   contents/description: low delay filterbank
 *
\***************************************************************************/

#if defined(MIPS_DSP_R2_LE)
#define FUNCTION_multE2_DinvF_fdk
#endif /* #if defined(MIPS_DSP_R2_LE) */

#if defined(FUNCTION_multE2_DinvF_fdk)

static void multE2_DinvF_fdk(INT_PCM *output, FIXP_DBL* x, const FIXP_WTB* fb, FIXP_DBL* z, const int N, const int  stride)
{
  int i, scale;

  /*  scale for FIXP_DBL -> INT_PCM conversion. */
  scale = (DFRACT_BITS - SAMPLE_BITS) - LDFB_HEADROOM;

  for(i=0;i<N/4;i++)
  {
    FIXP_DBL tmp5 = z[N + i];
    FIXP_DBL tmp6 = fb[2*N + N/2 + i];
    FIXP_DBL tmp2 = z[N/2+i];
    FIXP_DBL tmp3 = fb[2*N + i];
    FIXP_DBL tmp8 = z[i];
    FIXP_DBL tmp9 = fb[N+N/2+i];
    FIXP_DBL tmp1 = x[N/2+i];
    FIXP_DBL tmp4 = x[N/2-1-i];
    FIXP_DBL tmp7 = fb[N+N/2-1-i];

    __asm__ volatile(
      "mult      $ac0,       %[tmp5],      %[tmp6]       \n\t"
      "mult      $ac1,       %[tmp2],      %[tmp3]       \n\t"
      "mult      $ac2,       %[tmp8],      %[tmp9]       \n\t"
      "mfhi      %[tmp5],    $ac0                        \n\t"
      "mfhi      %[tmp2],    $ac1                        \n\t"
      "mfhi      %[tmp8],    $ac2                        \n\t"
      "sra       %[tmp5],    %[tmp5],      1             \n\t"
      "addu      %[tmp5],    %[tmp5],      %[tmp4]       \n\t"
      "mult      $ac3,       %[tmp5],      %[tmp7]       \n\t"
      "mfhi      %[tmp7],    $ac3                        \n\t"
      "sra       %[tmp2],    %[tmp2],      1             \n\t"
      "addu      %[tmp2],    %[tmp2],      %[tmp1]       \n\t"
      "addu      %[tmp8],    %[tmp7],      %[tmp8]       \n\t"

      : [tmp2] "+r" (tmp2), [tmp5] "+r" (tmp5), [tmp7] "+r" (tmp7),
        [tmp8] "+r" (tmp8)
      : [tmp1] "r" (tmp1), [tmp3] "r" (tmp3), [tmp4] "r" (tmp4),
        [tmp6] "r" (tmp6), [tmp9] "r" (tmp9)
      : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo",
        "$ac3hi", "$ac3lo", "memory"
    );

    z[N/2+i] = tmp5;
    z[i    ] = tmp2;
    z[N + i] = tmp1;

    output[(N*3/4-1-i)*stride] = (INT_PCM)SATURATE_RIGHT_SHIFT(tmp8, -WTS1-1 + scale, SAMPLE_BITS);
  }

  for(i=N/4;i<N/2;i++)
  {
    FIXP_DBL z0, z2, tmp0, tmp1;

    FIXP_DBL tmp5 = z[N + i];
    FIXP_DBL tmp6 = fb[2*N + N/2 + i];
    FIXP_DBL tmp2 = z[N/2+i];
    FIXP_DBL tmp3 = fb[2*N + i];
    FIXP_DBL tmp8 = z[i];
    FIXP_DBL tmp10 = fb[N/2+i];
    FIXP_DBL tmp11 = x[N/2+i];
    FIXP_DBL tmp4 = x[N/2-1-i];
    FIXP_DBL tmp7 = fb[N+N/2-1-i];
    FIXP_DBL tmp9 = fb[N+N/2+i];
    FIXP_DBL tmp12 = fb[N/2-1-i];

    __asm__ volatile(
      "mult      $ac0,       %[tmp5],      %[tmp6]       \n\t"
      "mult      $ac1,       %[tmp2],      %[tmp3]       \n\t"
      "mult      $ac2,       %[tmp8],      %[tmp10]      \n\t"
      "mult      $ac3,       %[tmp8],      %[tmp9]       \n\t"
      "mfhi      %[tmp5],    $ac0                        \n\t"
      "mfhi      %[tmp2],    $ac1                        \n\t"
      "mfhi      %[tmp10],   $ac2                        \n\t"
      "mfhi      %[tmp8],    $ac3                        \n\t"
      "sra       %[tmp5],    %[tmp5],      1             \n\t"
      "addu      %[tmp5],    %[tmp5],      %[tmp4]       \n\t"
      "mult      $ac0,       %[tmp5],      %[tmp12]      \n\t"
      "mult      $ac1,       %[tmp5],      %[tmp7]       \n\t"
      "sra       %[tmp2],    %[tmp2],      1             \n\t"
      "addu      %[tmp2],    %[tmp2],      %[tmp11]      \n\t"
      "mfhi      %[tmp9],    $ac0                        \n\t"
      "mfhi      %[tmp12],   $ac1                        \n\t"
      "addu      %[tmp9],    %[tmp9],      %[tmp10]      \n\t"
      "addu      %[tmp12],   %[tmp12],     %[tmp8]       \n\t"

      : [tmp2] "+r" (tmp2), [tmp5] "+r" (tmp5), [tmp9] "+r" (tmp9),
        [tmp10] "+r" (tmp10), [tmp12] "+r" (tmp12)
      : [tmp3] "r" (tmp3), [tmp4] "r" (tmp4), [tmp6] "r" (tmp6),
        [tmp7] "r" (tmp7), [tmp8] "r" (tmp8), [tmp11] "r" (tmp11)
      : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo",
        "$ac3hi", "$ac3lo", "memory"
    );

    z[N/2+i] = tmp5;
    z[i    ] = tmp2;
    z[N + i] = tmp11;
    output[(i-N/4)*stride    ] = (INT_PCM)SATURATE_RIGHT_SHIFT(tmp9, -WTS0-1 + scale, SAMPLE_BITS);
    output[(N*3/4-1-i)*stride] = (INT_PCM)SATURATE_RIGHT_SHIFT(tmp12, -WTS1-1 + scale, SAMPLE_BITS);

  }

  /* Exchange quarter parts of x to bring them in the "right" order */
  FIXP_DBL *pok_z = &z[0];
  FIXP_DBL *pok_fb = (FIXP_DBL *)(&fb[N/2]);
  for(i=0;i<N/4;i+=4)
  {
    FIXP_DBL tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;

    __asm__ volatile(
      "lw     %[tmp0],   0(%[pok_z])                   \n\t"
      "lw     %[tmp1],   0(%[pok_fb])                  \n\t"
      "lw     %[tmp2],   4(%[pok_z])                   \n\t"
      "lw     %[tmp3],   4(%[pok_fb])                  \n\t"
      "lw     %[tmp4],   8(%[pok_z])                   \n\t"
      "lw     %[tmp5],   8(%[pok_fb])                  \n\t"
      "lw     %[tmp6],   12(%[pok_z])                  \n\t"
      "lw     %[tmp7],   12(%[pok_fb])                 \n\t"
      "mult   $ac0,      %[tmp0],      %[tmp1]         \n\t"
      "mult   $ac1,      %[tmp2],      %[tmp3]         \n\t"
      "mult   $ac2,      %[tmp4],      %[tmp5]         \n\t"
      "mult   $ac3,      %[tmp6],      %[tmp7]         \n\t"
      "mfhi   %[tmp0],   $ac0                          \n\t"
      "mfhi   %[tmp1],   $ac1                          \n\t"
      "mfhi   %[tmp2],   $ac2                          \n\t"
      "mfhi   %[tmp3],   $ac3                          \n\t"
      "addu   %[pok_z],  %[pok_z],     16              \n\t"
      "addu   %[pok_fb], %[pok_fb],    16              \n\t"

      : [tmp0] "=&r" (tmp0), [tmp1] "=&r" (tmp1),
        [tmp2] "=&r" (tmp2), [tmp3] "=&r" (tmp3),
        [tmp4] "=&r" (tmp4), [tmp5] "=&r" (tmp5),
        [tmp6] "=&r" (tmp6), [tmp7] "=&r" (tmp7),
        [pok_z] "+r" (pok_z), [pok_fb] "+r" (pok_fb)
      :
      : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo",
        "$ac3hi", "$ac3lo", "memory"
    );

    output[(N*3/4 + i)*stride  ] = (INT_PCM)SATURATE_RIGHT_SHIFT(tmp0, -WTS0-1 + scale, SAMPLE_BITS);
    output[(N*3/4 + i+1)*stride] = (INT_PCM)SATURATE_RIGHT_SHIFT(tmp1, -WTS0-1 + scale, SAMPLE_BITS);
    output[(N*3/4 + i+2)*stride] = (INT_PCM)SATURATE_RIGHT_SHIFT(tmp2, -WTS0-1 + scale, SAMPLE_BITS);
    output[(N*3/4 + i+3)*stride] = (INT_PCM)SATURATE_RIGHT_SHIFT(tmp3, -WTS0-1 + scale, SAMPLE_BITS);
  }
}

#endif /* #if defined(FUNCTION_multE2_DinvF_fdk) */
