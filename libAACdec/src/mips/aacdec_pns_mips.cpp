
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
   Description: perceptual noise substitution tool

******************************************************************************/


#if defined(MIPS_DSP_R2_LE)
#define FUNCTION_GenerateRandomVector
#endif /* #if defined(MIPS_DSP_R2_LE) */

#if defined(MIPS_DSP_R1_LE)
#define FUNCTION_ScaleBand
#endif /* #if defined(MIPS_DSP_R1_LE) */

#if defined (FUNCTION_GenerateRandomVector)
/**
 * \brief Generate a vector of noise of given length. The noise values are
 *        scaled in order to yield a noise energy of 1.0
 * \param spec pointer to were the noise values will be written to.
 * \param size amount of noise values to be generated.
 * \param pRandomState pointer to the state of the random generator being used.
 * \return exponent of generated noise vector.
 */
int GenerateRandomVector (FIXP_DBL *RESTRICT spec,
                          int size,
                          int *pRandomState)
{
  int i, invNrg_e = 0, nrg_e = 0;
  FIXP_DBL invNrg_m, nrg_m = FL2FXCONST_DBL(0.0f) ;
  FIXP_DBL *RESTRICT ptr = spec;
  int randomState = *pRandomState;

#define GEN_NOISE_NRG_SCALE 7
#define STRINGIFY_(a) #a
#define STRINGIFY(a) STRINGIFY_(a)

  int randomState1, randomState2, randomState3;
  FIXP_DBL temp0, temp1, temp2, temp3;
  FIXP_DBL in0 = 1664525L;
  FIXP_DBL in1 = 1013904223L;

  /* Generate noise and calculate energy. */
  if(size>3){
      randomState1 = (1664525L * randomState);
      for (i=0; i<size; i+=4)
      {
        __asm__ volatile (
          "addu      %[randomState1],  %[in1],           %[randomState1]                 \n\t"
          "mul       %[randomState2],  %[in0],           %[randomState1]                 \n\t"
          "sra       %[temp0],         %[randomState1]," STRINGIFY(GEN_NOISE_NRG_SCALE) "\n\t"
          "sw        %[randomState1],  0(%[ptr])                                         \n\t"
          "mulq_s.w  %[temp0],         %[temp0],         %[temp0]                        \n\t"

          "addu      %[randomState2],  %[in1],           %[randomState2]                 \n\t"
          "mul       %[randomState3],  %[in0],           %[randomState2]                 \n\t"
          "sra       %[temp1],         %[randomState2]," STRINGIFY(GEN_NOISE_NRG_SCALE) "\n\t"
          "sw        %[randomState2],  4(%[ptr])                                         \n\t"
          "mulq_s.w  %[temp1],         %[temp1],         %[temp1]                        \n\t"

          "addu      %[randomState3],  %[in1],           %[randomState3]                 \n\t"
          "mul       %[randomState],   %[in0],           %[randomState3]                 \n\t"
          "sra       %[temp2],         %[randomState3]," STRINGIFY(GEN_NOISE_NRG_SCALE) "\n\t"
          "sw        %[randomState3],  8(%[ptr])                                         \n\t"
          "mulq_s.w  %[temp2],         %[temp2],         %[temp2]                        \n\t"

          "addu      %[randomState],   %[in1],           %[randomState]                  \n\t"
          "sra       %[temp3],         %[randomState],"  STRINGIFY(GEN_NOISE_NRG_SCALE) "\n\t"
          "sw        %[randomState],   12(%[ptr])                                        \n\t"
          "mulq_s.w  %[temp3],         %[temp3],         %[temp3]                        \n\t"
          "mul       %[randomState1],  %[in0],           %[randomState]                  \n\t"

          "addiu     %[ptr],           %[ptr],           16                              \n\t"
          "sra       %[temp0],         %[temp0],         0x1                             \n\t"
          "sra       %[temp1],         %[temp1],         0x1                             \n\t"
          "sra       %[temp2],         %[temp2],         0x1                             \n\t"
          "sra       %[temp3],         %[temp3],         0x1                             \n\t"
          "addu      %[nrg_m],         %[nrg_m],         %[temp0]                        \n\t"
          "addu      %[nrg_m],         %[nrg_m],         %[temp1]                        \n\t"
          "addu      %[nrg_m],         %[nrg_m],         %[temp2]                        \n\t"
          "addu      %[nrg_m],         %[nrg_m],         %[temp3]                        \n\t"

          : [temp0] "=&r" (temp0), [temp1] "=&r" (temp1),
            [temp2] "=&r" (temp2), [temp3] "=&r" (temp3),
            [randomState] "=&r" (randomState), [randomState1] "+r" (randomState1),
            [randomState2] "=&r" (randomState2), [randomState3] "=&r" (randomState3),
            [ptr] "+r" (ptr), [nrg_m] "+r" (nrg_m)
          : [in0] "r" (in0), [in1] "r" (in1)
          : "memory", "hi", "lo"
        );
      }

      for (i=0; i<size%4; i++)
      {
        randomState = (1664525L * randomState) + 1013904223L; // Numerical Recipes
        nrg_m = fPow2AddDiv2(nrg_m, (FIXP_DBL)randomState>>GEN_NOISE_NRG_SCALE);
        *ptr++ = (FIXP_DBL)randomState;
      }
  }
  else{
    for (i=0; i<size; i++)
    {
      randomState = (1664525L * randomState) + 1013904223L; // Numerical Recipes
      nrg_m = fPow2AddDiv2(nrg_m, (FIXP_DBL)randomState>>GEN_NOISE_NRG_SCALE);
      *ptr++ = (FIXP_DBL)randomState;
    }
  }

  nrg_e = GEN_NOISE_NRG_SCALE*2 + 1;

  /* weight noise with = 1 / sqrt_nrg; */
  invNrg_m = invSqrtNorm2(nrg_m<<1, &invNrg_e);
  invNrg_e += -((nrg_e-1)>>1);

  int const1 = 0xfffffffe;
  unsigned int min_loop = 3;
  i = size-1;
  if(size>3){
    __asm__ volatile (
      "sll       %[temp1],    %[i],           0x2         \n\t"
      "addu      %[spec],     %[spec],        %[temp1]    \n\t"
    "1:                                                   \n\t"
      "lw        %[temp0],    0(%[spec])                  \n\t"
      "lw        %[temp1],    -4(%[spec])                 \n\t"
      "lw        %[temp2],    -8(%[spec])                 \n\t"
      "lw        %[temp3],    -12(%[spec])                \n\t"
      "mulq_s.w  %[temp0],    %[temp0],       %[invNrg_m] \n\t"
      "mulq_s.w  %[temp1],    %[temp1],       %[invNrg_m] \n\t"
      "mulq_s.w  %[temp2],    %[temp2],       %[invNrg_m] \n\t"
      "mulq_s.w  %[temp3],    %[temp3],       %[invNrg_m] \n\t"
      "addiu     %[spec],     %[spec],        -16         \n\t"
      "addiu     %[i],        %[i],           -4          \n\t"
      "and       %[temp0],    %[temp0],       %[const1]   \n\t"
      "sw        %[temp0],    16(%[spec])                 \n\t"
      "and       %[temp1],    %[temp1],       %[const1]   \n\t"
      "sw        %[temp1],    12(%[spec])                 \n\t"
      "and       %[temp2],    %[temp2],       %[const1]   \n\t"
      "sw        %[temp2],    8(%[spec])                  \n\t"
      "and       %[temp3],    %[temp3],       %[const1]   \n\t"
      "sw        %[temp3],    4(%[spec])                  \n\t"
      "bge       %[i],        %[min_loop],    1b          \n\t"

      : [temp0] "=&r" (temp0), [temp1] "=&r" (temp1),
        [temp2] "=&r" (temp2), [temp3] "=&r" (temp3),
        [i] "+r" (i), [spec] "+r" (spec)
      : [invNrg_m] "r" (invNrg_m), [const1] "r" (const1),
        [min_loop] "r" (min_loop)
      : "memory", "hi", "lo"
    );

    spec++;
    if(i > 0){
      for (; i--; )
      {
        spec[i] = fMult(spec[i], invNrg_m);
      }
    }

  }
  else{
    for (i=size; i--; )
    {
      spec[i] = fMult(spec[i], invNrg_m);
    }
  }

  /* Store random state */
  *pRandomState = randomState;

  return invNrg_e;
}
#endif /* #if defined (FUNCTION_GenerateRandomVector) */


