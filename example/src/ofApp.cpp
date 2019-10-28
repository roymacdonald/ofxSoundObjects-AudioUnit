#include "ofApp.h"
#include "ofxAUSoundObjectUtils.h"
//--------------------------------------------------------------
void ofApp::setup(){
	
	
	ofxSoundUtils::printOutputSoundDevices();
	
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	
	// IMPORTANT!!!
	// The following line of code is where you set which audio interface to use.
	// the index is the number printed in the console inside [ ] before the interface name
	// You can use a different input and output device.
	
	int outDeviceIndex = 0;
	
	string soundFile =  ofToDataPath("../../../../../examples/sound/soundPlayerExample/bin/data/sounds/beat.wav",true);
//	string soundFile = "/Users/roy/Desktop/ramp.wav";
	OFXplayer.load(soundFile,false);
	
	
	ofSoundStreamSettings soundSettings;
	soundSettings.numInputChannels = 0;
	soundSettings.numOutputChannels = 2;
	soundSettings.sampleRate = OFXplayer.getSoundFile().getSampleRate();
	soundSettings.bufferSize = 256;
	soundSettings.numBuffers = 1;
	soundSettings.setOutDevice(outDevices[outDeviceIndex]);
	
	OFstream.setup(soundSettings);
	
	OFstream.setOutput(OFXoutput);
	
	
	// ------ waveforms ---------
	// the waveformDraw class setup receives the rectangle where it is going to be drawn
	// you can skip this and pass this while drawing if you are changing where this is going to be drawn.
	// As well, the waveformDraw class inherits from ofRectangle so you can access the functions of the latter.
	
	
	OFXfullFileWaveform.setup( 0, 0, ofGetWidth(), ofGetHeight()/3);
	OFXwave.setup(0, OFXfullFileWaveform.getMaxY(), ofGetWidth(), ofGetHeight()/3);
	
	OFXfullFileWaveform.makeMeshFromBuffer( OFXplayer.getSoundFile().getBuffer());
	
	fromAU.setName("fromAU");
	toAU.setName("toAU");
	
	
	AUreverb.setup('aufx', 'mrev', 'appl');
	
	
	OFXplayer.play();
	OFXplayer.setLoop(true);
	
	
	AUfilePlayer.name = "AUfilePlayer";
	AUtap.name = "AUtap";
	AUoutput.name = "AUoutput";
	AUreverb.name = "AUreverb";
	
	OFXoutput.setName("OFXoutput");
	OFXplayer.setName("OFXplayer");
	OFXwave.setName("OFXwave");
	OFXfullFileWaveform.setName("OFXfullFileWaveform");
	fromAU.setName("fromAU");
	toAU.setName("toAU");
	
	
	
	AUfilePlayer.setFile(soundFile);
	AUfilePlayer.loop();

	
	makeHelpText();
	setMode(AU_TO_OFX_TO_AU);

}

//--------------------------------------------------------------
void ofApp::makeHelpText(){
	helpText.resize(NUM_MODES+1);
	helpText[0] = " press [key]  ";
	for(int i =0; i < helpText.size()-1; i++){
		helpText[i+1] = "       [" + ofToString(i+1) + "] " + modeToString((Mode)i);
	}
}
//--------------------------------------------------------------
void ofApp::setMode( Mode newMode){
	if(newMode != mode){
		mode = newMode;
		
		if( mode == OFX_TO_AU){
			AUoutput.start();
			OFstream.stop();

			OFXplayer.connectTo(OFXwave).connectTo(toAU);
			
			toAU.connectToAU(AUtap).connectTo(AUoutput);
			
			chain = "";
			ofxSoundUtils::getSignalChainString(&AUoutput, chain);

		}
		else if( mode == OFX_TO_OFX){
			AUoutput.stop();
			OFstream.start();
			OFXplayer.connectTo(OFXwave).connectTo(OFXoutput);

			chain = "";
			ofxSoundUtils::getSignalChainString(&OFXoutput, chain);
		}
		else if( mode == AU_TO_AU){
			AUoutput.start();
			OFstream.stop();
			AUfilePlayer.loop();

			AUfilePlayer.connectTo(AUtap).connectTo(AUoutput);

			chain = "";
			ofxSoundUtils::getSignalChainString(&AUoutput, chain);
		}
		else if( mode == AU_TO_OFX){
			AUoutput.stop();
			OFstream.start();
			
			AUfilePlayer.loop();
			AUfilePlayer.connectTo(AUtap).connectTo(fromAU);
			
			fromAU.connectTo(OFXwave).connectTo(OFXoutput);
			
			chain = "";
			ofxSoundUtils::getSignalChainString(&OFXoutput, chain);

		}
		else if( mode == OFX_TO_AU_TO_OFX){
			AUoutput.stop();
			OFstream.start();
			
			OFXplayer.connectTo(toAU);
			
			toAU.connectToAU(AUreverb);
			
			AUreverb.connectTo(AUtap);

			AUtap.connectTo(fromAU);
			
			
			fromAU.connectTo(OFXwave).connectTo(OFXoutput);

			chain = "";
			ofxSoundUtils::getSignalChainString(&OFXoutput, chain);

		}
		else if( mode == AU_TO_OFX_TO_AU){
			OFstream.stop();
			AUoutput.start();
			
			AUfilePlayer.loop();
			
			AUfilePlayer.connectTo(AUtap).connectTo(fromAU);
			
			fromAU.connectTo(OFXwave).connectTo(toAU);
			
			toAU.connectToAU(AUoutput);

			
			chain = "";
			ofxSoundUtils::getSignalChainString(&AUoutput, chain);
		}
		
		cout << "setMode " << mode << "  " << modeToString(mode) << endl;
		cout << chain << endl;
	}

}

