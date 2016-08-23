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
    void setColor(ofColor color);
    void moveY(float amount, int screenHeight, int levelHeight);
    bool doesIntersect (ofPoint center, float radius);
    
    ofPoint point;
    bool isCollided = false;
    float radius = 5.0;
    int damping = 60;
    
    ofColor color;
};
#endif /* star_hpp */
