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

  ofImage title;
    
    ofQTKitGrabber grabber;
  
public:
  
  void setup() {

    ofLog(OF_LOG_NOTICE, "start:setup");
    title.loadImage("tebata_home.png");
  };
  
  void stateEnter() {
    ofLog(OF_LOG_NOTICE, "start:stateEnter");
      grabber.initGrabber(1920, 1280);
  };
  
  void update() {
    ofLog(OF_LOG_VERBOSE, "start:update");
      
    grabber.update();
  };
  
  void draw() {
    ofLog(OF_LOG_VERBOSE, "start:draw");
        
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("start", 15, 15);
    title.draw(0, 0);
      
    
  };
  
  void stateExit() {
    //grabber.close();
    
    ofLog(OF_LOG_NOTICE, "start:stateExit");
  };
  
  string getName() {
    return "start";
  };
  
private:
  
};

#endif
