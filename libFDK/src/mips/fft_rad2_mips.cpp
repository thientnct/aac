
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
   Description: dit_fft MIPS assembler replacements.

******************************************************************************/

#if defined(MIPS_DSP_R2_LE)
#define FUNCTION_dit_fft
#endif /* #if defined(MIPS_DSP_R2_LE) */


#ifdef FUNCTION_dit_fft

void dit_fft(FIXP_DBL *x, const INT ldn, const FIXP_STP *trigdata, const INT trigDataSize)
{
    const INT n=1<<ldn;
    INT trigstep,i,ldm;

    scramble(x,n);
    /*
     * 1+2 stage radix 4
     */

    for (i=0;i<n*2;i+=8)
    {
        FIXP_DBL a00, a10, a20, a30;
        FIXP_DBL tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
        FIXP_DBL *x_temp = x + i;

        __asm__ volatile (
            "lw            %[tmp0],   0(%[x_temp])                \n\t"
            "lw            %[tmp2],   8(%[x_temp])                \n\t"
            "lw            %[tmp4],   16(%[x_temp])               \n\t"
            "lw            %[tmp6],   24(%[x_temp])               \n\t"
            "lw            %[tmp1],   4(%[x_temp])                \n\t"
            "lw            %[tmp3],   12(%[x_temp])               \n\t"
            "lw            %[tmp5],   20(%[x_temp])               \n\t"
            "lw            %[tmp7],   28(%[x_temp])               \n\t"

            "addqh.W       %[a00],    %[tmp0],      %[tmp2]       \n\t"
            "addqh.W       %[a10],    %[tmp4],      %[tmp6]       \n\t"
            "addqh.W       %[a20],    %[tmp1],      %[tmp3]       \n\t"
            "addqh.W       %[a30],    %[tmp5],      %[tmp7]       \n\t"

            "addu          %[tmp0],   %[a00],       %[a10]        \n\t"
            "subu          %[tmp4],   %[a00],       %[a10]        \n\t"
            "addu          %[tmp1],   %[a20],       %[a30]        \n\t"
            "subu          %[tmp5],   %[a20],       %[a30]        \n\t"

            "sw            %[tmp0],   0(%[x_temp])                \n\t"
            "sw            %[tmp4],   16(%[x_temp])               \n\t"
            "sw            %[tmp1],   4(%[x_temp])                \n\t"
            "sw            %[tmp5],   20(%[x_temp])               \n\t"

            "subu          %[a00],    %[a00],       %[tmp2]       \n\t"
            "subu          %[a10],    %[a10],       %[tmp6]       \n\t"
            "subu          %[a20],    %[a20],       %[tmp3]       \n\t"
            "subu          %[a30],    %[a30],       %[tmp7]       \n\t"

            "addu          %[tmp2],   %[a00],       %[a30]        \n\t"
            "subu          %[tmp6],   %[a00],       %[a30]        \n\t"
            "subu          %[tmp3],   %[a20],       %[a10]        \n\t"
            "addu          %[tmp7],   %[a20],       %[a10]        \n\t"

            "sw            %[tmp2],   8(%[x_temp])                \n\t"
            "sw            %[tmp6],   24(%[x_temp])               \n\t"
            "sw            %[tmp3],   12(%[x_temp])               \n\t"
            "sw            %[tmp7],   28(%[x_temp])               \n\t"

            : [tmp7] "=&r" (tmp7), [tmp6] "=&r" (tmp6), [tmp5] "=&r" (tmp5),
              [tmp4] "=&r" (tmp4), [tmp3] "=&r" (tmp3), [tmp2] "=&r" (tmp2),
              [tmp1] "=&r" (tmp1), [tmp0] "=&r" (tmp0), [a00] "=&r" (a00),
              [a10] "=&r" (a10), [a20] "=&r" (a20), [a30] "=&r" (a30)
            : [x_temp] "r" (x_temp)
            : "memory"
        );
    }

    for(ldm=3; ldm<=ldn; ++ldm)
    {
        INT m=(1<<ldm);
        INT mh=(m>>1);
        INT j,r;

        trigstep=((trigDataSize << 2)>>ldm);

        FDK_ASSERT(trigstep > 0);

        /* Do first iteration with c=1.0 and s=0.0 separately to avoid loosing to much precision.
           Beware: The impact on the overal FFT precision is rather large. */
        {
            j = 0;

            for(r=0; r<n; r+=m)
            {
                INT t1 = (r+j)<<1;
                INT t2 = t1 + (mh<<1);
                FIXP_DBL vr,vi,ur,ui;

                //cplxMultDiv2(&vi, &vr, x[t2+1], x[t2], (FIXP_SGL)1.0, (FIXP_SGL)0.0);
                vi = x[t2+1]>>1;
                vr = x[t2]>>1;

                ur = x[t1]>>1;
                ui = x[t1+1]>>1;

                x[t1]   = ur+vr;
                x[t1+1] = ui+vi;

                x[t2]   = ur-vr;
                x[t2+1] = ui-vi;

                t1 += mh;
                t2 = t1+(mh<<1);

                //cplxMultDiv2(&vr, &vi, x[t2+1], x[t2], (FIXP_SGL)1.0, (FIXP_SGL)0.0);
                vr = x[t2+1]>>1;
                vi = x[t2]>>1;

                ur = x[t1]>>1;
                ui = x[t1+1]>>1;

                x[t1]   = ur+vr;
                x[t1+1] = ui-vi;

                x[t2]   = ur-vr;
                x[t2+1] = ui+vi;
            }
        }
        for(j=1; j<mh/4; ++j)
        {
            FIXP_STP cs;

            cs = trigdata[j*trigstep];
            FIXP_DBL tmp_cs_re = cs.v.re;
            FIXP_DBL tmp_cs_im = cs.v.im;

            for(r=0; r<n; r+=m)
            {
                INT t1 = (r+j)<<1;
                INT t2 = t1 + (m);
                FIXP_DBL vr,vi,ur,ui;
                FIXP_DBL vr1,vi1,ur1,ui1;
                int t_1_moj = t1 + mh;
                int t_2_moj = t_1_moj + m;

                FIXP_DBL *x_temp_1 = x + t1;
                FIXP_DBL *x_temp_2 = x + t2;
                FIXP_DBL *x_temp_3 = x + t_1_moj;
                FIXP_DBL *x_temp_4 = x + t_2_moj;

                FIXP_DBL tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;

                __asm__ volatile(
                    "lw    %[tmp3],     4(%[x_temp_2])              \n\t"
                    "lw    %[tmp1],     4(%[x_temp_4])              \n\t"
                    "lw    %[tmp4],     0(%[x_temp_2])              \n\t"
                    "lw    %[tmp2],     0(%[x_temp_4])              \n\t"

                    "mult  $ac0,        %[tmp3],       %[tmp_cs_re] \n\t"
                    "mult  $ac1,        %[tmp3],       %[tmp_cs_im] \n\t"
                    "mult  $ac3,        %[tmp1],       %[tmp_cs_re] \n\t"
                    "mult  $ac2,        %[tmp1],       %[tmp_cs_im] \n\t"

                    "lw    %[tmp3],     0(%[x_temp_1])              \n\t"
                    "lw    %[tmp1],     4(%[x_temp_1])              \n\t"

                    "msub  $ac0,        %[tmp4],       %[tmp_cs_im] \n\t"
                    "madd  $ac1,        %[tmp4],       %[tmp_cs_re] \n\t"
                    "msub  $ac3,        %[tmp2],       %[tmp_cs_im] \n\t"
                    "madd  $ac2,        %[tmp2],       %[tmp_cs_re] \n\t"

                    "lw    %[tmp4],     0(%[x_temp_3])              \n\t"
                    "lw    %[tmp2],     4(%[x_temp_3])              \n\t"

                    "mfhi  %[vi],       $ac0                        \n\t"
                    "mfhi  %[vr],       $ac1                        \n\t"
                    "mfhi  %[vr1],      $ac3                        \n\t"
                    "mfhi  %[vi1],      $ac2                        \n\t"

                    "sra   %[tmp3],     %[tmp3],       1            \n\t"
                    "sra   %[tmp1],     %[tmp1],       1            \n\t"
                    "sra   %[tmp4],     %[tmp4],       1            \n\t"
                    "sra   %[tmp2],     %[tmp2],       1            \n\t"

                    "addu  %[tmp5],     %[tmp1],       %[vi]        \n\t"
                    "subu  %[tmp1],     %[tmp1],       %[vi]        \n\t"
                    "addu  %[tmp6],     %[tmp3],       %[vr]        \n\t"
                    "subu  %[tmp3],     %[tmp3],       %[vr]        \n\t"

                    "sw    %[tmp5],     4(%[x_temp_1])              \n\t"
                    "sw    %[tmp1],     4(%[x_temp_2])              \n\t"
                    "sw    %[tmp6],     0(%[x_temp_1])              \n\t"
                    "sw    %[tmp3],     0(%[x_temp_2])              \n\t"

                    "addu  %[vi],       %[tmp4],       %[vr1]       \n\t"
                    "subu  %[tmp4],     %[tmp4],       %[vr1]       \n\t"
                    "addu  %[vr],       %[tmp2],       %[vi1]       \n\t"
                    "subu  %[tmp2],     %[tmp2],       %[vi1]       \n\t"

                    "sw    %[vi],       0(%[x_temp_3])              \n\t"
                    "sw    %[tmp4],     0(%[x_temp_4])              \n\t"
                    "sw    %[vr],       4(%[x_temp_4])              \n\t"
                    "sw    %[tmp2],     4(%[x_temp_3])              \n\t"

                    "sra   %[tmp5],     %[mh],         1            \n\t"
                    "subu  %[tmp1],     %[r],          %[j]         \n\t"
                    "addu  %[tmp5],     %[tmp5],       %[tmp1]      \n\t"
                    "sll   %[tmp5],     %[tmp5],       1            \n\t" //t1 = (r+mh/2-j)<<1;
                    "addu  %[tmp4],     %[tmp5],       %[m]         \n\t"
                    "addu  %[tmp3],     %[tmp5],       %[mh]        \n\t"
                    "addu  %[tmp2],     %[tmp3],       %[m]         \n\t"

                    "sll   %[tmp5],     %[tmp5],       2            \n\t"
                    "sll   %[tmp4],     %[tmp4],       2            \n\t"
                    "sll   %[tmp3],     %[tmp3],       2            \n\t"
                    "sll   %[tmp2],     %[tmp2],       2            \n\t"

                    "addu  %[x_temp_1], %[x],          %[tmp5]      \n\t"
                    "addu  %[x_temp_2], %[x],          %[tmp4]      \n\t"
                    "addu  %[x_temp_3], %[x],          %[tmp3]      \n\t"
                    "addu  %[x_temp_4], %[x],          %[tmp2]      \n\t"

                    "lw    %[tmp3],     4(%[x_temp_2])              \n\t"
                    "lw    %[tmp1],     4(%[x_temp_4])              \n\t"
                    "lw    %[tmp4],     0(%[x_temp_2])              \n\t"
                    "lw    %[tmp2],     0(%[x_temp_4])              \n\t"

                    "mult  $ac0,        %[tmp4],       %[tmp_cs_re] \n\t"
                    "mult  $ac1,        %[tmp4],       %[tmp_cs_im] \n\t"
                    "mult  $ac3,        %[tmp2],       %[tmp_cs_re] \n\t"
                    "mult  $ac2,        %[tmp2],       %[tmp_cs_im] \n\t"

                    "lw    %[tmp4],     0(%[x_temp_1])              \n\t"
                    "lw    %[tmp2],     4(%[x_temp_1])              \n\t"

                    "msub  $ac0,        %[tmp3],       %[tmp_cs_im] \n\t"
                    "madd  $ac1,        %[tmp3],       %[tmp_cs_re] \n\t"
                    "msub  $ac3,        %[tmp1],       %[tmp_cs_im] \n\t"
                    "madd  $ac2,        %[tmp1],       %[tmp_cs_re] \n\t"

                    "lw    %[tmp3],     0(%[x_temp_3])              \n\t"
                    "lw    %[tmp1],     4(%[x_temp_3])              \n\t"

                    "mfhi  %[vi],       $ac0                        \n\t"
                    "mfhi  %[vr],       $ac1                        \n\t"
                    "mfhi  %[vr1],      $ac3                        \n\t"
                    "mfhi  %[vi1],      $ac2                        \n\t"

                    "sra   %[tmp4],     %[tmp4],       1            \n\t"
                    "sra   %[tmp2],     %[tmp2],       1            \n\t"
                    "sra   %[tmp3],     %[tmp3],       1            \n\t"
                    "sra   %[tmp1],     %[tmp1],       1            \n\t"

                    "addu  %[tmp5],     %[tmp2],       %[vi]        \n\t"
                    "subu  %[tmp2],     %[tmp2],       %[vi]        \n\t"
                    "addu  %[tmp6],     %[tmp4],       %[vr]        \n\t"
                    "subu  %[tmp4],     %[tmp4],       %[vr]        \n\t"

                    "sw    %[tmp5],     4(%[x_temp_2])              \n\t"
                    "sw    %[tmp2],     4(%[x_temp_1])              \n\t"
                    "sw    %[tmp6],     0(%[x_temp_1])              \n\t"
                    "sw    %[tmp4],     0(%[x_temp_2])              \n\t"

                    "addu  %[tmp5],     %[tmp1],       %[vi1]       \n\t"
                    "addu  %[tmp6],     %[tmp3],       %[vr1]       \n\t"
                    "subu  %[tmp1],     %[tmp1],       %[vi1]       \n\t"
                    "subu  %[tmp3],     %[tmp3],       %[vr1]       \n\t"

                    "sw    %[tmp5],     4(%[x_temp_4])              \n\t"
                    "sw    %[tmp6],     0(%[x_temp_4])              \n\t"
                    "sw    %[tmp1],     4(%[x_temp_3])              \n\t"
                    "sw    %[tmp3],     0(%[x_temp_3])              \n\t"

                    : [vi] "=&r" (vi), [vr] "=&r" (vr),
                      [vi1] "=&r" (vi1), [vr1] "=&r" (vr1),
                      [tmp3] "=&r" (tmp3), [tmp4] "=&r" (tmp4),
                      [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2),
                      [tmp5] "=&r" (tmp5), [tmp6] "=&r" (tmp6),
                      [x_temp_1] "+r" (x_temp_1), [x_temp_2] "+r" (x_temp_2),
                      [x_temp_3] "+r" (x_temp_3), [x_temp_4] "+r" (x_temp_4)
                    : [tmp_cs_im] "r" (tmp_cs_im), [tmp_cs_re] "r" (tmp_cs_re),
                      [r] "r" (r), [mh] "r" (mh), [j] "r" (j), [m] "r" (m), [x] "r" (x)
                    : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo",
                      "$ac3hi", "$ac3lo", "memory"
                );

             }
        }
        {
            j = mh/4;
            int tmp_STC = 0x5a82799a;

            for(r=0; r<n; r+=m)
            {
                INT t1 = (r+j)<<1;
                INT t2 = t1 + (m);
                FIXP_DBL vr,vi,ur,ui;
                FIXP_DBL vr1,vi1,ur1,ui1;
                int t_1_moj = t1 + mh;
                int t_2_moj = t_1_moj + m;

                FIXP_DBL *x_temp_1 = x + t1;
                FIXP_DBL *x_temp_2 = x + t2;
                FIXP_DBL *x_temp_3 = x + t_1_moj;
                FIXP_DBL *x_temp_4 = x + t_2_moj;

                FIXP_DBL tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;

                __asm__ volatile(
                    "lw    %[tmp3],  4(%[x_temp_2])              \n\t"
                    "lw    %[tmp1],  4(%[x_temp_4])              \n\t"
                    "lw    %[tmp4],  0(%[x_temp_2])              \n\t"
                    "lw    %[tmp2],  0(%[x_temp_4])              \n\t"

                    "mult  $ac0,     %[tmp3],      %[tmp_STC]    \n\t"
                    "mult  $ac1,     %[tmp3],      %[tmp_STC]    \n\t"
                    "mult  $ac3,     %[tmp1],      %[tmp_STC]    \n\t"
                    "mult  $ac2,     %[tmp1],      %[tmp_STC]    \n\t"

                    "lw    %[tmp3],  0(%[x_temp_1])              \n\t"
                    "lw    %[tmp1],  4(%[x_temp_1])              \n\t"

                    "msub  $ac0,     %[tmp4],      %[tmp_STC]    \n\t"
                    "madd  $ac1,     %[tmp4],      %[tmp_STC]    \n\t"
                    "msub  $ac3,     %[tmp2],      %[tmp_STC]    \n\t"
                    "madd  $ac2,     %[tmp2],      %[tmp_STC]    \n\t"

                    "lw    %[tmp4],  0(%[x_temp_3])              \n\t"
                    "lw    %[tmp2],  4(%[x_temp_3])              \n\t"

                    "mfhi  %[vi],    $ac0                        \n\t"
                    "mfhi  %[vr],    $ac1                        \n\t"
                    "mfhi  %[vr1],   $ac3                        \n\t"
                    "mfhi  %[vi1],   $ac2                        \n\t"

                    "sra   %[tmp3],  %[tmp3],      1             \n\t"
                    "sra   %[tmp1],  %[tmp1],      1             \n\t"
                    "sra   %[tmp4],  %[tmp4],      1             \n\t"
                    "sra   %[tmp2],  %[tmp2],      1             \n\t"

                    "addu  %[tmp5],  %[tmp1],      %[vi]         \n\t"
                    "subu  %[tmp1],  %[tmp1],      %[vi]         \n\t"
                    "addu  %[tmp6],  %[tmp3],      %[vr]         \n\t"
                    "subu  %[tmp3],  %[tmp3],      %[vr]         \n\t"

                    "sw    %[tmp5],  4(%[x_temp_1])              \n\t"
                    "sw    %[tmp1],  4(%[x_temp_2])              \n\t"
                    "sw    %[tmp6],  0(%[x_temp_1])              \n\t"
                    "sw    %[tmp3],  0(%[x_temp_2])              \n\t"

                    "addu  %[vi],    %[tmp4],      %[vr1]        \n\t"
                    "subu  %[tmp4],  %[tmp4],      %[vr1]        \n\t"
                    "addu  %[vr],    %[tmp2],      %[vi1]        \n\t"
                    "subu  %[tmp2],  %[tmp2],      %[vi1]        \n\t"

                    "sw    %[vi],    0(%[x_temp_3])              \n\t"
                    "sw    %[tmp4],  0(%[x_temp_4])              \n\t"
                    "sw    %[vr],    4(%[x_temp_4])              \n\t"
                    "sw    %[tmp2],  4(%[x_temp_3])              \n\t"

                    : [vi] "=&r" (vi), [vr] "=&r" (vr),
                      [vi1] "=&r" (vi1), [vr1] "=&r" (vr1),
                      [tmp3] "=&r" (tmp3), [tmp4] "=&r" (tmp4),
                      [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2),
                      [tmp5] "=&r" (tmp5), [tmp6] "=&r" (tmp6)
                    : [tmp_STC] "r" (tmp_STC),
                      [x_temp_1] "r" (x_temp_1), [x_temp_2] "r" (x_temp_2),
                      [x_temp_3] "r" (x_temp_3), [x_temp_4] "r" (x_temp_4)
                    : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo",
                      "$ac3hi", "$ac3lo", "memory"
                );
            }
        }
    }
}

#endif /* #ifdef FUNCTION_dit_fft */
