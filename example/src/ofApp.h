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
	
	
	
	ofxAudioUnitFilePlayer AUfilePlayer;
	ofxAudioUnitTap AUtap;
	ofxAudioUnitOutput AUoutput;
	ofxAudioUnit AUreverb;
	
	
	
	enum Mode {
		OFX_TO_AU = 0,
		OFX_TO_OFX,
		AU_TO_AU,
		AU_TO_OFX,
		OFX_TO_AU_TO_OFX,
		AU_TO_OFX_TO_AU,
		NUM_MODES
	} mode = NUM_MODES;
	

	string modeToString(){
		switch(mode){
			case OFX_TO_AU: return "OFX_TO_AU";
			case OFX_TO_OFX: return "OFX_TO_OFX";
			case AU_TO_AU: return "AU_TO_AU";
			case AU_TO_OFX: return "AU_TO_OFX";
			case OFX_TO_AU_TO_OFX: return "OFX_TO_AU_TO_OFX";
			case AU_TO_OFX_TO_AU: return "AU_TO_OFX_TO_AU";
			case NUM_MODES: return "NUM_MODES " +  ofToString((int)NUM_MODES);
		}
		return "";
	} 


	ofSoundStream OFstream;
	ofxSoundOutput OFXoutput;
	
	ofxSoundPlayerObject OFXplayer;
	waveformDraw OFXwave;
	waveformDraw OFXfullFileWaveform;
	

	void setMode( Mode newMode);
	
	string status;
	
	string timeStampToString (const AudioTimeStamp& t){
		stringstream ss;
			
			ss << "SampleTime                 " << t.mSampleTime << endl;
			ss << "HostTime                   " << t.mHostTime << endl;
			ss << "RateScalar                 " << t.mRateScalar << endl;
			ss << "WordClockTime              " << t.mWordClockTime << endl;
			ss << "SMPTETime Subframes        " << t.mSMPTETime.mSubframes << endl;
			ss << "SMPTETime SubframeDivisor  " << t.mSMPTETime.mSubframeDivisor << endl;
			ss << "SMPTETime Counter          " << t.mSMPTETime.mCounter << endl;
			ss << "SMPTETime Type             " << t.mSMPTETime.mType << endl;
			ss << "SMPTETime Flags            " << t.mSMPTETime.mFlags << endl;
			ss << "SMPTETime Hours            " << t.mSMPTETime.mHours << endl;
			ss << "SMPTETime Minutes          " << t.mSMPTETime.mMinutes << endl;
			ss << "SMPTETime Seconds          " << t.mSMPTETime.mSeconds << endl;
			ss << "SMPTETime Frames           " << t.mSMPTETime.mFrames  << endl;
			

		return ss.str();
	
	}
	
	
};
