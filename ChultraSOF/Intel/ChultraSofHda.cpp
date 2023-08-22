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
    OSObject *topologyResult;
    
    if (super::probe(provider, score) == nullptr) {
        IOLog("SOF::Failed to probe\n");
        return nullptr;
    }
    
    pci = OSDynamicCast(IOPCIDevice, provider);
    if (pci == nullptr) {
        IOLog("SOF::Nub is not PCI device\n");
        return nullptr;
    }
    
    // Grab memory descriptors here possibly?
    pci->setIOEnable(true);
    pci->setBusMasterEnable(true);
    pci->setMemoryEnable(true);
    
    // Grab Topology from
    OSString *acpiDevPath = OSDynamicCast(OSString, pci->getProperty("acpi-path"));
    if (acpiDevPath == nullptr) {
        IOLog("SOF::Failed to grab ACPI Device\n");
        return nullptr;
    }
    
    IORegistryEntry *acpiEntry = IOService::fromPath(acpiDevPath->getCStringNoCopy());
    IOACPIPlatformDevice *acpiDev = OSDynamicCast(IOACPIPlatformDevice, acpiEntry);
    if (acpiDev == nullptr || acpiEntry == nullptr) {
        OSSafeReleaseNULL(acpiEntry);
        IOLog("SOF::Failed to grab ACPI Device\n");
        return nullptr;
    }
    
    IOReturn ret = acpiDev->evaluateObject("_DSD", &topologyResult);
    if (ret != kIOReturnSuccess) {
        IOLog("SOF::Failed to grab topology\n");
        OSSafeReleaseNULL(acpiDev);
        return nullptr;
    }
    
    OSSafeReleaseNULL(acpiDev);
    return this;
}

bool ChultraSofHda::start(IOService *provider) {
    registerService();
    return super::start(provider);
}
