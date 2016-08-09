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
    
    font.load("Ayuthaya", 14);

    
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
    
    /*
    synth2.load("sounds/synth4.wav");
    synth2.setLoop(true);
    synth2.setVolume(0.0f);
    synth.setSpeed(1.0f);
    synth2.setMultiPlay(true);
    synth2.play();

    
    fill.load("sounds/synth3.mp3");
    fill.setLoop(false);
    fill.setVolume(0.50f);
    fill.setSpeed(1.0f);
    fill.setMultiPlay(true);
    fill.play();

    
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
            
            /**
             * I can change the way intimacy is calcuated here, for example:
             * intimacyCounter = intimacyCounter + (weight1 * playerDistance) + (weight2 * otherThing);
             */
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
            
            // old value of 30 was good.
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
            
            if(level.doesIntersect(glowBall,glowBallWidth)) {
                badCollision = true;
                pop.play();
            }

        break;
        case MODE_GAME_OVER:
            if(ofGetElapsedTimeMillis() - timeGameOverSceneStarted > 3*1000){
                
                //ofGetElapsedTimef() >= 8){
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
    
    // Reset the sparkles
    for(unsigned int i = 0 ; i < sparkles.size() ; i++) {
        sparkles[i].reset();
    }
    
    for(unsigned int i = 0 ; i < sparkles2.size() ; i++) {
        sparkles2[i].reset();
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
    level.addTriangle(triangle(ofPoint(screenWidth,25256 - 26586 - screenHeight), 700, 256, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,24092 - 26586 - screenHeight), 716, 420, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,22652 - 26586 - screenHeight), 720, 512, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,21316 - 26586 - screenHeight), 948, 676, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,18932 - 26586 - screenHeight), 1328, 932, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,16672 - 26586 - screenHeight), 1372, 972, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,15712 - 26586 - screenHeight), 644, 436, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,13680 - 26586 - screenHeight), 756, 532, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,11664 - 26586 - screenHeight), 1004, 700, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,10420 - 26586 - screenHeight), 628, 424, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,8852 - 26586 - screenHeight), 1248, 884, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,6888 - 26586 - screenHeight), 1260, 892, triangle::LEFT));
    level.addTriangle(triangle(ofPoint(screenWidth,5812 - 26586 - screenHeight), 1224, 888, triangle::LEFT));
    level.addTriangle(trigrow(ofPoint(screenWidth,4392 - 26586 - screenHeight), 500, 350, 600, triangle::LEFT));
    level.addTriangle(trigrow(ofPoint(screenWidth,2576 - 26586 - screenHeight), 780, 540, 700,triangle::LEFT));
    //
    level.addTriangle(triangle(ofPoint(0,26072 - 26586 - screenHeight), 340, 96, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,24620 - 26586 - screenHeight), 720, 256, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,23396 - 26586 - screenHeight), 708, 436, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,22092 - 26586 - screenHeight), 628, 424, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,20236 - 26586 - screenHeight), 928, 617, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,17952 - 26586 - screenHeight), 1180, 792, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,15644 - 26586 - screenHeight), 1188, 796, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,14280 - 26586 - screenHeight), 1200, 816, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,13048 - 26586 - screenHeight), 1168, 808, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,11244 - 26586 - screenHeight), 1036, 720, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,10240 - 26586 - screenHeight), 912, 616, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,8436 - 26586 - screenHeight), 828, 548, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,6688 - 26586 - screenHeight), 793, 536, triangle::RIGHT));
    level.addTriangle(triangle(ofPoint(0,4304 - 26586 - screenHeight), 1216, 808, triangle::RIGHT));
    level.addTriangle(trigrow(ofPoint(0,3780 - 26586 - screenHeight), 1016, 708, 800, triangle::RIGHT));
    level.addTriangle(trigrow(ofPoint(0,2536 - 26586 - screenHeight), 808, 564, 700, triangle::RIGHT));
    
    //count things
    intimacyCounter = 0;
    scoreCounter = 0;
    
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
        int bgColor = (int) ofMap(playerDistance, 100.0f, 1200.0f, 0, 255);
        ofBackground(bgColor);
        
        // draw from the live kinect
        //kinect.drawDepth(0, 0, ofGetWidth(), ofGetHeight());//, 400, 300);
        //kinect.draw(420, 10, 400, 300);
        contourFinder.draw(0, 0, ofGetWidth(), ofGetHeight(), false);
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
            //ofDrawBitmapString("CLOSER [for two people || no touching]", 100, 100);
            
            
            // draw circles of mass for each blob
            ofSetColor(255,85);
            ofDrawCircle(blendCenter1.x, blendCenter1.y, radiusOfBlob1);
            ofDrawCircle(blendCenter2.x, blendCenter2.y, radiusOfBlob2);
            
            
            // draws the glowball between blobs (also, in update the glowball rise is calculated)
            ofSetColor(255,80);
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
        
        ofSetColor(255,80);
        ofDrawCircle(glowBall.x, glowBall.y, glowBallWidth);
        
        for(unsigned int p = 0; p < sparkles.size() ; p++) {
            if (p < intimacyCounter) {
                sparkles[p].setColour(0,0,255);
            } else {
                sparkles[p].setColour(255,63,180);
            }
            sparkles[p].draw();
        }
        
        for(unsigned int p = 0; p < sparkles2.size() ; p++) {
            if (p < intimacyCounter) {
                sparkles2[p].setColour(0,0,255);
            } else {
                sparkles2[p].setColour(255,63,180);
            }
            sparkles2[p].draw();
        }
        
        
