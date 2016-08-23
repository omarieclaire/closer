#pragma once
#include "ofMain.h"

enum particleMode{
	PARTICLE_MODE_ATTRACT = 0,
	PARTICLE_MODE_REPEL,
	PARTICLE_MODE_NEAREST_POINTS,
	PARTICLE_MODE_NOISE
};

class demoParticle{

	public:
		demoParticle(ofPoint aPoint);
		
		void setMode(particleMode newMode);	
		void setAttractPoints( vector <ofPoint> * attract );
        void setAttractPoint(ofPoint newAttractPt);
        void setColour(int red, int green, int blue);
        void setColor(ofColor &color);

		void reset();
		void update();
		void draw();		
		
		ofPoint pos;
		ofPoint vel;
		ofPoint frc;
		
		float drag; 
		float uniqueVal;
		float scale;
		
		particleMode mode;
    
        ofPoint attractPt;
		
		vector <ofPoint> * attractPoints;
    
        int colourRed;
        int colourGreen;
        int colourBlue;
};