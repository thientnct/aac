
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
   Description: Fix point FFT

******************************************************************************/

#ifndef __INCLUDE_FFT_MIPS__
#define __INCLUDE_FFT_MIPS__

#if defined(MIPS_DSP_R2_LE)
#define FUNCTION_fft_32
#endif /* #if defined(MIPS_DSP_R2_LE) */

#ifdef FUNCTION_fft_32

static const FIXP_STP fft32_w32[6] =
{
  STCP (0x7641af3d, 0x30fbc54d), STCP(0x30fbc54d, 0x7641af3d), STCP(0x7d8a5f40, 0x18f8b83c),
  STCP (0x6a6d98a4, 0x471cece7), STCP(0x471cece7, 0x6a6d98a4), STCP(0x18f8b83c, 0x7d8a5f40)
};

LNK_SECTION_CODE_L1
inline void fft_32(FIXP_DBL *x)
{

#define W_PiFOURTH STC(0x5a82799a)

  FIXP_DBL vr,vi,ur,ui;
  FIXP_DBL y[64];

  /*
   * 1+2 stage radix 4
   */

/////////////////////////////////////////////////////////////////////////////////////////

  FIXP_DBL tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8;
  FIXP_DBL res1, res2, res3, res4;
  FIXP_DBL tmp_PiF = W_PiFOURTH;
  __asm__ volatile(
    "lw        %[tmp1],         0(%[x])                   \n\t"
    "lw        %[tmp2],         32*4(%[x])                \n\t"
    "lw        %[tmp3],         16*4(%[x])                \n\t"
    "lw        %[tmp4],         48*4(%[x])                \n\t"
    "lw        %[tmp7],         17*4(%[x])                \n\t"
    "lw        %[tmp8],         49*4(%[x])                \n\t"
    "lw        %[tmp5],         1*4(%[x])                 \n\t"
    "lw        %[tmp6],         33*4(%[x])                \n\t"
    "addqh.w   %[vr],           %[tmp1],      %[tmp2]     \n\t"
    "addu      %[vi],           %[tmp3],      %[tmp4]     \n\t"
    "addu      %[ui],           %[tmp7],      %[tmp8]     \n\t"
    "addqh.w   %[ur],           %[tmp5],      %[tmp6]     \n\t"
    "sra       %[res3],         %[vi],        1           \n\t"
    "sra       %[res4],         %[ui],        1           \n\t"
    "addu      %[res1],         %[vr],        %[res3]     \n\t"
    "subu      %[res2],         %[vr],        %[res3]     \n\t"
    "addu      %[res3],         %[ur],        %[res4]     \n\t"
    "subu      %[res4],         %[ur],        %[res4]     \n\t"
    "sw        %[res1],         0(%[y])                   \n\t"
    "sw        %[res2],         4*4(%[y])                 \n\t"
    "sw        %[res3],         1*4(%[y])                 \n\t"
    "sw        %[res4],         5*4(%[y])                 \n\t"
    "sra       %[vi],           %[vi],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "subu      %[vr],           %[vr],        %[tmp2]     \n\t"
    "subu      %[ui],           %[ui],        %[tmp8]     \n\t"
    "subu      %[vi],           %[vi],        %[tmp4]     \n\t"
    "subu      %[ur],           %[ur],        %[tmp6]     \n\t"
    "addu      %[res1],         %[ui],        %[vr]       \n\t"
    "subu      %[res2],         %[vr],        %[ui]       \n\t"
    "subu      %[res3],         %[ur],        %[vi]       \n\t"
    "addu      %[res4],         %[vi],        %[ur]       \n\t"
    "sw        %[res1],         2*4(%[y])                 \n\t"
    "sw        %[res2],         6*4(%[y])                 \n\t"
    "sw        %[res3],         3*4(%[y])                 \n\t"
    "sw        %[res4],         7*4(%[y])                 \n\t"

    "lw        %[tmp1],         8*4(%[x])                 \n\t"//i=8
    "lw        %[tmp2],         40*4(%[x])                \n\t"
    "lw        %[tmp3],         24*4(%[x])                \n\t"
    "lw        %[tmp4],         56*4(%[x])                \n\t"
    "lw        %[tmp7],         25*4(%[x])                \n\t"
    "lw        %[tmp8],         57*4(%[x])                \n\t"
    "lw        %[tmp5],         9*4(%[x])                 \n\t"
    "lw        %[tmp6],         41*4(%[x])                \n\t"
    "addqh.w   %[vr],           %[tmp1],      %[tmp2]     \n\t"
    "addu      %[vi],           %[tmp3],      %[tmp4]     \n\t"
    "addu      %[ui],           %[tmp7],      %[tmp8]     \n\t"
    "addqh.w   %[ur],           %[tmp5],      %[tmp6]     \n\t"
    "sra       %[res3],         %[vi],        1           \n\t"
    "sra       %[res4],         %[ui],        1           \n\t"
    "addu      %[res1],         %[vr],        %[res3]     \n\t"
    "subu      %[res2],         %[vr],        %[res3]     \n\t"
    "addu      %[res3],         %[ur],        %[res4]     \n\t"
    "subu      %[res4],         %[ur],        %[res4]     \n\t"
    "sw        %[res1],         8*4(%[y])                 \n\t"
    "sw        %[res2],         12*4(%[y])                \n\t"
    "sw        %[res3],         9*4(%[y])                 \n\t"
    "sw        %[res4],         13*4(%[y])                \n\t"
    "sra       %[vi],           %[vi],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "subu      %[vr],           %[vr],        %[tmp2]     \n\t"
    "subu      %[ui],           %[ui],        %[tmp8]     \n\t"
    "subu      %[vi],           %[vi],        %[tmp4]     \n\t"
    "subu      %[ur],           %[ur],        %[tmp6]     \n\t"
    "addu      %[res1],         %[ui],        %[vr]       \n\t"
    "subu      %[res2],         %[vr],        %[ui]       \n\t"
    "subu      %[res3],         %[ur],        %[vi]       \n\t"
    "addu      %[res4],         %[vi],        %[ur]       \n\t"
    "sw        %[res1],         10*4(%[y])                \n\t"
    "sw        %[res2],         14*4(%[y])                \n\t"
    "sw        %[res3],         11*4(%[y])                \n\t"
    "sw        %[res4],         15*4(%[y])                \n\t"

    "lw        %[tmp1],         4*4(%[x])                 \n\t" //i=16
    "lw        %[tmp2],         36*4(%[x])                \n\t"
    "lw        %[tmp3],         20*4(%[x])                \n\t"
    "lw        %[tmp4],         52*4(%[x])                \n\t"
    "lw        %[tmp7],         21*4(%[x])                \n\t"
    "lw        %[tmp8],         53*4(%[x])                \n\t"
    "lw        %[tmp5],         5*4(%[x])                 \n\t"
    "lw        %[tmp6],         37*4(%[x])                \n\t"
    "addqh.w   %[vr],           %[tmp1],      %[tmp2]     \n\t"
    "addu      %[vi],           %[tmp3],      %[tmp4]     \n\t"
    "addu      %[ui],           %[tmp7],      %[tmp8]     \n\t"
    "addqh.w   %[ur],           %[tmp5],      %[tmp6]     \n\t"
    "sra       %[res3],         %[vi],        1           \n\t"
    "sra       %[res4],         %[ui],        1           \n\t"
    "addu      %[res1],         %[vr],        %[res3]     \n\t"
    "subu      %[res2],         %[vr],        %[res3]     \n\t"
    "addu      %[res3],         %[ur],        %[res4]     \n\t"
    "subu      %[res4],         %[ur],        %[res4]     \n\t"
    "sw        %[res1],         16*4(%[y])                \n\t"
    "sw        %[res2],         20*4(%[y])                \n\t"
    "sw        %[res3],         17*4(%[y])                \n\t"
    "sw        %[res4],         21*4(%[y])                \n\t"
    "sra       %[vi],           %[vi],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "subu      %[vr],           %[vr],        %[tmp2]     \n\t"
    "subu      %[ui],           %[ui],        %[tmp8]     \n\t"
    "subu      %[vi],           %[vi],        %[tmp4]     \n\t"
    "subu      %[ur],           %[ur],        %[tmp6]     \n\t"
    "addu      %[res1],         %[ui],        %[vr]       \n\t"
    "subu      %[res2],         %[vr],        %[ui]       \n\t"
    "subu      %[res3],         %[ur],        %[vi]       \n\t"
    "addu      %[res4],         %[vi],        %[ur]       \n\t"
    "sw        %[res1],         18*4(%[y])                \n\t"
    "sw        %[res2],         22*4(%[y])                \n\t"
    "sw        %[res3],         19*4(%[y])                \n\t"
    "sw        %[res4],         23*4(%[y])                \n\t"

    "lw        %[tmp1],         12*4(%[x])                \n\t"//i=24
    "lw        %[tmp2],         44*4(%[x])                \n\t"
    "lw        %[tmp3],         28*4(%[x])                \n\t"
    "lw        %[tmp4],         60*4(%[x])                \n\t"
    "lw        %[tmp7],         29*4(%[x])                \n\t"
    "lw        %[tmp8],         61*4(%[x])                \n\t"
    "lw        %[tmp5],         13*4(%[x])                \n\t"
    "lw        %[tmp6],         45*4(%[x])                \n\t"
    "addqh.w   %[vr],           %[tmp1],      %[tmp2]     \n\t"
    "addu      %[vi],           %[tmp3],      %[tmp4]     \n\t"
    "addu      %[ui],           %[tmp7],      %[tmp8]     \n\t"
    "addqh.w   %[ur],           %[tmp5],      %[tmp6]     \n\t"
    "sra       %[res3],         %[vi],        1           \n\t"
    "sra       %[res4],         %[ui],        1           \n\t"
    "addu      %[res1],         %[vr],        %[res3]     \n\t"
    "subu      %[res2],         %[vr],        %[res3]     \n\t"
    "addu      %[res3],         %[ur],        %[res4]     \n\t"
    "subu      %[res4],         %[ur],        %[res4]     \n\t"
    "sw        %[res1],         24*4(%[y])                \n\t"
    "sw        %[res2],         28*4(%[y])                \n\t"
    "sw        %[res3],         25*4(%[y])                \n\t"
    "sw        %[res4],         29*4(%[y])                \n\t"
    "sra       %[vi],           %[vi],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "subu      %[vr],           %[vr],        %[tmp2]     \n\t"
    "subu      %[ui],           %[ui],        %[tmp8]     \n\t"
    "subu      %[vi],           %[vi],        %[tmp4]     \n\t"
    "subu      %[ur],           %[ur],        %[tmp6]     \n\t"
    "addu      %[res1],         %[ui],        %[vr]       \n\t"
    "subu      %[res2],         %[vr],        %[ui]       \n\t"
    "subu      %[res3],         %[ur],        %[vi]       \n\t"
    "addu      %[res4],         %[vi],        %[ur]       \n\t"
    "sw        %[res1],         26*4(%[y])                \n\t"
    "sw        %[res2],         30*4(%[y])                \n\t"
    "sw        %[res3],         27*4(%[y])                \n\t"
    "sw        %[res4],         31*4(%[y])                \n\t"

    "lw        %[tmp1],         2*4(%[x])                 \n\t"// i = 32
    "lw        %[tmp2],         34*4(%[x])                \n\t"
    "lw        %[tmp3],         18*4(%[x])                \n\t"
    "lw        %[tmp4],         50*4(%[x])                \n\t"
    "lw        %[tmp7],         19*4(%[x])                \n\t"
    "lw        %[tmp8],         51*4(%[x])                \n\t"
    "lw        %[tmp5],         3*4(%[x])                 \n\t"
    "lw        %[tmp6],         35*4(%[x])                \n\t"
    "addqh.w   %[vr],           %[tmp1],      %[tmp2]     \n\t"
    "addu      %[vi],           %[tmp3],      %[tmp4]     \n\t"
    "addu      %[ui],           %[tmp7],      %[tmp8]     \n\t"
    "addqh.w   %[ur],           %[tmp5],      %[tmp6]     \n\t"
    "sra       %[res3],         %[vi],        1           \n\t"
    "sra       %[res4],         %[ui],        1           \n\t"
    "addu      %[res1],         %[vr],        %[res3]     \n\t"
    "subu      %[res2],         %[vr],        %[res3]     \n\t"
    "addu      %[res3],         %[ur],        %[res4]     \n\t"
    "subu      %[res4],         %[ur],        %[res4]     \n\t"
    "sw        %[res1],         32*4(%[y])                \n\t"
    "sw        %[res2],         36*4(%[y])                \n\t"
    "sw        %[res3],         33*4(%[y])                \n\t"
    "sw        %[res4],         37*4(%[y])                \n\t"
    "sra       %[vi],           %[vi],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "subu      %[vr],           %[vr],        %[tmp2]     \n\t"
    "subu      %[ui],           %[ui],        %[tmp8]     \n\t"
    "subu      %[vi],           %[vi],        %[tmp4]     \n\t"
    "subu      %[ur],           %[ur],        %[tmp6]     \n\t"
    "addu      %[res1],         %[ui],        %[vr]       \n\t"
    "subu      %[res2],         %[vr],        %[ui]       \n\t"
    "subu      %[res3],         %[ur],        %[vi]       \n\t"
    "addu      %[res4],         %[vi],        %[ur]       \n\t"
    "sw        %[res1],         34*4(%[y])                \n\t"
    "sw        %[res2],         38*4(%[y])                \n\t"
    "sw        %[res3],         35*4(%[y])                \n\t"
    "sw        %[res4],         39*4(%[y])                \n\t"

    "lw        %[tmp1],         10*4(%[x])                \n\t"//i=40
    "lw        %[tmp2],         42*4(%[x])                \n\t"
    "lw        %[tmp3],         26*4(%[x])                \n\t"
    "lw        %[tmp4],         58*4(%[x])                \n\t"
    "lw        %[tmp7],         27*4(%[x])                \n\t"
    "lw        %[tmp8],         59*4(%[x])                \n\t"
    "lw        %[tmp5],         11*4(%[x])                \n\t"
    "lw        %[tmp6],         43*4(%[x])                \n\t"
    "addqh.w   %[vr],           %[tmp1],      %[tmp2]     \n\t"
    "addu      %[vi],           %[tmp3],      %[tmp4]     \n\t"
    "addu      %[ui],           %[tmp7],      %[tmp8]     \n\t"
    "addqh.w   %[ur],           %[tmp5],      %[tmp6]     \n\t"
    "sra       %[res3],         %[vi],        1           \n\t"
    "sra       %[res4],         %[ui],        1           \n\t"
    "addu      %[res1],         %[vr],        %[res3]     \n\t"
    "subu      %[res2],         %[vr],        %[res3]     \n\t"
    "addu      %[res3],         %[ur],        %[res4]     \n\t"
    "subu      %[res4],         %[ur],        %[res4]     \n\t"
    "sw        %[res1],         40*4(%[y])                \n\t"
    "sw        %[res2],         44*4(%[y])                \n\t"
    "sw        %[res3],         41*4(%[y])                \n\t"
    "sw        %[res4],         45*4(%[y])                \n\t"
    "sra       %[vi],           %[vi],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "subu      %[vr],           %[vr],        %[tmp2]     \n\t"
    "subu      %[ui],           %[ui],        %[tmp8]     \n\t"
    "subu      %[vi],           %[vi],        %[tmp4]     \n\t"
    "subu      %[ur],           %[ur],        %[tmp6]     \n\t"
    "addu      %[res1],         %[ui],        %[vr]       \n\t"
    "subu      %[res2],         %[vr],        %[ui]       \n\t"
    "subu      %[res3],         %[ur],        %[vi]       \n\t"
    "addu      %[res4],         %[vi],        %[ur]       \n\t"
    "sw        %[res1],         42*4(%[y])                \n\t"
    "sw        %[res2],         46*4(%[y])                \n\t"
    "sw        %[res3],         43*4(%[y])                \n\t"
    "sw        %[res4],         47*4(%[y])                \n\t"

    "lw        %[tmp1],         6*4(%[x])                 \n\t"//i=48
    "lw        %[tmp2],         38*4(%[x])                \n\t"
    "lw        %[tmp3],         22*4(%[x])                \n\t"
    "lw        %[tmp4],         54*4(%[x])                \n\t"
    "lw        %[tmp7],         23*4(%[x])                \n\t"
    "lw        %[tmp8],         55*4(%[x])                \n\t"
    "lw        %[tmp5],         7*4(%[x])                 \n\t"
    "lw        %[tmp6],         39*4(%[x])                \n\t"
    "addqh.w   %[vr],           %[tmp1],      %[tmp2]     \n\t"
    "addu      %[vi],           %[tmp3],      %[tmp4]     \n\t"
    "addu      %[ui],           %[tmp7],      %[tmp8]     \n\t"
    "addqh.w   %[ur],           %[tmp5],      %[tmp6]     \n\t"
    "sra       %[res3],         %[vi],        1           \n\t"
    "sra       %[res4],         %[ui],        1           \n\t"
    "addu      %[res1],         %[vr],        %[res3]     \n\t"
    "subu      %[res2],         %[vr],        %[res3]     \n\t"
    "addu      %[res3],         %[ur],        %[res4]     \n\t"
    "subu      %[res4],         %[ur],        %[res4]     \n\t"
    "sw        %[res1],         48*4(%[y])                \n\t"
    "sw        %[res2],         52*4(%[y])                \n\t"
    "sw        %[res3],         49*4(%[y])                \n\t"
    "sw        %[res4],         53*4(%[y])                \n\t"
    "sra       %[vi],           %[vi],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "subu      %[vr],           %[vr],        %[tmp2]     \n\t"
    "subu      %[ui],           %[ui],        %[tmp8]     \n\t"
    "subu      %[vi],           %[vi],        %[tmp4]     \n\t"
    "subu      %[ur],           %[ur],        %[tmp6]     \n\t"
    "addu      %[res1],         %[ui],        %[vr]       \n\t"
    "subu      %[res2],         %[vr],        %[ui]       \n\t"
    "subu      %[res3],         %[ur],        %[vi]       \n\t"
    "addu      %[res4],         %[vi],        %[ur]       \n\t"
    "sw        %[res1],         50*4(%[y])                \n\t"
    "sw        %[res2],         54*4(%[y])                \n\t"
    "sw        %[res3],         51*4(%[y])                \n\t"
    "sw        %[res4],         55*4(%[y])                \n\t"

    "lw        %[tmp1],         14*4(%[x])                \n\t"//i=56
    "lw        %[tmp2],         46*4(%[x])                \n\t"
    "lw        %[tmp3],         30*4(%[x])                \n\t"
    "lw        %[tmp4],         62*4(%[x])                \n\t"
    "lw        %[tmp7],         31*4(%[x])                \n\t"
    "lw        %[tmp8],         63*4(%[x])                \n\t"
    "lw        %[tmp5],         15*4(%[x])                \n\t"
    "lw        %[tmp6],         47*4(%[x])                \n\t"
    "addqh.w   %[vr],           %[tmp1],      %[tmp2]     \n\t"
    "addu      %[vi],           %[tmp3],      %[tmp4]     \n\t"
    "addu      %[ui],           %[tmp7],      %[tmp8]     \n\t"
    "addqh.w   %[ur],           %[tmp5],      %[tmp6]     \n\t"
    "sra       %[res3],         %[vi],        1           \n\t"
    "sra       %[res4],         %[ui],        1           \n\t"
    "addu      %[res1],         %[vr],        %[res3]     \n\t"
    "subu      %[res2],         %[vr],        %[res3]     \n\t"
    "addu      %[res3],         %[ur],        %[res4]     \n\t"
    "subu      %[res4],         %[ur],        %[res4]     \n\t"
    "sw        %[res1],         56*4(%[y])                \n\t"
    "sw        %[res2],         60*4(%[y])                \n\t"
    "sw        %[res3],         57*4(%[y])                \n\t"
    "sw        %[res4],         61*4(%[y])                \n\t"
    "sra       %[vi],           %[vi],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "subu      %[vr],           %[vr],        %[tmp2]     \n\t"
    "subu      %[ui],           %[ui],        %[tmp8]     \n\t"
    "subu      %[vi],           %[vi],        %[tmp4]     \n\t"
    "subu      %[ur],           %[ur],        %[tmp6]     \n\t"
    "addu      %[res1],         %[ui],        %[vr]       \n\t"
    "subu      %[res2],         %[vr],        %[ui]       \n\t"
    "subu      %[res3],         %[ur],        %[vi]       \n\t"
    "addu      %[res4],         %[vi],        %[ur]       \n\t"
    "sw        %[res1],         58*4(%[y])                \n\t"
    "sw        %[res2],         62*4(%[y])                \n\t"
    "sw        %[res3],         59*4(%[y])                \n\t"
    "sw        %[res4],         63*4(%[y])                \n\t"


    "lw        %[vr],           8*4(%[y])                 \n\t"//do{}while
    "lw        %[vi],           9*4(%[y])                 \n\t"
    "lw        %[ur],           0*4(%[y])                 \n\t"
    "lw        %[ui],           1*4(%[y])                 \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         0*4(%[x])                 \n\t"
    "sw        %[res2],         1*4(%[x])                 \n\t"
    "sw        %[res3],         8*4(%[x])                 \n\t"
    "sw        %[res4],         9*4(%[x])                 \n\t"

    "lw        %[vr],           13*4(%[y])                \n\t"
    "lw        %[vi],           12*4(%[y])                \n\t"
    "lw        %[ur],           4*4(%[y])                 \n\t"
    "lw        %[ui],           5*4(%[y])                 \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         4*4(%[x])                 \n\t"
    "sw        %[res2],         5*4(%[x])                 \n\t"
    "sw        %[res3],         12*4(%[x])                \n\t"
    "sw        %[res4],         13*4(%[x])                \n\t"

    "lw        %[tmp3],         10*4(%[y])                \n\t"//SUMDIFF_PIFOURTH
    "lw        %[tmp4],         11*4(%[y])                \n\t"
    "lw        %[tmp5],         14*4(%[y])                \n\t"
    "lw        %[tmp6],         15*4(%[y])                \n\t"
    "lw        %[ur],           2*4(%[y])                 \n\t"
    "mult      $ac0,            %[tmp3],      %[tmp_PiF]  \n\t"
    "mult      $ac1,            %[tmp4],      %[tmp_PiF]  \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp_PiF]  \n\t"
    "mult      $ac3,            %[tmp6],      %[tmp_PiF]  \n\t"
    "sra       %[ur],           %[ur],        1           \n\t"
    "mfhi      %[tmp3],         $ac0                      \n\t"
    "mfhi      %[tmp4],         $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"
    "mfhi      %[tmp6],         $ac3                      \n\t"
    "lw        %[ui],           3*4(%[y])                 \n\t"
    "lw        %[tmp1],         6*4(%[y])                 \n\t"
    "lw        %[tmp2],         7*4(%[y])                 \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp1],         %[tmp1],      1           \n\t" //ur>>1
    "sra       %[tmp2],         %[tmp2],      1           \n\t" //ui>>1
    "subu      %[vi],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[vr],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "subu      %[tmp7],         %[tmp6],      %[tmp5]     \n\t"//vi
    "addu      %[tmp8],         %[tmp6],      %[tmp5]     \n\t"//vr
    "sw        %[res1],         2*4(%[x])                 \n\t"
    "sw        %[res2],         3*4(%[x])                 \n\t"
    "sw        %[res3],         10*4(%[x])                \n\t"
    "sw        %[res4],         11*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp1],      %[tmp7]     \n\t"
    "subu      %[res2],         %[tmp2],      %[tmp8]     \n\t"
    "subu      %[res3],         %[tmp1],      %[tmp7]     \n\t"
    "addu      %[res4],         %[tmp2],      %[tmp8]     \n\t"
    "sw        %[res1],         6*4(%[x])                 \n\t"
    "sw        %[res2],         7*4(%[x])                 \n\t"
    "sw        %[res3],         14*4(%[x])                \n\t"
    "sw        %[res4],         15*4(%[x])                \n\t"

    "lw        %[vr],           24*4(%[y])                \n\t"
    "lw        %[vi],           25*4(%[y])                \n\t"
    "lw        %[ur],           16*4(%[y])                \n\t"
    "lw        %[ui],           17*4(%[y])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         16*4(%[x])                \n\t"
    "sw        %[res2],         17*4(%[x])                \n\t"
    "sw        %[res3],         24*4(%[x])                \n\t"
    "sw        %[res4],         25*4(%[x])                \n\t"

    "lw        %[vr],           29*4(%[y])                \n\t"
    "lw        %[vi],           28*4(%[y])                \n\t"
    "lw        %[ur],           20*4(%[y])                \n\t"
    "lw        %[ui],           21*4(%[y])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         20*4(%[x])                \n\t"
    "sw        %[res2],         21*4(%[x])                \n\t"
    "sw        %[res3],         28*4(%[x])                \n\t"
    "sw        %[res4],         29*4(%[x])                \n\t"

    "lw        %[tmp3],         26*4(%[y])                \n\t"//SUMDIFF_PIFOURTH
    "lw        %[tmp4],         27*4(%[y])                \n\t"
    "lw        %[tmp5],         30*4(%[y])                \n\t"
    "lw        %[tmp6],         31*4(%[y])                \n\t"
    "lw        %[ur],           18*4(%[y])                \n\t"
    "mult      $ac0,            %[tmp3],      %[tmp_PiF]  \n\t"
    "mult      $ac1,            %[tmp4],      %[tmp_PiF]  \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp_PiF]  \n\t"
    "mult      $ac3,            %[tmp6],      %[tmp_PiF]  \n\t"
    "sra       %[ur],           %[ur],        1           \n\t"
    "mfhi      %[tmp3],         $ac0                      \n\t"
    "mfhi      %[tmp4],         $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"
    "mfhi      %[tmp6],         $ac3                      \n\t"
    "lw        %[ui],           19*4(%[y])                \n\t"
    "lw        %[tmp1],         22*4(%[y])                \n\t"
    "lw        %[tmp2],         23*4(%[y])                \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp1],         %[tmp1],      1           \n\t" //ur>>1
    "sra       %[tmp2],         %[tmp2],      1           \n\t" //ui>>1
    "subu      %[vi],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[vr],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "subu      %[tmp7],         %[tmp6],      %[tmp5]     \n\t"//vi
    "addu      %[tmp8],         %[tmp6],      %[tmp5]     \n\t"//vr
    "sw        %[res1],         18*4(%[x])                \n\t"
    "sw        %[res2],         19*4(%[x])                \n\t"
    "sw        %[res3],         26*4(%[x])                \n\t"
    "sw        %[res4],         27*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp1],      %[tmp7]     \n\t"
    "subu      %[res2],         %[tmp2],      %[tmp8]     \n\t"
    "subu      %[res3],         %[tmp1],      %[tmp7]     \n\t"
    "addu      %[res4],         %[tmp2],      %[tmp8]     \n\t"
    "sw        %[res1],         22*4(%[x])                \n\t"
    "sw        %[res2],         23*4(%[x])                \n\t"
    "sw        %[res3],         30*4(%[x])                \n\t"
    "sw        %[res4],         31*4(%[x])                \n\t"

    "lw        %[vr],           40*4(%[y])                \n\t"
    "lw        %[vi],           41*4(%[y])                \n\t"
    "lw        %[ur],           32*4(%[y])                \n\t"
    "lw        %[ui],           33*4(%[y])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         32*4(%[x])                \n\t"
    "sw        %[res2],         33*4(%[x])                \n\t"
    "sw        %[res3],         40*4(%[x])                \n\t"
    "sw        %[res4],         41*4(%[x])                \n\t"

    "lw        %[vr],           45*4(%[y])                \n\t"
    "lw        %[vi],           44*4(%[y])                \n\t"
    "lw        %[ur],           36*4(%[y])                \n\t"
    "lw        %[ui],           37*4(%[y])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         36*4(%[x])                \n\t"
    "sw        %[res2],         37*4(%[x])                \n\t"
    "sw        %[res3],         44*4(%[x])                \n\t"
    "sw        %[res4],         45*4(%[x])                \n\t"

    "lw        %[tmp3],         42*4(%[y])                \n\t"//SUMDIFF_PIFOURTH
    "lw        %[tmp4],         43*4(%[y])                \n\t"
    "lw        %[tmp5],         46*4(%[y])                \n\t"
    "lw        %[tmp6],         47*4(%[y])                \n\t"
    "lw        %[ur],           34*4(%[y])                \n\t"
    "mult      $ac0,            %[tmp3],      %[tmp_PiF]  \n\t"
    "mult      $ac1,            %[tmp4],      %[tmp_PiF]  \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp_PiF]  \n\t"
    "mult      $ac3,            %[tmp6],      %[tmp_PiF]  \n\t"
    "sra       %[ur],           %[ur],        1           \n\t"
    "mfhi      %[tmp3],         $ac0                      \n\t"
    "mfhi      %[tmp4],         $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"
    "mfhi      %[tmp6],         $ac3                      \n\t"
    "lw        %[ui],           35*4(%[y])                \n\t"
    "lw        %[tmp1],         38*4(%[y])                \n\t"
    "lw        %[tmp2],         39*4(%[y])                \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp1],         %[tmp1],      1           \n\t" //ur>>1
    "sra       %[tmp2],         %[tmp2],      1           \n\t" //ui>>1
    "subu      %[vi],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[vr],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "subu      %[tmp7],         %[tmp6],      %[tmp5]     \n\t"//vi
    "addu      %[tmp8],         %[tmp6],      %[tmp5]     \n\t"//vr
    "sw        %[res1],         34*4(%[x])                \n\t"
    "sw        %[res2],         35*4(%[x])                \n\t"
    "sw        %[res3],         42*4(%[x])                \n\t"
    "sw        %[res4],         43*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp1],      %[tmp7]     \n\t"
    "subu      %[res2],         %[tmp2],      %[tmp8]     \n\t"
    "subu      %[res3],         %[tmp1],      %[tmp7]     \n\t"
    "addu      %[res4],         %[tmp2],      %[tmp8]     \n\t"
    "sw        %[res1],         38*4(%[x])                \n\t"
    "sw        %[res2],         39*4(%[x])                \n\t"
    "sw        %[res3],         46*4(%[x])                \n\t"
    "sw        %[res4],         47*4(%[x])                \n\t"

    "lw        %[vr],           40*4(%[y])                \n\t"
    "lw        %[vi],           41*4(%[y])                \n\t"
    "lw        %[ur],           32*4(%[y])                \n\t"
    "lw        %[ui],           33*4(%[y])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         32*4(%[x])                \n\t"
    "sw        %[res2],         33*4(%[x])                \n\t"
    "sw        %[res3],         40*4(%[x])                \n\t"
    "sw        %[res4],         41*4(%[x])                \n\t"

    "lw        %[vr],           56*4(%[y])                \n\t"
    "lw        %[vi],           57*4(%[y])                \n\t"
    "lw        %[ur],           48*4(%[y])                \n\t"
    "lw        %[ui],           49*4(%[y])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         48*4(%[x])                \n\t"
    "sw        %[res2],         49*4(%[x])                \n\t"
    "sw        %[res3],         56*4(%[x])                \n\t"
    "sw        %[res4],         57*4(%[x])                \n\t"

    "lw        %[vr],           61*4(%[y])                \n\t"
    "lw        %[vi],           60*4(%[y])                \n\t"
    "lw        %[ur],           52*4(%[y])                \n\t"
    "lw        %[ui],           53*4(%[y])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         52*4(%[x])                \n\t"
    "sw        %[res2],         53*4(%[x])                \n\t"
    "sw        %[res3],         60*4(%[x])                \n\t"
    "sw        %[res4],         61*4(%[x])                \n\t"

    "lw        %[tmp3],         58*4(%[y])                \n\t"//SUMDIFF_PIFOURTH
    "lw        %[tmp4],         59*4(%[y])                \n\t"
    "lw        %[tmp5],         62*4(%[y])                \n\t"
    "lw        %[tmp6],         63*4(%[y])                \n\t"
    "lw        %[ur],           50*4(%[y])                \n\t"
    "mult      $ac0,            %[tmp3],      %[tmp_PiF]  \n\t"
    "mult      $ac1,            %[tmp4],      %[tmp_PiF]  \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp_PiF]  \n\t"
    "mult      $ac3,            %[tmp6],      %[tmp_PiF]  \n\t"
    "sra       %[ur],           %[ur],        1           \n\t"
    "mfhi      %[tmp3],         $ac0                      \n\t"
    "mfhi      %[tmp4],         $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"
    "mfhi      %[tmp6],         $ac3                      \n\t"
    "lw        %[ui],           51*4(%[y])                \n\t"
    "lw        %[tmp1],         54*4(%[y])                \n\t"
    "lw        %[tmp2],         55*4(%[y])                \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp1],         %[tmp1],      1           \n\t" //ur>>1
    "sra       %[tmp2],         %[tmp2],      1           \n\t" //ui>>1
    "subu      %[vi],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[vr],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "subu      %[tmp7],         %[tmp6],      %[tmp5]     \n\t"//vi
    "addu      %[tmp8],         %[tmp6],      %[tmp5]     \n\t"//vr
    "sw        %[res1],         50*4(%[x])                \n\t"
    "sw        %[res2],         51*4(%[x])                \n\t"
    "sw        %[res3],         58*4(%[x])                \n\t"
    "sw        %[res4],         59*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp1],      %[tmp7]     \n\t"
    "subu      %[res2],         %[tmp2],      %[tmp8]     \n\t"
    "subu      %[res3],         %[tmp1],      %[tmp7]     \n\t"
    "addu      %[res4],         %[tmp2],      %[tmp8]     \n\t"
    "sw        %[res1],         54*4(%[x])                \n\t"
    "sw        %[res2],         55*4(%[x])                \n\t"
    "sw        %[res3],         62*4(%[x])                \n\t"
    "sw        %[res4],         63*4(%[x])                \n\t"//end while

    "lw        %[vr],           16*4(%[x])                \n\t"
    "lw        %[vi],           17*4(%[x])                \n\t"
    "lw        %[ur],           0*4(%[x])                 \n\t"
    "lw        %[ui],           1*4(%[x])                 \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         0*4(%[x])                 \n\t"
    "sw        %[res2],         1*4(%[x])                 \n\t"
    "sw        %[res3],         16*4(%[x])                \n\t"
    "sw        %[res4],         17*4(%[x])                \n\t"
    //
    "lw        %[vr],           25*4(%[x])                \n\t"
    "lw        %[vi],           24*4(%[x])                \n\t"
    "lw        %[ur],           8*4(%[x])                 \n\t"
    "lw        %[ui],           9*4(%[x])                 \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         8*4(%[x])                 \n\t"
    "sw        %[res2],         9*4(%[x])                 \n\t"
    "sw        %[res3],         24*4(%[x])                \n\t"
    "sw        %[res4],         25*4(%[x])                \n\t"

    "lw        %[vr],           48*4(%[x])                \n\t"
    "lw        %[vi],           49*4(%[x])                \n\t"
    "lw        %[ur],           32*4(%[x])                \n\t"
    "lw        %[ui],           33*4(%[x])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         32*4(%[x])                \n\t"
    "sw        %[res2],         33*4(%[x])                \n\t"
    "sw        %[res3],         48*4(%[x])                \n\t"
    "sw        %[res4],         49*4(%[x])                \n\t"

    "lw        %[vr],           57*4(%[x])                \n\t"
    "lw        %[vi],           56*4(%[x])                \n\t"
    "lw        %[ur],           40*4(%[x])                \n\t"
    "lw        %[ui],           41*4(%[x])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         40*4(%[x])                \n\t"
    "sw        %[res2],         41*4(%[x])                \n\t"
    "sw        %[res3],         56*4(%[x])                \n\t"
    "sw        %[res4],         57*4(%[x])                \n\t"

    "lw        %[tmp1],         19*4(%[x])                \n\t"//cplxMultDiv2
    "lw        %[tmp2],         18*4(%[x])                \n\t"
    "lw        %[tmp3],         0(%[fft32_w32])           \n\t"
    "lw        %[tmp4],         4(%[fft32_w32])           \n\t"
    "lw        %[tmp5],         27*4(%[x])                \n\t"
    "lw        %[tmp6],         26*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           2*4(%[x])                 \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           3*4(%[x])                 \n\t"
    "lw        %[tmp7],         10*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         11*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         2*4(%[x])                 \n\t"
    "sw        %[res2],         3*4(%[x])                 \n\t"
    "sw        %[res3],         18*4(%[x])                \n\t"
    "sw        %[res4],         19*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         10*4(%[x])                \n\t"
    "sw        %[res2],         11*4(%[x])                \n\t"
    "sw        %[res3],         26*4(%[x])                \n\t"
    "sw        %[res4],         27*4(%[x])                \n\t"

    "lw        %[tmp1],         51*4(%[x])                \n\t"//cplxMultDiv2
    "lw        %[tmp2],         50*4(%[x])                \n\t"
    "lw        %[tmp5],         59*4(%[x])                \n\t"
    "lw        %[tmp6],         58*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           34*4(%[x])                \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           35*4(%[x])                \n\t"
    "lw        %[tmp7],         42*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         43*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         34*4(%[x])                \n\t"
    "sw        %[res2],         35*4(%[x])                \n\t"
    "sw        %[res3],         50*4(%[x])                \n\t"
    "sw        %[res4],         51*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         42*4(%[x])                \n\t"
    "sw        %[res2],         43*4(%[x])                \n\t"
    "sw        %[res3],         58*4(%[x])                \n\t"
    "sw        %[res4],         59*4(%[x])                \n\t"

    "lw        %[tmp3],         20*4(%[x])                \n\t"//SUMDIFF_PIFOURTH
    "lw        %[tmp4],         21*4(%[x])                \n\t"
    "lw        %[tmp5],         28*4(%[x])                \n\t"
    "lw        %[tmp6],         29*4(%[x])                \n\t"
    "lw        %[ur],           4*4(%[x])                 \n\t"
    "mult      $ac0,            %[tmp3],      %[tmp_PiF]  \n\t"
    "mult      $ac1,            %[tmp4],      %[tmp_PiF]  \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp_PiF]  \n\t"
    "mult      $ac3,            %[tmp6],      %[tmp_PiF]  \n\t"
    "sra       %[ur],           %[ur],        1           \n\t"
    "mfhi      %[tmp3],         $ac0                      \n\t"
    "mfhi      %[tmp4],         $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"
    "mfhi      %[tmp6],         $ac3                      \n\t"
    "lw        %[ui],           5*4(%[x])                 \n\t"
    "lw        %[tmp1],         12*4(%[x])                \n\t"
    "lw        %[tmp2],         13*4(%[x])                \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp1],         %[tmp1],      1           \n\t" //ur>>1
    "sra       %[tmp2],         %[tmp2],      1           \n\t" //ui>>1
    "subu      %[vi],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[vr],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "subu      %[tmp7],         %[tmp6],      %[tmp5]     \n\t"//vr
    "addu      %[tmp8],         %[tmp6],      %[tmp5]     \n\t"//vi
    "sw        %[res1],         4*4(%[x])                 \n\t"
    "sw        %[res2],         5*4(%[x])                 \n\t"
    "sw        %[res3],         20*4(%[x])                \n\t"
    "sw        %[res4],         21*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp1],      %[tmp7]     \n\t"
    "subu      %[res2],         %[tmp2],      %[tmp8]     \n\t"
    "subu      %[res3],         %[tmp1],      %[tmp7]     \n\t"
    "addu      %[res4],         %[tmp2],      %[tmp8]     \n\t"
    "sw        %[res1],         12*4(%[x])                \n\t"
    "sw        %[res2],         13*4(%[x])                \n\t"
    "sw        %[res3],         28*4(%[x])                \n\t"
    "sw        %[res4],         29*4(%[x])                \n\t"

    "lw        %[tmp3],         52*4(%[x])                \n\t"//SUMDIFF_PIFOURTH
    "lw        %[tmp4],         53*4(%[x])                \n\t"
    "lw        %[tmp5],         60*4(%[x])                \n\t"
    "lw        %[tmp6],         61*4(%[x])                \n\t"
    "lw        %[ur],           36*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp3],      %[tmp_PiF]  \n\t"
    "mult      $ac1,            %[tmp4],      %[tmp_PiF]  \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp_PiF]  \n\t"
    "mult      $ac3,            %[tmp6],      %[tmp_PiF]  \n\t"
    "sra       %[ur],           %[ur],        1           \n\t"
    "mfhi      %[tmp3],         $ac0                      \n\t"
    "mfhi      %[tmp4],         $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"
    "mfhi      %[tmp6],         $ac3                      \n\t"
    "lw        %[ui],           37*4(%[x])                \n\t"
    "lw        %[tmp1],         44*4(%[x])                \n\t"
    "lw        %[tmp2],         45*4(%[x])                \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp1],         %[tmp1],      1           \n\t" //ur>>1
    "sra       %[tmp2],         %[tmp2],      1           \n\t" //ui>>1
    "subu      %[vi],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[vr],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "subu      %[tmp7],         %[tmp6],      %[tmp5]     \n\t"//vr
    "addu      %[tmp8],         %[tmp6],      %[tmp5]     \n\t"//vi
    "sw        %[res1],         36*4(%[x])                \n\t"
    "sw        %[res2],         37*4(%[x])                \n\t"
    "sw        %[res3],         52*4(%[x])                \n\t"
    "sw        %[res4],         53*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp1],      %[tmp7]     \n\t"
    "subu      %[res2],         %[tmp2],      %[tmp8]     \n\t"
    "subu      %[res3],         %[tmp1],      %[tmp7]     \n\t"
    "addu      %[res4],         %[tmp2],      %[tmp8]     \n\t"
    "sw        %[res1],         44*4(%[x])                \n\t"
    "sw        %[res2],         45*4(%[x])                \n\t"
    "sw        %[res3],         60*4(%[x])                \n\t"
    "sw        %[res4],         61*4(%[x])                \n\t"

    "lw        %[tmp1],         23*4(%[x])                \n\t"//cplxMultDiv2
    "lw        %[tmp2],         22*4(%[x])                \n\t"
    "lw        %[tmp3],         8(%[fft32_w32])           \n\t"
    "lw        %[tmp4],         12(%[fft32_w32])          \n\t"
    "lw        %[tmp5],         31*4(%[x])                \n\t"
    "lw        %[tmp6],         30*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           6*4(%[x])                 \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           7*4(%[x])                 \n\t"
    "lw        %[tmp7],         14*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         15*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         6*4(%[x])                 \n\t"
    "sw        %[res2],         7*4(%[x])                 \n\t"
    "sw        %[res3],         22*4(%[x])                \n\t"
    "sw        %[res4],         23*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         14*4(%[x])                \n\t"
    "sw        %[res2],         15*4(%[x])                \n\t"
    "sw        %[res3],         30*4(%[x])                \n\t"
    "sw        %[res4],         31*4(%[x])                \n\t"

    "lw        %[tmp1],         55*4(%[x])                \n\t"//cplxMultDiv2
    "lw        %[tmp2],         54*4(%[x])                \n\t"
    "lw        %[tmp5],         63*4(%[x])                \n\t"
    "lw        %[tmp6],         62*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           38*4(%[x])                \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           39*4(%[x])                \n\t"
    "lw        %[tmp7],         46*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         47*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         38*4(%[x])                \n\t"
    "sw        %[res2],         39*4(%[x])                \n\t"
    "sw        %[res3],         54*4(%[x])                \n\t"
    "sw        %[res4],         55*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         46*4(%[x])                \n\t"
    "sw        %[res2],         47*4(%[x])                \n\t"
    "sw        %[res3],         62*4(%[x])                \n\t"
    "sw        %[res4],         63*4(%[x])                \n\t"

    "lw        %[vr],           32*4(%[x])                \n\t"
    "lw        %[vi],           33*4(%[x])                \n\t"
    "lw        %[ur],           0*4(%[x])                 \n\t"
    "lw        %[ui],           1*4(%[x])                 \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         0*4(%[x])                 \n\t"
    "sw        %[res2],         1*4(%[x])                 \n\t"
    "sw        %[res3],         32*4(%[x])                \n\t"
    "sw        %[res4],         33*4(%[x])                \n\t"

    "lw        %[vi],           48*4(%[x])                \n\t"
    "lw        %[vr],           49*4(%[x])                \n\t"
    "lw        %[ur],           16*4(%[x])                \n\t"
    "lw        %[ui],           17*4(%[x])                \n\t"
    "sra       %[tmp1],         %[vr],        1           \n\t" //vr>>1
    "sra       %[tmp2],         %[vi],        1           \n\t" //vi>>1
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "addu      %[res1],         %[ur],        %[tmp1]     \n\t"
    "subu      %[res2],         %[ui],        %[tmp2]     \n\t"
    "subu      %[res3],         %[ur],        %[tmp1]     \n\t"
    "addu      %[res4],         %[ui],        %[tmp2]     \n\t"
    "sw        %[res1],         16*4(%[x])                \n\t"
    "sw        %[res2],         17*4(%[x])                \n\t"
    "sw        %[res3],         48*4(%[x])                \n\t"
    "sw        %[res4],         49*4(%[x])                \n\t"

    "lw        %[tmp1],         35*4(%[x])                \n\t"//cplxMultDiv2
    "lw        %[tmp2],         34*4(%[x])                \n\t"
    "lw        %[tmp3],         16(%[fft32_w32])          \n\t"
    "lw        %[tmp4],         20(%[fft32_w32])          \n\t"
    "lw        %[tmp5],         51*4(%[x])                \n\t"
    "lw        %[tmp6],         50*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           2*4(%[x])                 \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           3*4(%[x])                 \n\t"
    "lw        %[tmp7],         18*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         19*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         2*4(%[x])                 \n\t"
    "sw        %[res2],         3*4(%[x])                 \n\t"
    "sw        %[res3],         34*4(%[x])                \n\t"
    "sw        %[res4],         35*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         18*4(%[x])                \n\t"
    "sw        %[res2],         19*4(%[x])                \n\t"
    "sw        %[res3],         50*4(%[x])                \n\t"
    "sw        %[res4],         51*4(%[x])                \n\t"

    "lw        %[tmp1],         37*4(%[x])                \n\t"//cplxMultDiv2
    "lw        %[tmp2],         36*4(%[x])                \n\t"
    "lw        %[tmp3],         0(%[fft32_w32])           \n\t"
    "lw        %[tmp4],         4(%[fft32_w32])           \n\t"
    "lw        %[tmp5],         53*4(%[x])                \n\t"
    "lw        %[tmp6],         52*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           4*4(%[x])                 \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           5*4(%[x])                 \n\t"
    "lw        %[tmp7],         20*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         21*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         4*4(%[x])                 \n\t"
    "sw        %[res2],         5*4(%[x])                 \n\t"
    "sw        %[res3],         36*4(%[x])                \n\t"
    "sw        %[res4],         37*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         20*4(%[x])                \n\t"
    "sw        %[res2],         21*4(%[x])                \n\t"
    "sw        %[res3],         52*4(%[x])                \n\t"
    "sw        %[res4],         53*4(%[x])                \n\t"

    "lw        %[tmp1],         39*4(%[x])                \n\t" //cplxMultDiv2
    "lw        %[tmp2],         38*4(%[x])                \n\t"
    "lw        %[tmp3],         24(%[fft32_w32])          \n\t"
    "lw        %[tmp4],         28(%[fft32_w32])          \n\t"
    "lw        %[tmp5],         55*4(%[x])                \n\t"
    "lw        %[tmp6],         54*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           6*4(%[x])                 \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           7*4(%[x])                 \n\t"
    "lw        %[tmp7],         22*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         23*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         6*4(%[x])                 \n\t"
    "sw        %[res2],         7*4(%[x])                 \n\t"
    "sw        %[res3],         38*4(%[x])                \n\t"
    "sw        %[res4],         39*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         22*4(%[x])                \n\t"
    "sw        %[res2],         23*4(%[x])                \n\t"
    "sw        %[res3],         54*4(%[x])                \n\t"
    "sw        %[res4],         55*4(%[x])                \n\t"

    "lw        %[tmp3],         40*4(%[x])                \n\t"//SUMDIFF_PIFOURTH
    "lw        %[tmp4],         41*4(%[x])                \n\t"
    "lw        %[tmp5],         56*4(%[x])                \n\t"
    "lw        %[tmp6],         57*4(%[x])                \n\t"
    "lw        %[ur],           8*4(%[x])                 \n\t"
    "mult      $ac0,            %[tmp3],      %[tmp_PiF]  \n\t"
    "mult      $ac1,            %[tmp4],      %[tmp_PiF]  \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp_PiF]  \n\t"
    "mult      $ac3,            %[tmp6],      %[tmp_PiF]  \n\t"
    "sra       %[ur],           %[ur],        1           \n\t"
    "mfhi      %[tmp3],         $ac0                      \n\t"
    "mfhi      %[tmp4],         $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"
    "mfhi      %[tmp6],         $ac3                      \n\t"
    "lw        %[ui],           9*4(%[x])                 \n\t"
    "lw        %[tmp1],         24*4(%[x])                \n\t"
    "lw        %[tmp2],         25*4(%[x])                \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp1],         %[tmp1],      1           \n\t" //ur>>1
    "sra       %[tmp2],         %[tmp2],      1           \n\t" //ui>>1
    "subu      %[vi],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[vr],           %[tmp4],      %[tmp3]     \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "subu      %[tmp7],         %[tmp6],      %[tmp5]     \n\t"//vr
    "addu      %[tmp8],         %[tmp6],      %[tmp5]     \n\t"//vi
    "sw        %[res1],         8*4(%[x])                 \n\t"
    "sw        %[res2],         9*4(%[x])                 \n\t"
    "sw        %[res3],         40*4(%[x])                \n\t"
    "sw        %[res4],         41*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp1],      %[tmp7]     \n\t"
    "subu      %[res2],         %[tmp2],      %[tmp8]     \n\t"
    "subu      %[res3],         %[tmp1],      %[tmp7]     \n\t"
    "addu      %[res4],         %[tmp2],      %[tmp8]     \n\t"
    "sw        %[res1],         24*4(%[x])                \n\t"
    "sw        %[res2],         25*4(%[x])                \n\t"
    "sw        %[res3],         56*4(%[x])                \n\t"
    "sw        %[res4],         57*4(%[x])                \n\t"

    "lw        %[tmp1],         43*4(%[x])                \n\t"//cplxMultDiv2
    "lw        %[tmp2],         42*4(%[x])                \n\t"
    "lw        %[tmp3],         32(%[fft32_w32])          \n\t"
    "lw        %[tmp4],         36(%[fft32_w32])          \n\t"
    "lw        %[tmp5],         59*4(%[x])                \n\t"
    "lw        %[tmp6],         58*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           10*4(%[x])                \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           11*4(%[x])                \n\t"
    "lw        %[tmp7],         26*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         27*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         10*4(%[x])                \n\t"
    "sw        %[res2],         11*4(%[x])                \n\t"
    "sw        %[res3],         42*4(%[x])                \n\t"
    "sw        %[res4],         43*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         26*4(%[x])                \n\t"
    "sw        %[res2],         27*4(%[x])                \n\t"
    "sw        %[res3],         58*4(%[x])                \n\t"
    "sw        %[res4],         59*4(%[x])                \n\t"

    "lw        %[tmp1],         45*4(%[x])                \n\t"//cplxMultDiv2
    "lw        %[tmp2],         44*4(%[x])                \n\t"
    "lw        %[tmp3],         8(%[fft32_w32])           \n\t"
    "lw        %[tmp4],         12(%[fft32_w32])          \n\t"
    "lw        %[tmp5],         61*4(%[x])                \n\t"
    "lw        %[tmp6],         60*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           12*4(%[x])                \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           13*4(%[x])                \n\t"
    "lw        %[tmp7],         28*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         29*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         12*4(%[x])                \n\t"
    "sw        %[res2],         13*4(%[x])                \n\t"
    "sw        %[res3],         44*4(%[x])                \n\t"
    "sw        %[res4],         45*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         28*4(%[x])                \n\t"
    "sw        %[res2],         29*4(%[x])                \n\t"
    "sw        %[res3],         60*4(%[x])                \n\t"
    "sw        %[res4],         61*4(%[x])                \n\t"

    "lw        %[tmp1],         47*4(%[x])                \n\t"//cplxMultDiv2
    "lw        %[tmp2],         46*4(%[x])                \n\t"
    "lw        %[tmp3],         40(%[fft32_w32])          \n\t"
    "lw        %[tmp4],         44(%[fft32_w32])          \n\t"
    "lw        %[tmp5],         63*4(%[x])                \n\t"
    "lw        %[tmp6],         62*4(%[x])                \n\t"
    "mult      $ac0,            %[tmp1],      %[tmp3]     \n\t"
    "msub      $ac0,            %[tmp2],      %[tmp4]     \n\t"
    "mult      $ac1,            %[tmp1],      %[tmp4]     \n\t"
    "madd      $ac1,            %[tmp2],      %[tmp3]     \n\t"
    "mult      $ac2,            %[tmp5],      %[tmp3]     \n\t"
    "msub      $ac2,            %[tmp6],      %[tmp4]     \n\t"
    "mult      $ac3,            %[tmp5],      %[tmp4]     \n\t"
    "madd      $ac3,            %[tmp6],      %[tmp3]     \n\t"
    "lw        %[ur],           14*4(%[x])                \n\t"
    "mfhi      %[vi],           $ac0                      \n\t"
    "mfhi      %[vr],           $ac1                      \n\t"
    "mfhi      %[tmp5],         $ac2                      \n\t"//vr
    "mfhi      %[tmp6],         $ac3                      \n\t"//vi
    "lw        %[ui],           15*4(%[x])                \n\t"
    "lw        %[tmp7],         30*4(%[x])                \n\t"//ur
    "lw        %[tmp8],         31*4(%[x])                \n\t"//ui
    "sra       %[ur],           %[ur],        1           \n\t"
    "sra       %[ui],           %[ui],        1           \n\t"
    "sra       %[tmp7],         %[tmp7],      1           \n\t"
    "sra       %[tmp8],         %[tmp8],      1           \n\t"
    "addu      %[res1],         %[ur],        %[vr]       \n\t"
    "addu      %[res2],         %[ui],        %[vi]       \n\t"
    "subu      %[res3],         %[ur],        %[vr]       \n\t"
    "subu      %[res4],         %[ui],        %[vi]       \n\t"
    "sw        %[res1],         14*4(%[x])                \n\t"
    "sw        %[res2],         15*4(%[x])                \n\t"
    "sw        %[res3],         46*4(%[x])                \n\t"
    "sw        %[res4],         47*4(%[x])                \n\t"
    "addu      %[res1],         %[tmp7],      %[tmp5]     \n\t"
    "subu      %[res2],         %[tmp8],      %[tmp6]     \n\t"
    "subu      %[res3],         %[tmp7],      %[tmp5]     \n\t"
    "addu      %[res4],         %[tmp8],      %[tmp6]     \n\t"
    "sw        %[res1],         30*4(%[x])                \n\t"
    "sw        %[res2],         31*4(%[x])                \n\t"
    "sw        %[res3],         62*4(%[x])                \n\t"
    "sw        %[res4],         63*4(%[x])                \n\t"

    : [vi] "=&r" (vi), [vr] "=&r" (vr),
      [ur] "=&r" (ur), [ui] "=&r" (ui),
      [tmp3] "=&r" (tmp3), [tmp4] "=&r" (tmp4),
      [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2),
      [tmp5] "=&r" (tmp5), [tmp6] "=&r" (tmp6),
      [tmp7] "=&r" (tmp7), [tmp8] "=&r" (tmp8),
      [res1] "=&r" (res1), [res2] "=&r" (res2),
      [res3] "=&r" (res3), [res4] "=&r" (res4)
    : [y] "r" (y), [x] "r" (x), [tmp_PiF] "r" (tmp_PiF),
      [fft32_w32] "r" (fft32_w32)
    : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo",
      "$ac3hi", "$ac3lo", "memory"
  );

}

#endif /* #ifdef FUNCTION_fft_32 */

#endif /* #ifndef __INCLUDE_SCALE_MIPS__ */
