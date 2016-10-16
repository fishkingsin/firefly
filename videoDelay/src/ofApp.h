#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxEdsdk.h"
#include "ofxSyphon.h"
#define USE_EDSDK 1

using namespace ofxEdsdk;
#define PORT 3333
#define NUM_MSG_STRINGS 20
class ofApp : public ofBaseApp{
    
public:
    void setup();
    void exit();
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
    //32 bits red, 32 bits green, 32 bits blue, from 0 to 1 in 'infinite' steps
    ofFbo rgbaFboFloat; // with alpha
    
    int fadeAmnt;
    
#if USE_EDSDK
    ofxEdsdk::Camera camera;
#else
    ofVideoGrabber videoGrabber;
#endif
    ofTexture videoTexture;
    
    ofShader shader;
    
    ofxOscReceiver receiver;
    
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    ofxSyphonServer server;
    
    float filter;
    int fps;
};