//COLLISION TEST: draws temp line for collisions
        
        ofDrawLine(0, 20, ofGetScreenWidth(), 20);
        
        if (glowBall.y <= 20) {
        
//GAME OVER>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        
        }else if(currScene == MODE_GAME_OVER){
            ofBackground(255,255,255);
            ofDrawBitmapString("GAME OVER", 100, 50);
            ofDrawBitmapString("Your Score: " + ofToString(scoreCounter), 100, 100);
        
            //might need to set scoreCounter to zero here, depending on what happens
        }
    }
    
    contourFinder.draw(0, 0, ofGetWidth(), ofGetHeight(), false);
    
    if (bDrawDebug == true) {
        grayImage.draw(0,0,320,240);
    }
    
    
//END MY STUFF+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
    
    switch(currScene ) {
        case MODE_TITLE_SCREEN:
            font.drawString("\n\n CLOSER\n\n for two people || no touching\n\n", ofGetScreenWidth()/3, 50);
            break;
        case MODE_START:
            font.drawString("\n\n CLOSER\n\n for two people || no touching\n\n", ofGetScreenWidth()/3, 50);
            break;
        case MODE_PLAY:
            ofDrawBitmapString(ofToString( (int) scoreCounter), 508, 100);
            break;
        case MODE_GAME_OVER:
            ofSetColor(255);
            ofDrawBitmapString("GAME OVER! SCORE: " + ofToString((int) scoreCounter),50, 100);
            ofDrawBitmapString("TRY AGAIN?", 50, 150);
            
            //when in GAME OVER mode, the background is a random grey scale.
            randomNum = (int) ofRandom(0,255);
            ofSetBackgroundColor(randomNum, randomNum, randomNum);
            
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
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
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
            
        case 'd':
            bDrawDebug = !bDrawDebug;
            break;
            
       // case 'f':
            ofToggleFullscreen();
            ///ofHideCursor();
            //bDrawDebug = false;
       //     break;
            
        case 'g':
            hideGui = !hideGui;
            break;
            
        case 'p':
            currScene = MODE_PLAY;
            break;
            
		case '1':
			kinect.setLed(ofxKinect::LED_GREEN);
			break;
			
		case '2':
			kinect.setLed(ofxKinect::LED_YELLOW);
			break;
			
		case '3':
			kinect.setLed(ofxKinect::LED_RED);
			break;
			
		case '4':
			kinect.setLed(ofxKinect::LED_BLINK_GREEN);
			break;
			
		case '5':
			kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;
			
		case '0':
			kinect.setLed(ofxKinect::LED_OFF);
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
        case 'R':
            currScene = MODE_START;
            break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}
