//
//  ChultraSofHda.cpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#include "ChultraSofHda.hpp"
#include "Logger.h"
#include <IOKit/IOFilterInterruptEventSource.h>
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

bool ChultraSofHda::init() {
    bzero(barMaps, sizeof(barMaps));
    return true;
}

ChultraSofHda *ChultraSofHda::probe(IOService *provider, SInt32 *score) {
    OSObject *topologyResult;
    OSData *firmware = nullptr;
    
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
    
    barMaps[SOF_DSP_BAR] = pci->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress4);
    barMaps[SOF_HDA_BAR] = pci->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0);
    
    if (barMaps[SOF_DSP_BAR] != nullptr) {
        IOLogInfo("DSP Bar Addr: 0x%llx Length: 0x%llx", barMaps[SOF_DSP_BAR]->getPhysicalAddress(), barMaps[SOF_DSP_BAR]->getSize());
    }
    
    if (barMaps[SOF_HDA_BAR] != nullptr) {
        IOLogInfo("HDA Bar Addr: 0x%llx Length: 0x%llx", barMaps[SOF_HDA_BAR]->getPhysicalAddress(), barMaps[SOF_HDA_BAR]->getSize());
    }
    
    ctrlGetCaps();
    getHdaStreams();
    
err:
    OSSafeReleaseNULL(firmware);
    OSSafeReleaseNULL(acpiDev);
    return ret != kIOReturnSuccess ? nullptr : this;
}

bool ChultraSofHda::start(IOService *provider) {
    int interruptIndex;
    
    // TODO: Register Interrupts
    interruptIndex = getMsiInterruptIndex();
    IOLogInfo("Found MSI interrupt index %d", interruptIndex);
    
    workloop = IOWorkLoop::workLoop();
    cmdGate = IOCommandGate::commandGate(this);
    intSrc = IOFilterInterruptEventSource::filterInterruptEventSource(this, nullptr, nullptr, provider);
    if (intSrc == nullptr || workloop == nullptr || cmdGate == nullptr) {
        IOLogError("Failed to construct interrupt sources");
        return false;
    }
    
    if (workloop->addEventSource(cmdGate) != kIOReturnSuccess ||
        workloop->addEventSource(intSrc) != kIOReturnSuccess) {
        IOLogError("Failed to add event sources to workloop");
        return false;
    }
    
    registerService();
    return super::start(provider);
}

void ChultraSofHda::free() {
    for (size_t i = 0; i < SOF_MAX_BAR; i++) {
        OSSafeReleaseNULL(barMaps[i]);
    }
}

IOReturn ChultraSofHda::ctrlGetCaps() {
    // TODO: Throw reset in it's own method and return reset errors
    // Enter reset
    uint32_t ctrlReg = read32(SOF_HDA_BAR, 0x8);
    ctrlReg &= ~0x1;
    write32(SOF_HDA_BAR, 0x8, ctrlReg);
    IOSleep(100);
    
    ctrlReg = read32(SOF_HDA_BAR, 0x8);
    IOLogInfo("Ctrl Reg = 0x%x", ctrlReg);
    
    // Exit reset
    ctrlReg |= ~0x1;
    write32(SOF_HDA_BAR, 0x8, ctrlReg);
    IOSleep(100);
    IOLogInfo("Ctrl Reg = 0x%x", ctrlReg);
    
    uint32_t offset = read32(SOF_HDA_BAR, 0x14);
    int count = 0;
    
    do {
        uint32_t capability = read32(SOF_HDA_BAR, offset);
        uint32_t feature = (capability & 0x7FF0000) >> 16;
        IOLogInfo("Found capability %d at 0x%x", feature, offset);
        switch (feature) {
            case SOF_HDA_PP_CAP_ID: bars[SOF_PP_BAR] = bars[SOF_HDA_BAR] + offset; break;
            case SOF_HDA_SPIB_CAP_ID: bars[SOF_SPIB_BAR] = bars[SOF_SPIB_BAR] + offset; break;
            case SOF_HDA_DRSM_CAP_ID: bars[SOF_DRSM_BAR] = bars[SOF_DRSM_BAR] + offset; break;
            case SOF_HDA_GTS_CAP_ID: break;
            case SOF_HDA_ML_CAP_ID: break;
            default: break;
        }
        offset = capability & 0xFFFF;
        count++;
    } while (count <= 10 && offset != 0);
    
    return kIOReturnSuccess;
}

IOReturn ChultraSofHda::getHdaStreams() {
    uint32_t streamCaps = read32(SOF_HDA_BAR, 0x0);
    
    int captureStreams = (streamCaps >> 8) & 0x0F;
    int playbackStreams = (streamCaps>>12) & 0x0F;
//    int totalStream = captureStreams + playbackStreams;
    IOLogInfo("Found %d playback and %d capture streams", playbackStreams, captureStreams);
    
    // TODO: Actually parse/create streams
    
    return kIOReturnSuccess;
}
