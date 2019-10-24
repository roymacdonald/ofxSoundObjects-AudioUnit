
#pragma once

#include "ofxSoundObject.h"
#include "ofxAudioUnit.h"


OSStatus RenderToOfSoundBuffer(void * inRefCon,
					   AudioUnitRenderActionFlags * ioActionFlags,
					   const AudioTimeStamp * inTimeStamp,
					   UInt32 inBusNumber,
					   UInt32 inNumberFrames,
					   AudioBufferList * ioData);

//--------------------------------------------------------------------------
class ofxSoundObjectAudioUnitBridge : public ofxSoundObject, public ofxAudioUnitDSPNode{
	
public:
	ofxSoundObjectAudioUnitBridge():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){}
		
	/// ofxAudioUnit stuff
	ofxAudioUnit& connectToAU(ofxAudioUnit &destination, int destinationBus = 0, int sourceBus = 0);
	ofxAudioUnitDSPNode& connectToAU(ofxAudioUnitDSPNode &destination, int destinationBus = 0, int sourceBus = 0);
	
	
	///ofxSoundObjects stuff
	virtual void audioOut(ofSoundBuffer &output) override;
	
	
	OSStatus RenderToSoundObject(AudioUnitRenderActionFlags * ioActionFlags,
								   const AudioTimeStamp * inTimeStamp,
								   UInt32 inBusNumber,
								   UInt32 inNumberFrames,
								   AudioBufferList * ioData);
	using ofxSoundObject::connectTo ;
private:
	
	void copyToCircBuffer(AudioBufferList * ioData);
	
	ofSoundBuffer workBuffer;
	
	struct ABL{
		~ABL();
		void allocate(UInt32 numChannels, UInt32 numFrames);
		void dealocate();
		void set(AudioBufferList *bufferList);
		AudioBufferList *bufferList = NULL;
		UInt32 numChannels = 0, numFrames =0;
		void toOf(ofSoundBuffer& buffer);
		void fromOf(ofSoundBuffer& buffer);
	} abl;
};
////--------------------------------------------------------------------------
//class ofxSoundObjectFromAudioUnit : public ofxAudioUnitDSPNode{
//	
//public:
//	
//};


















