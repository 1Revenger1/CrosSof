//
//  bitutil.h
//  ChultraSOF
//
//  Created by Gwydien on 11/10/23.
//

#pragma once

#define BIT(x) (1 << (x))
#define GENMASK(hi, lo) ((BIT(hi) - 1) - (BIT(lo) - 1))
