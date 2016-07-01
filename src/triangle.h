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
        triangle();
        triangle(ofPoint x, ofPoint y, ofPoint z);
        void setPoints(ofPoint d, ofPoint e, ofPoint f);
        void setPoints(triangle t, float gap, float height, float width, float peakHeight);
        void draw();
        bool doesIntersect (ofPoint center, float radius);
        void setColor(int r, int g, int b);
        void moveY(float amount);
    
    
    
        ofPoint a;
        ofPoint b;
        ofPoint c;

        int red;
        int green;
        int blue;
};


#endif /* triangle_h */
