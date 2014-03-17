#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableAntiAliasing();
	ofEnableSmoothing();
	ofSetFrameRate(60);

	screenWidth = 1280;
	screenHeight = 960;
	
	guideImage.loadImage("img/LaserableArea2.jpg");
	music.loadSound("../../../Music/Down the Road Edit.aif");
	
	nyanSvg.load("SmashingCat.svg");
	//nyanSvg.load("NyanCat.svg");
	
	previewProjector = false;
	
	projectorFbo.allocate(1024, 768, GL_RGB, 4);
	uiFbo.allocate(screenWidth, screenHeight, GL_RGB, 2); 

	projectorFbo.begin();
	ofSetColor(0);
	ofRect(0,0,1024,768);
	projectorFbo.end();
	
	ofSetColor(255);
	projectorPosition.set(screenWidth/5*2, screenHeight*4/5, screenWidth/5, screenHeight/5);
	
	laserManager.setup(screenWidth, screenHeight);
	//laserManager.connectToEtherdream();
	
	laserManager.renderLaserPreview = true;
	laserManager.showPostTransformPreview = true;

	int panelwidth = 200;
	
	ofxBaseGui::setDefaultWidth(panelwidth);
    ofxBaseGui::setDefaultHeight(16);
	ofxBaseGui::setDefaultSpacing(3);
    ofxBaseGui::setDefaultElementSpacing(3);
	ofxBaseGui::setDefaultElementIndentation(1);
	ofxBaseGui::setDefaultTextPadding(5);
	ofxBaseGui::setUseTTF(true);
	
	ofxBaseGui::loadFont("VeraMoIt.ttf",8, true);
	
	laserGui.setup("LaserManager", "laserSettings.xml");
	laserGui.setPosition(ofPoint(screenWidth+220 - panelwidth - 10,10));
	laserGui.setVisible(true);
	laserGui.add(&laserManager.connectButton);
	laserGui.add(laserManager.parameters);
	
	laserGui.load();
	panels.push_back(&laserGui);
	
	appGui.setup("App");
	appGui.setPosition(ofPoint(screenWidth+220 - panelwidth - 10,10));
	appGui.setVisible(false);
	appGui.add(pipeOrganData.editable.set("Pipe Organ editable", false));
	appGui.add(laserDomePoints.set("laser dome points", false));
	appGui.add(laserOrganPoints.set("laser organ points", false));
	appGui.add(showGuideImage.set("show guide image", true));
	
	redGui.setup("Laser Red", "laserred.xml");
	redGui.add(laserManager.redParams );
    greenGui.setup("Laser Green", "lasergreen.xml");
	greenGui.add(laserManager.greenParams );
    blueGui.setup("Laser Blue", "laserblue.xml");
	blueGui.add(laserManager.blueParams );
    redGui.load();
	greenGui.load();
	blueGui.load();
	redGui.setPosition(ofPoint(10 ,10));
	greenGui.setPosition(ofPoint(10 +panelwidth + 10,10));
	blueGui.setPosition(ofPoint(10 +panelwidth*2 + 20,10));
	
	
	//appGui.load();
	panels.push_back(&appGui);

	
	
	soundPositionMS = 70000;
	
	sync.tempo = 111;
	sync.startPosition = (60000/111) - 5; // start after 1 beat intro

	
	pipeOrganData.load();
	domeData.init();
	domeData.load();
	
	domeData.gui.setPosition(ofPoint(screenWidth+220 - panelwidth - 10,10));
	domeData.gui.setVisible(false);
	
	panels.push_back(&domeData.gui);
	
	effectDomeLines.setDomeData(&domeData);
	effectPipeOrganLines.setObjects(&pipeOrganData, &particleSystemManager);
	effectParticles.setObjects(&pipeOrganData, &domeData);
	
	
	
	smoothVol = 0;
	
	calculateScreenSizes();
	
	}


