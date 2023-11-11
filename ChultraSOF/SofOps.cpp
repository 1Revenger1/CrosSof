//
//  SofOps.cpp
//  ChultraSOF
//
//  Created by Gwydien on 11/10/23.
//

#include <IOKit/pci/IOPCIDevice.h>

#include "SofDsp.hpp"

bool SofDsp::pciCfgUpdate(IOByteCount offset, UInt32 mask, UInt32 value) {
    UInt32 newVal;
    UInt32 oldVal = pci->configRead32(offset);
    
    newVal = (oldVal & ~mask) | value;
    if (oldVal == newVal) {
        return false;
    }
    
    pci->configWrite32(offset, newVal);
    return true;
}

bool SofDsp::dspUpdate(hda_bars_t bar, IOByteCount offset, UInt32 mask, UInt32 value) {
    UInt32 newVal;
    UInt32 oldVal = read32(bar, offset);
    
    newVal = (oldVal & ~mask) | value;
    if (oldVal == newVal) {
        return false;
    }
    
    write32(bar, offset, newVal);
    return true;
}
