//
//  ChultraSofHda.hpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#ifndef ChultraSofHda_hpp
#define ChultraSofHda_hpp

#include <IOKit/IOService.h>
#include <IOKit/IOFilterInterruptEventSource.h>
#include <IOKit/IOCommandGate.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

enum hda_cap_ids_t {
    SOF_HDA_GTS_CAP_ID = 1,
    SOF_HDA_ML_CAP_ID = 2,
    SOF_HDA_PP_CAP_ID = 3,
    SOF_HDA_SPIB_CAP_ID = 4,
    SOF_HDA_DRSM_CAP_ID = 5
};

enum hda_bars_t {
    SOF_HDA_BAR,
    SOF_PP_BAR,
    SOF_SPIB_BAR,
    SOF_DRSM_BAR,
    SOF_DSP_BAR,
    SOF_MAX_BAR
};

struct SOFMailbox {
    uint32_t offset;
    size_t size;
};

class ChultraSofHda : public IOService {
    OSDeclareDefaultStructors(ChultraSofHda);
  
    bool init() override;
    bool start(IOService *provider) override;
    void free() override;
    ChultraSofHda *probe(IOService *provider, SInt32 *score) override;
    
    int getMsiInterruptIndex();
    OSData *getFirmware(const char *firmwareName);
protected:
    IOPCIDevice *pci {nullptr};
    
    inline uint32_t read32(hda_bars_t bar, IOVirtualAddress offset) {
        return *(uint32_t *)((uint8_t *) bars[bar] + offset);
    }
    
    inline void write32(hda_bars_t bar, IOVirtualAddress offset, uint32_t newVal) {
        *(uint32_t *)((uint8_t *) bars[bar] + offset) = newVal;
    }
    
    IOMemoryMap *barMaps[SOF_MAX_BAR];
    IOVirtualAddress bars[SOF_MAX_BAR];
    
    IOWorkLoop *workloop {nullptr};
    IOFilterInterruptEventSource *intSrc {nullptr};
    IOCommandGate *cmdGate {nullptr};

    SOFMailbox dspMailbox;
    SOFMailbox hostMailbox;
    SOFMailbox fwMailbox;
    
private:
    IOReturn ctrlGetCaps();
    IOReturn getHdaStreams();
};


#endif // ChultraSofHda_hpp