//--------------------------------------------------------------
void ofApp::update(){
	
	float deltaTime = ofGetLastFrameTime(); 
	ofSoundUpdate();
	laserManager.update();
	if(music.getIsPlaying()) soundPositionMS = music.getPositionMS();

	sync.update(soundPositionMS);
	
	screenAnimation.update();
	
	particleSystemManager.update(deltaTime);
	effectLaserBeams.update(deltaTime);
	effectDomeLines.update(deltaTime);
	effectPipeOrganLines.update(deltaTime);
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	
	
	int numBands = 500;
	float vol = 0;
	
	
	float * val = ofSoundGetSpectrum(numBands);
	
	for(int i = 0;i<numBands; i++) {
		vol+=val[i];
		
	}

	vol/=(float)numBands / 20.0f;
	//cout << vol << endl;
	updatePeakFrequency(val, numBands);
	//ofSetupScreenPerspective(1280, 960,ofGetMouseY());
	
	smoothVol += (vol-smoothVol) *0.5;
	
	
	uiFbo.begin();
	ofSetupScreenPerspective(1280,960,50);

	ofSetColor(0);
	ofFill(); 
	ofRect(0,0,screenWidth, screenHeight);
	ofNoFill();
	ofSetColor(255);
	
	if(!previewProjector) {
		ofSetColor(200);
		if(showGuideImage) guideImage.draw(0,0,screenWidth, screenHeight);
		ofSetColor(255);
		ofRect(projectorPosition.x-1, projectorPosition.y-1, projectorPosition.width+2, projectorPosition.height+2);
		projectorFbo.draw(projectorPosition);
		
		pipeOrganData.draw();
		
		if((pipeOrganData.editable) && (laserOrganPoints)){
			vector<Pipe>& pipes = pipeOrganData.pipes;
			for(int i = 0; i< pipes.size(); i++) {
				Pipe& pipe = pipes[i];
				if((pipe.topDragging) || (pipe.bottomDragging)) {
					laserManager.addLaserLineEased(pipe.top, pipe.bottom, ofColor::white);
				} else {
					laserManager.addLaserLineEased(pipe.top, pipe.bottom, ofColor::cyan);
				} 
			}
		}
		ofSetupScreenPerspective(1280,960,50);

		domeData.draw();
		
		
		if((domeData.editable) && (laserDomePoints)){
			vector<ofPoint> points = domeData.getLevelPoints();
			
			for(int i = 0; i<points.size(); i++) {
				laserManager.addLaserDot(points[i], ofColor :: white);
				//ofSetColor(255,0,0);
				//ofCircle(points[i], 20);
				
			}
		}


	} else {
		ofSetColor(255);
		ofRect(screenWidth/2 - projectorFbo.getWidth()/2 -1, screenHeight/2 - projectorFbo.getHeight()/2-1, projectorFbo.getWidth()+2, projectorFbo.getHeight()+2);

		projectorFbo.draw(screenWidth/2 - projectorFbo.getWidth()/2, screenHeight/2 - projectorFbo.getHeight()/2);
		
	}

	ofDrawBitmapString(ofToString(round(ofGetFrameRate())), 0,10);
	
	numBands = 100;
	int barWidth = (1024/numBands);
	ofFill();
	for(int i = 0; i<numBands; i++) {
		
		float size = val[i] * 100;
		//ofRect(i*barWidth, 768 - size, barWidth-1, size );
		ofRect(i*barWidth, 0, barWidth-1, size );
		
	}
	
	ofSetColor(255,0,0);

	ofRect((numBands+1)*barWidth, 0, barWidth-1, vol*100 );
	ofSetColor(255);
	
	ofNoFill();
	
	float time = soundPositionMS/1000.0f;

	ofDrawBitmapString(ofToString(time), 0,25);
	
	uiFbo.end();

	//----------------- FBO BEGIN --------------------------------
	projectorFbo.begin();
	ofPushStyle();

	ofFill();
	ofSetColor(0);
	ofRect(0,0,1024,768);
	ofSetColor(255);
	
	ofPushMatrix();
	ofTranslate(512,384);
	
	screenAnimation.draw(sync, vol);
	
	ofPopMatrix(); 
	
	ofPopStyle();
	projectorFbo.end();

	//----------------- FBO END --------------------------------

		
	ofPopMatrix();
	
	uiFbo.begin();
	ofSetupScreenPerspective(1280,960,50);

	
	// EFFECTS ---------------------------------------------
	
	drawEffects();
	//laserManager.addLaserSVG(nyanSvg, ofPoint(640,580));
	
	
	laserManager.draw();
	laserManager.renderLaserPreview = !previewProjector; 
	if(!previewProjector) {
		laserManager.renderLaserPath(ofRectangle(0,0,screenWidth, screenHeight), false);
		//laserManager.renderPreview();
	}
	
	ofDrawBitmapString(sync.getString(), 1000,10);
	sync.draw(1100,10);
	
	uiFbo.end();
	uiFbo.draw(0,0);
	
	projectorFbo.draw(secondScreenRect); 
	
	// GUI PANELS
	for(int i = 0; i<panels.size(); i++) {
		panels[i]->draw();
	}
	redGui.draw();
	greenGui.draw();
	blueGui.draw();
	

}

