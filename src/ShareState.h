//
//  ShareState.h
//  Signalman
//
//  Created by nulltask on 2013/04/10.
//
//

#ifndef Signalman_ShareState_h
#define Signalman_ShareState_h

#include "ofxState.h"

class ShareState : public Apex::ofxState<SharedData> {
  
public:
  
  void setup() {
    ofLog(OF_LOG_NOTICE, "share:setup");
  };
  
  void stateEnter() {
    ofLog(OF_LOG_NOTICE, "share:stateEnter");
  };
  
  void update() {
    ofLog(OF_LOG_VERBOSE, "share:update");
  };
  
  void draw() {
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("share", 15, 15);
    ofLog(OF_LOG_VERBOSE, "share:draw");
  };
  
  void stateExit() {
    ofLog(OF_LOG_NOTICE, "share:stateExit");
  };
  
  string getName() {
    return "share";
  };
  
private:
  
};

#endif
