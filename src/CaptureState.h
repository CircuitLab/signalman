//
//  CaptureState.h
//  Signalman
//
//  Created by nulltask on 2013/04/10.
//
//

#ifndef Signalman_CaptureState_h
#define Signalman_CaptureState_h

#include "ofxState.h"
#include "ofxHttpUtils.h"
#include "ofxGifEncoder.h"

class CaptureState : public Apex::ofxState<SharedData> {
  
  string target;
  int cursor;
  string chars[8][8];
  ofVideoGrabber videoGrabber;
  ofxOpenNI openNIDevice;
  ofTrueTypeFont verdana;
  ofxHttpUtils httpUtils;
  ofxGifEncoder encoder;
  vector<ofImage> images;
  
public:
    
  void setup() {
    ofLog(OF_LOG_NOTICE, "capture:start");

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
    
    openNIDevice.setup();
    // openNIDevice.startPlayer("test.oni");
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.addUserGenerator();
    openNIDevice.addHandsGenerator();
    openNIDevice.addAllHandFocusGestures();
    
    verdana.loadFont(ofToDataPath("verdana.ttf"), 64);
  };
  
  void stateEnter() {
    ofLog(OF_LOG_NOTICE, "capture:stateEnter");
    getSharedData().timestamp = ofGetTimestampString();
    openNIDevice.start();
    
    videoGrabber.setVerbose(true);
    videoGrabber.setDeviceID(2);
    videoGrabber.initGrabber(1920, 1080);
    
    cursor = 0;
    target = "YMCA";
    
    httpUtils.start();
    encoder.setup(1080 / 4, 1920 / 4, .25, 256);
    ofAddListener(ofxGifEncoder::OFX_GIF_SAVE_FINISHED, this, &CaptureState::onGifSaved);
  };
  
  void update() {
    ofLog(OF_LOG_VERBOSE, "capture:update");
    openNIDevice.update();
    videoGrabber.grabFrame();
  };
  
  void draw() {
    ofLog(OF_LOG_VERBOSE, "capture:draw");

    ofPushMatrix();
    openNIDevice.drawDebug();
    ofPopMatrix();
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("capture", 15, 15);
    
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
        
        cout << c << ":" << target[cursor] << endl;
        
        if (c == ofToString(target[cursor])) {
          ofImage image;
          image.setFromPixels(videoGrabber.getPixels(), 1920, 1080, OF_IMAGE_COLOR);
          image.rotate90(1);
          images.push_back(image);
          
          ofDirectory::createDirectory(ofToDataPath(getSharedData().timestamp));
          image.saveImage(getSharedData().timestamp + "/" + ofGetTimestampString() + ".jpg");
          
          if (++cursor >= target.length()) {

            vector<ofImage>::iterator it = images.begin();
            while (it != images.end()) {
              it->resize(1080 / 4, 1920 / 4);
              encoder.addFrame(*it, .1f);
              ++it;
            }
            encoder.save(getSharedData().timestamp + ".gif");

            return;
          }
        }
        verdana.drawString("[" + c + "] " + ofToString(lp) + ":" + ofToString(rp), 20, 100);
      }
    }
    
    ofPopMatrix();
  };
  
  void stateExit() {
    ofLog(OF_LOG_NOTICE, "capture:stateExit");
    videoGrabber.close();
  };
  
  void exit() {
    openNIDevice.stop();
  };
  
  string getName() {
    return "capture";
  };
  
private:
  
  void
  handEvent(ofxOpenNIHandEvent & event) {
    ofLogNotice()
      << getHandStatusAsString(event.handStatus)
      << "for hand" << event.id << "from device" << event.deviceID;
  };
  
  void
  keyPressed(int key) {
    switch (key) {
      case ' ': {
        ofImage image;
        image.setFromPixels(videoGrabber.getPixels(), 1920, 1080, OF_IMAGE_COLOR);
        image.rotate90(1);
        images.push_back(image);
        image.saveImage(getSharedData().timestamp + "/" + ofGetTimestampString() + ".jpg");
        break;
      }
      case 's': {
        ofxHttpForm form;
        form.action = "http://localhost:3000/";
        form.method = OFX_HTTP_POST;
        form.addFormField("id", getSharedData().timestamp);
        form.addFile("file", getSharedData().timestamp + ".gif");
        httpUtils.addForm(form);
        break;
      }
      case 'g': {
        vector<ofImage>::iterator it = images.begin();
        while (it != images.end()) {
          it->resize(1080 / 4, 1920 / 4);
          encoder.addFrame(*it, .1f);
          ++it;
        }
        encoder.save(getSharedData().timestamp + ".gif");
        break;
      }
    }
  };
  
  void onResponse(ofxHttpResponse &response) {
    ofLog(OF_LOG_NOTICE, ofToString(response.status));
    ofLog(OF_LOG_NOTICE, ofToString(response.responseBody));
  };
  
  void onGifSaved(string &filename) {
    ofLog(OF_LOG_NOTICE, "gif saved as " + filename);
    
    ofxHttpForm form;
    form.action = "http://localhost:3000/";
    form.method = OFX_HTTP_POST;
    form.addFormField("id", getSharedData().timestamp);
    form.addFile("file", getSharedData().timestamp + ".gif");
    httpUtils.addForm(form);
    
    encoder.reset();
  };
};

#endif