void ofApp :: drawEffects() {
	
	particleSystemManager.draw();
	effectLaserBeams.draw(laserManager,smoothVol);
	effectDomeLines.draw(sync, smoothVol, laserManager);
	effectPipeOrganLines.draw(sync, smoothVol, laserManager, currentPeakFrequency);
	if((sync.currentBar>=24) && (sync.currentBar<32)) {
		if ((sync.barTriggered) && (sync.currentBar%2==0)) effectParticles.makeStarBurst();
		effectPipeOrganLines.mode = 0;
		effectLaserBeams.mode = 0;
		effectDomeLines.mode = 0;
	}
	// SOLO
	if((sync.currentBar >= 32) && (sync.currentBar < 42)) {
		effectPipeOrganLines.mode = 2;
		effectLaserBeams.mode = 0;
		effectDomeLines.mode = 0;
	}
	// POST SOLO BUILD (DOME LINES TURNING)
	if((sync.currentBar >= 42) && (sync.currentBar < 50)) {
		effectPipeOrganLines.mode = 0;
		effectLaserBeams.mode = 0;
		effectDomeLines.mode = 1;
	}
	// MELLOW DROP OUT
	if((sync.currentBar >= 50) && (sync.currentBar < 54)) {
		effectPipeOrganLines.mode = 0;
		effectLaserBeams.mode = 1;
		effectDomeLines.mode = 0;
	}
	if((sync.currentBar >= 54) && (sync.currentBar < 58)) {
		effectPipeOrganLines.mode = 0;
		effectLaserBeams.mode = 2;
		effectDomeLines.mode = 0;
	}
	if((sync.currentBar>58) && (sync.currentBar<80)) {
		if ((sync.barTriggered) && (sync.currentBar%2==0)) effectParticles.makeRainbowBurst();
		effectPipeOrganLines.mode = 0;
		effectLaserBeams.mode = 0;
		effectDomeLines.mode = 0;
	}
	//effectDomeLines.mode = 3;

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	if(key == 'f') ofToggleFullscreen();
	if(key == '\t') {
		
		int activePanelIndex = -1; 
		for(int i =0; i<panels.size(); i++) {
			if(panels[i]->getVisible()) activePanelIndex = i;
			panels[i]->setVisible(false);
			
		}
		if(activePanelIndex ==-1) {
			panels[0]->setVisible(true);
		} else if(activePanelIndex<panels.size()-1) {
			panels[activePanelIndex+1]->setVisible(true);
		}
		
		redGui.setVisible(laserGui.getVisible());
		greenGui.setVisible(laserGui.getVisible());
		blueGui.setVisible(laserGui.getVisible());
			
	}
	if(key == 'w') laserManager.showWarpPoints = !laserManager.showWarpPoints;
	if(key == ' ') previewProjector = !previewProjector;
	if(key == OF_KEY_DOWN) {
		music.setPosition(0);
		music.play();
	}
	if(key == OF_KEY_LEFT) {
		soundPositionMS = sync.getMSForBarNumber(sync.currentBar-1);
		music.setPositionMS(soundPositionMS);
	} else if(key == OF_KEY_RIGHT) {
		soundPositionMS = sync.getMSForBarNumber(sync.currentBar+1);
		music.setPositionMS(soundPositionMS);
	}
	if(key =='l') laserManager.showLaserPath = !laserManager.showLaserPath;
		
		
	
	if(key == 'p') {
		if(music.getIsPlaying()) {
			
			music.stop();
		} else {

			music.play();
			music.setPositionMS(soundPositionMS);
		}
	
	}
	
    

	if(key == '1') {
		effectLaserBeams.mode = 0;
		effectPipeOrganLines.mode = 0;
		effectDomeLines.mode = 1;
	}
	if(key == '2') {
		effectLaserBeams.mode = 0;
		effectPipeOrganLines.mode = 0;
		effectDomeLines.mode = 2;
	}
	if(key == '3') {
		effectLaserBeams.mode = 1;
		effectPipeOrganLines.mode = 0;
		effectDomeLines.mode = 0;
	}
	if(key == '4') {
		effectLaserBeams.mode = 2;
		effectPipeOrganLines.mode = 0;
		effectDomeLines.mode = 0;
	}
	if(key == '5') {
		effectLaserBeams.mode = 0;
		effectPipeOrganLines.mode = 1;
		effectDomeLines.mode = 0;
	}
	if(key == '6') {
		effectLaserBeams.mode = 0;
		effectPipeOrganLines.mode = 2;
		effectDomeLines.mode = 0;
	}
	if(key == '7') {
	
		effectLaserBeams.mode = 0;
		effectPipeOrganLines.mode = 3;
		effectDomeLines.mode = 0;
	}
	
	if(key == '0') {
		effectLaserBeams.mode = 0;
		effectPipeOrganLines.mode = 0;
		effectDomeLines.mode = 0;
	}
	

}


