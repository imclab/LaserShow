//
//  AnimationSequence.cpp
//  LaserShow
//
//  Created by Seb Lee-Delisle on 15/03/2014.
//
//

#include "AnimationSequence.h"

void AnimationSequence :: update() {


}


void AnimationSequence:: draw(Synchroniser& sync, float volume) {

	ofPushStyle();
	ofFill();
	ofSetColor(255,0,0);
	
	// pulse vars go from 0 to 1
	ofCircle(100,100, sync.barPulse * 50);
	ofCircle(200,100, sync.beatPulse * 50);
	ofCircle(300,100, sync.eighthPulse * 50);
	ofCircle(400,100, sync.sixteenthPulse * 50);
	
	// get bar / beat / note numbers :
	
	int bar = sync.currentBar;
	int beat = sync.currentBeat;
	int eighth = sync.current8th;
	int sixteenth = sync.current16th;
	
	// get bar as float :
	 
	float barfloat = sync.currentBarFloat;
	ofPopStyle();

}