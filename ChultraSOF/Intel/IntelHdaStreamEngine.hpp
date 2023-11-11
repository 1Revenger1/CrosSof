//
//  ChultraHDAStreamEngine.hpp
//  ChultraSOF
//
//  Created by Gwydien on 11/4/23.
//

#pragma once

#include <IOKit/audio/IOAudioEngine.h>
#include "SofDai.hpp"
#include "IntelHdaDsp.hpp"
#include "pcm.h"


class IntelHdaStreamEngine : public IOAudioEngine {
    OSDeclareDefaultStructors(IntelHdaStreamEngine);
public:
    
    bool initWithDai(SofDai *dai);
    IOReturn initAudioStream();
    
    virtual void free() override;
    
    virtual bool initHardware(IOService *provider) override;
    virtual UInt32 getCurrentSampleFrame() override;
protected:
private:
    SofDai *dai;
    IntelHdaDsp *dsp;
    
    UInt8 buf[1024];
};
