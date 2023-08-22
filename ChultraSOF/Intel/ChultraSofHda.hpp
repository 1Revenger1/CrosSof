//
//  ChultraSofHda.hpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#ifndef ChultraSofHda_hpp
#define ChultraSofHda_hpp

#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

class ChultraSofHda : public IOService {
    OSDeclareDefaultStructors(ChultraSofHda);
  
    bool start(IOService *provider) override;
    ChultraSofHda *probe(IOService *provider, SInt32 *score) override;
    
protected:
    IOPCIDevice *pci {nullptr};
};


#endif // ChultraSofHda_hpp