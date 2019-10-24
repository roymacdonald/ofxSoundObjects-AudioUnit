#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	reverb.setup('aufx', 'mrev', 'appl');
	
	
	player.load( ofToDataPath("../../../../../examples/sound/soundPlayerExample/bin/data/sounds/beat.wav",true),false);
	ofxSoundUtils::printOutputSoundDevices();
	
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	
	// IMPORTANT!!!
	// The following line of code is where you set which audio interface to use.
	// the index is the number printed in the console inside [ ] before the interface name
	// You can use a different input and output device.
	
	int outDeviceIndex = 0;
	
	
	ofSoundStreamSettings soundSettings;
	soundSettings.numInputChannels = 0;
	soundSettings.numOutputChannels = 2;
	soundSettings.sampleRate = player.getSoundFile().getSampleRate();
	soundSettings.bufferSize = 256;
	soundSettings.numBuffers = 1;
	
	stream.setup(soundSettings);
	
	stream.setOutput(output);
	
	// ------ waveforms ---------
	// the waveformDraw class setup receives the rectangle where it is going to be drawn
	// you can skip this and pass this while drawing if you are changing where this is going to be drawn.
	// As well, the waveformDraw class inherits from ofRectangle so you can access the functions of the latter.
	
	
	fullFileWaveform.setup( 0, 0, ofGetWidth(), ofGetHeight()/3);
	wave.setup(0, fullFileWaveform.getMaxY(), ofGetWidth(), ofGetHeight() - fullFileWaveform.getMaxY());
	
	fullFileWaveform.makeMeshFromBuffer( player.getSoundFile().getBuffer());
	
	
	connectAU();
	
	player.play();
	
	
	player.setLoop(true);
	
	
}
//--------------------------------------------------------------
void ofApp::connectAU(){
	player.connectTo(toAU);
	
	toAU.connectToAU(reverb).connectTo(fromAU);
	
	fromAU.connectTo(wave).connectTo(output);
	

}
//--------------------------------------------------------------
void ofApp::disconnectAU(){
	player.connectTo(wave).connectTo(output);
	

}
//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(ofColor::white);
	
	fullFileWaveform.draw();
	
	ofSetColor(ofColor::red);
	float playhead = ofMap(player.getPosition(), 0,1, fullFileWaveform.getMinX(),fullFileWaveform.getMaxX());
	ofDrawLine(playhead, 0, playhead, fullFileWaveform.getMaxY());
	
	if(fullFileWaveform.inside(ofGetMouseX(), ofGetMouseY())){
		ofSetColor(ofColor::cyan);
		ofDrawLine(ofGetMouseX(), 0, ofGetMouseX(), fullFileWaveform.getMaxY());
	}
	ofSetColor(ofColor::white);
	wave.draw();
	
	ofSetColor(ofColor::yellow);
	player.drawDebug(20, gui.getShape().getMaxY() + 20);
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == ' ') reverb.showUI();
	if(key == 'c') connectAU();
	if(key == 'd') disconnectAU();
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
	if(fullFileWaveform.inside(x, y)){
		player.setPositionMS(ofMap(x, fullFileWaveform.getMinX(), fullFileWaveform.getMaxX(), 0, player.getDurationMS()));
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
