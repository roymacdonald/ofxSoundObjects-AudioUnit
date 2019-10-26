//
//  ofxNDISoundObject.cpp
//
//  Created by Roy Macdonald on 9/14/19.
//
//

#include "ofxAUSoundObject.h"
#include "ofLog.h"
#include <CoreAudio/HostTime.h>
#include "ofxAudioUnitUtils.h"

// ----------------------------------------------------------
OSStatus SilentRenderCallback(void * inRefCon,
							  AudioUnitRenderActionFlags * ioActionFlags,
							  const AudioTimeStamp * inTimeStamp,
							  UInt32 inBusNumber,
							  UInt32 inNumberFrames,
							  AudioBufferList * ioData)
{
	for(int i = 0; i < ioData->mNumberBuffers; i++) {
		memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[0].mDataByteSize);
	}
	
	*ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;
	
	return noErr;
}



// ----------------------------------------------------------
OSStatus RenderToOf(void * inRefCon,
							   AudioUnitRenderActionFlags * ioActionFlags,
							   const AudioTimeStamp * inTimeStamp,
							   UInt32 inBusNumber,
							   UInt32 inNumberFrames,
							   AudioBufferList * ioData)
{
	ofxSoundObjectAudioUnitBridge * sndObj = static_cast<ofxSoundObjectAudioUnitBridge *>(inRefCon);
	
	if(sndObj){
		
		return sndObj->RenderToOf(ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
		
	}
	
}
// ----------------------------------------------------------
OSStatus ofxSoundObjectAudioUnitBridge::RenderToOf(AudioUnitRenderActionFlags * ioActionFlags,
															const AudioTimeStamp * inTimeStamp,
															UInt32 inBusNumber,
															UInt32 inNumberFrames,
															AudioBufferList * ioData){
	
	
	
	if(inNumberFrames != workBuffer.getNumFrames() || ioData->mNumberBuffers != workBuffer.getNumChannels()){
		workBuffer.allocate(inNumberFrames, ioData->mNumberBuffers);
	}
//	unsigned int sr = getSourceASBD(inBusNumber).mSampleRate;
//		 std::cout << "Sample Time: " << inTimeStamp->mSampleTime << " Host Time "  << inTimeStamp->mHostTime <<  std::endl;
//	if(sr == 0){
//		sr = inTimeStamp->mSampleTime/inTimeStamp->mHostTime;
//		sr = round(1000000000 * (inNumberFrames/(float)(inTimeStamp->mHostTime - prevTimeStamp.mHostTime)));
//		cout << "samplerate " << inTimeStamp->mSampleTime/inTimeStamp->mHostTime << endl;
//	}
//	prevTimeStamp = *inTimeStamp;
	
	workBuffer.setSampleRate(44100);
	
	//	ofxSoundUtils::checkBuffers(output, workBuffer);
	if(inputObject!=nullptr) {
		inputObject->audioOut(workBuffer);
	}
	
	abl.set(ioData);
	abl.fromOf(workBuffer);
	
//	ioData = abl.bufferList;
	
	OSStatus status;
	if(_impl->ctx.processCallback.inputProc) {
		status = (_impl->ctx.processCallback.inputProc)(_impl->ctx.processCallback.inputProcRefCon,
											   ioActionFlags,
											   inTimeStamp,
											   inBusNumber,
											   inNumberFrames,
											   ioData);
	}
	copyToCircBuffer(ioData);
	
	
	return noErr;
	
}
// ----------------------------------------------------------
void ofxSoundObjectAudioUnitBridge::audioOut(ofSoundBuffer &output){
	
	abl.allocate(output.getNumChannels(), output.getNumFrames());
	
	OSStatus status;
	
	AudioUnitRenderActionFlags ioActionFlags;
	AudioTimeStamp inTimeStamp;
//	cout << getName() << " Tick Count: " << output.getTickCount() << endl;
	FillOutAudioTimeStampWithSampleAndHostTime(inTimeStamp, output.getTickCount()* output.getNumFrames(), AudioGetCurrentHostTime());
	
	
	
	if(_impl->ctx.sourceType == NodeSourceUnit && _impl->ctx.sourceUnit->getUnitRef()) {
		status = _impl->ctx.sourceUnit->render(&ioActionFlags,
											   &inTimeStamp,
											   _impl->ctx.sourceBus,
											   output.getNumFrames(),
											   abl.bufferList);
		
	} else if(_impl->ctx.sourceType == NodeSourceCallback) {
		status = (_impl->ctx.sourceCallback.inputProc)(_impl->ctx.sourceCallback.inputProcRefCon,
													   &ioActionFlags,
													   &inTimeStamp,
													   _impl->ctx.sourceBus,
													   output.getNumFrames(),
													   abl.bufferList);
	} else {
		status = SilentRenderCallback(NULL, &ioActionFlags, &inTimeStamp, 0, 0, abl.bufferList);
	}
	
	
	
	if(_impl->ctx.processCallback.inputProc) {
		(_impl->ctx.processCallback.inputProc)(_impl->ctx.processCallback.inputProcRefCon,
											   &ioActionFlags,
											   &inTimeStamp,
											   _impl->ctx.sourceBus,
											   output.getNumFrames(),
											   abl.bufferList);
	}
	
	copyToCircBuffer(abl.bufferList);
	
	
	abl.toOf(output);
}
// ----------------------------------------------------------
string ofxSoundObjectAudioUnitBridge:: getName(){
	
	if(name.empty()){
		return ofxSoundObject::getName();
	}
	return name;

}
// ----------------------------------------------------------
void ofxSoundObjectAudioUnitBridge::copyToCircBuffer(AudioBufferList * ioData){
	
	if(_impl->ctx.bufferMutex.try_lock()) {
		
		const size_t buffersToCopy = std::min<size_t>(_impl->ctx.circularBuffers.size(), ioData->mNumberBuffers);
		
		for(int i = 0; i < buffersToCopy; i++) {
			CopyAudioBufferIntoCircularBuffer(&_impl->ctx.circularBuffers[i], ioData->mBuffers[i]);
		}
		
		_impl->ctx.bufferMutex.unlock();
	}
}

// ----------------------------------------------------------
ofxAudioUnit& ofxSoundObjectAudioUnitBridge::connectToAU(ofxAudioUnit &destination, int destinationBus, int sourceBus){
	ofxAudioUnitDSPNode::connectTo(destination);
//	_impl->ctx.sourceBus = sourceBus;
	AURenderCallbackStruct callback = {RenderToOf, this};
	destination.setRenderCallback(callback, destinationBus);
	destination.sourceDSP = this;
	destination.sourceUnit = nullptr;
	return destination;
}
// ----------------------------------------------------------
ofxAudioUnitDSPNode& ofxSoundObjectAudioUnitBridge::connectToAU(ofxAudioUnitDSPNode &destination, int destinationBus , int sourceBus ){
	_impl->ctx.sourceBus = sourceBus;
	
	_impl->ctx.sourceUnit = nullptr;
	_impl->ctx.sourceBus = sourceBus;
	AURenderCallbackStruct callback = {RenderToOf, this};
	destination.setSource(callback);
	destination._impl->ctx.sourceDSPNode = this;
	destination._impl->ctx.sourceUnit = nullptr;
	return destination;
}

