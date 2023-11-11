
#include <IOKit/IOLib.h>
#include <IOKit/IOLocks.h>
#include <libkern/OSKextLib.h>

#include "SofDsp.hpp"
#include "Logger.h"

#ifdef NO_AUDIO_FAMILY
#define super IOService
OSDefineMetaClassAndAbstractStructors(SofDsp, IOService);
#else
#define super IOAudioDevice
OSDefineMetaClassAndAbstractStructors(SofDsp, IOAudioDevice);
#endif

struct SofResourceLoadContext {
    IOLock *lock;
    OSData *resource;
};

static void SofOnFirmwareCallback(
    OSKextRequestTag requestTag,
    OSReturn result,
    const void *resourceData,
    uint32_t resourceDataLength,
    void *context) {
    
    SofResourceLoadContext *ctx = (SofResourceLoadContext *) context;
    IOLockLock(ctx->lock);
    if (result == kOSReturnSuccess) {
        ctx->resource = OSData::withBytes(resourceData, resourceDataLength);
    }
    IOLockUnlock(ctx->lock);
    IOLockWakeup(ctx->lock, ctx, false);
}

LIBKERN_RETURNS_RETAINED OSData *SofDsp::getFirmware(const char *firmwareName) {
    SofResourceLoadContext ctx {0};
    IOReturn ret;
    
    ctx.lock = IOLockAlloc();
    ctx.resource = nullptr;
    
    if (ctx.lock == nullptr) {
        return nullptr;
    }
    
    IOLockLock(ctx.lock);
    ret = OSKextRequestResource(OSKextGetCurrentIdentifier(), firmwareName, SofOnFirmwareCallback, &ctx, nullptr);
    if (ret != kIOReturnSuccess) {
        IOLogError("Failed to request firmware\n");
        return nullptr;
    }
    
    ret = IOLockSleep(ctx.lock, &ctx, false);
    if (ret != kIOReturnSuccess || ctx.resource == nullptr) {
        IOLogError("Failed to find firmware\n");
    }
    
    IOLockUnlock(ctx.lock);
    IOLockFree(ctx.lock);
    return ctx.resource;
}

void SofDsp::setFwState(sof_fw_state_t newState) {
    if (fwState == newState)
        return;

    IOLogDebug("Firmware state changed: %d -> %d", fwState, newState);
    fwState = newState;

    switch (newState) {
        case SOF_FW_BOOT_NOT_STARTED:
        case SOF_FW_BOOT_COMPLETE:
        case SOF_FW_CRASHED:
//            sof_client_fw_state_dispatcher(sdev);
            break;
        default:
            break;
    }
}

bool SofDsp::initHardware(IOService *provider) {
#ifndef NO_AUDIO_FAMILY
    if (!super::initHardware(provider)) {
        IOLogError("Failed to initHardware in IOAudioDevice");
        return false;
    }
#endif
    
    if (!sofProbe(provider)) {
        IOLogError("Failed to probe HDA DSP");
        return false;
    }
    
    // TODO: Load Firmware
    
    if (runFirmware() != kIOReturnSuccess) {
        IOLogError("Failed to start firmware");
        return false;
    }
    
    
    return true;
}

IOReturn SofDsp::runFirmware() {
    IOReturn ret;
    
    ret = preFwRun();
    if (ret != kIOReturnSuccess) {
        IOLogError("Failed pre-run op");
        return ret;
    }
    
    IOLogDebug("Booting DSP Firmware");
    ret = runFw();
    if (ret != kIOReturnSuccess) {
        IOLogError("Failed to upload/run fw");
        return ret;
    }
    
    ret = postFwRun();
    if (ret != kIOReturnSuccess) {
        IOLogError("Failed post-run op");
        return ret;
    }
    
    return kIOReturnSuccess;
}
