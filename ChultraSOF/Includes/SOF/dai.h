//
//  dai.h
//  ChultraSOF
//
//  Created by Gwydien on 11/4/23.
//

#pragma once

struct DaiStream {
    const char *streamName;
    UInt32 formats;
    UInt32 rates;
    UInt32 rateMin;
    UInt32 rateMax;
    UInt32 channelsMin;
    UInt32 channelsMax;
};

struct DaiDriver {
    const char *name;
    DaiStream playback;
    DaiStream capture;
};