//--------------------------------------------------------------
void ofApp::update(){

}
//--------------------------------------------------------------
void ofApp::drawHelpText(){
	ofDrawBitmapStringHighlight(chain, 20,20);
	
	
//		ofDrawBitmapStringHighlight(, 20, 40
	int y = 58;
	
	for(int i = 0; i < helpText.size(); i++){
		ofDrawBitmapStringHighlight(helpText[i], 20, y, (i==(mode + 1))?ofColor::red:ofColor::black, (i==(mode + 1))?ofColor::black:ofColor::white);
		
		y += 18;
		
	}
	
	
	
//	ofDrawBitmapStringHighlight(timeStampToString(AUtap.getCurrentTimeStamp()), 20, y);
	
}
//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(60);
	ofSetColor(ofColor::white);
	
	OFXfullFileWaveform.draw();
	
	ofSetColor(ofColor::red);
	float playhead = ofMap(OFXplayer.getPosition(), 0,1, OFXfullFileWaveform.getMinX(),OFXfullFileWaveform.getMaxX());
	ofDrawLine(playhead, 0, playhead, OFXfullFileWaveform.getMaxY());
	
	if(OFXfullFileWaveform.inside(ofGetMouseX(), ofGetMouseY())){
		ofSetColor(ofColor::cyan);
		ofDrawLine(ofGetMouseX(), 0, ofGetMouseX(), OFXfullFileWaveform.getMaxY());
	}
	ofSetColor(ofColor::white);

	
	OFXwave.draw();
	
	
	ofPushStyle();
	ofSetColor(ofColor::yellow);
	ofSetLineWidth(3);
	ofPushMatrix();
	
	ofTranslate(0, 2* ofGetHeight()/3);
	ofPolyline p;
	AUtap.getWaveform(p, ofGetWidth(), ofGetHeight()/3);
	p.draw();
	
	ofPopMatrix();
	ofPopStyle();
	
	
	drawHelpText();
	
//	stringstream ss;
//	ss << "OFXplayer: " << OFXplayer.getTicks()
//	<< "  OFXwave: " << OFXwave.getTicks()
//	<< "  OFXoutput: " << OFXoutput.getTicks() << endl 
//	
//	<< " AUfilePlayer: " << AUfilePlayer.getTicks()
//	<< " AUtap: " << AUtap.getTicks()
//	<< " AUoutput: " << AUoutput.getTicks()
//	<< " AUreverb: " << AUreverb.getTicks();
	
	
	
//	ofDrawBitmapStringHighlight(ss.str(), 20, ofGetHeight() - 60, ofColor::yellow, ofColor::black);
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == ' ') AUreverb.showUI();
	if(key >= '1' && key <= '6'){
		cout << (char)key << endl;
		setMode((Mode) (key - '1') );
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if(OFXfullFileWaveform.inside(x, y)){
		OFXplayer.setPositionMS(ofMap(x, OFXfullFileWaveform.getMinX(), OFXfullFileWaveform.getMaxX(), 0, OFXplayer.getDurationMS()));
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
