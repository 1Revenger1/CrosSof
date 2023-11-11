
#pragma once

#include <IOKit/audio/IOAudioDevice.h>

#include "hda.h"
#include "sof.h"

class IOPCIDevice;
struct SOFIntelChipInfo;

#define NO_AUDIO_FAMILY


#ifdef NO_AUDIO_FAMILY
class SofDsp : public IOService {
#else
class SofDsp : public IOAudioDevice {
#endif
    OSDeclareAbstractStructors(SofDsp);
    
public:
    bool start(IOService *provider) override {
        if (!IOService::start(provider)) return false;
#ifdef NO_AUDIO_FAMILY
        return initHardware(provider);
#endif
    };
protected:
    LIBKERN_RETURNS_RETAINED OSData *getFirmware(const char *firmwareName);
    
#ifdef NO_AUDIO_FAMILY
    virtual bool initHardware(IOService *provider);
#else
    virtual bool initHardware(IOService *provider) override;
#endif
    
    // Sof ops
    
    virtual bool sofProbe(IOService *provider) = 0;
//    virtual IOReturn loadFirmware() = 0;
    virtual IOReturn preFwRun() = 0;
    virtual IOReturn postFwRun() = 0;
    virtual IOReturn runFw() = 0;
    
    virtual const SOFDeviceDesc *getDeviceDesc() = 0;
    virtual const SOFIntelChipInfo *getChipDesc() = 0;
    
    inline uint32_t read32(hda_bars_t bar, IOVirtualAddress offset) {
        return *(uint32_t *)((uint8_t *) bars[bar] + offset);
    }
    
    inline void write32(hda_bars_t bar, IOVirtualAddress offset, uint32_t newVal) {
        *(uint32_t *)((uint8_t *) bars[bar] + offset) = newVal;
    }
    
    bool pciCfgUpdate(IOByteCount offset, UInt32 mask, UInt32 value);
    bool dspUpdate(hda_bars_t bar, IOByteCount offset, UInt32 mask, UInt32 value);
    
    IOPCIDevice *pci {nullptr};
    
    const SOFDeviceDesc *desc;
    const SOFIntelChipInfo *chip;
    
    SOFMailbox dspMailbox;
    SOFMailbox hostMailbox;
    SOFMailbox fwMailbox;
    
    size_t streamCount {0};
    
    IOMemoryMap *barMaps[HDA_DSP_MAX_BAR] {};
    IOVirtualAddress bars[HDA_DSP_MAX_BAR] {};
private:
    /* Helper to manage DSP state */
    void setFwState(sof_fw_state_t newState);
    IOReturn runFirmware();

    sof_fw_state_t fwState {SOF_FW_BOOT_NOT_STARTED};
    bool firstBoot {true};
};
