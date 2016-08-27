#include "ofApp.h"
#include "math.h"
#include "level.hpp"

/*
    If you are struggling to get the device to connect ( especially Windows Users )
    please look at the ReadMe: in addons/ofxKinect/README.md
*/

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
    
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
    
	// print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}

	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 230;
    farThreshold = 196;//70;
	bThreshWithOpenCV = true;
	
	ofSetFrameRate(60);
	
// zero the tilt on startup
	angle = 0;
//	kinect.setCameraTiltAngle(angle);
    
    font.load("Ayuthaya", 22);

    
//sound
    
    synth.load("sounds/synth.mp3");
    synth.setLoop(true);
    synth.setVolume(0.8f);
    synth.setSpeed(1.0f);
    synth.setMultiPlay(true);
    synth.play();
    
    
    beat.load("sounds/beat.wav");
    beat.setLoop(true);
    beat.setVolume(0.2f);
    beat.setSpeed(1.0f);
    beat.setMultiPlay(true);
    beat.play();
    

    synth2.load("sounds/closerPoint.mp3");
    synth2.setVolume(1.0f);
    synth.setSpeed(1.0f);


    fill.load("sounds/death2.mp3");
    fill.setVolume(0.50f);
    fill.setSpeed(1.0f);

 /*
    sax.load("sounds/synth3.mp3");
    sax.setLoop(false);
    sax.setVolume(0.75f);
    sax.setSpeed(1.0f);
    sax.setMultiPlay(true);
    sax.play();

    sample.load("sounds/synth3.mp3");
    sample.setLoop(false);
    sample.setVolume(0.80f);
    sample.setSpeed(1.0f);
    sample.setMultiPlay(true);
    sample.play();
    */
    
    pop.load("sounds/popp.mp3");
    //pop.setLoop(true);
    pop.setVolume(0.8f);
    pop.setSpeed(1.0f);
    pop.setMultiPlay(true);
    //pop.play();

    numSparkles = 1000;
    ofPoint initialSparklePoint(300,300);
    sparkles.assign(numSparkles, demoParticle(initialSparklePoint));
    sparkles2.assign(numSparkles, demoParticle(initialSparklePoint));
    resetGame();
    
// setup GUI stuff
    hideGui = true;
    
    gui.setup(); // most of the time this doesn't need a name
    
    modeStartGlowRiseIntimacyFactor = 0.8;
    
    /**
     * CHANGE DEFAULT VALUES FOR THINGS IN SLIDERS. The first number is the default value.
     * the next numbers are min and max.
     */
    gui.add(modeStartGlowRiseIntimacyFactorSlider.setup("MS: glowrise IF", 0.8, 0.5, 1.5));
    gui.add(modePlayGlowRiseFactorSlider.setup("MP: glowrise F", 0.003, 0, 0.01));
    gui.add(modePlayGlowballWidthExponentSlider.setup("MP: glowball Exp", 2, 0, 3));
    gui.add(nearThreshold.set("near Thresh",255,0,255));
    gui.add(farThreshold.set("near Thresh",99,0,255));

    modeStartGlowRiseIntimacyFactorSlider.addListener(this, &ofApp::modeStartGlowRiseIntimacyFactorListener);
    modePlayGlowRiseFactorSlider.addListener(this, &ofApp::modePlayGlowRiseFactorListener);
    modePlayGlowRiseFactorSlider.addListener(this, &ofApp::modePlayGlowballWidthExponentListener);

    timeGameOverSceneStarted = 0;
    
    highScore = 0;
}

//-------------------- GUI LISTENERS ---------------------------
void ofApp::modeStartGlowRiseIntimacyFactorListener (float & intimacyFactor) {
    modeStartGlowRiseIntimacyFactor = intimacyFactor;
}

void ofApp::modePlayGlowRiseFactorListener (float & factor) {
    modePlayGlowRiseFactor = factor;
}

void ofApp::modePlayGlowballWidthExponentListener(float & exponent) {
    modePlayGlowballWidthExponent = exponent;
}

