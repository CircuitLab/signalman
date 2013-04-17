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
  
  ofImage title;
  // map<string, ofImage> captured;
  // map<string, ofImage>::iterator it;
  
public:
  
  void setup() {
    ofLog(OF_LOG_NOTICE, "share:setup");
    title.loadImage("end.png");
  };
  
  void stateEnter() {
    ofLog(OF_LOG_NOTICE, "share:stateEnter");
    
    // captured = getSharedData().captured;
    // it = captured.begin();
  };
  
  void update() {
    ofLog(OF_LOG_VERBOSE, "share:update");
  };
  
  void draw() {
    ofLog(OF_LOG_VERBOSE, "share:draw");
    
    // it->second.draw(0, 0);
    // ++it;
    // if (it == captured.end()) it = captured.begin();

    ofSetColor(255, 255, 255);
    ofDrawBitmapString("share", 15, 15);
    title.draw(0, 0);
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
