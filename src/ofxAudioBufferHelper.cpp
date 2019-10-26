//
//  ofxAudioBufferHelper.cpp
//  example
//
//  Created by Roy Macdonald on 10/25/19.
//
//

#include "ofxAudioBufferHelper.h"
#include "ofxAudioUnitUtils.h"
#include "ofLog.h"
// ----------------------------------------------------------
ofxAudioBufferHelper::~ofxAudioBufferHelper(){
	dealocate();
}
// ----------------------------------------------------------
void ofxAudioBufferHelper::dealocate(){
	if( bufferList != NULL && bOwnsPointer){
		AudioBufferListRelease(bufferList);
		bufferList = NULL;
		this->numChannels =  0;
		this->numFrames  = 0;
		bOwnsPointer = false;
	}
}
// ----------------------------------------------------------
void ofxAudioBufferHelper::set(AudioBufferList *bufferList){

	dealocate();

	this->bufferList = bufferList;
	
	this->numChannels =  this->bufferList->mNumberBuffers;
	
	UInt32 dataSize = bufferList->mBuffers[0].mDataByteSize;
	
	this->numFrames  = dataSize / sizeof(Float32);
	
	bOwnsPointer = false;
	
	// CHeck that it is properly setup
	for(UInt32 i = 0; i < bufferList->mNumberBuffers; ++i) {
		if(bufferList->mBuffers[i].mDataByteSize != dataSize){
			ofLogError("ofxAudioBufferHelper::set") << "wrong data size on buffer " << i;
		}
	}
	
}
// ----------------------------------------------------------
void ofxAudioBufferHelper::allocate(UInt32 numChannels, UInt32 numFrames){
	
	if( bufferList != NULL && this->numChannels ==  numChannels &&  this->numFrames  == numFrames){ return;}
	
	if( bufferList != NULL){
		dealocate();
	}
	
	bufferList = AudioBufferListAlloc(numChannels, numFrames);
	
	this->numChannels =  numChannels;
	this->numFrames  = numFrames;
	bOwnsPointer = true;
	
}

// ----------------------------------------------------------
void ofxAudioBufferHelper::toOf(ofSoundBuffer& buffer){
	
	if(bufferList == NULL){
		ofLogError("ofxAudioBufferHelper::toOf") << "AudioBufferList not allocated!";
	}
	
	if(buffer.getNumFrames() != numFrames || buffer.getNumChannels() != numChannels){
		buffer.allocate(numFrames, numChannels);
	}
	for(size_t c = 0; c < numChannels; c++ ){
		float * data = static_cast<float *>(bufferList->mBuffers[c].mData);
		for(size_t f = 0; f < numFrames; f++ ){
			buffer[f*numChannels + c] = data[f];
		}
	}
	
	
}
// ----------------------------------------------------------
void ofxAudioBufferHelper::fromOf(ofSoundBuffer& buffer){
	if(bufferList == NULL){
		allocate(buffer.getNumChannels(), buffer.getNumFrames());
	}else{
		if(buffer.getNumChannels() != this->numChannels ||
		   buffer.getNumFrames() != this->numFrames){
			std::cout << "ofxAudioBufferHelper::fromOf "<< std::endl;
			std::cout << "chans  " << buffer.getNumChannels() << " | " << this->numChannels << std::endl;
			std::cout << "frames " << buffer.getNumFrames() <<  " | " << this->numFrames << std::endl;
		}
	}
	
	
	for(size_t c = 0; c < numChannels; c++ ){
		float * data = static_cast<float *>(bufferList->mBuffers[c].mData);
		for(size_t f = 0; f < numFrames; f++ ){
			data[f] = buffer[f*numChannels + c];
		}
	}
}

