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

#include <stdint.h>
#include <cmath>
#include <algorithm>
#include "aacdecoder_lib.h"

#include "../libAACdec/src/aacdecoder.h"
#include "../libMpegTPDec/src/tpdec_lib_struct.h"

constexpr uint8_t kNumberOfLayers = 1;
constexpr uint8_t kMaxChannelCount = 8;
constexpr uint32_t kMaxConfigurationSize = 1024;

UINT getHeaderSize(UCHAR *data, UINT size) {
  HANDLE_AACDECODER aacDecoderHandle = aacDecoder_Open(TT_MP4_ADIF, kNumberOfLayers);
  if (!aacDecoderHandle) {
    return 0;
  }

  HANDLE_TRANSPORTDEC hTp = aacDecoderHandle->hInput;
  FDK_BITSTREAM bs;
  UCHAR tmpConf[kMaxConfigurationSize] = {0};
  size = std::min(size, kMaxConfigurationSize);

  FDKmemcpy(tmpConf, data, size);
  FDKinitBitStream(&bs, tmpConf, kMaxConfigurationSize, size << 3, BS_READER);

  UINT ascStartAnchor = FDKgetValidBits(&bs);
  TRANSPORTDEC_ERROR err = AudioSpecificConfig_Parse(&hTp->asc[(1 * 1)], &bs, 1, &hTp->callbacks,
                                                     AC_CM_DET_CFG_CHANGE, 0, AOT_NULL_OBJECT);
  aacDecoder_Close(aacDecoderHandle);
  if (err != TRANSPORTDEC_OK) {
    return 0;
  }
  UINT ascEndAnchor = FDKgetValidBits(&bs);
  UINT headerLengthInBits = (ascStartAnchor - ascEndAnchor);
  UINT headerLengthInBytes = ceil(((double)headerLengthInBits / 8));
  return headerLengthInBytes;
}

class Codec {
 public:
  Codec() = default;
  ~Codec() { deInitDecoder(); }
  bool initDecoder();
  void decodeFrames(UCHAR *data, UINT size);
  void deInitDecoder();

 private:
  HANDLE_AACDECODER mAacDecoderHandle = nullptr;
  AAC_DECODER_ERROR mErrorCode = AAC_DEC_OK;
};

bool Codec::initDecoder() {
  mAacDecoderHandle = aacDecoder_Open(TT_MP4_ADIF, kNumberOfLayers);
  if (!mAacDecoderHandle) {
    return false;
  }
  return true;
}

void Codec::deInitDecoder() {
  aacDecoder_Close(mAacDecoderHandle);
  mAacDecoderHandle = nullptr;
}

void Codec::decodeFrames(UCHAR *data, UINT size) {
  UINT headerSize = getHeaderSize(data, size);
  if (headerSize != 0) {
    aacDecoder_ConfigRaw(mAacDecoderHandle, &data, &headerSize);
    data += headerSize;
    size -= headerSize;
  }
  while (size > 0) {
    UINT inputSize = size;
    UINT valid = size;
    mErrorCode = aacDecoder_Fill(mAacDecoderHandle, &data, &inputSize, &valid);
    if (mErrorCode != AAC_DEC_OK) {
      ++data;
      --size;
    } else {
      INT_PCM outputBuf[2048 * kMaxChannelCount];
      do {
        mErrorCode =
            aacDecoder_DecodeFrame(mAacDecoderHandle, outputBuf, 2048 * kMaxChannelCount, 0);
      } while (mErrorCode == AAC_DEC_OK || mErrorCode == AAC_DEC_NOT_ENOUGH_BITS);
      UINT offset = inputSize - valid;
      data += offset;
      size = valid;
    }
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  Codec *codec = new Codec();
  if (!codec) {
    return 0;
  }
  if (codec->initDecoder()) {
    codec->decodeFrames((UCHAR *)(data), static_cast<UINT>(size));
  }
  delete codec;
  return 0;
}
