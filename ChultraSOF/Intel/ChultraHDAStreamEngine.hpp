//
//  ChultraHDAStreamEngine.hpp
//  ChultraSOF
//
//  Created by Gwydien on 11/4/23.
//

#pragma once

#include <IOKit/audio/IOAudioEngine.h>

#define BIT(x) (1 << x)
#define PCM_RATE_5512   BIT(0)
#define PCM_RATE_8000   BIT(1)
#define PCM_RATE_11025  BIT(2)
#define PCM_RATE_16000  BIT(3)
#define PCM_RATE_22050  BIT(4)
#define PCM_RATE_32000  BIT(5)
#define PCM_RATE_44100  BIT(6)
#define PCM_RATE_48000  BIT(7)
#define PCM_RATE_64000  BIT(8)
#define PCM_RATE_88200  BIT(9)
#define PCM_RATE_96000  BIT(10)

#define PCM_FMT_S8      BIT(0)
#define PCM_FMT_U8      BIT(1)

#define PCM_FMT_S16_LE  BIT(2)
#define PCM_FMT_S16_BE  BIT(3)
#define PCM_FMT_U16_LE  BIT(4)
#define PCM_FMT_U16_BE  BIT(5)

#define PCM_FMT_S24_LE  BIT(6)
#define PCM_FMT_S24_BE  BIT(7)
#define PCM_FMT_U24_LE  BIT(8)
#define PCM_FMT_U24_BE  BIT(9)

#define PCM_FMT_S32_LE  BIT(10)
#define PCM_FMT_S32_BE  BIT(11)
#define PCM_FMT_U32_LE  BIT(12)
#define PCM_FMT_U32_BE  BIT(13)

// This will always be ran on a little endiann machine
#define PCM_FMT_S16     PCM_FMT_S16_LE
#define PCM_FMT_U16     PCM_FMT_U16_LE
#define PCM_FMT_S24     PCM_FMT_S24_LE
#define PCM_FMT_U24     PCM_FMT_U24_LE
#define PCM_FMT_S32     PCM_FMT_S32_LE
#define PCM_FMT_U32     PCM_FMT_U32_LE

struct CrosStreamDefinition {
    const char *streamName;
    UInt32 formatFlags;
    UInt32 rateFlags;
    UInt32 rateMin;
    UInt32 rateMax;
    UInt32 channelsMin;
    UInt32 channelsMax;
};

struct CrosCodecStreams {
    CrosStreamDefinition playback;
    CrosStreamDefinition capture;
    const char *name;
};

class IntelHdaStreamEngine : public IOAudioEngine {
    OSDeclareDefaultStructors(IntelHdaStreamEngine);
public:
    
    IOReturn initAudioStream();
protected:
}
