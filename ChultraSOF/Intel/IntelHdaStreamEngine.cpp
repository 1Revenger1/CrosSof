//
//  ChultraHDAStreamEngine.cpp
//  ChultraSOF
//
//  Created by Gwydien on 11/4/23.
//

#include <IOKit/audio/IOAudioStream.h>
#include <IOKit/audio/IOAudioTypes.h>
#include <IOKit/audio/IOAudioLevelControl.h>
#include <IOKit/audio/IOAudioToggleControl.h>
#include <IOKit/audio/IOAudioSelectorControl.h>

#include "IntelHdaStreamEngine.hpp"
#include "Logger.h"

#define super IOAudioEngine
OSDefineMetaClassAndStructors(IntelHdaStreamEngine, IOAudioEngine);

bool IntelHdaStreamEngine::initWithDai(SofDai *dai) {
    this->dai = dai;
    dai->retain();
    
    return super::init(nullptr);
}

void IntelHdaStreamEngine::free() {
    OSSafeReleaseNULL(dai);
    super::free();
}

UInt32 IntelHdaStreamEngine::getCurrentSampleFrame() { return 0; };

bool IntelHdaStreamEngine::initHardware(IOService *provider) {
    UInt32 daiCount;
    DaiDriver *drivers = dai->getDaiStreams(&daiCount);
    
    if (!super::initHardware(provider)) {
        IOLogError("Failed to init stream!");
        return false;
    }
    
    dsp = OSDynamicCast(IntelHdaDsp, provider);
    if (dsp == nullptr) {
        IOLogError("Stream parent is not an Intel DSP");
        return false;
    }
    
    setDescription(drivers[0].playback.streamName);
    
    IOAudioStream *stream = new IOAudioStream;
    if (stream == nullptr) {
        return false;
    }
    
    if (!stream->initWithAudioEngine(this, kIOAudioStreamDirectionOutput, kIOAudioControlChannelIDDefaultLeft)) {
        OSSafeReleaseNULL(stream);
        IOLogError("Failed to init audio engine");
        return false;
    }
    
    stream->setSampleBuffer(buf, sizeof(buf));
    
    IOAudioStreamFormat format = {};
    IOAudioSampleRate minRate = {};
    IOAudioSampleRate maxRate = {};
    
    format.fByteOrder = kIOAudioStreamByteOrderLittleEndian;
    format.fAlignment = kIOAudioStreamAlignmentLowByte;
    format.fSampleFormat = kIOAudioStreamSampleFormatLinearPCM;
    format.fIsMixable = true;
    
    minRate.whole = drivers[0].playback.rateMin;
    maxRate.whole = drivers[0].playback.rateMax;
    
//    for (int i = 0; i < (sizeof(PcmRates) / sizeof(UInt32)); i++) {
        for (int channels = 1; channels <= drivers[0].playback.channelsMax; channels++) {
            format.fNumChannels = channels;
            format.fBitDepth = 16;
            format.fBitWidth = 16;
            format.fNumericRepresentation = kIOAudioStreamNumericRepresentationSignedInt;
            stream->addAvailableFormat(&format, &minRate, &maxRate);
            
            format.fBitDepth = 24;
            format.fBitWidth = 32;
            stream->addAvailableFormat(&format, &minRate, &maxRate);

            format.fBitDepth = 32;
            format.fBitWidth = 32;
            stream->addAvailableFormat(&format, &minRate, &maxRate);
            
            
        }
//    }
    
    stream->setFormat(&format);
    stream->setTerminalType(OUTPUT_SPEAKER);
    setSampleRate(&maxRate);
    addAudioStream(stream);
    
    
    IOAudioToggleControl *mute = IOAudioToggleControl::createMuteControl(false, kIOAudioControlChannelIDAll);
    addDefaultAudioControl(mute);
    OSSafeReleaseNULL(mute);
    
//    IOAudioLevelControl *level = IOAudioLevelControl::createVolumeControl(0, 0, 1, 0, 0, 1);
//    addDefaultAudioControl(level);
//    OSSafeReleaseNULL(level);
    
    OSSafeReleaseNULL(stream);
    
    
    return true;
}

