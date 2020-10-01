/******************************************************************************
 *
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *****************************************************************************
 * Originally developed and contributed by Ittiam Systems Pvt. Ltd, Bangalore
 */

#include <string>
#include "aacenc_lib.h"

using namespace std;

constexpr uint32_t kMinBitRate = 8000;
constexpr uint32_t kMaxBitRate = 960000;

constexpr uint32_t kSampleRates[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};
constexpr size_t kSampleRatesSize = size(kSampleRates);

constexpr CHANNEL_MODE kChannelModes[] = {MODE_1,     MODE_2,       MODE_1_2,       MODE_1_2_1,
                                          MODE_1_2_2, MODE_1_2_2_1, MODE_1_2_2_2_1, MODE_7_1_BACK};
constexpr size_t kChannelModesSize = size(kChannelModes);

constexpr TRANSPORT_TYPE kIdentifiers[] = {
    TT_MP4_RAW, TT_MP4_ADIF, TT_MP4_ADTS, TT_MP4_LATM_MCP1, TT_MP4_LATM_MCP0, TT_MP4_LOAS, TT_DRM};
constexpr size_t kIdentifiersSize = size(kIdentifiers);

constexpr AUDIO_OBJECT_TYPE kAudioObjectTypes[] = {AOT_AAC_LC, AOT_ER_AAC_ELD, AOT_SBR, AOT_PS,
                                                   AOT_ER_AAC_LD};
constexpr size_t kAudioObjectTypesSize = size(kAudioObjectTypes);

constexpr int32_t kSbrRatios[] = {0, 1, 2};
constexpr size_t kSbrRatiosSize = size(kSbrRatios);

enum {
    IDX_SBR_MODE = 0,
    IDX_AAC_AOT,
    IDX_SAMPLE_RATE,
    IDX_BIT_RATE_1,
    IDX_BIT_RATE_2,
    IDX_BIT_RATE_3,
    IDX_CHANNEL,
    IDX_IDENTIFIER,
    IDX_SBR_RATIO,
    IDX_MAX_BUFFERS,
    IDX_LAST
};

enum aac_sbr_mode_t : uint32_t {
    AAC_SBR_OFF,
    AAC_SBR_SINGLE_RATE,
    AAC_SBR_DUAL_RATE,
    AAC_SBR_AUTO
};

template <typename type1, typename type2, typename type3>
auto generateNumberInRangeFromData(type1 data, type2 min, type3 max) -> decltype(max) {
    return (data % (1 + max - min)) + min;
}

class Codec {
   public:
    ~Codec() { deInitEncoder(); }
    bool initEncoder(uint8_t **dataPtr, size_t *sizePtr);
    void encodeFrames(const uint8_t *data, size_t size);
    void deInitEncoder();

   private:
    HANDLE_AACENCODER mEncoder = nullptr;
    int32_t mMaxBuffers = 0;
};

