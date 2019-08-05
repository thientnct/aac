#include "aacdecoder_lib.h"
#include <stdint.h>

#define FILEREAD_MAX_LAYERS 1
#define OUT_BUF_SIZE (8 * 2048 * 4)

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  HANDLE_AACDECODER aacDecoderInfo = NULL;

  INT_PCM TimeData[OUT_BUF_SIZE];
  AAC_DECODER_ERROR err;
  aacDecoderInfo = aacDecoder_Open(TT_MP4_LOAS, FILEREAD_MAX_LAYERS);
  FDK_ASSERT(aacDecoderInfo != NULL);

  const uint8_t *start = Data;
  UINT valid, buffer_size;

  do {
    valid = buffer_size = Data + Size - start;
    err = aacDecoder_Fill(aacDecoderInfo, const_cast<UCHAR **>(&start),
                          &buffer_size, &valid);
    start += buffer_size - valid;
    if (err == AAC_DEC_OK) {
      do {
        err = aacDecoder_DecodeFrame(aacDecoderInfo, TimeData, OUT_BUF_SIZE, 0);
        if (err != AAC_DEC_OK && err != AAC_DEC_NOT_ENOUGH_BITS) {
          aacDecoder_Close(aacDecoderInfo);
          aacDecoderInfo = NULL;
          return 0;
        }
      } while (err != AAC_DEC_NOT_ENOUGH_BITS);
    }
  } while (valid > 0);
  aacDecoder_Close(aacDecoderInfo);
  aacDecoderInfo = NULL;
  return 0;
}
