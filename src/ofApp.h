#ifndef _OF_APP
#define _OF_APP

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxOpenNI.h"
#include "ofxState.h"
#include "ofxStateMachine.h"

#include "SharedData.h"
#include "StartState.h"
#include "CaptureState.h"
#include "ShareState.h"

#define OSC_SEND_HOST "localhost"
#define OSC_SEND_PORT 3486  // node
#define OSC_RECEIVE_PORT 9337 // of

class ofApp : public ofBaseApp {
  
  ofxOscSender sender;
  ofxOscReceiver receiver;
  Apex::ofxStateMachine<SharedData> stateMachine;
  
public:
  
  typedef Apex::ofxStateMachine<SharedData>::StatePtr State;
  
  void
  setup() {
    // ofSetLogLevel(OF_LOG_VERBOSE);
    ofLog(OF_LOG_NOTICE, "main:setup");

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    ofSetFullscreen(true);
    ofHideCursor();
    
    sender.setup(OSC_SEND_HOST, OSC_SEND_PORT);
    receiver.setup(OSC_RECEIVE_PORT);
    
    stateMachine.getSharedData().sender = sender;
    stateMachine.getSharedData().receiver = receiver;
    
    for (char c = 'a'; c <= 'z'; ++c) {
      ofImage image;
      image.loadImage("guides/" + ofToString(c) + ".jpg");
      stateMachine.getSharedData().guides.insert(map<string, ofImage>::value_type(ofToString(c), image));
    }
    
    State startState = stateMachine.addState<StartState>();
    State captureState = stateMachine.addState<CaptureState>();
    State shareState = stateMachine.addState<ShareState>();
        
    stateMachine.changeState("start");
  };
  
  void
  update() {
    ofLog(OF_LOG_VERBOSE, "main:update");
    
    while (receiver.hasWaitingMessages()) {
      ofxOscMessage message;
      receiver.getNextMessage(&message);
      
      ofLog(OF_LOG_NOTICE, "osc received: " + message.getAddress());
      
      if (message.getAddress() == "/states/start") {
        stateMachine.getSharedData().timestamp = message.getArgAsString(0);
        stateMachine.getSharedData().target = message.getArgAsString(1);
        stateMachine.changeState("capture");  // received after sharing.
      } else if (message.getAddress() == "/end") {
        stateMachine.changeState("start");
      }
    }
  };
  
  void
  draw() {
    ofLog(OF_LOG_VERBOSE, "main:draw");
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2) + "fps");
  };
    
  void
  exit() {
    ofLog(OF_LOG_NOTICE, "main:exit");
  };

private:
    
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
      case 'f':
        ofToggleFullscreen();
        break;
      case 's':
        ofShowCursor();
        break;
      case 'h':
        ofHideCursor();
        break;
    }
  };
};

#endif
