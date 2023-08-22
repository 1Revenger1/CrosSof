//
//  ChultraSofHda.cpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#include "ChultraSofHda.hpp"
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

ChultraSofHda *ChultraSofHda::probe(IOService *provider, SInt32 *score) {
    OSObject *topologyResult;
    ChultraResourceLoadContext ctx {0};
    
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
    
    //
    // Grab topology
    //
    
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
        goto err;
    }
    
    //
    // Load Firmware from kext bundle
    //
    
    ctx.lock = IOLockAlloc();
    ctx.resource = nullptr;
    if (ctx.lock == nullptr) {
        ret = kIOReturnError;
        goto err;
    }
    
    IOLockLock(ctx.lock);
    ret = OSKextRequestResource(OSKextGetCurrentIdentifier(), "firmware name", ChultraSofOnFirmwareCallback, &ctx, nullptr);
    if (ret != kIOReturnSuccess) {
        IOLog("SOF::Failed to request firmware\n");
        goto err;
    }
    
    ret = IOLockSleep(ctx.lock, &ctx, false);
    if (ret != kIOReturnSuccess || ctx.resource == nullptr) {
        IOLog("SOF::Failed to grab firmware\n");
        ret = kIOReturnError;
        goto err;
    }
    
    IOLockUnlock(ctx.lock);
err:
    if (ctx.lock != nullptr) IOLockFree(ctx.lock);
    OSSafeReleaseNULL(ctx.resource);
    OSSafeReleaseNULL(acpiDev);
    return ret != kIOReturnSuccess ? nullptr : this;
}

bool ChultraSofHda::start(IOService *provider) {
    registerService();
    return super::start(provider);
}
