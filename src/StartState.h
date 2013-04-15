//
//  StartState.h
//  Signalman
//
//  Created by nulltask on 2013/04/10.
//
//

#ifndef Signalman_StartState_h
#define Signalman_StartState_h

#include "ofxState.h"

class StartState : public Apex::ofxState<SharedData> {

public:
  
  void setup() {
    ofLog(OF_LOG_NOTICE, "start:setup");
  };
  
  void stateEnter() {
    ofLog(OF_LOG_NOTICE, "start:stateEnter");
  };
  
  void update() {
    ofLog(OF_LOG_VERBOSE, "start:update");
  };
  
  void draw() {
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("start", 15, 15);
    ofLog(OF_LOG_VERBOSE, "start:draw");
  };
  
  void stateExit() {
    ofLog(OF_LOG_NOTICE, "start:stateExit");
  };
  
  string getName() {
    return "start";
  };
  
private:
  
};

#endif
