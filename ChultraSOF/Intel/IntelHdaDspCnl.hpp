//
//  ChultraSofCml.hpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#ifndef ChultraSofCml_hpp
#define ChultraSofCml_hpp

#include <IOKit/IOService.h>
#include <IntelHdaDsp.hpp>

class IntelHdaDspCnl : public IntelHdaDsp {
    OSDeclareDefaultStructors(IntelHdaDspCnl);
    
    const SOFIntelChipInfo *getChipDesc() override;
    const SOFDeviceDesc *getDeviceDesc() override;
};

class IntelHdaDspCml : public IntelHdaDspCnl {
    OSDeclareDefaultStructors(IntelHdaDspCml);
    
    const SOFDeviceDesc *getDeviceDesc() override;
};

#endif /* ChultraSofCml_hpp */
