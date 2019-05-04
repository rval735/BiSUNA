//
//  Discretizer.h
//  SUNA
//
//  Created by RHVT on 12/4/19.
//  Copyright © 2019 RHVT. All rights reserved.
//

#ifndef Discretizer_h
#define Discretizer_h

#include "../parameters.h"
#include <cmath>

inline float transformFromPT(unsigned short int val, float minBound = -1, float maxBound = 1) {
    if (val >= MAXIMUM_WEIGHT) {
        return maxBound;
    }
    else if (val == 0) {
        return minBound;
    }
    
    float v = (val - MID_WEIGHT) * maxBound / MID_WEIGHT;
    
    return v;
}

inline unsigned short int transformToPT(float val, float minBound = -1, float maxBound = 1) {
    if (val >= maxBound) {
        return MAXIMUM_WEIGHT;
    }
    else if (val <= minBound) {
        return 0;
    }

    float v = abs(round(MID_WEIGHT + (MID_WEIGHT * val) / maxBound));
// TODO!! For some reason to be explored, using HALF_WEIGHT in some environments
// helps to reach a solution faster (ex. Mountain_Car or Function_Approximation)
//     float v = abs(round(HALF_WEIGHT + (HALF_WEIGHT * val) / maxBound));
    
    return static_cast<ParameterType>(v);
}

#endif /* Discretizer_h */
