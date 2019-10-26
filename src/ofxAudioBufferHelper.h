//
//  ofxAudioBufferHelper.h
//  example
//
//  Created by Roy Macdonald on 10/25/19.
//
//

#pragma once


#include "ofxAudioUnit.h"
#include "ofSoundBuffer.h"

class ofxAudioBufferHelper{
public:
	~ofxAudioBufferHelper();
	void allocate(UInt32 numChannels, UInt32 numFrames);
	void dealocate();
	void set(AudioBufferList *bufferList);

	void toOf(ofSoundBuffer& buffer);
	void fromOf(ofSoundBuffer& buffer);
	
	void drawCurrentOfBuffer();
	AudioBufferList *bufferList = NULL;
	UInt32 numChannels = 0, numFrames =0;
	private:
	bool bOwnsPointer = false;
};
