//
//  ChultraSofCml.cpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#include "IntelHdaDspCml.hpp"
#include <IOKit/IOLib.h>
#include <IOKit/pci/IOPCIDevice.h>

#define super IntelHdaDsp
OSDefineMetaClassAndStructors(IntelHdaDspCml, IntelHdaDsp);

IntelHdaDspCml *IntelHdaDspCml::probe(IOService *provider, SInt32 *score) {
    if (super::probe(provider, score) == nullptr) {
        IOLog("SOF::Failed to probe");
        return nullptr;
    }
    
    return this;
}

bool IntelHdaDspCml::start(IOService *provider) {
    return super::start(provider);
}
