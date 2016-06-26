#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundColor(0);
    ofSetLogLevel(OF_LOG_VERBOSE);
    //setup our directory
    dir.setup();
    //setup our client
    client.setup();
    
    //register for our directory's callbacks
    ofAddListener(dir.events.serverAnnounced, this, &ofApp::serverAnnounced);
    // not yet implemented
    ofAddListener(dir.events.serverUpdated, this, &ofApp::serverUpdated);
    ofAddListener(dir.events.serverRetired, this, &ofApp::serverRetired);
    
    dirIdx = -1;
    
    receiver.setup(PORT);
}

//these are our directory's callbacks
void ofApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

void ofApp::serverUpdated(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Updated")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

void ofApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
    
}
//--------------------------------------------------------------
void ofApp::update(){
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        // check for mouse moved message
        if(m.getAddress() == "/syphon"){
            // both the arguments are int32's
            string serverName = m.getArgAsString(0);
            string applicationName = m.getArgAsString(1);
            for( auto& dir : dir.getServerList() ){
                if(serverName.compare(dir.serverName)==0 && applicationName.compare(dir.appName)){
                    client.set(serverName,applicationName);
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0, 0, 0);
    ofColor(255, 255, 255, 255);
    ofEnableAlphaBlending();
    if(dir.isValidIndex(dirIdx) ){
        client.draw(0, 0,ofGetWidth(),ofGetHeight());
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(key == 'f'){
        ofToggleFullscreen();
    }
    if(key == 'r'){
        if (dir.size() > 0)
        {
            dirIdx++;
            if(dirIdx > dir.size() - 1)
                dirIdx = 0;
            
            client.set(dir.getDescription(dirIdx));
            string serverName = client.getServerName();
            string appName = client.getApplicationName();
            
            if(serverName == ""){
                serverName = "null";
            }
            if(appName == ""){
                appName = "null";
            }
            ofSetWindowTitle(serverName + ":" + appName);
        }
        else
        {
            ofSetWindowTitle("No Server");
        }
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
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
