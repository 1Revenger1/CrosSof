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
  
    bool start(IOService *provider) override;
    IntelHdaDspCnl *probe(IOService *provider, SInt32 *score) override;
};

#endif /* ChultraSofCml_hpp */
