//
//  ofxNDISoundObject.cpp
//
//  Created by Roy Macdonald on 9/14/19.
//
//

#include "ofxAUSoundObject.h"
#include "ofLog.h"

#include "ofxAudioUnitUtils.h"
// ----------------------------------------------------------
OSStatus RenderToOfSoundBuffer(void * inRefCon,
					   AudioUnitRenderActionFlags * ioActionFlags,
					   const AudioTimeStamp * inTimeStamp,
					   UInt32 inBusNumber,
					   UInt32 inNumberFrames,
					   AudioBufferList * ioData)
{
	ofxSoundObjectAudioUnitBridge * sndObj = static_cast<ofxSoundObjectAudioUnitBridge *>(inRefCon);
	
	if(sndObj){
		
		return sndObj->RenderToSoundObject(ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
	
	}

}
// ----------------------------------------------------------
OSStatus ofxSoundObjectAudioUnitBridge::RenderToSoundObject(AudioUnitRenderActionFlags * ioActionFlags,
							   const AudioTimeStamp * inTimeStamp,
							   UInt32 inBusNumber,
							   UInt32 inNumberFrames,
							   AudioBufferList * ioData){
	
	
	
	if(inNumberFrames != workBuffer.getNumFrames() || ioData->mNumberBuffers != workBuffer.getNumChannels()){
		workBuffer.allocate(inNumberFrames, ioData->mNumberBuffers);
	}
	workBuffer.setSampleRate(getSourceASBD(inBusNumber).mSampleRate);
	
//	ofxSoundUtils::checkBuffers(output, workBuffer);
	if(inputObject!=nullptr) {
		inputObject->audioOut(workBuffer);
	}

	
	
	abl.fromOf(workBuffer);
	
	
	OSStatus status;
	if(_impl->ctx.processCallback.inputProc) {
		status = (_impl->ctx.processCallback.inputProc)(_impl->ctx.processCallback.inputProcRefCon,
											   ioActionFlags,
											   inTimeStamp,
											   inBusNumber,
											   inNumberFrames,
											   abl.bufferList);
	}
	copyToCircBuffer(abl.bufferList);
//	
//	if(_impl->ctx.bufferMutex.try_lock()) {
//		if(status == noErr) {
//			const size_t buffersToCopy = std::min<size_t>(_impl->ctx.circularBuffers.size(), ioData->mNumberBuffers);
//			
//			for(int i = 0; i < buffersToCopy; i++) {
//				CopyAudioBufferIntoCircularBuffer(&_impl->ctx.circularBuffers[i], ioData->mBuffers[i]);
//			}
//		}
//		_impl->ctx.bufferMutex.unlock();
//	}
	
	
	return noErr;

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
	
	_impl->ctx.sourceBus = sourceBus;
	AURenderCallbackStruct callback = {RenderToOfSoundBuffer, this};
	destination.setRenderCallback(callback, destinationBus);
	return destination;
}
// ----------------------------------------------------------
ofxAudioUnitDSPNode& ofxSoundObjectAudioUnitBridge::connectToAU(ofxAudioUnitDSPNode &destination, int destinationBus , int sourceBus ){
	_impl->ctx.sourceBus = sourceBus;
	AURenderCallbackStruct callback = {RenderToOfSoundBuffer, this};
	destination.setSource(callback);
	return destination;

}
/// this 2 functions will copy the audio data from an ofSoundBuffer to a vector<TPCircularBuffer> or vice-versa.
/// Data is not aranged in the same way on each so it is not possible to memcpy, although there are ways for optimizing.
/// These will allocate memory only when needed
//void copySoundBufferToCircularBuffers(const ofSoundBuffer& sndBuffer, std::shared_ptr<ofxAudioUnitDSPNode::NodeImpl> _impl){
//	imitateSoundBufferToCircularBuffers(sndBuffer, _impl);
//	
//	auto nc = sndBuffer.getNumChannels();
//	auto nf = sndBuffer.getNumFrames();
//	auto & cb = _impl->ctx.circularBuffers;
//	ofSoundBufer tempBuff;
//	for(size_t c = 0; c < nc; c++ ){
//
//	
//	int32_t availableBytesInCircBuffer;
//	TPCircularBufferHead(cb[c], &availableBytesInCircBuffer);
//	
//	auto buffChanNumBytes = sndBuffer.getNumFrames()*sizeof(float);
//	
//	if(availableBytesInCircBuffer < buffChanNumBytes) {
//		TPCircularBufferConsume(cb[c], buffChanNumBytes - availableBytesInCircBuffer);
//	}
//		auto buffChan = sndBuffer.getChannel(tempBuff, c);
//		TPCircularBufferProduceBytes(cb[c], buffChan.getBuffer().data(), buffChanNumBytes);
//	}
////	for(size_t c = 0; c < nc; c++ ){
////		float* b = static_cast<float*>(cb[c].buffer);
////		for(size_t f = 0; f < nf; f++ ){
////			b[f] = sndBuffer[f*nc + c];
////		}
////	}
//	
//
//}
//void copyCircularBuffersToSoundBuffer( std::shared_ptr<ofxAudioUnitDSPNode::NodeImpl> _impl, ofSoundBuffer& sndBuffer){
//	imitateCircularBuffersToSoundBuffer(_impl, sndBuffer);
//	
//	auto nc = sndBuffer.getNumChannels();
//	auto nf = sndBuffer.getNumFrames();
//	auto & cb = _impl->ctx.circularBuffers;
//	for(size_t c = 0; c < nc; c++ ){
//		
//			int32_t circBufferSize;
//			Float32 * circBufferTail = (Float32 *)TPCircularBufferTail(cb[c], &circBufferSize);
////			Float32 * circBufferHead = circBufferTail + (circBufferSize / sizeof(Float32));
//			auto cbs = (circBufferSize / sizeof(Float32));
//			auto framessToCopy = std::min((circBufferSize / sizeof(Float32)), nf);
////			outBuffer.assign(circBufferTail, circBufferHead);
//				for(size_t f = 0; f < framessToCopy; f++ ){
//					
//					sndBuffer[f*nc + c] = circBufferTail[f];
//					
//				}
//		
//		
////
////		float* b = static_cast<float*>(cb[c].buffer);
////		for(size_t f = 0; f < nf; f++ ){
////			sndBuffer[f*nc + c] = b[f];
////		}
//	}
//	
//	
//}
///// this 2 functions will only allocate memory to have at least the same as the source. The data is not copied
//void imitateSoundBufferToCircularBuffers(const ofSoundBuffer& sndBuffer, std::shared_ptr<ofxAudioUnitDSPNode::NodeImpl> _impl){
//	if(_impl->samplesToBuffer  != sndBuffer.getNumFrames() || _impl->channelsToBuffer != sndBuffer.getNumChannels()){
//		_impl->samplesToBuffer = sndBuffer.getNumFrames();
//		_impl->channelsToBuffer = sndBuffer.getNumChannels();
//		_impl->ctx.setCircularBufferSize(sndBuffer.getNumChannels(), sndBuffer.getNumFrames());
//	}
//}
//void imitateCircularBuffersToSoundBuffer(std::shared_ptr<ofxAudioUnitDSPNode::NodeImpl> _impl, ofSoundBuffer& sndBuffer){
//	if(_impl->samplesToBuffer  != sndBuffer.getNumFrames() || _impl->channelsToBuffer != sndBuffer.getNumChannels()){
//		sndBuffer.allocate(_impl->samplesToBuffer,_impl->channelsToBuffer);
//	}
//}
// ----------------------------------------------------------
ofxSoundObjectAudioUnitBridge::ABL::~ABL(){
	dealocate();
}
// ----------------------------------------------------------
void ofxSoundObjectAudioUnitBridge::ABL::dealocate(){
	if( bufferList != NULL){
//		for(int i = 0; i < 2; i++) {
//			free(bufferList->mBuffers[i].mData);
//		}
//		free(bufferList);
		AudioBufferListRelease(bufferList);
		bufferList = NULL;
		this->numChannels =  0;
		this->numFrames  = 0;
	}
}
// ----------------------------------------------------------
void ofxSoundObjectAudioUnitBridge::ABL::set(AudioBufferList *bufferList){
	this->bufferList = bufferList;
	
	this->numChannels =  this->bufferList->mNumberBuffers;
	
	UInt32 dataSize = bufferList->mBuffers[0].mDataByteSize;
	
	this->numFrames  = dataSize / sizeof(Float32);
	
	
	for(UInt32 i = 0; i < bufferList->mNumberBuffers; ++i) {
		if(bufferList->mBuffers[i].mDataByteSize != dataSize){
			ofLogError("ofxSoundObjectAudioUnitBridge::ABL::set") << "wrong data size on buffer " << i;
		}
	}
	
}
// ----------------------------------------------------------
void ofxSoundObjectAudioUnitBridge::ABL::allocate(UInt32 numChannels, UInt32 numFrames){
	
	if( bufferList != NULL && this->numChannels ==  numChannels &&  this->numFrames  == numFrames){ return;}
	
	if( bufferList != NULL){
		dealocate();
	}
	
	bufferList = AudioBufferListAlloc(numChannels, numFrames);
//	bufferList = (AudioBufferList*)malloc(sizeof(AudioBufferList) * numChannels);
	
//	bufferList->mNumberBuffers = numChannels;
	
	this->numChannels =  numChannels;
	this->numFrames  = numFrames;
	
//	UInt32 dataSize = sizeof(Float32) * numFrames;
//	
//	for(UInt32 i = 0; i < bufferList->mNumberBuffers; ++i) {
//		bufferList->mBuffers[i].mData = (Float32*)malloc(dataSize);
//		bufferList->mBuffers[i].mDataByteSize = dataSize;
//		bufferList->mBuffers[i].mNumberChannels = 1;
//	}
}

// ----------------------------------------------------------
void ofxSoundObjectAudioUnitBridge::ABL::toOf(ofSoundBuffer& buffer){
	
	if(bufferList == NULL){
		ofLogError("ofxSoundObjectAudioUnitBridge::ABL::toOf") << "AudioBufferList not allocated!";
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
void ofxSoundObjectAudioUnitBridge::ABL::fromOf(ofSoundBuffer& buffer){
	allocate(buffer.getNumChannels(), buffer.getNumFrames());
	
	for(size_t c = 0; c < numChannels; c++ ){
		float * data = static_cast<float *>(bufferList->mBuffers[c].mData);
		for(size_t f = 0; f < numFrames; f++ ){
			data[f] = buffer[f*numChannels + c];
		}
	}
}
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
void ofxSoundObjectAudioUnitBridge::audioOut(ofSoundBuffer &output){
	
	
	abl.allocate(output.getNumChannels(), output.getNumFrames());
	
	OSStatus status;
	
	AudioUnitRenderActionFlags ioActionFlags;
	AudioTimeStamp inTimeStamp;
	
	if(_impl->ctx.sourceType == NodeSourceUnit && _impl->ctx.sourceUnit->getUnitRef()) {
		status = _impl->ctx.sourceUnit->render(&ioActionFlags, &inTimeStamp, _impl->ctx.sourceBus, output.getNumFrames(), abl.bufferList);
	} else if(_impl->ctx.sourceType == NodeSourceCallback) {
		status = (_impl->ctx.sourceCallback.inputProc)(_impl->ctx.sourceCallback.inputProcRefCon,
												 &ioActionFlags,
												 &inTimeStamp,
												 _impl->ctx.sourceBus,
												 output.getNumFrames(),
												 abl.bufferList);
	} else {
		// if we don't have a source, render silence (or else you'll get an extremely loud
		// buzzing noise when we attempt to render a NULL unit. Ow.)
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
//	if(_impl->ctx.bufferMutex.try_lock()) {
//		if(status == noErr) {
//			const size_t buffersToCopy = std::min<size_t>(_impl->ctx.circularBuffers.size(), abl.bufferList->mNumberBuffers);
//			
//			for(int i = 0; i < buffersToCopy; i++) {
//				CopyAudioBufferIntoCircularBuffer(&_impl->ctx.circularBuffers[i], abl.bufferList->mBuffers[i]);
//			}
//		}
//		_impl->ctx.bufferMutex.unlock();
//	}
	
	abl.toOf(output);
}