void ofApp :: updatePeakFrequency(float * val, int numBands){

	
	//pipeOrganData.draw();

	//int numPipes = 20;
	//ofRectangle rect(506,631,270,190);

	int bottom = 30;
	int top = 70;
	float threshold = 0.1;
	
	float loudestLevel = threshold;
	int loudestIndex = 0;
	
	
	for(int i = bottom; i<numBands && i<top; i++) {
	
		if(val[i]>loudestLevel) {
			loudestIndex = i;
			loudestLevel = val[i];
		} else {
			// feeble attempt at countering the low end weighting of FFT.
			loudestLevel *=0.95;
		}
	}
	
	if(loudestIndex>0) {
		
		currentPeakFrequency = ofMap(loudestIndex, bottom, top, 0, 1);
		
		//loudestIndex = floor(ofMap(loudestIndex, bottom, top, 0, pipeOrganData.pipes.size()));
		//if(loudestIndex>=pipeOrganData.pipes.size()) loudestIndex = pipeOrganData.pipes.size()-1;
		
		//currentPipeIndex += (loudestIndex-currentPipeIndex) * 0.5;
		
		//if(currentPipeIndex<loudestIndex) currentPipeIndex++;
		//else if(currentPipeIndex>loudestIndex) currentPipeIndex--;
		
		//ofLine(pipeOrganData.pipes[(int)currentPipeIndex].top, pipeOrganData.pipes[(int)currentPipeIndex].bottom);
	} else {
		currentPeakFrequency = -1;
	}
	
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int width, int height){
	
	
	calculateScreenSizes();
}


void ofApp::calculateScreenSizes(){
    int monitorCount;
	
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	
	//cout << "RESIZE" << " " << ofGetWindowMode()<< endl;
   // screens.clear();
    
    int leftMost = 0,
	topMost = 0;
    
	vector<ofRectangle> screens;
	
    for(int i = 0; i < monitorCount; i++){
		
		ofRectangle screen;
		
		int x=0,
		y=0,
		w=0,
		h=0;
		
        glfwGetMonitorPos(monitors[i],&x,&y);
        const GLFWvidmode * desktopMode = glfwGetVideoMode(monitors[i]);
		screen.x = x;
		screen.y = y;
        screen.width = desktopMode->width;
        screen.height = desktopMode->height;
        
        screens.push_back(screen);
        cout << i << " " << screen << endl;
		if( leftMost > screen.x ) leftMost = screen.x;
		if( topMost > screen.y ) topMost = screen.y;
		
    }
    
    for(int i = 0; i < monitorCount; i++){
	//	screens[i].x -= leftMost;
	//	screens[i].y -= topMost;
		
    }
    
	//std::sort( screens.begin(), screens.end(), screenSort );
	
	//uiScreenRect = screens.back();
	
	
	if(screens.size()>1) {
		secondScreenRect = screens[1];
	} else {
		secondScreenRect.set(screens[0].getRight(), screens[0].getTop(), 1024,768);
	}
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

	
	
}


//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int numChannels){
	
		
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
	pipeOrganData.mouseDragged(x,y); 
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	cout << "MOUSE : " << x << " " << y << endl;
	pipeOrganData.mousePressed(x,y); 
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	pipeOrganData.mouseReleased(x,y);

}


//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

void ofApp::exit() {
	redGui.save();
	greenGui.save();
	blueGui.save();
	
	laserGui.save();
	pipeOrganData.save();
	domeData.save();
	laserManager.warp.saveSettings();

}