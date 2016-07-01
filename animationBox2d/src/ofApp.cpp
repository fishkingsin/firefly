#include "ofApp.h"
int gravity = -1;
//--------------------------------------------------------------
void ofApp::setup() {
    svg.load("shape.svg");
    ofSetFrameRate(60);
    ofBackgroundHex(0x00000);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    box2d.init();
    box2d.setGravity(0, gravity);
    box2d.setFPS(30.0);
    
    box2d.registerGrabbing();
    int extend = 100;
    box2d.createBounds(0,-extend*0.5,ofGetWidth(),ofGetHeight()+extend*5);
    particles.setParticleFlag(b2_waterParticle);
    particles.setup(box2d.getWorld());
    
    for (int i = 0; i < 8000; i++) {
        ofVec2f position = ofVec2f(ofRandomWidth(),
                                   ofRandomHeight()*0.2);
        ofVec2f velocity = ofVec2f(0, 0);
        particles.createParticle(position, velocity);
    }
    drawParticle = false;
    
    int density = 10;
    
    //start shape
    
    for (int i = 0; i < svg.getNumPath(); i++){
        ofPtr <ofxBox2dEdge> edge = ofPtr<ofxBox2dEdge>(new ofxBox2dEdge);
        ofPath p = svg.getPathAt(i);
        // svg defaults to non zero winding which doesn't look so good as contours
        p.setPolyWindingMode(OF_POLY_WINDING_ODD);
        vector<ofPolyline> lines = p.getOutline();
        for(int j=0;j<(int)lines.size();j++){
            vector<ofVec3f>points  = lines[j].getVertices();
            for(int k = 0 ; k < points.size() ; k++ )
            {
                edge.get()->addVertex(points[k]);
                
            }
        }
        edge.get()->setFriction(0);
        edge.get()->setBounce(0);
        edge.get()->setDamping(0);
        edge.get()->create(box2d.getWorld());
        edges.push_back(edge);
    }
    
    
    
//    for(int j = 0 ; j < 500 ; j++){
//        circles.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
//        circles.back().get()->setPhysics(0, 0.0, 0);
//        circles.back().get()->setup(box2d.getWorld(), ofRandomWidth(),ofRandomHeight()*0.2  , 10);
//
//    }
    
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::update() {
    box2d.update();
}


//--------------------------------------------------------------
void ofApp::draw() {
    for(int i=0; i<circles.size(); i++) {
        ofFill();
        ofSetHexColor(0xf6c738);
        circles[i].get()->draw();
    }
    
    for(int i=0; i<boxes.size(); i++) {
        ofFill();
        ofSetHexColor(0xBF2545);
        boxes[i].get()->draw();
    }
    ofSetColor(ofColor::white);
    particles.draw();
    if(drawParticle){
        particles.draw();
        ofSetColor(ofColor::white);
        for(int i = 0 ; i < edges.size() ; i++)
        {
            edges[i]->draw();
        }
    }
    
    string info = "";
    info += "Press [c] for circles\n";
    info += "Press [b] for blocks\n";
    info += "Mouse Drag for particles\n";
    info += "Total Bodies: "+ofToString(box2d.getBodyCount())+"\n";
    info += "Total Particles: "+ofToString(particles.getParticleCount())+"\n\n";
    info += "FPS: "+ofToString(ofGetFrameRate(), 1)+"\n";
    ofSetHexColor(0xffffff);
    ofDrawBitmapString(info, 30, 30);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(key == 'c') {
        float r = ofRandom(4, 20);
        circles.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
        circles.back().get()->setPhysics(1, 0.0, 0);
        circles.back().get()->setup(box2d.getWorld(), mouseX, mouseY, r);
    }
    if(key == 'b') {
        float w = ofRandom(20, 40);
        float h = ofRandom(20, 40);
        boxes.push_back(ofPtr<ofxBox2dRect>(new ofxBox2dRect));
        boxes.back().get()->setPhysics(4.0, 0.53, 0.1);
        boxes.back().get()->setup(box2d.getWorld(), mouseX, mouseY, w, h);
    }
    if(key == OF_KEY_UP){
        gravity--;
        box2d.setGravity(0, gravity);
    }
    if(key == OF_KEY_DOWN){
        gravity++;
        box2d.setGravity(0, gravity);
    }
    if(key == 'p'){
        drawParticle = !drawParticle;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    for (int i = 0; i < 20; i++) {
        float radius = 40;
        float x = cos(ofRandom(PI*2.0)) * radius + mouseX;
        float y = sin(ofRandom(PI*2.0)) * radius + mouseY;
        ofVec2f position = ofVec2f(x, y);
        ofVec2f velocity = ofVec2f(ofRandom(-200, 200), ofRandom(-200, 200));
        particles.createParticle(position, velocity);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::resized(int w, int h){
}

