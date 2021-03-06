#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    fps = 60;
    ofSetFrameRate(fps);
    receiver.setup(PORT);
    
    current_msg_string = 0;
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    
    server.setName("Video Delay");
   
    
#if USE_EDSDK
    camera.setup();
#else
    vector<ofVideoDevice> devices = videoGrabber.listDevices();
    
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }
    
    
    videoGrabber.setDeviceID(0);
    
    videoGrabber.initGrabber(640,480);
    ofPixels pixels;
    pixels.allocate(videoGrabber.getWidth(), videoGrabber.getHeight(), OF_PIXELS_RGBA);
    rgbaFboFloat.readToPixels(pixels);
#endif
#ifdef TARGET_OPENGLES
    rgbaFboFloat.allocate(640, 480, GL_RGBA ); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
    ofLogWarning("ofApp") << "GL_RGBA32F_ARB is not available for OPENGLES.  Using RGBA.";
#else
    //    rgbaFboFloat.allocate(640, 480, GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
#endif
    //    rgbaFboFloat.begin();
    //    ofClear(0,0,0, 255);
    //    rgbaFboFloat.end();
    
    
    
    
    
    
    
    if(ofIsGLProgrammableRenderer()){
        string vertex = "#version 150\n\
        \n\
        uniform mat4 projectionMatrix;\n\
        uniform mat4 modelViewMatrix;\n\
        uniform mat4 modelViewProjectionMatrix;\n\
        \n\
        \n\
        in vec4  position;\n\
        in vec2  texcoord;\n\
        \n\
        out vec2 texCoordVarying;\n\
        \n\
        void main()\n\
        {\n\
        texCoordVarying = texcoord;\
        gl_Position = modelViewProjectionMatrix * position;\n\
        }";
        string fragment = "#version 150\n\
        \n\
        uniform sampler2DRect tex0;\
        uniform sampler2DRect maskTex;\
        uniform float filter = 0.5f;\
        in vec2 texCoordVarying;\n\
        \
        out vec4 fragColor;\n\
        void main (void){\
        vec2 pos = texCoordVarying;\
        \
        vec3 src = texture(tex0, pos).rgb;\
        float mask = texture(maskTex, pos).r;\
        \
        fragColor = vec4( src , mask);\
        }";
        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
        shader.bindDefaults();
        shader.linkProgram();
    }else{
        string shaderProgram = "#version 120\n \
        #extension GL_ARB_texture_rectangle : enable\n \
        \
        uniform sampler2DRect tex0;\
        uniform sampler2DRect maskTex;\
        uniform float filter = 0.5f;\
        \
        void main (void){\
        vec2 pos = gl_TexCoord[0].st;\
        \
        vec3 src = texture2DRect(tex0, pos).rgb;\
        float mask = (src.g+src.r+src.b)/3.0f;\
        \
        gl_FragColor = vec4( src , (mask>filter)?mask:0.0f);\
        }";
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
        shader.linkProgram();
    }
    filter = 0.9;
    //setup our directory
    
}

void ofApp::exit() {
#if USE_EDSDK
    camera.close();
#endif
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        // check for mouse moved message
        if(m.getAddress() == "/fade"){
            fadeAmnt = m.getArgAsInt(0);
        }
        if(m.getAddress() == "/filter"){
            filter = m.getArgAsFloat(0);
        }
    }
    // check for waiting messages
    
    ofEnableAlphaBlending();
#if USE_EDSDK
    //lets draw some graphics into our two fbos
    camera.update();
    
    
    if(camera.isFrameNew()) {
        
        ofPixels pixels = camera.getLivePixels();
        if(!videoTexture.isAllocated()){
            videoTexture.allocate(camera.getWidth(), camera.getHeight(), OF_PIXELS_RGBA);
            rgbaFboFloat.allocate(camera.getWidth(), camera.getHeight(), GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
            rgbaFboFloat.begin();
            ofClear(0,0,0, 255);
            rgbaFboFloat.end();
            
        }
        videoTexture.loadData(pixels);
        
        
    }
#else
    {
        
        videoGrabber.update();
        
        if(videoGrabber.isFrameNew()){
            ofPixels & pixels = videoGrabber.getPixels();
            if(!videoTexture.isAllocated()){
                videoTexture.allocate(videoGrabber.getWidth(), videoGrabber.getHeight(), OF_PIXELS_RGBA);
                rgbaFboFloat.allocate(videoGrabber.getWidth(), videoGrabber.getHeight(), GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
                rgbaFboFloat.begin();
                ofClear(0,0,0, 255);
                rgbaFboFloat.end();
            }
            videoTexture.loadData(pixels);
        }
        
    }
#endif
    if(rgbaFboFloat.isAllocated()){
        rgbaFboFloat.begin();
        
        if(ofGetKeyPressed('1')){
            fadeAmnt = 0;
        }else if(ofGetKeyPressed('2')){
            fadeAmnt = 1;
        }else if(ofGetKeyPressed('3')){
            fadeAmnt = 2;
        }
        else if(ofGetKeyPressed('4')){
            fadeAmnt = 256;
        }
        ofSetColor(0, fadeAmnt);
        ofDrawRectangle(0, 0, rgbaFboFloat.getWidth(), rgbaFboFloat.getHeight());
        shader.begin();
        shader.setUniform1f("filter", filter);
        ofSetColor(255);
        videoTexture.draw(0, 0);
        shader.end();
        rgbaFboFloat.end();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    if(rgbaFboFloat.isAllocated()){
        rgbaFboFloat.draw(0,0,ofGetWidth(), ofGetHeight());
        //        videoTexture.draw(0,0, 320,240);
    }
    server.publishScreen();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'f'){
        ofToggleFullscreen();
    }
    if(key == 's' || key == 'S'){
#ifndef USE_EDSDK
        videoGrabber.videoSettings();
#endif
    }
    
    switch(key){
            case '-':
            fps = (fps>0)?fps-1:fps;
            ofSetFrameRate(fps);
            break;
        case '=':
            fps++;
            ofSetFrameRate(fps);
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
