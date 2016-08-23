//
//  star.cpp
//  kinectExample
//
//  Created by Marie  on 2016-08-16.
//
//

#include "star.hpp"

star::star(triangle t1, triangle t2) {
    
    float width = abs(t1.peak().x - t2.peak().x);
    float height = abs(t1.peak().y - t2.peak().y);
    
    if(t1.peak().x <= t2.peak().x) {
        point.x = t1.peak().x + width/2;
    } else {
        point.x = t1.peak().x - width/2;
    }
    if(t1.peak().y <= t2.peak().y) {
        point.y = t1.peak().y + height/2;
    } else {
        point.y = t1.peak().y - height/2;
    }
    
}

void star::draw() {
    if(isCollided && damping > 0 ) {
        damping -= 1;
        ofSetColor(color, damping);
        ofDrawCircle(point, radius);
    } else if (!isCollided) {
        ofSetColor(color);
        ofDrawCircle(point, radius);
    }
}

void star::setColor(ofColor c) {
    color = c;
}

bool star::doesIntersect(ofPoint center, float r) {
    if (isCollided) {
        return false;
    } else {
        if (point.distance(center) <= (r + radius)) {
                isCollided = true;
                return true;
        }
    }
    return false;
}



void star::moveY(float amount, int screenHeight, int levelHeight) {
    
    point.y += amount;
    
    if(point.y >= screenHeight) {
        point.y = 0 - levelHeight;
        isCollided = false;
        damping = 60;
    }
    
}



