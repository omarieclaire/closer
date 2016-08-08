//
//  level.hpp
//  kinectExample
//
//  Created by Marie  on 2016-08-07.
//
//

#ifndef level_hpp
#define level_hpp

#include <stdio.h>
#include "triangle.h"
#include "ofMain.h"

class Level {
public:
    
    Level();
    
    void setHeight(int h);
    void setScreenHeight(int sh);
    void clearTriangles();
    void addTriangle(triangle t);
    bool doesIntersect(ofPoint center, float radius);
    
    vector<triangle> triangles;
    void draw();
    void moveY(float amount);
    
    int height;
    int screenHeight;
};

#endif /* level_hpp */
