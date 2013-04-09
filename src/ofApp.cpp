
#include "ofApp.h"

string chars[8][8];

void
ofApp::setup() {
  chars[4][4] = " ";
  chars[5][4] = "A";
  chars[6][4] = "B";
  chars[7][4] = "C";
  chars[0][4] = "D";
  chars[4][1] = "E";
  chars[4][2] = "F";
  chars[4][3] = "G";
  chars[6][5] = "H";
  chars[7][5] = "I";
  chars[0][2] = "J";
  chars[5][0] = "K";
  chars[5][1] = "L";
  chars[5][2] = "M";
  chars[5][3] = "N";
  chars[7][6] = "O";
  chars[6][0] = "P";
  chars[6][1] = "Q";
  chars[6][2] = "R";
  chars[6][3] = "S";
  chars[7][0] = "T";
  chars[7][1] = "U";
  chars[0][3] = "V";
  chars[1][2] = "W";
  chars[1][3] = "X";
  chars[7][2] = "Y";
  chars[3][2] = "Z";
  
  ofSetLogLevel(OF_LOG_VERBOSE);
  
  openNIDevice.setup();
  // openNIDevice.startPlayer("test.oni");
  openNIDevice.addImageGenerator();
  openNIDevice.addDepthGenerator();
  openNIDevice.setRegister(true);
  openNIDevice.setMirror(true);
  openNIDevice.addUserGenerator();
  openNIDevice.addHandsGenerator();
  openNIDevice.addAllHandFocusGestures();
  openNIDevice.start();

  verdana.loadFont(ofToDataPath("verdana.ttf"), 64);
}

void
ofApp::update() {
  openNIDevice.update();
}

void
ofApp::draw() {
  ofSetColor(255, 255, 255);
  
  ofPushMatrix();
  openNIDevice.drawDebug();
  ofPopMatrix();
  
  ofPushMatrix();
  
  int numUsers = openNIDevice.getNumTrackedUsers();
  
  for (int i = 0; i < numUsers; i++) {
    ofxOpenNIUser &user = openNIDevice.getTrackedUser(i);
    
    ofxOpenNIJoint leftHand = user.getJoint(JOINT_LEFT_HAND);
    ofxOpenNIJoint leftShoulder = user.getJoint(JOINT_LEFT_SHOULDER);
    ofxOpenNIJoint rightHand = user.getJoint(JOINT_RIGHT_HAND);
    ofxOpenNIJoint rightShoulder = user.getJoint(JOINT_RIGHT_SHOULDER);
    
    ofPoint lhp = leftHand.getProjectivePosition();
    ofPoint lsp = leftShoulder.getProjectivePosition();
    ofPoint rhp = rightHand.getProjectivePosition();
    ofPoint rsp = rightShoulder.getProjectivePosition();
    
    float ld = 270 + ofRadToDeg(atan2(lsp.y - lhp.y, lsp.x - lhp.x));
    float rd = 90 + ofRadToDeg(atan2(rhp.y - rsp.y, rhp.x - rsp.x));
  
    int lp = int(ld + 22.5) / 45 % 8;
    int rp = int(rd + 22.5) / 45 % 8;
  
    if (lp >= 0 && rp >= 0) {
      string c = chars[lp][rp];
      verdana.drawString("[" + c + "] " + ofToString(lp) + ":" + ofToString(rp), 20, 100);
    }
  }
    
  ofPopMatrix();
}

void
ofApp::handEvent(ofxOpenNIHandEvent & event) {
  ofLogNotice()
    << getHandStatusAsString(event.handStatus)
    << "for hand" << event.id << "from device" << event.deviceID;
}

void
ofApp::exit() {
  openNIDevice.stop();
}
