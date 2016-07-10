#include "ofApp.h"
#include "math.h"

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
    
    finalScore = 0.0;
    
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
         
        //!gets global center for each blob, also prints it out
        worldPoint1 = kinect.getWorldCoordinateAt(blobCenter1.x,blobCenter1.y);
        worldPoint2 = kinect.getWorldCoordinateAt(blobCenter2.x,blobCenter2.y);
        
        blendCenter1 = 0.8 * blendCenter1 + 0.2 * blobCenter1; //worldPoint1;
        blendCenter2 = 0.8 * blendCenter2 + 0.2 * blobCenter2; //worldPoint2;
        
         worldDist = blendCenter1.distance(blendCenter2);
        
        
        /* in case I fuck it up
        // tries to "blend" the values so it is less jerky
        // change the value of blendCenter1:
        // new value of blendCenter1 = (old value of blendCenter1) + 0.2 * blob
        blendCenter1 = 0.8 * blendCenter1 + 0.2 * blobCenter1;
        blendCenter2 = 0.8 * blendCenter2 + 0.2 * blobCenter2;
        
        //!gets global center for each blob, also prints it out
        worldPoint1 = kinect.getWorldCoordinateAt(blobCenter1.x,blobCenter1.y);
        worldPoint2 = kinect.getWorldCoordinateAt(blobCenter2.x,blobCenter2.y);
        worldDist = blendCenter1.distance(blendCenter2);
        */
        
        
        // draw circles of mass for each blob
        areaOfBlob1 = contourFinder.blobs[0].area;
        radiusOfBlob1 = sqrt(areaOfBlob1 / PI);
        areaOfBlob2 = contourFinder.blobs[1].area;
        radiusOfBlob2 = sqrt(areaOfBlob2 / PI);
        
        
        // intimacy threshold
        if (worldDist <= intimacyThreshold) {
            
            /**
             * I can change the way intimacy is calcuated here, for example:
             * intimacyCounter = intimacyCounter + (weight1 * worldDist) + (weight2 * otherThing); 
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
        if (intimacyCounter >= 100) {
            if (currScene == MODE_START) {
                glowBallRise = intimacyCounter * modeStartGlowRiseIntimacyFactor;
                glowBallWidth = intimacyCounter * 0.1;
                glowBall.y = glowBall.y - glowBallRise;
            } else if (currScene == MODE_PLAY) {
                glowBallRise = worldDist * modePlayGlowRiseFactor;
                
                // old value of 30 was good.
                glowBallWidth = ofMap(worldDist, 100, 700, 200, 20);
                //glowBall.y = oldGlowBallY - glowBallRise;
            }
            
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
        
        soundCounter++;
        if(ofGetFrameNum() % 60 == 0){
       // if (soundCounter >= 60){
            
            // SOUND: changes speed of sound to match map
            //synthSpeed = ofMap(worldDist, 100.0f, 1200.0f, 0.2f, 2.5f);
            //synth.setSpeed(synthSpeed);
            
            beatSpeed = ofMap(worldDist, 100.0f, 1200.0f, 2.0f, 0.2f);
            beat.setSpeed(beatSpeed);
            
            popSpeed = ofMap(worldDist, 100.0f, 1200.0f, 0.2f, 2.5f);
            pop.setSpeed(popSpeed);
            
            /*
             synth2Speed = ofMap(worldDist, 100.0f, 1200.0f, 0.2f, 2.5f);
             synth2.setSpeed(synth2Speed);
             
             fillSpeed = ofMap(worldDist, 100.0f, 1200.0f, 2.0f, 0.2f);
             fill.setSpeed(fillSpeed);
             
             saxSpeed = ofMap(worldDist, 100.0f, 1200.0f, 0.2f, 2.5f);
             sax.setSpeed(saxSpeed);
             
             sampleSpeed = ofMap(worldDist, 100.0f, 1200.0f, 2.0f, 0.2f);
             sample.setSpeed(sampleSpeed);
             
             */
            
            soundCounter = 0;
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
            
            glowBallCounter = glowBallCounter + 0.1;
            
            /**
             * TRIANGLE MOVEMENT HERE
             */
            triangleSpeedIncrease = glowBallCounter/500;
            triangleSpeed = ofMap(worldDist, 100, 700, 0.5, 3.0) + triangleSpeedIncrease;
            trianglea.moveY(triangleSpeed);
            triangleb.moveY(triangleSpeed);
            trianglec.moveY(triangleSpeed);
            triangled.moveY(triangleSpeed);
            trianglee.moveY(triangleSpeed);
            trianglef.moveY(triangleSpeed);
            triangleg.moveY(triangleSpeed);
            triangleh.moveY(triangleSpeed);
            trianglei.moveY(triangleSpeed);
            
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
            
            if(trianglea.doesIntersect(glowBall, glowBallWidth) ||
               triangleb.doesIntersect(glowBall, glowBallWidth) ||
               trianglec.doesIntersect(glowBall, glowBallWidth) ||
               triangled.doesIntersect(glowBall, glowBallWidth) ||
               trianglee.doesIntersect(glowBall, glowBallWidth) ||
               trianglef.doesIntersect(glowBall, glowBallWidth) ||
               triangleg.doesIntersect(glowBall, glowBallWidth) ||
               triangleh.doesIntersect(glowBall, glowBallWidth) ||
               trianglei.doesIntersect(glowBall, glowBallWidth) ) {
                badCollision = true;
                pop.play();
                finalScore = glowBallRise;
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
    
    distance = 0;
    worldDist = 0;
    
    //make things smooth
    blendCenter1.set(0,0);
    blendCenter2.set(0,0);
    
    //Set the scene (don't need to otherwise setup
    //    { MODE STANDBY, MODE_START, MODE_PLAY, MODE_GAME_OVER}
    currScene = MODE_TITLE_SCREEN;
    
    // Reset the sparkles
    for(unsigned int i = 0 ; i < sparkles.size() ; i++) {
        sparkles[i].reset();
    }
    
    for(unsigned int i = 0 ; i < sparkles2.size() ; i++) {
        sparkles2[i].reset();
    }
    
    /**
     * Setup triangles
     * void triangle::setPoints(triangle t, float gap, float height, float width, float peakHeight)
     * parameters are:
     * GAP | HEIGHT | WIDTH | PEAK HEIGHT
     */
    trianglea.setPoints(ofPoint(0,-100), ofPoint(200,-250), ofPoint(0,-500));
    triangleb.setPoints(ofPoint(ofGetScreenWidth(),-100), ofPoint(ofGetScreenWidth()-600,-250),ofPoint(ofGetScreenWidth(),-400));
    trianglec.setPoints(trianglea, 50, 150, 400, 150);
    triangled.setPoints(triangleb, 50, 150, -550, 150);
    trianglee.setPoints(trianglec, 50, 250, 450, 250);
    trianglef.setPoints(triangled, 50, 250, -600, 250);
    triangleg.setPoints(trianglee, 50, 300, 550, 250);
    triangleh.setPoints(trianglef, 50, 300, -700, 250);
    trianglei.setPoints(triangleg, 50, 350, 650, 300);
    
 
    //trianglea.setColor(0,255,255);
    
    
    //count things
    soundCounter = 0;
    intimacyCounter = 0;
    glowBallCounter = 0;
    
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
        
        //ofDrawBitmapString("STANDBY :: CLOSER (for two people || no touching)", 100, 50);
     
        
    /*
        if(contourFinder.nBlobs == 1 ){
            if(ofGetElapsedTimef() >= 3){
            ofDrawBitmapString("CLOSER [for two people (you are alone) || no touching]", 100, 50);
            
        } else if (contourFinder.nBlobs > 2 )
            if(ofGetElapsedTimef() >= 3){
            ofDrawBitmapString("CLOSER [for two people (too many people) || no  touching]", 100, 50);
        }
            
        } else if (contourFinder.nBlobs == 2 )
            ofDrawBitmapString("CLOSER || no touching]", 100, 50);
            currScene = MODE_START;
            
        }
    */

//START MODE>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    }else if(currScene == MODE_START){
        //ofDrawBitmapString("START", 100, 50);

        // this changes the background colour
        int bgColor = (int) ofMap(worldDist, 100.0f, 1200.0f, 0, 255);
        ofBackground(bgColor);
        
        // draw from the live kinect
        //kinect.drawDepth(0, 0, ofGetWidth(), ofGetHeight());//, 400, 300);
        //kinect.draw(420, 10, 400, 300);
        contourFinder.draw(0, 0, ofGetWidth(), ofGetHeight());
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
        //ofDrawBitmapString("Your Score: " + ofToString( (int) glowBallCounter), 100, 100);
        
        

        trianglea.draw();
        triangleb.draw();
        trianglec.draw();
        triangled.draw();
        trianglee.draw();
        trianglef.draw();
        triangleg.draw();
        triangleh.draw();
        trianglei.draw();

        
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
 //           ofDrawTriangle(ofGetScreenWidth()/2+20,10,ofGetScreenWidth()/2+60,40,ofGetScreenWidth()/2-60,40);
        
        
        
//GAME OVER>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        
        }else if(currScene == MODE_GAME_OVER){
            ofBackground(255,255,255);
            ofDrawBitmapString("GAME OVER", 100, 50);
            ofDrawBitmapString("Your Score: " + ofToString(glowBallCounter), 100, 100);
        
            //might need to set glowBallCounter to zero here, depending on what happens
        }
    }
    
    
    if (bDrawDebug == true) {
        grayImage.draw(0,0,320,240);
        contourFinder.draw(0, 0, ofGetWidth(), ofGetHeight());
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
            ofDrawBitmapString("CLOSER", 50, 50);
            ofDrawBitmapString("for two people || no touching", 50, 100);


            
            break;
        case MODE_START:
            ofDrawBitmapString("CLOSER", 50, 50);
            ofDrawBitmapString("for two people || no touching", 50, 100);
            break;
        case MODE_PLAY:
            ofDrawBitmapString(ofToString( (int) glowBallCounter), 508, 100);
            break;
        case MODE_GAME_OVER:
            ofSetColor(255);
            ofDrawBitmapString("GAME OVER! SCORE: " + ofToString((int) glowBallCounter),50, 100);
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
            ofSetWindowPosition(-1,-40); break;
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
            
        case 'f':
            ofToggleFullscreen();
            ///ofHideCursor();
            //bDrawDebug = false;
            break;
            
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
//        case 'R':
//            currScene = MODE_STANDBY; break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}