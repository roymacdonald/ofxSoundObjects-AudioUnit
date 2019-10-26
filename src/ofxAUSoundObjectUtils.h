//
//  ofxAUSoundObjectUtils.h
//  example
//
//  Created by Roy Macdonald on 10/26/19.
//
//

#pragma once
#include "ofxSoundObject.h"
#include "ofxAudioUnit.h"

namespace ofxSoundUtils{
	void getSignalChainString(ofxSoundObject * startPtr, string &chain);
	void getSignalChainString(ofxAudioUnit * startPtr, string &chain);
	void getSignalChainString(ofxAudioUnitDSPNode * startPtr, string &chain);
}


//--------------------------------------------------------------
void ofxSoundUtils::getSignalChainString(ofxAudioUnit * startPtr, string &chain){
	if(startPtr){
		if(chain.empty()){
			chain = startPtr->getName();
		}else{
			chain = startPtr->getName() + " --> " + chain;
		}
		
		
		
		
		if(startPtr->getSourceAU()){
			getSignalChainString(startPtr->getSourceAU(), chain);
		}else if(startPtr->getSourceDSPNode()){
			getSignalChainString(startPtr->getSourceDSPNode(), chain);
			
		}else{
			auto i = dynamic_cast<ofxSoundObjectAudioUnitBridge*>(startPtr);
			if(i){
				if(i->getInputObject()){
					getSignalChainString(i->getInputObject(), chain);
				}
			}
		}
	}
}
//--------------------------------------------------------------
void ofxSoundUtils::getSignalChainString(ofxAudioUnitDSPNode * startPtr, string &chain){
	if(startPtr){
		if(chain.empty()){
			chain = startPtr->getName();
		}else{
			chain = startPtr->getName() + " --> " + chain;
		}
		
		
		
		if(startPtr->getSourceAU()){
			getSignalChainString(startPtr->getSourceAU(), chain);
		}else if(startPtr->getSourceDSPNode()){
			getSignalChainString(startPtr->getSourceDSPNode(), chain);
			
		}else{
			auto i = dynamic_cast<ofxSoundObjectAudioUnitBridge*>(startPtr);
			if(i){
				if(i->getInputObject()){
					getSignalChainString(i->getInputObject(), chain);
				}
			}
		}
	}
	
}
//--------------------------------------------------------------
void ofxSoundUtils::getSignalChainString(ofxSoundObject * startPtr, string &chain){
	
	if(startPtr){
		if(chain.empty()){
			chain = startPtr->getName();
		}else{
			chain = startPtr->getName() + " --> " + chain;
		}
		
		
		
		if(startPtr->getInputObject()){
			getSignalChainString(startPtr->getInputObject(), chain);
		}else{
			auto i = dynamic_cast<ofxSoundObjectAudioUnitBridge*>(startPtr);
			if(i){
				auto au = i->getSourceAU();
				if(au){
					getSignalChainString(au, chain);
				}else{
					auto dsp = i->getSourceDSPNode();
					if(dsp){
						getSignalChainString(dsp, chain);
					}
				}
			}
		}
	}
	
	
	
	
	
}
