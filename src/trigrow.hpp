//
//  trigrow.hpp
//  kinectExample
//
//  Created by Marie  on 2016-08-08.
//
//

#ifndef trigrow_hpp
#define trigrow_hpp

#include <stdio.h>
#include "triangle.h"

class trigrow: public triangle {
public:
    trigrow(ofPoint top, float h, float w1, float w2, direction d);
    void moveY(float amount, int screenHeight, int levelHeight);
    int numWraps;
    float width2;
    direction dir;
};

#endif /* trigrow_hpp */
