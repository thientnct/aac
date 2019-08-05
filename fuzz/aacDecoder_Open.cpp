#include "aacdecoder_lib.h"
#include <stdint.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  HANDLE_AACDECODER aacDecoderInfo = NULL;

  TRANSPORT_TYPE transportType;
  INT nrOfLayers;

  if (Size != 8) return 0;

  transportType = (TRANSPORT_TYPE)(Data[0] + (Data[1] << 8) + (Data[2] << 16) +
                                   (Data[3] << 24));
  nrOfLayers =
      (UINT)(Data[4] + (Data[5] << 8) + (Data[6] << 16) + (Data[7] << 24));

  aacDecoderInfo = aacDecoder_Open(transportType, nrOfLayers);
  if (aacDecoderInfo != NULL) {
    aacDecoder_Close(aacDecoderInfo);
  }
  return 0;
}
