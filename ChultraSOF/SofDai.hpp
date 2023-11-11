//
//  SofDai.hpp
//  ChultraSOF
//
//  Created by Gwydien on 11/4/23.
//

#pragma once

#include <IOKit/IOService.h>
#include "dai.h"

class SofDai : public IOService {
    OSDeclareAbstractStructors(SofDai);
public:
    virtual DaiDriver *getDaiStreams(UInt32 *count) const = 0;
};
