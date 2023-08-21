//
//  ChultraSofCml.cpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#include "ChultraSofCml.hpp"
#include <IOKit/IOLib.h>

#define super ChultraSofHda
OSDefineMetaClassAndStructors(ChultraSofCml, ChultraSofHda);

ChultraSofCml *ChultraSofCml::probe(IOService *provider, SInt32 *score) {
    if (super::probe(provider, score) == nullptr) {
        IOLog("SOF::Failed to probe");
        return nullptr;
    }
    
    return this;
}

bool ChultraSofCml::start(IOService *provider) {
    return super::start(provider);
}
