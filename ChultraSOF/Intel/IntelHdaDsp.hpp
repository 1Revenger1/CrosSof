//
//  ChultraSofHda.hpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#ifndef ChultraSofHda_hpp
#define ChultraSofHda_hpp

#include <IOKit/IOFilterInterruptEventSource.h>
#include <IOKit/IOCommandGate.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

#include "SofDsp.hpp"


class IntelHdaDsp : public SofDsp {
    OSDeclareAbstractStructors(IntelHdaDsp);
    
    void free() override;
    IntelHdaDsp *probe(IOService *provider, SInt32 *score) override;
    
    int getMsiInterruptIndex();
protected:
    
    IOWorkLoop *workloop {nullptr};
    IOFilterInterruptEventSource *intSrc {nullptr};
    IOCommandGate *cmdGate {nullptr};
    
    // SOF Ops
    bool sofProbe(IOService *provider) override;
    IOReturn preFwRun() override;
    IOReturn postFwRun() override;
    IOReturn runFw() override;
    
    // Dsp Control
    IOReturn ctrlGetCaps();
    IOReturn ctrlLinkReset(bool reset);
    void ctrlPpcapEnable(bool enable);
    void ctrlPpcapIntEnable(bool enable);
    void ctrlMiscClockGating(bool enable);
    IOReturn ctrlClockPowerGating(bool enable);
    
    
private:
    IOReturn getHdaStreams();
    void freeStreams();
    
    OSArray *streamEngines {nullptr};
    OSArray *streams {nullptr};
    bool l1Disabled {false};
    
    IOBufferMemoryDescriptor *posbuf {nullptr};
    IOBufferMemoryDescriptor *corb {nullptr};
};


#endif // ChultraSofHda_hpp