//--------------------------------------------------------------
void ofApp::update() {
	
	
	kinect.update();
	
// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels());
		
// we do two thresholds - one for the far plane and one for the near plane
// we then do a cvAnd to get the pixels which are a union of the two thresholds
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		
// update the cv images
		grayImage.flagImageChanged();
		
// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
	}
    
  
    //scales the drawn image so to size
    imageScale.x = ofGetWidth() / kinect.getWidth();
    imageScale.y = ofGetHeight() / kinect.getHeight();
    
    //find how many blobs there are?
    if(contourFinder.nBlobs > 0 ){
        
        //get blob center
        blobCenter1 = contourFinder.blobs[0].centroid * imageScale;
        blobCenter2 = contourFinder.blobs[1].centroid * imageScale;
        
        // blob one should be on left
        if(blobCenter1.x > blobCenter2.x){
            ofPoint tempCent = blobCenter1;
            blobCenter1 = blobCenter2;
            blobCenter2 = tempCent;
        }

        
        //replace blobcenter with global
        
        blendCenter1 = 0.8 * blendCenter1 + 0.2 * blobCenter1;
        blendCenter2 = 0.8 * blendCenter2 + 0.2 * blobCenter2;
        
        playerDistance = blendCenter1.distance(blendCenter2);
        
        // draw circles of mass for each blob
        areaOfBlob1 = contourFinder.blobs[0].area;
        radiusOfBlob1 = sqrt(areaOfBlob1 / PI);
        areaOfBlob2 = contourFinder.blobs[1].area;
        radiusOfBlob2 = sqrt(areaOfBlob2 / PI);
        
        
        // intimacy threshold
        if (playerDistance <= intimacyThreshold) {
            
            intimacyCounter++;
            
            if (intimacyCounter > numSparkles)
                intimacyCounter = numSparkles;
            
        } else {
            intimacyCounter--;
            if (intimacyCounter < 0)
                intimacyCounter = 0;
        }
        
        // calculates the glowBall position
        float oldGlowBallY = glowBall.y;
        glowBall = blobCenter1.getMiddle(blobCenter2);
        
        if(currScene == MODE_START) {
            if(intimacyCounter >= 100) {
                glowBallRise = intimacyCounter * modeStartGlowRiseIntimacyFactor;
                glowBallWidth = intimacyCounter * 0.1;
                glowBall.y = glowBall.y - glowBallRise;
            }
        } else if (currScene == MODE_PLAY) {
            glowBallRise = playerDistance * modePlayGlowRiseFactor;
            
            glowBallWidth = ofMap(playerDistance, 100, 700, 200, 5, true);
        } else {
            glowBallRise = 0;
        }
        
        //calculates all sparkle attract points>>>>>
        for(unsigned int p = 0; p < sparkles.size() ; p++) {
            
            if (p < intimacyCounter) {
                sparkles[p].setAttractPoint(glowBall);
            } else {
                sparkles[p].setAttractPoint(blobCenter1);
            }
            sparkles[p].update();
        }
        
        //calculates all sparkle attract points
        for(unsigned int p = 0; p < sparkles2.size() ; p++) {
            
            if (p < intimacyCounter) {
                sparkles2[p].setAttractPoint(glowBall);
            } else {
                sparkles2[p].setAttractPoint(blobCenter2);
            }
            sparkles2[p].update();

        }

        
        //SOUND: COUNTER
        
        if(ofGetFrameNum() % 60 == 0){
            
            // SOUND: changes speed of sound to match map
            //synthSpeed = ofMap(playerDistance, 100.0f, 1200.0f, 0.2f, 2.5f);
            //synth.setSpeed(synthSpeed);
            
            beatSpeed = ofMap(playerDistance, 100.0f, 1200.0f, 2.0f, 0.2f);
            beat.setSpeed(beatSpeed);
            
            popSpeed = ofMap(playerDistance, 100.0f, 1200.0f, 0.2f, 2.5f);
            pop.setSpeed(popSpeed);
            
            /*
             synth2Speed = ofMap(playerDistance, 100.0f, 1200.0f, 0.2f, 2.5f);
             synth2.setSpeed(synth2Speed);
             
             fillSpeed = ofMap(playerDistance, 100.0f, 1200.0f, 2.0f, 0.2f);
             fill.setSpeed(fillSpeed);
             
             saxSpeed = ofMap(playerDistance, 100.0f, 1200.0f, 0.2f, 2.5f);
             sax.setSpeed(saxSpeed);
             
             sampleSpeed = ofMap(playerDistance, 100.0f, 1200.0f, 2.0f, 0.2f);
             sample.setSpeed(sampleSpeed);
             
             */
            
        }
        
    } else {
        // TO DO: not enough blobs detected, perhaps change mode?
    }
    
    // determine which scene we go to
    
    /*
     * The triangle speed gets faster as the score rises, making the game more difficult as you play. Had to define it here because can't create variables within a switch statement (why?) 
     */
    float triangleSpeedIncrease;
    
    switch (currScene) {
        case MODE_TITLE_SCREEN:
            if( contourFinder.nBlobs >= 2 ){
                if(ofGetElapsedTimef() >= 4){
                    currScene = MODE_START;
                }
            }
        break;
        case MODE_START:
            if( glowBall.y <= 0){
                if(ofGetElapsedTimef() >= 5){
                    currScene = MODE_TRANSITION;
                }
            }

        break;
        case MODE_TRANSITION:
            currScene = MODE_PLAY;
            glowBall.y = 500;//ofGetScreenHeight();
            
        break;
        case MODE_PLAY:
            if( badCollision == true){
                pop.play();
                fill.play();
                //if(ofGetElapsedTimef() >= 0){
                    currScene = MODE_GAME_OVER;
                    timeGameOverSceneStarted = ofGetElapsedTimeMillis();
 //                   sparkles.clear();
                //}
            }
            
            scoreCounter = scoreCounter + 0.1;
            
            /**
             * TRIANGLE MOVEMENT HERE
             */
            triangleSpeedIncrease = scoreCounter/500;
            triangleSpeed = ofMap(playerDistance, 100, 700, 0.5, 3.0) + triangleSpeedIncrease;
            level.moveY(triangleSpeed);

            //calculates all sparkle attract points
            for(unsigned int p = 0; p < sparkles.size() ; p++) {
                if (p < intimacyCounter) {
                    sparkles[p].setAttractPoint(glowBall);
                } else {
                    sparkles[p].setAttractPoint(blobCenter1);
                }
                sparkles[p].update();
            }
            
            //calculates all sparkle attract points
            for(unsigned int p = 0; p < sparkles2.size() ; p++) {
                if (p < intimacyCounter) {
                    sparkles2[p].setAttractPoint(glowBall);
                } else {
                    sparkles2[p].setAttractPoint(blobCenter2);
                }
                sparkles2[p].update();
            }
            
            if(level.doesIntersectStar(glowBall, glowBallWidth)) {
                synth2.play();
                scoreCounter += 100;

            }
            
            if(level.doesIntersectTriangle(glowBall,glowBallWidth)) {
                badCollision = true;
                pop.play();
            }

        break;
        case MODE_GAME_OVER:
            if (!newHighScore && scoreCounter > highScore){
                newHighScore = true;
                highScore = scoreCounter;
            }
            
            if(ofGetElapsedTimeMillis() - timeGameOverSceneStarted > 5*1000){
                resetGame();
                currScene = MODE_TITLE_SCREEN;
            }
        break;
    }
}

