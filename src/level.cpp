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
}

void Level::clearTriangles() {
    triangles.clear();
}

void Level::moveY(float amount) {
    for(unsigned int i = 0; i < triangles.size() ; i++) {
        triangles[i].moveY(amount, height, screenHeight);
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
    return collision;
}
