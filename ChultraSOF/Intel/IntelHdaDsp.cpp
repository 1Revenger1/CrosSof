//
//  ChultraSofHda.cpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#include <IOKit/IOFilterInterruptEventSource.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#include <Availability.h>

#include "IntelHdaDsp.hpp"
#include "IntelHdaStreamEngine.hpp"
#include "Max98357a.hpp"
#include "Logger.h"
#include "hda.h"

#ifndef __ACIDANTHERA_MAC_SDK
#error "MacKernelSDK is missing. Please make sure it is cloned!"
#endif // __ACIDANTHERA_MAC_SDK

#define super SofDsp
OSDefineMetaClassAndAbstractStructors(IntelHdaDsp, SofDsp);

// PCI devices usually are listed with both IO-APIC interrupts and
//  MSI interrupts. We just want MSI interrupts, which is usually
//  at index 1.
int IntelHdaDsp::getMsiInterruptIndex() {
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

IntelHdaDsp *IntelHdaDsp::probe(IOService *provider, SInt32 *score) {
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
    
    barMaps[HDA_DSP_HDA_BAR] = pci->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0);
    barMaps[HDA_DSP_DSP_BAR] = pci->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress4);
    
    if (barMaps[HDA_DSP_DSP_BAR] != nullptr) {
        IOLogInfo("DSP Bar Addr: 0x%llx Length: 0x%llx", barMaps[HDA_DSP_DSP_BAR]->getPhysicalAddress(), barMaps[HDA_DSP_DSP_BAR]->getSize());
        bars[HDA_DSP_DSP_BAR] = barMaps[HDA_DSP_DSP_BAR]->getVirtualAddress();
    }
    
    if (barMaps[HDA_DSP_HDA_BAR] != nullptr) {
        IOLogInfo("HDA Bar Addr: 0x%llx Length: 0x%llx", barMaps[HDA_DSP_HDA_BAR]->getPhysicalAddress(), barMaps[HDA_DSP_HDA_BAR]->getSize());
        bars[HDA_DSP_HDA_BAR] = barMaps[HDA_DSP_HDA_BAR]->getVirtualAddress();
    }
    
    ctrlGetCaps();
    getHdaStreams();
    
err:
    OSSafeReleaseNULL(firmware);
    OSSafeReleaseNULL(acpiDev);
    return ret != kIOReturnSuccess ? nullptr : this;
}

bool IntelHdaDsp::sofProbe(IOService *provider) {
    int interruptIndex;
    
    // TODO: Register Interrupts
    interruptIndex = getMsiInterruptIndex();
    IOLogInfo("Found MSI interrupt index %d", interruptIndex);
    
#ifdef NO_AUDIO_FAMILY
    cmdGate = IOCommandGate::commandGate(this);
    workloop = IOWorkLoop::workLoop();
    
    if (cmdGate == nullptr || workloop == nullptr) {
        IOLogError("Failed to create workloop and command gate!");
        return false;
    }
    
    if (workloop->addEventSource(cmdGate) != kIOReturnSuccess) {
        IOLogError("Failed to add command gate to workloop");
        return false;
    }
#else
    cmdGate = super::getCommandGate();
    workloop = super::getWorkLoop();
    
    if (cmdGate == nullptr || workLoop == nullptr) {
        IOLogError("Failed to retrieve workloop from IOAudioDevice!");
        return false;
    }
    
    cmdGate->retain();
    workloop->retain();
#endif
    
//    if (workloop->addEventSource(intSrc) != kIOReturnSuccess) {
//        IOLogError("Failed to add interrupt source to workloop");
//        return false;
//    }
    
//    intSrc = IOFilterInterruptEventSource::filterInterruptEventSource(this, nullptr, nullptr, provider);
//    if (intSrc == nullptr) {
//        IOLogError("Failed to construct interrupt sources");
//        return false;
//    }
    
    // Reset HDA playback for HDAudio codecs
    pciCfgUpdate(PCI_TCSEL, 0x07, 0);
    
    // Assume we are using DSP
    // Interrupt Enable | PP Enable
    ctrlPpcapIntEnable(true);
    ctrlPpcapEnable(true);
    
    // Default mailbox offset for FW ready messages
    dspMailbox.offset = HDA_DSP_MBOX_UPLINK_OFFSET;
    
#ifndef NO_AUDIO_FAMILY
    streamEngines = OSArray::withCapacity(1);
    
    Max98357a *dai = new Max98357a;
    dai->init();
    
    IntelHdaStreamEngine *engine = new IntelHdaStreamEngine;
    engine->initWithDai(dai);
    
    OSSafeReleaseNULL(dai);
    
    activateAudioEngine(engine);
    streamEngines->setObject(engine);
    OSSafeReleaseNULL(engine);
#endif
    
    // TODO: Is NHLT acpi table required?
    
    registerService();
    return true;
}

void IntelHdaDsp::free() {
    if (workloop != nullptr && intSrc != nullptr) {
        workloop->removeEventSource(intSrc);
        OSSafeReleaseNULL(intSrc);
    }
    
#ifdef NO_AUDIO_FAMILY
    if (workloop != nullptr && cmdGate != nullptr) {
        workloop->removeEventSource(cmdGate);
        OSSafeReleaseNULL(cmdGate);
    }
#endif
    
    OSSafeReleaseNULL(workloop);
    OSSafeReleaseNULL(cmdGate);
    OSSafeReleaseNULL(streamEngines);
    
    for (size_t i = 0; i < HDA_DSP_MAX_BAR; i++) {
        OSSafeReleaseNULL(barMaps[i]);
    }
    
    super::free();
}

IOReturn IntelHdaDsp::getHdaStreams() {
    uint32_t streamCaps = read32(HDA_DSP_HDA_BAR, 0x0);
    
    int captureStreams = (streamCaps >> 8) & 0x0F;
    int playbackStreams = (streamCaps>>12) & 0x0F;
//    int totalStream = captureStreams + playbackStreams;
    IOLogInfo("Found %d playback and %d capture streams", playbackStreams, captureStreams);
    
//    IOBufferMemoryDescriptor *posBuffer = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(kernel_task,
//                                                                                           kIOMemoryPhysicallyContiguous | kIODirectionInOut | kIOMapInhibitCache,
//                                                                                           8 * totalStream,
//                                                                                           0);
//    
    // TODO: Actually parse/create streams
    
    return kIOReturnSuccess;
}
