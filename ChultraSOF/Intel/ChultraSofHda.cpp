//
//  ChultraSofHda.cpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#include "ChultraSofHda.hpp"
#include <IOKit/IOLib.h>
#include <Availability.h>

#ifndef __ACIDANTHERA_MAC_SDK
#error "MacKernelSDK is missing. Please make sure it is cloned!"
#endif // __ACIDANTHERA_MAC_SDK

#define super IOService
OSDefineMetaClassAndStructors(ChultraSofHda, IOService);

ChultraSofHda *ChultraSofHda::probe(IOService *provider, SInt32 *score) {
    if (super::probe(provider, score) == nullptr) {
        IOLog("SOF::Failed to probe");
        return nullptr;
    }
    
    return this;
}

bool ChultraSofHda::start(IOService *provider) {
    registerService();
    return super::start(provider);
}
