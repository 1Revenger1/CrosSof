//
//  ChultraSofHda.cpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#include "ChultraSofHda.hpp"
#include "Logger.h"
#include <IOKit/IOLib.h>
#include <IOKit/IOLocks.h>
#include <libkern/OSKextLib.h>
#include <Availability.h>

#ifndef __ACIDANTHERA_MAC_SDK
#error "MacKernelSDK is missing. Please make sure it is cloned!"
#endif // __ACIDANTHERA_MAC_SDK

#define super IOService
OSDefineMetaClassAndStructors(ChultraSofHda, IOService);

struct ChultraResourceLoadContext {
    IOLock *lock;
    OSData *resource;
};

static void ChultraSofOnFirmwareCallback(
    OSKextRequestTag requestTag,
    OSReturn result,
    const void *resourceData,
    uint32_t resourceDataLength,
    void *context) {
    
    ChultraResourceLoadContext *ctx = (ChultraResourceLoadContext *) context;
    IOLockLock(ctx->lock);
    if (result == kOSReturnSuccess) {
        ctx->resource = OSData::withBytes(resourceData, resourceDataLength);
    }
    IOLockUnlock(ctx->lock);
    IOLockWakeup(ctx->lock, ctx, false);
}

// PCI devices usually are listed with both IO-APIC interrupts and
//  MSI interrupts. We just want MSI interrupts, which is usually
//  at index 1.
int ChultraSofHda::getMsiInterruptIndex() {
    for (int i = 0; ; i++) {
        IOReturn result;
        int interruptType;
        
        result = pci->getInterruptType(i, &interruptType);
        if (result != kIOReturnSuccess)
            return -1;
        
        if (interruptType & kIOInterruptTypePCIMessaged)
            return i;
    }
}

LIBKERN_RETURNS_RETAINED OSData *ChultraSofHda::getFirmware(const char *firmwareName) {
    ChultraResourceLoadContext ctx {0};
    IOReturn ret;
    
    ctx.lock = IOLockAlloc();
    ctx.resource = nullptr;
    
    if (ctx.lock == nullptr) {
        return nullptr;
    }
    
    IOLockLock(ctx.lock);
    ret = OSKextRequestResource(OSKextGetCurrentIdentifier(), firmwareName, ChultraSofOnFirmwareCallback, &ctx, nullptr);
    if (ret != kIOReturnSuccess) {
        IOLog("SOF::Failed to request firmware\n");
        return nullptr;
    }
    
    ret = IOLockSleep(ctx.lock, &ctx, false);
    if (ret != kIOReturnSuccess || ctx.resource == nullptr) {
        IOLog("SOF::Failed to grab firmware\n");
    }
    
    IOLockUnlock(ctx.lock);
    if (ctx.lock != nullptr) IOLockFree(ctx.lock);
    return ctx.resource;
}

ChultraSofHda *ChultraSofHda::probe(IOService *provider, SInt32 *score) {
    OSObject *topologyResult;
    OSData *firmware = nullptr;
    IOMemoryMap *dspBar = nullptr;
    IOMemoryMap *hdaBar = nullptr;
    int interruptIndex;
    
    IOLogInfo("Driver startup!");
    
    if (super::probe(provider, score) == nullptr) {
        IOLogError("Failed to probe");
        return nullptr;
    }
    
    pci = OSDynamicCast(IOPCIDevice, provider);
    if (pci == nullptr) {
        IOLogError("Nub is not PCI device");
        return nullptr;
    }
    
    // Grab memory descriptors here possibly?
    pci->setIOEnable(true);
    pci->setBusMasterEnable(true);
    pci->setMemoryEnable(true);
    
    //
    // Grab topology
    //
    
    OSString *acpiDevPath = OSDynamicCast(OSString, pci->getProperty("acpi-path"));
    if (acpiDevPath == nullptr) {
        IOLogError("Failed to grab ACPI Device");
        return nullptr;
    }
    
    IORegistryEntry *acpiEntry = IOService::fromPath(acpiDevPath->getCStringNoCopy());
    IOACPIPlatformDevice *acpiDev = OSDynamicCast(IOACPIPlatformDevice, acpiEntry);
    if (acpiDev == nullptr || acpiEntry == nullptr) {
        OSSafeReleaseNULL(acpiEntry);
        IOLogError("Failed to grab ACPI Device");
        return nullptr;
    }
    
    IOReturn ret = acpiDev->evaluateObject("_DSD", &topologyResult);
    if (ret != kIOReturnSuccess) {
        IOLogError("Failed to grab topology");
        goto err;
    }
    
    //
    // Load Firmware from kext bundle
    //
    
    firmware = getFirmware("firmware name");
    
    dspBar = pci->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress4);
    hdaBar = pci->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0);
    
    if (dspBar != nullptr) {
        IOLogInfo("DSP Bar Addr: 0x%llx Length: 0x%llx", dspBar->getPhysicalAddress(), dspBar->getSize());
    }
    
    if (hdaBar != nullptr) {
        IOLogInfo("HDA Bar Addr: 0x%llx Length: 0x%llx", hdaBar->getPhysicalAddress(), hdaBar->getSize());
    }
    
    interruptIndex = getMsiInterruptIndex();
    IOLogInfo("Found MSI interrupt index %d", interruptIndex);
    
err:
    OSSafeReleaseNULL(dspBar);
    OSSafeReleaseNULL(hdaBar);
    OSSafeReleaseNULL(firmware);
    OSSafeReleaseNULL(acpiDev);
    return ret != kIOReturnSuccess ? nullptr : this;
}

bool ChultraSofHda::start(IOService *provider) {
    registerService();
    return super::start(provider);
}
