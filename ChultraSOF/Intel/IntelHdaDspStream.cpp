//
//  IntelHdaDspStream.cpp
//  ChultraSOF
//
//  Created by Gwydien on 11/10/23.
//

#include "IntelHdaDsp.hpp"
#include "Logger.h"
#include "hda.h"

IOReturn IntelHdaDsp::getHdaStreams() {
    UInt32 streamCaps = read32(HDA_DSP_HDA_BAR, 0x0);
    IOLogDebug("hda global caps = 0x%x", streamCaps);
    
    int captureStreams = (streamCaps >> 8) & 0x0F;
    int playbackStreams = (streamCaps>>12) & 0x0F;
    int totalStreams = captureStreams + playbackStreams;
    
    IOLogDebug("Found %d playback and %d capture streams", playbackStreams, captureStreams);
    
    if (playbackStreams >= SOF_HDA_PLAYBACK_STREAMS) {
        IOLogError("Too many playback streams: %d", playbackStreams);
        return kIOReturnInvalid;
    }
    
    if (captureStreams >= SOF_HDA_CAPTURE_STREAMS) {
        IOLogError("Too many capture streams: %d", captureStreams);
        return kIOReturnInvalid;
    }
    
    streams = OSArray::withCapacity(totalStreams);
    if (streams == nullptr) {
        IOLogError("Failed to allocate streams array");
        return kIOReturnNoMemory;
    }
    
    /*
     * Position buffer
     */
    
    posbuf = IOBufferMemoryDescriptor::withCapacity(SOF_HDA_DPIB_ENTRY_SIZE * totalStreams, kIODirectionInOut);
    if (posbuf == nullptr) {
        IOLogError("Failed to allocate position buffer");
        return kIOReturnNoMemory;
    }

    /*
     * mem alloc for the CORB/RIRB ringbuffers - this will be used only for
     * HDAudio codecs
     */
    corb = IOBufferMemoryDescriptor::withCapacity(4096, kIODirectionInOut);
    if (corb == nullptr) {
        IOLogError("Failed to allocate corb buffer");
        return kIOReturnNoMemory;
    }

    /* create capture and playback streams */
    for (size_t i = 0; i < totalStreams; i++) {
        SOFIntelHdaStream *hda_stream = new SOFIntelHdaStream;
        if (!hda_stream)
            return kIOReturnNoMemory;

        hdac_ext_stream *hext_stream = &hda_stream->hext_stream;

        if (bars[HDA_DSP_PP_BAR]) {
            hext_stream->pphc_addr = bars[HDA_DSP_PP_BAR] +
                SOF_HDA_PPHC_BASE + SOF_HDA_PPHC_INTERVAL * i;

            hext_stream->pplc_addr = bars[HDA_DSP_PP_BAR] +
                SOF_HDA_PPLC_BASE + SOF_HDA_PPLC_MULTI * totalStreams +
                SOF_HDA_PPLC_INTERVAL * i;
        }

        hdac_stream *hstream = &hext_stream->hstream;

        /* do we support SPIB */
        if (bars[HDA_DSP_SPIB_BAR]) {
            hstream->spib_addr = bars[HDA_DSP_SPIB_BAR] +
                SOF_HDA_SPIB_BASE + SOF_HDA_SPIB_INTERVAL * i +
                SOF_HDA_SPIB_SPIB;

            hstream->fifo_addr = bars[HDA_DSP_SPIB_BAR] +
                SOF_HDA_SPIB_BASE + SOF_HDA_SPIB_INTERVAL * i +
                SOF_HDA_SPIB_MAXFIFO;
        }

        hstream->sd_int_sta_mask = BIT(i);
        hstream->index = i;
        IOByteCount sd_offset = SOF_STREAM_SD_OFFSET(hstream);
        hstream->sd_addr = bars[HDA_DSP_HDA_BAR] + sd_offset;
        hstream->opened = false;
        hstream->running = false;

        if (i < captureStreams) {
            hstream->stream_tag = i + 1;
            hstream->direction = kIOAudioStreamDirectionInput;
        } else {
            hstream->stream_tag = i - captureStreams + 1;
            hstream->direction = kIOAudioStreamDirectionOutput;
        }

        /* mem alloc for stream BDL */
        hstream->bdl = IOBufferMemoryDescriptor::withCapacity(HDA_DSP_BDL_SIZE, kIODirectionInOut);
        if (hstream->bdl == nullptr) {
            OSSafeReleaseNULL(hda_stream);
            IOLogError("Stream bdl dma alloc failed");
            return kIOReturnNoMemory;
        }

        hstream->posbuf = (UInt32 *) ((UInt8 *) posbuf->getBytesNoCopy() +
            (hstream->index) * 8);

        streams->setObject(hda_stream);
        OSSafeReleaseNULL(hda_stream);
    }

    /* store total stream count (playback + capture) from GCAP */
    streamCount = totalStreams;
    
    return kIOReturnSuccess;
}

void IntelHdaDsp::freeStreams() {
    SOFIntelHdaStream *stream;
    
    if (streams != nullptr) {
        for (int i = 0; i < streams->getCount(); i++) {
            stream = OSDynamicCast(SOFIntelHdaStream, streams->getObject(i));
            OSSafeReleaseNULL(stream->hext_stream.hstream.bdl);
        }
        
        streams->release();
        streams = nullptr;
    }
    
    OSSafeReleaseNULL(posbuf);
    OSSafeReleaseNULL(corb);
}