#if defined (FUNCTION_ScaleBand)
void ScaleBand (FIXP_DBL *RESTRICT spec, int size, int scaleFactor, int specScale, int noise_e, int out_of_phase)
{
  int i, shift, sfExponent;
  FIXP_DBL sfMatissa;

  /* Get gain from scale factor value = 2^(scaleFactor * 0.25) */
  sfMatissa = MantissaTable[scaleFactor & 0x03][0];
  /* sfExponent = (scaleFactor >> 2) + ExponentTable[scaleFactor & 0x03][0]; */
  /* Note:  ExponentTable[scaleFactor & 0x03][0] is always 1. */
  sfExponent = (scaleFactor >> 2) + 1;

  if (out_of_phase != 0) {
    sfMatissa = -sfMatissa;
  }

  /* +1 because of fMultDiv2 below. */
  shift = sfExponent - specScale + 1 + noise_e;

  /* Apply gain to noise values */
  if (shift>=0) {
    shift = fixMin( shift, DFRACT_BITS-1 );
    FIXP_DBL tmp1,tmp2,tmp3,tmp4;
    FIXP_DBL *temp_pok = &spec[size-1];

    for (i = size ; i>0;i-=4) {
      __asm__ volatile(
        "lw    %[tmp1],     0(%[temp_pok])              \n\t"
        "lw    %[tmp2],     -4(%[temp_pok])             \n\t"
        "lw    %[tmp3],     -8(%[temp_pok])             \n\t"
        "lw    %[tmp4],     -12(%[temp_pok])            \n\t"

        "mult  $ac0,        %[tmp1],       %[sfMatissa] \n\t"
        "mult  $ac1,        %[tmp2],       %[sfMatissa] \n\t"
        "mult  $ac2,        %[tmp3],       %[sfMatissa] \n\t"
        "mult  $ac3,        %[tmp4],       %[sfMatissa] \n\t"

        "mfhi  %[tmp1],     $ac0                        \n\t"
        "mfhi  %[tmp2],     $ac1                        \n\t"
        "mfhi  %[tmp3],     $ac2                        \n\t"
        "mfhi  %[tmp4],     $ac3                        \n\t"

        "sllv  %[tmp1],     %[tmp1],       %[shift]     \n\t"
        "sllv  %[tmp2],     %[tmp2],       %[shift]     \n\t"
        "sllv  %[tmp3],     %[tmp3],       %[shift]     \n\t"
        "sllv  %[tmp4],     %[tmp4],       %[shift]     \n\t"

        "sw    %[tmp1],     0(%[temp_pok])              \n\t"
        "sw    %[tmp2],     -4(%[temp_pok])             \n\t"
        "sw    %[tmp3],     -8(%[temp_pok])             \n\t"
        "sw    %[tmp4],     -12(%[temp_pok])            \n\t"

        "addiu %[temp_pok], %[temp_pok],   -16          \n\t"

        : [tmp3] "=&r" (tmp3), [tmp4] "=&r" (tmp4),
          [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2),
          [temp_pok] "+r" (temp_pok)
        : [sfMatissa] "r" (sfMatissa), [shift] "r" (shift)
        : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo",
          "$ac3hi", "$ac3lo", "memory"
     );
    }
  } else {
    shift = fixMin( -shift, DFRACT_BITS-1 );
    FIXP_DBL tmp1,tmp2,tmp3,tmp4;
    FIXP_DBL *temp_pok = &spec[size-1];

    for (i = size ; i>0;i-=4) {
      __asm__ volatile(
        "lw    %[tmp1],     0(%[temp_pok])              \n\t"
        "lw    %[tmp2],     -4(%[temp_pok])             \n\t"
        "lw    %[tmp3],     -8(%[temp_pok])             \n\t"
        "lw    %[tmp4],     -12(%[temp_pok])            \n\t"

        "mult  $ac0,        %[tmp1],       %[sfMatissa] \n\t"
        "mult  $ac1,        %[tmp2],       %[sfMatissa] \n\t"
        "mult  $ac2,        %[tmp3],       %[sfMatissa] \n\t"
        "mult  $ac3,        %[tmp4],       %[sfMatissa] \n\t"

        "mfhi  %[tmp1],     $ac0                        \n\t"
        "mfhi  %[tmp2],     $ac1                        \n\t"
        "mfhi  %[tmp3],     $ac2                        \n\t"
        "mfhi  %[tmp4],     $ac3                        \n\t"

        "srav  %[tmp1],     %[tmp1],       %[shift]     \n\t"
        "srav  %[tmp2],     %[tmp2],       %[shift]     \n\t"
        "srav  %[tmp3],     %[tmp3],       %[shift]     \n\t"
        "srav  %[tmp4],     %[tmp4],       %[shift]     \n\t"

        "sw    %[tmp1],     0(%[temp_pok])              \n\t"
        "sw    %[tmp2],     -4(%[temp_pok])             \n\t"
        "sw    %[tmp3],     -8(%[temp_pok])             \n\t"
        "sw    %[tmp4],     -12(%[temp_pok])            \n\t"

        "addiu %[temp_pok], %[temp_pok],   -16          \n\t"

        : [tmp3] "=&r" (tmp3), [tmp4] "=&r" (tmp4),
          [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2),
          [temp_pok] "+r" (temp_pok)
        : [sfMatissa] "r" (sfMatissa), [shift] "r" (shift)
        : "hi", "lo", "$ac1hi", "$ac1lo", "$ac2hi", "$ac2lo",
          "$ac3hi", "$ac3lo", "memory"
     );
    }
  }
}
#endif /* #if defined (FUNCTION_ScaleBand) */
