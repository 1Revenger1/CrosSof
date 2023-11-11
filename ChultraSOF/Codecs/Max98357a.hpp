//
//  Max98357a.hpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#ifndef Max98357a_hpp
#define Max98357a_hpp

#include "SofDai.hpp"

class Max98357a : public SofDai {
    OSDeclareDefaultStructors(Max98357a);
public:
    DaiDriver *getDaiStreams(UInt32 *count) const override;
};

#endif /* Max98357a_hpp */
