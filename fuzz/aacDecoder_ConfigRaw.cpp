#include "aacdecoder_lib.h"
#include <stdint.h>

#define FILEREAD_MAX_LAYERS 1

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  HANDLE_AACDECODER aacDecoderInfo = NULL;

  UCHAR *conf[FILEREAD_MAX_LAYERS];
  UINT confSize[FILEREAD_MAX_LAYERS];

  if (Size > 255) return 0;

  aacDecoderInfo = aacDecoder_Open(TT_MP4_ADIF, FILEREAD_MAX_LAYERS);
  FDK_ASSERT(aacDecoderInfo != NULL);

  for (UINT layer = 0; layer < FILEREAD_MAX_LAYERS; layer++) {
    conf[layer] = const_cast<UCHAR *>(Data);
    confSize[layer] = Size;
  }

  aacDecoder_ConfigRaw(aacDecoderInfo, conf, confSize);
  aacDecoder_Close(aacDecoderInfo);
  return 0;
}
