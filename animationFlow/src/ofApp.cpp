#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    extend = 50;
    drawWidth = ofGetWidth();
    drawHeight = ((int)ofGetHeight());
    // process all but the density on 16th resolution
    flowWidth = drawWidth / 4;
    flowHeight = drawHeight / 4;
    
    // FLOW & MASK
    opticalFlow.setup(flowWidth, flowHeight);
    velocityMask.setup(drawWidth, drawHeight);
    
    
    // FLUID & PARTICLES
    fluidSimulation.setup(flowWidth, flowHeight, drawWidth, drawHeight);
    particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight);
    
    //    flowToolsLogoImage.load("shape.png");
    //    fluidSimulation.addObstacle(flowToolsLogoImage.getTexture());
    
    
    // MOUSE DRAW
    mouseForces.setup(flowWidth, flowHeight, drawWidth, drawHeight);
    
    float foce = 0.07;
    float radius = 150;
    setupForce(ofVec2f(ofGetWidth()*0.25,ofGetHeight()*0.1),    ofVec2f(0,foce), radius);
    setupForce(ofVec2f(50,ofGetHeight()*0.1),                   ofVec2f(foce,0), radius);
    setupForce(ofVec2f(ofGetWidth()*0.45,ofGetHeight()*0.1),    ofVec2f(-foce,0), radius);
    
    setupForce(ofVec2f(ofGetWidth()*0.25,ofGetHeight()*0.3),    ofVec2f(0,foce*1.5), radius);
    setupForce(ofVec2f(100,ofGetHeight()*0.3),                   ofVec2f(foce,0), radius);
    setupForce(ofVec2f(ofGetWidth()*0.45,ofGetHeight()*0.3),    ofVec2f(-foce,0), radius);

    setupForce(ofVec2f(ofGetWidth()*0.25,ofGetHeight()*0.5),    ofVec2f(0,foce*2), radius);
    setupForce(ofVec2f(150,ofGetHeight()*0.5),                   ofVec2f(foce,0), radius);
    setupForce(ofVec2f(ofGetWidth()*0.35,ofGetHeight()*0.5),    ofVec2f(-foce,0), radius);

    setupForce(ofVec2f(ofGetWidth()*0.75,ofGetHeight()*0.1),    ofVec2f(0,foce), radius);
    setupForce(ofVec2f(ofGetWidth()*0.6,ofGetHeight()*0.1),    ofVec2f(foce,0), radius);
    setupForce(ofVec2f(ofGetWidth()-50,ofGetHeight()*0.1),      ofVec2f(-foce,0), radius);
    
    setupForce(ofVec2f(ofGetWidth()*0.75,ofGetHeight()*0.3),    ofVec2f(0,foce*1.5), radius);
    setupForce(ofVec2f(ofGetWidth()*0.6,ofGetHeight()*0.3),    ofVec2f(foce,0), radius);
    setupForce(ofVec2f(ofGetWidth()-50,ofGetHeight()*0.3),      ofVec2f(-foce,0), radius);
    
    setupForce(ofVec2f(ofGetWidth()*0.75,ofGetHeight()*0.5),    ofVec2f(0,foce*2), radius);
    setupForce(ofVec2f(ofGetWidth()*0.6,ofGetHeight()*0.5),    ofVec2f(foce,0), radius);
    setupForce(ofVec2f(ofGetWidth()-150,ofGetHeight()*0.5),      ofVec2f(-foce,0), radius);
    
    
    fbo.allocate(640, 480);
    fbo.black();
    image.load("image.png");
    showFbo = true;
    tweenlinear.setParameters(7,easinglinear,ofxTween::easeOut,0,255,0,0);
}
void ofApp::setupForce(ofVec2f position , ofVec2f direction, float radius){
    ftDrawForce *forceV = new ftDrawForce();
    forceV->setup(flowWidth, flowHeight, ftDrawForceType::FT_VELOCITY, false);
    forceV->setRadius(radius/ofGetWidth());
    forceV->setForce(direction/ofVec2f(ofGetWidth(),ofGetHeight()));
    forceV->applyForce(position/ofVec2f(ofGetWidth(),ofGetHeight()));
    forceVs.push_back(forceV);

    
    server.setName("flow");
    
    
    
}
//--------------------------------------------------------------
void ofApp::update(){
    deltaTime = ofGetElapsedTimef() - lastTime;
    lastTime = ofGetElapsedTimef();
    
    //    simpleCam.update();
    
    //        if (simpleCam.isFrameNew()) {
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    fbo.begin();
    if(drawText){
        
        //        drawText = false;
        ofPushStyle();
        ofSetColor(ofColor::black);
        ofDrawRectangle(0,0,fbo.getWidth(), fbo.getHeight());
        ofEnableAlphaBlending();
        ofSetColor(255,255,255,tweenlinear.update());
        image.draw(0,0,fbo.getWidth(), fbo.getHeight());
        ofDisableAlphaBlending();
        ofPopStyle();
    }
    //draw something
    //            simpleCam.draw(0, 0, fbo.getWidth(), fbo.getHeight());
    fbo.end();
    ofPopStyle();
    
    opticalFlow.setSource(fbo.getTexture());
    
    
     opticalFlow.update();
    // use internal deltatime instead
//    opticalFlow.update();
    
    velocityMask.setDensity(fbo.getTexture());
    velocityMask.setVelocity(opticalFlow.getOpticalFlow());
    velocityMask.update();
    if(drawText){
        particleFlow.addFlowVelocity(fbo.getTexture());
    }
    //        }
    
    
    fluidSimulation.addVelocity(opticalFlow.getOpticalFlowDecay());
    fluidSimulation.addDensity(velocityMask.getColorMask());
    
    fluidSimulation.addTemperature(velocityMask.getLuminanceMask());
    
    mouseForces.update(deltaTime);
    //    forceD.update();
    //    fluidSimulation.addDensity(forceD.getTexture(), forceD.getStrength());
    //    particleFlow.addFlowVelocity(forceD.getTexture(), forceD.getStrength());
    if(drawText ){
        
        fluidSimulation.addDensity(fbo.getTexture());
    }
    for (int i=0; i<forceVs.size(); i++) {
        forceVs[i]->update();
        
        fluidSimulation.addVelocity(forceVs[i]->getTexture(), forceVs[i]->getStrength());
//        fluidSimulation.addDensity(forceVs[i]->getTexture(), forceVs[i]->getStrength());
        
        if(drawText ){
            particleFlow.addFlowVelocity(forceVs[i]->getTexture(), forceVs[i]->getStrength());
//            drawText = false;
            
        }
    }
    //        particleFlow.addFlowVelocity(forceV.getTexture(), forceV.getStrength());
    
    for (int i=0; i<mouseForces.getNumForces(); i++) {
        if (mouseForces.didChange(i)) {
            switch (mouseForces.getType(i)) {
                case FT_DENSITY:
                    fluidSimulation.addDensity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
                    break;
                case FT_VELOCITY:
                    fluidSimulation.addVelocity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
                    particleFlow.addFlowVelocity(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
                    break;
                case FT_TEMPERATURE:
                    fluidSimulation.addTemperature(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
                    break;
                case FT_PRESSURE:
                    fluidSimulation.addPressure(mouseForces.getTextureReference(i), mouseForces.getStrength(i));
                    break;
                case FT_OBSTACLE:
                    fluidSimulation.addTempObstacle(mouseForces.getTextureReference(i));
                default:
                    break;
            }
        }
    }
    
    fluidSimulation.update();
    
    if (particleFlow.isActive()) {
        particleFlow.setSpeed(fluidSimulation.getSpeed());
        particleFlow.setCellSize(fluidSimulation.getCellSize());
        //        particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
        particleFlow.addFluidVelocity(fluidSimulation.getVelocity());
        //		particleFlow.addDensity(fluidSimulation.getDensity());
        particleFlow.setObstacle(fluidSimulation.getObstacle());
    }
    particleFlow.update();
    if(tweenlinear.update()<125){
        drawText = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofEnableBlendMode(OF_BLENDMODE_ADD);

    drawComposite(-extend,-extend,drawWidth+extend*2, drawHeight+extend*2);
    server.publishScreen();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == ' '){
        
        showFbo = !showFbo;
        drawText = !showFbo;
        
        fbo.black();
        opticalFlow.setSource(fbo.getTexture());
        opticalFlow.update();
        
        velocityMask.setDensity(fbo.getTexture());
        velocityMask.update();
        fluidSimulation.addVelocity(opticalFlow.getOpticalFlowDecay());
        fluidSimulation.addDensity(velocityMask.getColorMask());
        fluidSimulation.addTemperature(velocityMask.getLuminanceMask());
        fluidSimulation.update();
        
        unsigned delay = 0;
        unsigned duration = 1000;
        tweenlinear.setParameters(1,easinglinear,ofxTween::easeOut,255,0,duration,delay);

    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void ofApp::drawComposite(int _x, int _y, int _width, int _height) {
    ofPushStyle();
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    fluidSimulation.draw(_x, _y, _width, _height);
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    if (particleFlow.isActive())
        particleFlow.draw(_x, _y, _width, _height);
    
    //    if(showFbo)
    {
        
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255,255,255,tweenlinear.update());
        image.draw(-extend,-extend,drawWidth+extend*2,drawHeight+extend*2);
        ofDisableAlphaBlending();
        ofPopStyle();
    }

    
    ofPopStyle();
}
