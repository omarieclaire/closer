#include "triangle.h"
#include "math.h"

//
//  triangle.cpp
//  kinectExample
//
//  Created by Marie  on 2016-06-28.
//
//

#include <stdio.h>

//this first triangle is just for the default constructor, who is a finicky bossy annoyance. Usage in ofApp requires all members of the class ofApp to have default constructors. Things like "vector" have them already, but since I made this one from scratch it doesn't.
// see: http://stackoverflow.com/questions/31168135/call-to-implicitly-deleted-default-constructor
//this triangle contructor takes no arguments, we just pick 0,0 for all the points.

triangle::triangle() {
    a = ofPoint(0,0);
    b = ofPoint(0,0);
    c = ofPoint(0,0);
    
    red = 255;
    green = 255;
    blue = 255;
}


//this triangle constructor does take points
triangle::triangle(ofPoint x, ofPoint y, ofPoint z) {
    a = x;
    b = y;
    c = z;
    
    red = 0;
    green = 0;
    blue = 0;
}

//in either of the above contructors it will be handy to have a method so we can update the points

void triangle::setPoints(ofPoint d, ofPoint e, ofPoint f){
    a = d;
    b = e;
    c = f;
    
}

/**
 * assumes we get positive numbers for heights
 *
 * for width: positive means -> and negative means <-
 */
void triangle::setPoints(triangle t, float gap, float height, float width, float peakHeight){
 
    //a is the bottom point of the triangle. This makes the "x coordinate of a" equal to the "x coordinate of the c point of the triangle"
    a.x = t.c.x;
    a.y = t.c.y - gap;
    
    //c is the top of the triangle
    c.x = a.x;
    c.y = a.y - height;
 
    //b is the death point
    b.x = a.x + width;
    b.y = a.y - peakHeight;

}


void triangle::draw(){
    ofSetColor(red, green, blue);
    ofDrawTriangle(a.x, a.y, b.x, b.y, c.x, c.y);

}

void triangle::setColor(int r, int g, int b){
    red = r;
    green = g;
    blue = b;

}

bool triangle::doesIntersect(ofPoint center, float radius){
    ofPolyline tri;
    tri.addVertex(a);
    tri.addVertex(b);
    tri.addVertex(c);
    tri.close();

    
    /*
    ofPoint topPoint = center;
    topPoint.y = topPoint.y - radius;
    
    ofPoint rightPoint = center;
    rightPoint.x = rightPoint.x + radius;
    
    ofPoint bottomPoint = center;
    bottomPoint.x = bottomPoint.x + radius;
    
    ofPoint leftPoint = center;
    leftPoint.x = leftPoint.x - radius;
    
//    ofPoint centerPoint = center;
//    centerPoint.x = centerPoint.x + radius;
    
    if (Figures::CollisionHelper::CollideTriangle(a,b,c,topPoint.x,topPoint.y)) {
        return true;
    } else if (Figures::CollisionHelper::CollideTriangle(a,b,c,rightPoint.x,rightPoint.y)) {
        return true;
    } else if (Figures::CollisionHelper::CollideTriangle(a,b,c,bottomPoint.x,bottomPoint.y)) {
        return true;
    } else if (Figures::CollisionHelper::CollideTriangle(a,b,c,leftPoint.x,leftPoint.y)) {
        return true;
//    } else if (Figures::CollisionHelper::CollideTriangle(a,b,c,centerPoint.x,centerPoint.y)) {
//        return true;
    } else {
        return false;
    }
    */

    bool circleinside = false;
    
    for (int i = 0; i < 8; i ++) {
        float angle = ofMap(i, 0, 8, 0, TWO_PI);
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        bool pointinside = tri.inside(x, y, tri);
        if (pointinside == true) {
            circleinside = true;
        }
    }
    return circleinside;
}

void triangle::moveY(float amount){

    a.y = a.y + amount;
    b.y = b.y + amount;
    c.y = c.y + amount;
    
        if (c.y >= ofGetScreenHeight()) {
            float currHeight = abs(a.y - c.y);
            float currPeakHeight = abs(a.y - b.y);
            
            a.y = -100;
            b.y = a.y - currPeakHeight;
            c.y = a.y - currHeight;
        }
}


//How to loop drawn triangles (and make it go a little faster every loop
//           Some thing to add to the triangle class (in the moveY method)
//             If all Ypoints are == outside the screen, then make it == its initial starting point
//               Could have triangles move at a random speed


