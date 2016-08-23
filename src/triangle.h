#include "ofMain.h"


//
//  triangle.h
//  kinectExample
//
//  Created by Marie  on 2016-06-28.
//
//

#ifndef triangle_h
#define triangle_h

class triangle{
    
public:
    enum direction{ LEFT, RIGHT };
    triangle();
    triangle(ofPoint x, ofPoint y, ofPoint z);
    triangle(ofPoint top, float h, float w, direction d);
    void setPoints(ofPoint d, ofPoint e, ofPoint f);
    void setPoints(triangle t, float gap, float height, float width, float peakHeight);
    void setPoints(ofPoint top, float h, float w, direction d);
    void draw();
    bool doesIntersect (ofPoint center, float radius);
    bool outOfScreen(int screenHeight);
    void setColor(int r, int g, int b);
    void setColor(ofColor interior, ofColor outline);
    void moveY(float amount);
    void moveY(float amount, int screenHeight, int levelHeight);
    
    ofPoint peak();
    
    
    /**
     * we assume a is the "top" of the triangle
     * this is important for wrapping.
     */
    ofPoint a;
    ofPoint b;
    ofPoint c;
    
    ofColor interior;
    ofColor outline;
};


#endif /* triangle_h */