void ofApp::resetGame(){
    
    playerDistance = 0;
    
    //make things smooth
    blendCenter1.set(0,0);
    blendCenter2.set(0,0);
    
    //Set the scene (don't need to otherwise setup
    currScene = MODE_TITLE_SCREEN;
    
    // reset colours
    // Forth parameter is transperancy and is optional.

//    int randomNumber = (int) ofRandom(200,255);

    
    glowballColor.set(0,0,255,80);
    glowballSparklesColor.set(0,0,255);
    peopleBlobColor.set(255,0,255,10);
    peopleOutlineColor.set(255,0,255,200);
    peopleSparklesColor.set(255,0,255);
    triangleOutlineColor.set(255,0,255);
    triangleInteriorColor.set(255,0,255,70);
    starColor.set(0,0,255);
    
    
    
    // Reset the sparkles
    for(unsigned int i = 0 ; i < sparkles.size() ; i++) {
        sparkles[i].reset();
        sparkles[i].setMode(PARTICLE_MODE_ATTRACT);
    }
    
    for(unsigned int i = 0 ; i < sparkles2.size() ; i++) {
        sparkles2[i].reset();
        sparkles2[i].setMode(PARTICLE_MODE_ATTRACT);
    }
    
    float screenWidth = ofGetScreenWidth();
    
    /**
     * Setup triangles
     * void triangle::setPoints(triangle t, float gap, float height, float width, float peakHeight)
     * parameters are:
     * GAP | HEIGHT | WIDTH | PEAK HEIGHT
     */
    float screenHeight = ofGetScreenHeight();
    level.setHeight(26586);
    level.setScreenHeight(screenHeight);
    level.clearTriangles();
    level.clearStars();
    
    level.setHeight(16383);

    level.addTriangle(triangle(ofPoint(0,0 - 16383),ofPoint(640.15,276.107 - 16383),ofPoint(0,552.215 - 16383)));
    level.addTriangle(triangle(ofPoint(0,581.301 - 16383),ofPoint(848.718,947.444 - 16383),ofPoint(0,1313.587 - 16383)));
    level.addTriangle(triangle(ofPoint(0,1622.525 - 16383),ofPoint(463.41,1822.444 - 16383),ofPoint(0,2022.363 - 16383)));
    level.addTriangle(triangle(ofPoint(0,2617.027 - 16383),ofPoint(528.14,2844.87 - 16383),ofPoint(0,3072.714 - 16383)));
    level.addTriangle(triangle(ofPoint(0,3511.872 - 16383),ofPoint(1001.195,3943.795 - 16383),ofPoint(0,4375.718 - 16383)));
    level.addTriangle(triangle(ofPoint(0,4222.144 - 16383),ofPoint(946.112,4630.303 - 16383),ofPoint(0,5038.463 - 16383)));
    level.addTriangle(triangle(ofPoint(0,5500.167 - 16383),ofPoint(640,5776.267 - 16383),ofPoint(0,6052.368 - 16383)));
    level.addTriangle(triangle(ofPoint(0,6307.993 - 16383),ofPoint(528.14,6535.836 - 16383),ofPoint(0,6763.679 - 16383)));
    level.addTriangle(triangle(ofPoint(0,7124.104 - 16383),ofPoint(716.748,7433.314 - 16383),ofPoint(0,7742.524 - 16383)));
    level.addTriangle(triangle(ofPoint(0,8390.451 - 16383),ofPoint(716.748,8699.661 - 16383),ofPoint(0,9008.871 - 16383)));
    level.addTriangle(triangle(ofPoint(0,9535.604 - 16383),ofPoint(567.284,9780.334 - 16383),ofPoint(0,10025.065 - 16383)));
    level.addTriangle(triangle(ofPoint(0,10188.494 - 16383),ofPoint(946.111,10596.653 - 16383),ofPoint(0,11004.813 - 16383)));
    level.addTriangle(triangle(ofPoint(0,11552.926 - 16383),ofPoint(946.112,11961.085 - 16383),ofPoint(0,12369.245 - 16383)));
    level.addTriangle(triangle(ofPoint(0,12866.191 - 16383),ofPoint(946.111,13274.351 - 16383),ofPoint(0,13682.511 - 16383)));
    level.addTriangle(triangle(ofPoint(0,13935.19 - 16383),ofPoint(716.748,14244.4 - 16383),ofPoint(0,14553.611 - 16383)));
    level.addTriangle(triangle(ofPoint(0,14573.886 - 16383),ofPoint(576.253,14822.485 - 16383),ofPoint(0,15071.086 - 16383)));
    level.addTriangle(triangle(ofPoint(0,15329.592 - 16383),ofPoint(473.056,15533.672 - 16383),ofPoint(0,15737.752 - 16383)));
    level.addTriangle(triangle(ofPoint(0,15941.831 - 16383),ofPoint(251.474,16050.318 - 16383),ofPoint(0,16158.806 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,14285.331 - 16383),ofPoint(716.748,14528.322 - 16383),ofPoint(screenWidth,14771.312 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,15329.592 - 16383),ofPoint(806.943,15533.672 - 16383),ofPoint(screenWidth,15737.752 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,15980.863 - 16383),ofPoint(946.111,16124.904 - 16383),ofPoint(screenWidth,16268.946 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,13339.246 - 16383),ofPoint(716.748,13582.236 - 16383),ofPoint(screenWidth,13825.227 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,12216.091 - 16383),ofPoint(333.888,12624.25 - 16383),ofPoint(screenWidth,13032.409 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,11095.331 - 16383),ofPoint(358.374,11492.926 - 16383),ofPoint(screenWidth,11890.521 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,9537.344 - 16383),ofPoint(716.748,9780.334 - 16383),ofPoint(screenWidth,10023.324 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,8672.003 - 16383),ofPoint(264.07,9110.283 - 16383),ofPoint(screenWidth,9548.561 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,7755.767 - 16383),ofPoint(333.887,8163.926 - 16383),ofPoint(screenWidth,8572.085 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,6812.5 - 16383),ofPoint(473.056,7160.621 - 16383),ofPoint(screenWidth,7508.742 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,6259.736 - 16383),ofPoint(640,6535.836 - 16383),ofPoint(screenWidth,6811.937 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,4959.948 - 16383),ofPoint(333.888,5368.107 - 16383),ofPoint(screenWidth,5776.267 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,4108.965 - 16383),ofPoint(716.748,4351.955 - 16383),ofPoint(screenWidth,4594.946 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,2692.438 - 16383),ofPoint(500.598,3028.678 - 16383),ofPoint(screenWidth,3364.917 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,2109.568 - 16383),ofPoint(350.598,2510.519 - 16383),ofPoint(screenWidth,2911.469 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,1528.1 - 16383),ofPoint(640,1804.2 - 16383),ofPoint(screenWidth,2080.3 - 16383)));
    level.addTriangle(triangle(ofPoint(screenWidth,215.975 - 16383),ofPoint(500.598,552.214 - 16383),ofPoint(screenWidth,888.454 - 16383)));
    
    // Set color of triangles
    level.setTrianglesColor(triangleInteriorColor, triangleOutlineColor);
    
    
    // Create the stars based on the locations of the triangles.
    level.createStars();
    
    // Set color of star
    level.setStarColor(starColor);
    
    //count things
    intimacyCounter = 0;
    scoreCounter = 0;
    newHighScore = false;
    
    // glowball and intimacy stuff
    glowBallRise = 0;
    intimacyThreshold = 450;
    glowBallWidth = 0;
    badCollision = 0;
    
    ofResetElapsedTimeCounter();
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofPushMatrix();
    ofScale(-1,1);
    ofTranslate(-ofGetWidth(), 0);
    //ofBackground(255,0,0);

    // draw depending on scene
    if(currScene == MODE_TITLE_SCREEN){
        ofSetBackgroundColor(0, 0, 0);

    //START MODE>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    } else if(currScene == MODE_START){
        //ofDrawBitmapString("START", 100, 50);

        // this changes the background colour
        //int bgColor = (int) ofMap(playerDistance, 100.0f, 1200.0f, 0, 255);
        //ofBackground(bgColor);
        
        ofSetBackgroundColor(0, 0, 0);
        
        // draw from the live kinect
        //kinect.drawDepth(0, 0, ofGetWidth(), ofGetHeight());//, 400, 300);
        //kinect.draw(420, 10, 400, 300);
        ofSetColor(peopleOutlineColor);
        contourFinder.draw(0, 0, ofGetWidth(), ofGetHeight(), false,peopleOutlineColor);
        //contourFinder.draw(10, 320, 400, 300);
        //grayImage.draw(0,0);
        ofEasyCam easyCam;
        
        //!looks for the centre in the two biggest blobs
        if(contourFinder.nBlobs > 0 ){
            
            //ofDrawCircle(getBlobCenter1.x, getBlobCenter1.y, 10);
            //ofDrawCircle(getBlobCenter2.x, getBlobCenter2.y, 10);
            ofDrawLine(blobCenter1.x, blobCenter1.y, blobCenter2.x, blobCenter2.y);
            
            //instructions
            ofClear(0);
            //ofDrawBitmapString("CLOSER [two players || no touching /n/n oo]", 100, 100);
            
            
            // draw circles of mass for each blob
            //ofSetColor(255,85);
            ofSetColor(peopleBlobColor);
            ofDrawCircle(blendCenter1.x, blendCenter1.y, radiusOfBlob1);
            ofDrawCircle(blendCenter2.x, blendCenter2.y, radiusOfBlob2);
            
            
            // draws the glowball between blobs (also, in update the glowball rise is calculated)
            //ofSetColor(255,80);
            ofSetColor(glowballColor);
            if (intimacyCounter >= 100) {
                //TODO: investigate here
                ofDrawCircle(glowBall.x, glowBall.y,glowBallWidth);
            }
            
            //SPARKLE DRAW sets colours and draws the sparkles (attract point is calculated in update)
            // UPDATE PARTICLE COLOURS IN HERE!!!!!!
            for(unsigned int p = 0; p < sparkles.size() ; p++) {
                if (p < intimacyCounter) {
                    // these particles are traveling towards the center ball
                    sparkles[p].setColour(0,0,255);
                } else {
                    sparkles[p].setColour(255,63,180);
                }
                sparkles[p].draw();
            }
            
             //SPARKLE DRAW sets colours and draws the sparkles2 (attract point is calculated in update)
            // UPDATE PARTICLE COLOURS IN HERE!!!!!!
            for(unsigned int p = 0; p < sparkles2.size() ; p++) {
                if (p < intimacyCounter) {
                    // these particles are traveling towards the center ball
                    sparkles2[p].setColour(0,0,255);
                } else {
                    sparkles2[p].setColour(255,63,180);
                }
                sparkles2[p].draw();
            }
            
            ofDrawLine(0, 20, ofGetScreenWidth(), 20);
            
        }
        
        
//TRANSITION>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

        
//PLAY>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

        
    }else if(currScene == MODE_PLAY){
        
        //GLOWBALL: COUNTER
        
        //ofDrawBitmapString("PLAY", 100, 50);
        //ofDrawBitmapString("Your Score: " + ofToString( (int) scoreCounter), 100, 100);
        
        level.draw();
        
        //ofSetColor(255,80);
        ofSetColor(glowballColor);
        ofDrawCircle(glowBall.x, glowBall.y, glowBallWidth);
        
        for(unsigned int p = 0; p < sparkles.size() ; p++) {
            if (p < intimacyCounter) {
                //sparkles[p].setColour(0,0,255);
                sparkles[p].setColor(glowballSparklesColor);
            } else {
                //sparkles[p].setColour(255,63,180);
                sparkles[p].setColor(peopleSparklesColor);
            }
            sparkles[p].draw();
        }
        
        for(unsigned int p = 0; p < sparkles2.size() ; p++) {
            if (p < intimacyCounter) {
                //sparkles[p].setColour(0,0,255);
                sparkles[p].setColor(glowballSparklesColor);
            } else {
                //sparkles[p].setColour(255,63,180);
                sparkles[p].setColor(peopleSparklesColor);
            }
            sparkles2[p].draw();
        }
        
        
//COLLISION TEST: draws temp line for collisions
        
        ofDrawLine(0, 40, ofGetScreenWidth(), 20);
        
        if (glowBall.y <= 40) {
        
//GAME OVER>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        
        }else if(currScene == MODE_GAME_OVER){
            ofBackground(255,255,255);
        
        }
    }
    
    contourFinder.draw(0, 0, ofGetWidth(), ofGetHeight(), false,peopleOutlineColor);
    
    if (bDrawDebug == true) {
        grayImage.draw(0,0,320,240);
    }
    
    
// draw instructions

    
    ofSetColor(255, 255, 255);
	
    /*
    
    HIDEME
    
    stringstream reportStream;
        
    if(kinect.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
        << ofToString(kinect.getMksAccel().y, 2) << " / "
        << ofToString(kinect.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
		<< "motor / led / accel controls are not currently supported" << endl << endl;
    }
    
	reportStream << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
	<< "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl
    << "press g to toggle the gui " << endl;



    if(kinect.hasCamTiltControl()) {
    	reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }
    
     ofDrawBitmapString(reportStream.str(), 20, 652);
     
    */
    
    ofPopMatrix();
    
    /**
     * this is used in the switch below. for some reason we cannot create
     * variables in a switch statement in C++
     */
    int randomNum;
    
    string title = "CLOSER";
    string introMsg = "two players || no touching";
    string instruct = "please stand close together to begin";
    string gameOverMsg = "";
    string yourScoreMsg = "";
    string highScoreMsg = "";
    string newHighScoreMsg = "";

    
    switch(currScene ) {
        case MODE_TITLE_SCREEN:
            ofSetColor(255,0,255);

            font.drawString(title, ofGetScreenWidth()/2 - font.stringWidth(title)/2, 80);
 
            ofSetColor(0,0,255);
            font.drawString(introMsg, ofGetScreenWidth()/2 - font.stringWidth(introMsg)/2, 680);
            break;
        case MODE_START:
            
            ofSetColor(255,0,255);

            font.drawString(title, ofGetScreenWidth()/2 - font.stringWidth(title)/2, 80);
            
            ofSetColor(0,0,255);
            font.drawString(introMsg, ofGetScreenWidth()/2 - font.stringWidth(introMsg)/2, 650);

            font.drawString(instruct, ofGetScreenWidth()/2 - font.stringWidth(instruct)/2, 700);
            break;
        case MODE_PLAY:
 
            ofSetColor(0,0,255);
            font.drawString(ofToString( (int) scoreCounter), ofGetScreenWidth()/2 + 10, 700);
            break;
        case MODE_GAME_OVER:

            gameOverMsg = "GAME OVER";
            yourScoreMsg = "your score " + ofToString((int) scoreCounter);
            highScoreMsg = " :: high score " + ofToString((int) highScore);
            newHighScoreMsg = "new high score!";

            ofSetColor(255,0,255);
            font.drawString(gameOverMsg, ofGetScreenWidth()/2 - font.stringWidth(gameOverMsg)/2, 400);
            
            
            ofSetColor(255,0,255);
           font.drawString(yourScoreMsg + highScoreMsg, ofGetScreenWidth()/2 - font.stringWidth(highScoreMsg +yourScoreMsg)/2, 550);
            
            
            if (newHighScore) {
                ofSetColor(0,0,255);
                font.drawString(newHighScoreMsg, ofGetScreenWidth()/2 - font.stringWidth(newHighScoreMsg)/2, 500);
            }
            else {
            ofSetColor(0,0,255);
            font.drawString("TRY AGAIN?", ofGetScreenWidth()/2 - font.stringWidth("TRY AGAIN?")/2, 650);
            }
            
            //when in GAME OVER mode, the background is a random grey scale.
            //randomNum = (int) ofRandom(0,255);
            //ofSetBackgroundColor(randomNum, randomNum, randomNum);
           // ofSetBackgroundColor(0, 0, 0);
            
            //calculates all sparkle attract points>>>>>
            for(unsigned int p = 0; p < sparkles.size() ; p++) {
                
                sparkles[p].setMode(PARTICLE_MODE_REPEL);
                sparkles[p].draw();
            }
            
            //calculates all sparkle attract points
            for(unsigned int p = 0; p < sparkles2.size() ; p++) {
                
                sparkles2[p].setMode(PARTICLE_MODE_REPEL);
                sparkles2[p].draw();
                
            }

            
            
            break;
        case MODE_TRANSITION:
            break;

    }
    
    if(!hideGui)
        gui.draw();

}



//--------------------------------------------------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    
	switch (key) {
		case '?':
            ofSetWindowPosition(-1,-40);
            break;
        case ' ':
			bThreshWithOpenCV = !bThreshWithOpenCV;
			break;

        case OF_KEY_RIGHT:
        case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
        case OF_KEY_LEFT:
        case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
            
       // case 'f':
            ofToggleFullscreen();
            ///ofHideCursor();

            
        case 'i':
            bDrawDebug = !bDrawDebug;
            hideGui = !hideGui;
            break;
            
        case 'p':
            currScene = MODE_PLAY;
            break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
        case '@':
            currScene = MODE_GAME_OVER;
            break;
        case 'r':
            currScene = MODE_START;
            break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}
