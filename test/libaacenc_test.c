/*
 * Copyright (C) 2014 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* 
 * function: simple AAC encoder
 *
 * Takes a PCM WAV file, encodes, and writes to AAC sound file. Output format
 * is fixed at stereo, 128000 sample rate, AAC-LC, and ADTS foramt.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <aacenc_lib.h>
#include <wav_file.h>

INT_PCM wav_buf[4096];
UCHAR aac_buf[2048];

struct timespec timespec_diff(struct timespec *start, struct timespec *end)
{
	struct timespec diff;

	if ((end->tv_nsec-start->tv_nsec) < 0) {
		diff.tv_sec=end->tv_sec-start->tv_sec-1;
		diff.tv_nsec=1000000000+end->tv_nsec-start->tv_nsec;
	} else {
		diff.tv_sec=end->tv_sec-start->tv_sec;
		diff.tv_nsec=end->tv_nsec-start->tv_nsec;
	}
	return diff;
}


static int aac_enc_init(HANDLE_AACENCODER *encoder, WAV_HEADER *wav_header)
{
	if (AACENC_OK != aacEncOpen(encoder, 0, wav_header->numChannels)) {
		fprintf(stderr, "Failed to open AAC encoder\n");
		return -1;
	}

	if (AACENC_OK != aacEncoder_SetParam(*encoder, AACENC_AOT, AOT_AAC_LC)) {
		fprintf(stderr, "Failed to set AAC encoder parameters\n");
		goto close_encoder;
	}
	if (AACENC_OK != aacEncoder_SetParam(*encoder, AACENC_BITRATE, 128000)) {
		fprintf(stderr, "Failed to set AAC encoder parameters\n");
		goto close_encoder;
	}
	if (AACENC_OK != aacEncoder_SetParam(*encoder, AACENC_SAMPLERATE,
						wav_header->sampleRate)) {
		fprintf(stderr, "Failed to set AAC encoder parameters\n");
		goto close_encoder;
	}
	if (AACENC_OK != aacEncoder_SetParam(*encoder, AACENC_CHANNELMODE,
						MODE_2)) {
		fprintf(stderr, "Failed to set AAC encoder parameters\n");
		goto close_encoder;
	}
	if (AACENC_OK != aacEncoder_SetParam(*encoder, AACENC_TRANSMUX,
						TT_MP4_ADTS)) {
		fprintf(stderr, "Failed to set AAC encoder parameters\n");
		goto close_encoder;
	}
	if (AACENC_OK != aacEncEncode(*encoder, NULL, NULL, NULL, NULL)) {
		fprintf(stderr, "Failed to initalize AAC encoder parameters\n");
		goto close_encoder;
	}

	return 0;

close_encoder:
	aacEncClose(encoder);
	return -1;
}

static int aac_enc_encode(HANDLE_AACENCODER encoder, int samples)
{
	AACENC_InArgs inargs;
	AACENC_OutArgs outargs;
	memset(&inargs, 0, sizeof(inargs));
	memset(&outargs, 0, sizeof(outargs));
	inargs.numInSamples = samples;

	void *inBuffer[]     = { wav_buf };
	INT inBufferIds[]    = { IN_AUDIO_DATA };
	INT inBufferSize[]   = { sizeof(wav_buf) };
	INT inBufferElSize[] = { sizeof(INT_PCM) };

	AACENC_BufDesc inBufDesc = { 0 };
	inBufDesc.numBufs           = sizeof(inBuffer) / sizeof(void *);
	inBufDesc.bufs              = (void **)&inBuffer;
	inBufDesc.bufferIdentifiers = inBufferIds;
	inBufDesc.bufSizes          = inBufferSize;
	inBufDesc.bufElSizes        = inBufferElSize;

	UCHAR *out_ptr = aac_buf;
	INT out_avail = sizeof(aac_buf);

	void* outBuffer[]       = { out_ptr };
	INT   outBufferIds[]    = { OUT_BITSTREAM_DATA };
	INT   outBufferSize[]   = { 0 };
	INT   outBufferElSize[] = { sizeof(UCHAR) };

	AACENC_BufDesc outBufDesc = { 0 };
	outBufDesc.numBufs           = sizeof(outBuffer) / sizeof(void*);
	outBufDesc.bufs              = (void**)&outBuffer;
	outBufDesc.bufferIdentifiers = outBufferIds;
	outBufDesc.bufSizes          = outBufferSize;
	outBufDesc.bufElSizes        = outBufferElSize;

	AACENC_ERROR err;
	INT out_bytes = 0;
	do {
		memset(&outargs, 0, sizeof(outargs));
		outBuffer[0] = out_ptr;
		outBufferSize[0] = out_avail - out_bytes;

		err = aacEncEncode(encoder, &inBufDesc, &outBufDesc, &inargs,
					&outargs);
		if (err == AACENC_OK) {
			out_ptr += outargs.numOutBytes;
			out_bytes += outargs.numOutBytes;

			if (outargs.numInSamples > 0) {
				int numRemainingSamples = inargs.numInSamples -
							outargs.numInSamples;
				if (numRemainingSamples > 0) {
					memmove(wav_buf,
						&wav_buf[outargs.numInSamples],
						sizeof(INT_PCM) *
						numRemainingSamples);
				}
				inargs.numInSamples -= outargs.numInSamples;
			}
		}
	} while (err == AACENC_OK && inargs.numInSamples > 0);

	return out_bytes;
}

/*
 * aac_encode - Encode a PCM file into an AAC file
 */
