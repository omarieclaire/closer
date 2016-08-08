//
//  trigrow.cpp
//  kinectExample
//
//  Created by Marie  on 2016-08-08.
//
//

#include "trigrow.hpp"

trigrow::trigrow(ofPoint top, float h, float w1, float w2, direction d) {
    triangle(top, h, w1, d);
    width2 = w2;
    dir = d;
}


void trigrow::moveY(float amount, int screenHeight, int levelHeight) {
    if(outOfScreen(screenHeight)) {
        numWraps++;
        if(dir == LEFT) {
            b.x = a.x + width2;
        } else {
            b.x = a.x - width2;
        }
    }
    triangle::moveY(amount, screenHeight, levelHeight);
}