bool Codec::initEncoder(uint8_t **dataPtr, size_t *sizePtr) {
    uint8_t *data = *dataPtr;

    if (AACENC_OK != aacEncOpen(&mEncoder, 0, 0)) {
        return false;
    }

    uint32_t sbrMode = generateNumberInRangeFromData(data[IDX_SBR_MODE], (uint32_t)AAC_SBR_OFF,
                                                     (uint32_t)AAC_SBR_AUTO);
    aacEncoder_SetParam(mEncoder, AACENC_SBR_MODE, sbrMode);

    uint32_t sbrRatioIndex =
        generateNumberInRangeFromData(data[IDX_SBR_RATIO], 0, kSbrRatiosSize - 1);
    int32_t sbrRatio = kSbrRatios[sbrRatioIndex];
    aacEncoder_SetParam(mEncoder, AACENC_SBR_RATIO, sbrRatio);

    uint32_t aacAOTIndex =
        generateNumberInRangeFromData(data[IDX_AAC_AOT], 0, kAudioObjectTypesSize - 1);
    uint32_t aacAOT = kAudioObjectTypes[aacAOTIndex];
    aacEncoder_SetParam(mEncoder, AACENC_AOT, aacAOT);

    uint32_t sampleRateIndex =
        generateNumberInRangeFromData(data[IDX_SAMPLE_RATE], 0, kSampleRatesSize - 1);
    uint32_t sampleRate = kSampleRates[sampleRateIndex];
    aacEncoder_SetParam(mEncoder, AACENC_SAMPLERATE, sampleRate);

    uint32_t tempValue =
        (data[IDX_BIT_RATE_1] << 16) | (data[IDX_BIT_RATE_2] << 8) | data[IDX_BIT_RATE_3];
    uint32_t bitRate = generateNumberInRangeFromData(tempValue, kMinBitRate, kMaxBitRate);
    aacEncoder_SetParam(mEncoder, AACENC_BITRATE, bitRate);

    uint32_t channelModeIndex =
        generateNumberInRangeFromData(data[IDX_CHANNEL], 0, kChannelModesSize - 1);
    CHANNEL_MODE channelMode = kChannelModes[channelModeIndex];
    aacEncoder_SetParam(mEncoder, AACENC_CHANNELMODE, channelMode);

    uint32_t identifierIndex =
        generateNumberInRangeFromData(data[IDX_IDENTIFIER], 0, kIdentifiersSize - 1);
    uint32_t identifier = kIdentifiers[identifierIndex];
    aacEncoder_SetParam(mEncoder, AACENC_TRANSMUX, identifier);

    mMaxBuffers = generateNumberInRangeFromData(data[IDX_MAX_BUFFERS], 1, UINT8_MAX);

    // Not re-using the data which was used for configuration for encoding
    *dataPtr += IDX_LAST;
    *sizePtr -= IDX_LAST;

    return true;
}

void Codec::encodeFrames(const uint8_t *data, size_t size) {
    while (size > 0) {
        AACENC_InArgs inargs;
        memset(&inargs, 0, sizeof(inargs));
        inargs.numInSamples = size / sizeof(int16_t);

        void *inBuffer[mMaxBuffers];
        INT inBufferIds[mMaxBuffers];
        INT inBufferSize[mMaxBuffers];
        INT inBufferElSize[mMaxBuffers];

        for (int32_t buffer = 0; buffer < mMaxBuffers; ++buffer) {
            inBuffer[buffer] = (unsigned char *)data;
            inBufferIds[buffer] = IN_AUDIO_DATA;
            inBufferSize[buffer] = size;
            inBufferElSize[buffer] = sizeof(int16_t);
        }

        AACENC_BufDesc inBufDesc;
        inBufDesc.numBufs = mMaxBuffers;
        inBufDesc.bufs = (void **)&inBuffer;
        inBufDesc.bufferIdentifiers = inBufferIds;
        inBufDesc.bufSizes = inBufferSize;
        inBufDesc.bufElSizes = inBufferElSize;

        uint8_t outPtr[size];
        void *outBuffer[mMaxBuffers];
        INT outBufferIds[mMaxBuffers];
        INT outBufferSize[mMaxBuffers];
        INT outBufferElSize[mMaxBuffers];

        for (int32_t buffer = 0; buffer < mMaxBuffers; ++buffer) {
            outBuffer[buffer] = outPtr;
            outBufferIds[buffer] = OUT_BITSTREAM_DATA;
            outBufferSize[buffer] = (INT)size;
            outBufferElSize[buffer] = sizeof(UCHAR);
        }

        AACENC_BufDesc outBufDesc;
        outBufDesc.numBufs = mMaxBuffers;
        outBufDesc.bufs = (void **)&outBuffer;
        outBufDesc.bufferIdentifiers = outBufferIds;
        outBufDesc.bufSizes = outBufferSize;
        outBufDesc.bufElSizes = outBufferElSize;

        AACENC_OutArgs outargs = {};
        aacEncEncode(mEncoder, &inBufDesc, &outBufDesc, &inargs, &outargs);

        if ((outargs.numOutBytes == 0) && (size > 0)) {
            ++data;
            --size;
        } else {
            size_t consumed = outargs.numInSamples * sizeof(int16_t);
            data += consumed;
            size -= consumed;
        }
    }
}

void Codec::deInitEncoder() { aacEncClose(&mEncoder); }

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < IDX_LAST) {
        return 0;
    }
    Codec encoder;
    if (encoder.initEncoder(const_cast<uint8_t **>(&data), &size)) {
        encoder.encodeFrames(data, size);
    }
    return 0;
}
