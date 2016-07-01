#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "demoParticle.h"
#include "triangle.h"


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
	
	void drawPointCloud();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	
    void resetGame();
    
	ofxKinect kinect;

	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	//int nearThreshold;
	//int farThreshold;
    	
	int angle;
    float distance;
    float worldDist;
    
    ofPoint imageScale;

    //for sound player
    ofSoundPlayer beat;
    ofSoundPlayer synth;
    ofSoundPlayer synth2;
    ofSoundPlayer fill;
    ofSoundPlayer sax;
    ofSoundPlayer sample;
    ofSoundPlayer pop;

    float synthSpeed;
    float beatSpeed;
    float synth2Speed;
    float fillSpeed;
    float saxSpeed;
    float sampleSpeed;
    float popSpeed;

    
    ofPoint blendCenter1;
    ofPoint blendCenter2;
    ofVec3f worldPoint1;
    ofVec3f worldPoint2;
    
    bool bDrawDebug;
    
    //creating control for different "scenes"
    enum appScene{ MODE_TITLE_SCREEN, MODE_START, MODE_TRANSITION, MODE_PLAY, MODE_GAME_OVER};
    appScene currScene;
    
    
    // visuals
    ofPoint blobCenter1;
    ofPoint blobCenter2;
    float areaOfBlob1;
    float areaOfBlob2;
    float radiusOfBlob1;
    float radiusOfBlob2;
    
    vector <demoParticle> sparkles;
    vector <demoParticle> sparkles2;
    
    triangle trianglea;
    triangle triangleb;
    triangle trianglec;
    triangle triangled;
    triangle trianglee;
    triangle trianglef;
    triangle triangleg;
    triangle triangleh;
    triangle trianglei;


 
    //counters
    int soundCounter;
    int intimacyCounter;
    float glowBallCounter;
    float finalScore;
    
    int numSparkles;
    
    float glowBallRise;
    
    float intimacyThreshold;
    float glowBallWidth;
    float triangleSpeed;
    
    bool badCollision;
    ofVec3f glowBall;
    
    // The gui
    ofxPanel gui;
    
    // Constants that can be tweaked with sliders / gui
    bool hideGui;
    float modeStartGlowRiseIntimacyFactor;
    float modePlayGlowRiseFactor;
    float modePlayGlowballWidthExponent;
    
    // Sliders for the gui
    ofxFloatSlider modeStartGlowRiseIntimacyFactorSlider;
    ofxFloatSlider modePlayGlowRiseFactorSlider;
    ofxFloatSlider modePlayGlowballWidthExponentSlider;
    ofParameter<int> nearThreshold;
    ofParameter<int> farThreshold;
    
    // Listeners for the sliders and gui stuff
    void modeStartGlowRiseIntimacyFactorListener (float & intimacyFactor);
    void modePlayGlowRiseFactorListener (float & factor);
    void modePlayGlowballWidthExponentListener ( float & exponent);


    // timers
    int timeGameOverSceneStarted;
    
};
