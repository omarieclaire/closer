//
//  star.hpp
//  kinectExample
//
//  Created by Marie  on 2016-08-16.
//
//

#ifndef star_hpp
#define star_hpp

#include <stdio.h>
#include "triangle.h"

class star {
public:
    
    star(triangle t1, triangle t2);
    void draw();
    void setColour(int r, int g, int b);
    void moveY(float amount, int screenHeight, int levelHeight);
    bool doesIntersect (ofPoint center, float radius);
    
    ofPoint point;
    bool isCollided = false;
    float radius = 5.0;
    int damping = 60;
    int red = 255;
    int green = 255;
    int blue = 255;
};
#endif /* star_hpp */
