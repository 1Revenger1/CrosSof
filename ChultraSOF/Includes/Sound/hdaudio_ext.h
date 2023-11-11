//
//  hdaudio_ext.h
//  ChultraSOF
//
//  Created by Gwydien on 11/10/23.
//

#pragma once

#include <IOKit/IODeviceMemory.h>

#include "hdaudio.h"

/**
 * hdac_ext_stream: HDAC extended stream for extended HDA caps
 *
 * @hstream: hdac_stream
 * @pphc_addr: processing pipe host stream pointer
 * @pplc_addr: processing pipe link stream pointer
 * @decoupled: stream host and link is decoupled
 * @link_locked: link is locked
 * @link_prepared: link is prepared
 * @link_substream: link substream
 */
struct hdac_ext_stream {
    struct hdac_stream hstream;

    IOVirtualAddress pphc_addr;
    IOVirtualAddress pplc_addr;

    UInt32 pphcllpl;
    UInt32 pphcllpu;
    UInt32 pphcldpl;
    UInt32 pphcldpu;

    bool decoupled:1;
    bool link_locked:1;
    bool link_prepared;
};

