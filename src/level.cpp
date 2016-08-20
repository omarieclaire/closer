//
//  level.cpp
//  kinectExample
//
//  Created by Marie  on 2016-08-07.
//
//

#include "level.hpp"
#include "triangle.h"
#include <stdio.h>

Level::Level() {
    height = 0;
    screenHeight = 0;
    triangles = vector<triangle>();
}

void Level::setHeight(int h) {
    height = h;
}

void Level::setScreenHeight(int sh) {
    screenHeight = sh;
}

void Level::addTriangle(triangle t) {
    triangles.push_back(t);
}

void Level::draw() {
    for(unsigned int i = 0 ; i < triangles.size() ; i++) {
        triangles[i].draw();
    }
    for(unsigned int i = 0; i < stars.size() ; i++) {
        stars[i].draw();
    }
}

void Level::clearTriangles() {
    triangles.clear();
}

void Level::clearStars() {
    stars.clear();
}

void Level::moveY(float amount) {
    for(unsigned int i = 0; i < triangles.size() ; i++) {
        triangles[i].moveY(amount, height, screenHeight);
    }
    for(unsigned int i = 0; i < stars.size() ; i++) {
        stars[i].moveY(amount, height, screenHeight);
    }
}

bool Level::doesIntersect(ofPoint center, float radius) {
    bool collision = false;
    for(unsigned int i = 0 ; i < triangles.size() ; i ++) {
        if(triangles[i].doesIntersect(center, radius)) {
            collision = true;
            break;
        }
    }
    for(unsigned int i = 0 ; i < stars.size() ; i ++) {
        if(stars[i].doesIntersect(center, radius)) {
            // TODO: put effect here when collission happens.
            // a side effect of running doesIntersect is updating
            // the star for a intersection when it happens.
        }
    }
    return collision;
}

bool trianglePeakSort (triangle t1,triangle t2) {
    return (t1.peak().y > t2.peak().y);
}

void Level::createStars() {
    // Sort the triangles by their peaks
    std::sort(triangles.begin(), triangles.end(), trianglePeakSort);
    
    if(triangles.size() > 2) {
        for(unsigned int i = 0; i < triangles.size() - 1; i ++) {
            stars.push_back(star(triangles[i],triangles[i+1]));
        }
        // TODO: make star for last two triangles.
    } else if(triangles.size() ==2) {
        stars.push_back(star(triangles[0], triangles[1]));
    }
}
