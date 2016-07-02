#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"
#include "ofxSyphon.h"
#include "ofxTween.h"
//#define USE_PROGRAMMABLE_GL

using namespace flowTools;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    // Camera
    ofVideoGrabber		simpleCam;
    
    ftFbo				fbo;
    // Time
    float				lastTime;
    float				deltaTime;
    
    // FlowTools
    int					flowWidth;
    int					flowHeight;
    int					drawWidth;
    int					drawHeight;
    ftOpticalFlow		opticalFlow;
    ftVelocityMask		velocityMask;
    ftFluidSimulation	fluidSimulation;
    ftParticleFlow		particleFlow;
    
    // MouseDraw
    ftDrawMouseForces	mouseForces;
    vector<ftDrawForce*> forceVs;
    void setupForce(ofVec2f position , ofVec2f direction, float radius);
    ofImage				flowToolsLogoImage;
    
    void drawComposite(int _x, int _y, int _width, int _height);
    bool drawText;
    ofImage image;
    
    bool showFbo;
    ofxSyphonServer server;
    
    ofxTween tweenlinear;
    ofxEasingLinear 	easinglinear;
    int extend;
};
