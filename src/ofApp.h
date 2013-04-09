#ifndef _OF_APP
#define _OF_APP

#include "ofxOpenNI.h"
#include "ofMain.h"

#define MAX_DEVICES 2

class ofApp : public ofBaseApp {

public:
  void setup();
  void update();
  void draw();
  void exit();

private:
  void handEvent(ofxOpenNIHandEvent & event);    
  ofxOpenNI openNIDevice;
  ofTrueTypeFont verdana;
};

#endif
