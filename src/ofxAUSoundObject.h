
#pragma once

#include "ofxSoundObject.h"
#include "ofxAudioUnit.h"

#include "ofxAudioBufferHelper.h"

OSStatus RenderOFtoAU(void * inRefCon,
					   AudioUnitRenderActionFlags * ioActionFlags,
					   const AudioTimeStamp * inTimeStamp,
					   UInt32 inBusNumber,
					   UInt32 inNumberFrames,
					   AudioBufferList * ioData);




//--------------------------------------------------------------------------
class ofxSoundObjectAudioUnitBridge : public ofxSoundObject, public ofxAudioUnitDSPNode{
	
public:
	ofxSoundObjectAudioUnitBridge():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR), ofxAudioUnitDSPNode(2048){}
		
	/// ofxAudioUnit stuff
	ofxAudioUnit& connectToAU(ofxAudioUnit &destination, int destinationBus = 0, int sourceBus = 0);
	ofxAudioUnitDSPNode& connectToAU(ofxAudioUnitDSPNode &destination, int destinationBus = 0, int sourceBus = 0);
	
	
	///ofxSoundObjects stuff
	virtual void audioOut(ofSoundBuffer &output) override;
	
	
	OSStatus renderToAU(AudioUnitRenderActionFlags * ioActionFlags,
								   const AudioTimeStamp * inTimeStamp,
								   UInt32 inBusNumber,
								   UInt32 inNumberFrames,
								   AudioBufferList * ioData);
	
	
	using ofxSoundObject::connectTo ;

	virtual string getName() override;
	
private:
	
	void copyToCircBuffer(AudioBufferList * ioData);
	
	ofSoundBuffer workBuffer;
	

	ofxAudioBufferHelper abl;
	
	
};
