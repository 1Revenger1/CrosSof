//
//  Max98357a.cpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#include "Max98357a.hpp"
#include "pcm.h"

#define super SofDai
OSDefineMetaClassAndStructors(Max98357a, SofDai);

static DaiDriver max98357aDai = {
    .name = "HiFi",
    .playback = {
        .streamName     = "HiFi Playback",
        .formats        = PCM_FMT_S16 |
                          PCM_FMT_S24 |
                          PCM_FMT_S32,
        .rates          = PCM_RATE_8000  |
                          PCM_RATE_16000 |
                          PCM_RATE_32000 |
                          PCM_RATE_44100 |
                          PCM_RATE_48000 |
                          PCM_RATE_88200 |
                          PCM_RATE_96000,
        .rateMin        = 8000,
        .rateMax        = 96000,
        .channelsMin    = 1,
        .channelsMax    = 2
    },
    .capture = {
        // No Capture
    },
};

DaiDriver *Max98357a::getDaiStreams(UInt32 *count) const {
    *count = 1;
    return &max98357aDai;
}
