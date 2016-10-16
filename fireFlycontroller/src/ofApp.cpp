#include "ofApp.h"

//--------------------------------------------------------------
bool hide = true;
int _count=0;
int step = 10;
void ofApp::setup(){
    ofSetFrameRate(60);
    
    
    // Connect to the fcserver
    opcClient.setup("10.0.1.14", 7890);
//    opcClient.setup("127.0.0.1", 7890);
    
    // You define the stage size and draw into the stage
    opcClient.setupStage(500, 500);
    
    
    stripHand.setupLedStrip(240 , 0, 32, 500/32);
    stripLeg.setupLedStrip(260 , 0, 32, 500/32);
    image.load("arrow-01.png");
    startThread();
    
    c.resize(image.getWidth());
    for(int i = 0 ; i < image.getWidth() ; i++){
        
        for(int j = 0 ; j < image.getHeight() ; j++){
            
            c[i].push_back(image.getColor(i, j));
        }
    }
    
    
}

void ofApp::threadedFunction(){
    while(isThreadRunning()){
        if(lock()){
             opcClient.update();
            // If the client is not connected do not try and send information
            if (!opcClient.isConnected()) {
                // Will continue to try and reconnect to the Pixel Server
                opcClient.tryConnecting();
            }
            else {
                // Write out the first set of data
                for(int i = 0 ; i < image.getWidth() ; i++){
                    vector<ofColor>   sub(&c[i][0],&c[i][31]);
                    vector<ofColor>   sub2(&c[i][32],&c[i][63]);
                    opcClient.writeChannelOne(sub);
                    opcClient.writeChannelTwo(sub);
                    ofSleepMillis(step);
                    
                }
            }
            unlock();
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
//    opcClient.update();
//    for(int i = 0 ; i < 10 ; i ++){
        // Now Draw the effects to the stage
//        opcClient.beginStage();
//        
//        // Draw what you want rendered here
//        int x = (_count%(int)image.getWidth())*2-image.getWidth()*2;
//        _count+=step;
//        _count %=(int)image.getWidth();
//        ofSetColor(255);
//        image.draw(x,0,image.getWidth()*2,image.getHeight());
//        image.draw(x+image.getWidth(),0,image.getWidth()*2,image.getHeight());
//        // For now here are some default effects
//        
//        
//        opcClient.endStage();
//        
//        // New Get Method
//        opcClient.getStagePixels(stripHand.getPixelCoordinates(), stripHand.colors);
//        opcClient.getStagePixels(stripLeg.getPixelCoordinates(), stripLeg.colors);
    
        // If the client is not connected do not try and send information
//        if (!opcClient.isConnected()) {
//            // Will continue to try and reconnect to the Pixel Server
//            opcClient.tryConnecting();
//        }
//        else {
//            // Write out the first set of data
//            for(int i = 0 ; i < image.getWidth() ; i++){
//                vector<ofColor> c ;
//                
//                for(int j = 0 ; j < image.getHeight() ; j++){
//                    
//                    c.push_back(image.getColor(i, j));
//                }
//               opcClient.writeChannelOne(c);
//                ofSleepMillis(10);
//                opcClient.writeChannelTwo(stripLeg.colorData());
//            }
//        }
//    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    opcClient.drawStage(hide);
    
    // Show the grabber area
    stripLeg.drawGrabRegion(hide);
    stripHand.drawGrabRegion(hide);
    
    // Draw the output
    stripHand.draw(opcClient.getStageWidth()+50, 10);
    stripLeg.draw(opcClient.getStageWidth()+100, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == ' ') {
        hide = !hide;
    }
    switch(key){
            case '1':
            if(step>0)step--;
            break;
            case '2':
            step++;
            break;
            
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
//--------------------------------------------------------------
void ofApp::exit()
{
    stopThread();
    // Close Connection
    opcClient.close();
}