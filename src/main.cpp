#include "ofMain.h"
#include "ofApp.h"

// here is where I set the size of the screen

int main() {
    ofSetupOpenGL(1280, 720, OF_GAME_MODE);
	ofRunApp(new ofApp());
}
