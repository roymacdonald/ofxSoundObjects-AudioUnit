
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
    
    enum Mode{
        BRIDGE_MODE_NONE = 0,
        BRIDGE_MODE_TO_AU,
        BRIDGE_MODE_FROM_AU
    };
    
	ofxSoundObjectAudioUnitBridge():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR), ofxAudioUnitDSPNode(2048){}
		
	/// ofxAudioUnit stuff
	ofxAudioUnit& connectToAU(ofxAudioUnit &destination, int destinationBus = 0, int sourceBus = 0);
	ofxAudioUnitDSPNode& connectToAU(ofxAudioUnitDSPNode &destination, int destinationBus = 0, int sourceBus = 0);
	
    virtual ofxSoundObject &connectTo(ofxSoundObject &soundObject) override{
        mode = BRIDGE_MODE_FROM_AU;
        return ofxSoundObject::connectTo(soundObject);
    }
    
	///ofxSoundObjects stuff
	virtual void audioOut(ofSoundBuffer &output) override;
	
	
	OSStatus renderToAU(AudioUnitRenderActionFlags * ioActionFlags,
								   const AudioTimeStamp * inTimeStamp,
								   UInt32 inBusNumber,
								   UInt32 inNumberFrames,
								   AudioBufferList * ioData);
	
	
//	using ofxSoundObject::connectTo ;

	virtual string getName() override;
    
    Mode getMode(){return mode;}
	
private:
	
    Mode mode = BRIDGE_MODE_NONE;
    
	void copyToCircBuffer(AudioBufferList * ioData);
	
	ofSoundBuffer workBuffer;
	

	ofxAudioBufferHelper abl;
	
	
};
