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
  int fps;
  map<string, ofImage> *captured;
  map<string, ofImage>::iterator it;
  
public:
  
  void setup() {
    ofLog(OF_LOG_NOTICE, "share:setup");
    
    fps = 5;
    title.loadImage("end.png");
  };
  
  void stateEnter() {
    ofLog(OF_LOG_NOTICE, "share:stateEnter");
    
    captured = getSharedData().captured;
    it = captured->begin();
    
    ofSetFrameRate(fps);
  };
  
  void update() {
    ofLog(OF_LOG_VERBOSE, "share:update");
  };
  
  void draw() {
    ofLog(OF_LOG_VERBOSE, "share:draw");
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("share", 15, 15);
    title.draw(0, 0);
    
    it->second.draw((1980 - 640) / 2, (1080 - 480) / 2 - 50, 640, 480);
    ++it;
    if (it == captured->end()) it = captured->begin();
  };
  
  void stateExit() {
    ofLog(OF_LOG_NOTICE, "share:stateExit");
    
    ofSetFrameRate(60);
  };
  
  string getName() {
    return "share";
  };
  
private:
  
  void
  keyPressed(int key) {
    switch (key) {
      case '<': {
        fps -= 5;
        if (fps <= 0) fps = 5;
        break;
      }
      case '>': {
        fps += 5;
        break;
      }
      case 's': {
        fps = 60;
        break;
      }
    }
    cout << fps << endl;
    ofSetFrameRate(fps);
  };
  
};

#endif
