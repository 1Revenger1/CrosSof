//
//  IntelHdaDspLoader.cpp
//  ChultraSOF
//
//  Created by Gwydien on 11/10/23.
//

#include <IOkit/audio/IOAudioTypes.h>

#include "IntelHdaDsp.hpp"
#include "Logger.h"

struct hdac_ext_stream *clStreamPrepare(UInt32 format,
                          size_t size, IOBufferMemoryDescriptor **dmab,
                          IOAudioStreamDirection direction)
{
//    struct hdac_ext_stream *hext_stream;
//    struct hdac_stream *hstream;
    int ret;
    
//    hext_stream = hda_dsp_stream_get(sdev, direction, 0);

//    if (!hext_stream) {
//        dev_err(sdev->dev, "error: no stream available\n");
//        return ERR_PTR(-ENODEV);
//    }
//    hstream = &hext_stream->hstream;
//    hstream->substream = NULL;

    /* allocate DMA buffer */
    // TODO: IODMACommand?
    //    ret = snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV_SG, &pci->dev, size, dmab);
    *dmab = IOBufferMemoryDescriptor::withCapacity(size, kIODirectionInOut);
    if (*dmab == nullptr) {
        IOLogError("Memory alloc failed!");
        goto out_put;
    }

//    hstream->period_bytes = 0;/* initialize period_bytes */
//    hstream->format_val = format;
//    hstream->bufsize = size;

    if (  direction == kIOAudioStreamDirectionInput) {
//        ret = hda_dsp_iccmax_stream_hw_params(sdev, hext_stream, dmab, NULL);
        if (ret < 0) {
            IOLogError("iccmax stream prepare failed: %d", ret);
            goto out_free;
        }
    } else {
//        ret = hda_dsp_stream_hw_params(sdev, hext_stream, dmab, NULL);
        if (ret < 0) {
            IOLogError("hdac prepare failed: %d", ret);
            goto out_free;
        }
//        hda_dsp_stream_spib_config(sdev, hext_stream, HDA_DSP_SPIB_ENABLE, size);
    }

//    return hext_stream;

out_free:
    OSSafeReleaseNULL(*dmab);
out_put:
//    hda_dsp_stream_put(sdev, direction, hstream->stream_tag);
    return nullptr;
}

IOReturn IntelHdaDsp::runFw() {
    
    // TODO: IMR restore
    
    
    
    return kIOReturnSuccess;
}

IOReturn IntelHdaDsp::preFwRun() {
    /* disable clock gating and power gating */
    return ctrlClockPowerGating(false);
}

IOReturn IntelHdaDsp::postFwRun() {
    /* re-enable clock gating and power gating */
    return ctrlClockPowerGating(true);
}
