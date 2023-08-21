//
//  ChultraSofCml.hpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#ifndef ChultraSofCml_hpp
#define ChultraSofCml_hpp

#include <IOKit/IOService.h>
#include <ChultraSofHda.hpp>

class ChultraSofCml : public ChultraSofHda {
    OSDeclareDefaultStructors(ChultraSofCml);
  
    bool start(IOService *provider) override;
    ChultraSofCml *probe(IOService *provider, SInt32 *score) override;
};

#endif /* ChultraSofCml_hpp */
