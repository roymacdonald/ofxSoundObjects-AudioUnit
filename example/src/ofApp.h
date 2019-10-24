#pragma once

#include "ofMain.h"
#include "ofxAUSoundObject.h"
#include "waveformDraw.h"

#include "ofxSoundPlayerObject.h"
#include "ofxGui.h"

#include "SineWaveGenerator.h"
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

	ofxSoundObjectAudioUnitBridge fromAU, toAU;
	
	
	ofxAudioUnit reverb;
	
	ofSoundStream stream;
	ofxSoundOutput output;
	
	ofxSoundPlayerObject player;
	waveformDraw wave;
	
	ofxPanel gui;
	ofParameter<float> pan, speed;
	void speedChanged(float&);
	void panChanged(float&f);
	
	waveformDraw fullFileWaveform;
	

	void connectAU();
	void disconnectAU();
	
	
};
