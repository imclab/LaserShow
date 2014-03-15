//
//  AnimationSequence.h
//  LaserShow
//
//  Created by Seb Lee-Delisle on 15/03/2014.
//
//

#pragma once 

#include "ofMain.h"
#include "Synchroniser.h"
#include "CurveSquare.h"
#include "ArrowShape.h"
#include "OctoplusShape.h"
#include "SoundShape.h"



class AnimationSequence {
	

	public :
	
	void update();
	void draw(Synchroniser& sync, float volume);
   
    ofTrueTypeFont	proximaNovaL;

	AnimationSequence();
	
	
	
	
};

