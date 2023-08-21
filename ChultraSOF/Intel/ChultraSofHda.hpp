//
//  ChultraSofHda.hpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#ifndef ChultraSofHda_hpp
#define ChultraSofHda_hpp

#include <IOKit/IOService.h>

class ChultraSofHda : public IOService {
    OSDeclareDefaultStructors(ChultraSofHda);
  
    bool start(IOService *provider) override;
    ChultraSofHda *probe(IOService *provider, SInt32 *score) override;
};


#endif // ChultraSofHda_hpp
