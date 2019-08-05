#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "aacdecoder_lib.h"

const UINT timeDataSize = 1 << 14;
INT_PCM pTimeData[timeDataSize];

// This fuzz target is just a first quick-and-dirty attmpt,
// it will need to be significantly improved
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Size < 4) return 0;
  TRANSPORT_TYPE transportType = TT_MP4_RAW;
  switch (Data[0]) {
    case TT_UNKNOWN:
    case TT_MP4_RAW:
    case TT_MP4_ADIF:
    case TT_MP4_ADTS:
    case TT_MP4_LATM_MCP1:
    case TT_MP4_LATM_MCP0:
    case TT_MP4_LOAS:
    case TT_DRM:
      transportType = (TRANSPORT_TYPE)Data[0];
  }
  // This makes nrOfLayers in [0..255].
  // Is that ok, or we need other values for nrOfLayers?
  UINT nrOfLayers = Data[1];
  // Same here: what are the interesting values for flags?
  UINT flags = Data[2];

  auto aacDecoderInfo = aacDecoder_Open(transportType, nrOfLayers);
  assert(aacDecoderInfo);

  UINT bytesRead = Size - 3;
  UINT bytesValid = Size - 3;
  UCHAR *pBuffer = const_cast<UCHAR*>(Data + 3);
  auto ErrorStatus =
      aacDecoder_Fill(aacDecoderInfo, &pBuffer, &bytesRead, &bytesValid);

  ErrorStatus =
      aacDecoder_DecodeFrame(aacDecoderInfo, pTimeData, timeDataSize, flags);
  ErrorStatus =
      aacDecoder_DecodeFrame(aacDecoderInfo, pTimeData, timeDataSize, flags);

  aacDecoder_Close(aacDecoderInfo);
  return 0;
}
