#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "demoParticle.h"
#include "triangle.h"
#include "trigrow.hpp"
#include "level.hpp"


// Windows users:
// You MUST install the libfreenect kinect drivers in order to be able to use
// ofxKinect. Plug in the kinect and point your Windows Device Manager to the
// driver folder in:
//
//     ofxKinect/libs/libfreenect/platform/windows/inf
//
// This should install the Kinect camera, motor, & audio drivers.
//
// You CANNOT use this driver and the OpenNI driver with the same device. You
// will have to manually update the kinect device to use the libfreenect drivers
// and/or uninstall/reinstall it in Device Manager.
//
// No way around the Windows driver dance, sorry.

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class ofApp : public ofBaseApp {

public:
	
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void windowResized(int w, int h);
    void resetGame();

	ofxKinect kinect;
    // kinect display related
	//ofxCvColorImage colorImg;
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	ofxCvContourFinder contourFinder; // finding the countour of the players using the kinect
    
	bool bThreshWithOpenCV; // determining how far away the kinect should look for players
    int angle; //determines the angle of the kinect

    
    ofPoint imageScale; //scales the drawn image to size
    
    //creating control for different "scenes"
    enum appScene{ MODE_TITLE_SCREEN, MODE_START, MODE_TRANSITION, MODE_PLAY, MODE_GAME_OVER};
    appScene currScene;    // current scene


    //for sound player
    ofSoundPlayer synth;
    ofSoundPlayer beat;
    ofSoundPlayer synth2;
    ofSoundPlayer fill;
    ofSoundPlayer pop;

    float synthSpeed;
    float beatSpeed;
    float synth2Speed;
    float fillSpeed;
    float popSpeed;

    // distance between the players
    float playerDistance;

    //center of player blob, unblended
    ofPoint blobCenter1;
    ofPoint blobCenter2;
    
    //center of player blob, blended
    ofPoint blendCenter1;
    ofPoint blendCenter2;
    
    // get area of blob to calculate radius
    float areaOfBlob1;
    float areaOfBlob2;
    
    // get radius of blob to draw a circle of that size
    float radiusOfBlob1;
    float radiusOfBlob2;
    
    // sparkles for each blob
    vector <demoParticle> sparkles;
    vector <demoParticle> sparkles2;
    
    // The level (holds triangles)
    Level level;
  
    // triangles of DEATH
    triangle trianglea;
    triangle triangleb;
    triangle trianglec;
    triangle triangled;
    triangle trianglee;
    triangle trianglef;
    triangle triangleg;
    triangle triangleh;
    triangle trianglei;
    
    // Colours!
    ofColor glowballColor;
    ofColor glowballSparklesColor;
    ofColor peopleBlobColor;
    ofColor peopleOutlineColor;
    ofColor peopleSparklesColor;
    ofColor triangleOutlineColor;
    ofColor triangleInteriorColor;
    ofColor starColor;
    
    // Transparancy
    int glowballTransperancy;
    int triangleInteriorTransperancy;
    
  
    // counters
    // I use intimacy counter for many things (draw, grow, and rise ball in first level, trigger next level. Can't go lower than 0, can't go higher than number of sparkles
    int intimacyCounter;
    
    // Used to count score
    float scoreCounter;
    
    // Used to store highest score
    float highScore;
    
    // Used to identify a new high scorer
    bool newHighScore;
    
    // keeping the game over screen in place for 3 seconds
    int timeGameOverSceneStarted;
    
    // Number of sparkles used (also used to bound intimacy
    int numSparkles;
    
    //speed at which glowball rises in the beginning
    float glowBallRise;
    
    // distance you need to be before the intimacy counter will increase
    float intimacyThreshold;
    
    // determines the width of the glowBall based on intimacy counter
    float glowBallWidth;
    
    // determines the speed of the triangles, using the scoreCounter
    float triangleSpeed;
    
    // how you die
    bool badCollision;
    
    // postition of glowBall
    ofVec3f glowBall;
    
    // for flipping the screen for setup reasons
    bool mirrorScreen = false;
   
    
    //sliders for the obsolete gui
    ofParameter<int> nearThreshold;
    ofParameter<int> farThreshold;
    
    // Constants that can be tweaked with sliders / gui (some of these may be obsolete)
    bool showInfo;
    float modeStartGlowRiseIntimacyFactor;
    float modePlayGlowRiseFactor;
    float modePlayGlowballWidthExponent;
    bool bDrawDebug;
    
    ofTrueTypeFont font;
    ofTrueTypeFont fontTwo;

    
};