static int aac_encode(HANDLE_WAV wav_file, FILE *aac_file)
{
	HANDLE_AACENCODER encoder;

	if ((wav_file == NULL) || (aac_file == NULL))
		return -1;

	/* Initialize the encoder */
	if (aac_enc_init(&encoder, &wav_file->header)) {
		return -1;
	}

	/* Allocate AAC buffer here */
	AACENC_InfoStruct enc_info;
	if (AACENC_OK != aacEncInfo(encoder, &enc_info)) {
		fprintf(stderr, "Failed to get encoder info\n");
		aacEncClose(&encoder);
		return -1;
	} else {
		printf("Encoder information:\n");
		printf("\tmaxOutBufBytes: %u\n", enc_info.maxOutBufBytes);
		printf("\tmaxAncBytes: %u\n", enc_info.maxAncBytes);
		printf("\tinBufFillLevel: %u\n", enc_info.inBufFillLevel);
		printf("\tinputChannels: %u\n", enc_info.inputChannels);
		printf("\tframeLength: %u\n", enc_info.frameLength);
		printf("\tencoderDelay: %u\n", enc_info.encoderDelay);
	}

	/* Main encoding loop */
	while (1) {
		static int total_bytes = 0;
		/* Note: frame = sample (16 bit) * numChannels */
		static int total_samples = 0;
		int samples = WAV_InputRead(wav_file, wav_buf,
					FDKmin(enc_info.inputChannels *
						enc_info.frameLength,
						sizeof(wav_buf) /
						sizeof(INT_PCM)),
					SAMPLE_BITS);
		if (samples == 0) {
			printf("Total samples extracted %d\n", total_samples);
			printf("Total encoded bytes %d\n", total_bytes);
			break;
		}

		int bytes = aac_enc_encode(encoder, samples);
		fwrite(aac_buf, 1, bytes, aac_file);

		total_samples += samples;
		total_bytes += bytes;
	}

	aacEncClose(&encoder);

	return 0;
}

int main(int argc, const char **argv)
{
	int ret = -1;
	HANDLE_WAV wav_file;
	FILE *aac_file;

	if (argc != 3) {
		fprintf(stderr, "Usage %s <input_pcm_wav_file> <output_raw_aac_file>\n",
			argv[0]);
		goto out;
	}

	/* Open the input file */
	if (WAV_InputOpen(&wav_file, argv[1])){
		fprintf(stderr, "Invalid input file %s\n", argv[1]);
		goto out;
	}

	printf("Input file information:\n");
	printf("\tsample rate: %d\n", wav_file->header.sampleRate);
	printf("\tchannels: %d\n", wav_file->header.numChannels);
	printf("\tbits per sample: %d\n", wav_file->header.bitsPerSample);
	printf("\tdata size: %d\n", wav_file->header.dataSize);

	/* Open the output file */
	aac_file = fopen(argv[2], "wb");
	if (!aac_file) {
		fprintf(stderr, "Failed to open output file %s\n", argv[2]);
		goto close_infile;
	}

	/* Track the total time it took to decode */
	struct timespec start_time;
	struct timespec end_time;
	int track_time = 1;
	if (clock_gettime(CLOCK_MONOTONIC, &start_time)) {
		fprintf(stderr, "Failed to get start time for profiling\n");
		track_time = 0;
	}

	ret = aac_encode(wav_file, aac_file);

	if (!ret && track_time) {
		if (clock_gettime(CLOCK_MONOTONIC,&end_time)) {
			printf("Failed to get end time for profiling\n");
		} else {
			struct timespec diff=timespec_diff(&start_time,&end_time);
			printf("Total encode time: %ld.%09ld seconds\n",
				diff.tv_sec, diff.tv_nsec);
		}
	}

	/* Clean up */
	fclose(aac_file);
close_infile:
	WAV_InputClose(&wav_file);
out:
	return ret;
}
