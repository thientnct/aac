# Fuzzer for libFraunhoferAAC decoder

## Plugin Design Considerations
The fuzzer plugin for aac decoder is designed based on the understanding of the
codec and tries to achieve the following:

##### Maximize code coverage

This fuzzer makes use of the following config parameters:
1. Transport type (parameter name: `TRANSPORT_TYPE`)

| Parameter| Valid Values| Configured Value|
|------------- |-------------| ----- |
| `TRANSPORT_TYPE` | 0.`TT_UNKNOWN  ` 1.`TT_MP4_RAW ` 2.`TT_MP4_ADIF ` 3.`TT_MP4_ADTS ` 4.`TT_MP4_LATM_MCP1 ` 5.`TT_MP4_LATM_MCP0  ` 6.`TT_MP4_LOAS ` 7.`TT_DRM ` | `TT_MP4_ADIF ` |

Note: Value of `TRANSPORT_TYPE` could be set to any of these values.
It is set to `TT_MP4_ADIF` in the fuzzer plugin.

##### Maximize utilization of input data
The plugin feeds the entire input data to the codec using a loop.
 * If the decode operation was successful, the input is advanced by an
   offset calculated using valid bytes.
 * If the decode operation was un-successful, the input is advanced by 1 byte
   till it reaches a valid frame or end of stream.

This ensures that the plugin tolerates any kind of input (empty, huge,
malformed, etc) and doesnt `exit()` on any input and thereby increasing the
chance of identifying vulnerabilities.

## Build

This describes steps to build aac_dec_fuzzer binary.

## Android

### Steps to build
Build the fuzzer
```
  $ mm -j$(nproc) aac_dec_fuzzer
```

### Steps to run
Create a directory CORPUS_DIR and copy some aac files to that folder.
Push this directory to device.

To run on device
```
  $ adb sync data
  $ adb shell /data/fuzz/arm64/aac_dec_fuzzer/aac_dec_fuzzer CORPUS_DIR
```
To run on host
```
  $ $ANDROID_HOST_OUT/fuzz/x86_64/aac_dec_fuzzer/aac_dec_fuzzer CORPUS_DIR
```

# Fuzzer for libFraunhoferAAC encoder

## Plugin Design Considerations
The fuzzer plugin for aac encoder is designed based on the understanding of the
codec and tries to achieve the following:

##### Maximize code coverage

The configuration parameters are not hardcoded, but instead selected based on
incoming data. This ensures more code paths are reached by the fuzzer.

Following arguments are passed to aacEncoder_SetParam to set the respective AACENC_PARAM parameter:

| Variable name| AACENC_PARAM param| Valid Values| Configured Value|
|------------- |-------------| ----- |----- |
| `sbrMode`   |`AACENC_SBR_MODE` | `AAC_SBR_OFF ` `AAC_SBR_SINGLE_RATE ` `AAC_SBR_DUAL_RATE ` `AAC_SBR_AUTO ` | Calculated using first byte of data |
| `sbrRatio`   | `AACENC_SBR_RATIO` |`0 ` `1 ` `2 ` | Derived from `sbrMode` |
| `aacAOT`   | `AACENC_AOT` |`AOT_AAC_LC ` `AOT_ER_AAC_ELD ` `AOT_SBR ` `AOT_PS ` `AOT_ER_AAC_LD ` | Calculated using second byte of data  |
| `sampleRate`   |`AACENC_SAMPLERATE` |  `8000 ` `11025 ` `12000 ` `16000 ` `22050 ` `24000 ` `32000 ` `44100 ` `48000 `| Calculated using third byte of data  |
| `bitRate`   |`AACENC_BITRATE` | In range `8000 ` to `960000 ` | Calculated using fourth, fifth and sixth byte of data  |
| `channelMode`   |`AACENC_CHANNELMODE` | `MODE_1 ` `MODE_2 ` `MODE_1_2 ` `MODE_1_2_1 ` `MODE_1_2_2 ` `MODE_1_2_2_1 ` `MODE_1_2_2_2_1 ` `MODE_7_1_BACK `  | Calculated using seventh byte of data |
| `identifier`   |`AACENC_TRANSMUX` | `TT_MP4_RAW ` `TT_MP4_ADIF ` `TT_MP4_ADTS ` `TT_MP4_LATM_MCP1 ` `TT_MP4_LATM_MCP0 ` `TT_MP4_LOAS ` `TT_DRM `  | Calculated using eigth byte of data  |

##### Maximize utilization of input data
The plugin feeds the entire input data to the codec and continues with the encoding even on a failure. This ensures that the plugin tolerates any kind of input (empty, huge, malformed, etc) and doesnt `exit()` on any input and thereby increasing the chance of identifying vulnerabilities.

## Build

This describes steps to build aac_enc_fuzzer binary.

## Android

### Steps to build
Build the fuzzer
```
  $ mm -j$(nproc) aac_enc_fuzzer
```

### Steps to run
Create a directory CORPUS_DIR and copy some raw files to that folder.
Push this directory to device.

To run on device
```
  $ adb sync data
  $ adb shell /data/fuzz/arm64/aac_enc_fuzzer/aac_enc_fuzzer CORPUS_DIR
```
To run on host
```
  $ $ANDROID_HOST_OUT/fuzz/x86_64/aac_enc_fuzzer/aac_enc_fuzzer CORPUS_DIR
```

## References:
 * http://llvm.org/docs/LibFuzzer.html
 * https://github.com/google/oss-fuzz
