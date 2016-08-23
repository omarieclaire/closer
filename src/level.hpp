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
#include "trigrow.hpp"
#include "star.hpp"
#include "ofMain.h"

class Level {
public:
    
    Level();
    
    void setHeight(int h);
    void setScreenHeight(int sh);
    void clearTriangles();
    void clearStars();
    void addTriangle(triangle t);
    void createStars();
    bool doesIntersectTriangle(ofPoint center, float radius);
    bool doesIntersectStar(ofPoint center, float radius);
    void setTrianglesColor(ofColor interior, ofColor outline);
    void setStarColor(ofColor color);
    
    vector<triangle> triangles;
    vector<star> stars;
    void draw();
    void moveY(float amount);
    
    int height;
    int screenHeight;
};

#endif /* level_hpp */
