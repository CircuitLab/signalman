#ifndef _OF_APP
#define _OF_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxStateMachine.h"

#include "StartState.h"
#include "CaptureState.h"
#include "ShareState.h"

#define MAX_DEVICES 2

class ofApp : public ofBaseApp {

public:
  
  void
  setup() {
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofLog(OF_LOG_VERBOSE, "main:setup");

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    stateMachine.addState(new StartState());
    stateMachine.addState(new CaptureState());
    stateMachine.addState(new ShareState());
    
    stateMachine.changeState("start");
  };
  
  void
  update() {
    ofLog(OF_LOG_VERBOSE, "main:update");
  };
  
  void
  draw() {
    ofLog(OF_LOG_VERBOSE, "main:draw");
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2) + "fps");
  };
    
  void
  exit() {
    
  };

private:
  
  Apex::ofxStateMachine<> stateMachine;
  
  void
  keyPressed(int key) {
    switch (key) {
      case '1':
        stateMachine.changeState("start");
        break;
      case '2':
        stateMachine.changeState("capture");
        break;
      case '3':
        stateMachine.changeState("share");
        break;
    }
  };
};

#endif
