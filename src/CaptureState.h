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

#define CAMERA_WIDTH 1920
#define CAMERA_HEIGHT 1080

class CaptureState : public Apex::ofxState<SharedData> {
  
  string target;
  int cursor;
  char chars[8][8];
  ofVideoGrabber videoGrabber;
  ofxOpenNI openNIDevice;
  ofxHttpUtils httpUtils;
  ofxGifEncoder encoder;
  vector<ofImage> images;
  unsigned char * mirrored;
  bool isCapturing;
  char capturedChar;
  char nextChar;

public:
    
  void setup() {
    ofLog(OF_LOG_NOTICE, "capture:start");

    chars[4][4] = ' ';
    chars[5][4] = 'A';
    chars[6][4] = 'B';
    chars[7][4] = 'C';
    chars[0][4] = 'D';
    chars[4][1] = 'E';
    chars[4][2] = 'F';
    chars[4][3] = 'G';
    chars[6][5] = 'H';
    chars[7][5] = 'I';
    chars[0][2] = 'J';
    chars[5][0] = 'K';
    chars[5][1] = 'L';
    chars[5][2] = 'M';
    chars[5][3] = 'N';
    chars[7][6] = 'O';
    chars[6][0] = 'P';
    chars[6][1] = 'Q';
    chars[6][2] = 'R';
    chars[6][3] = 'S';
    chars[7][0] = 'T';
    chars[7][1] = 'U';
    chars[0][3] = 'V';
    chars[1][2] = 'W';
    chars[1][3] = 'X';
    chars[7][2] = 'Y';
    chars[3][2] = 'Z';
    
    openNIDevice.setup();
    // openNIDevice.startPlayer("test.oni");
    openNIDevice.setSkeletonProfile(XN_SKEL_PROFILE_UPPER);
    openNIDevice.addDepthGenerator();
    openNIDevice.addImageGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.addUserGenerator();
    
    ofAddListener(httpUtils.newResponseEvent, this, &CaptureState::onResponse);
    
    mirrored = new unsigned char[CAMERA_WIDTH * CAMERA_HEIGHT * 3];
  };
  
  void stateEnter() {
    ofLog(OF_LOG_NOTICE, "capture:stateEnter");    
    cursor = 0;
    target = getSharedData().target;
    isCapturing = true;

    openNIDevice.start();
    
    videoGrabber.setVerbose(true);
    videoGrabber.setDeviceID(2);
    videoGrabber.initGrabber(CAMERA_WIDTH, CAMERA_HEIGHT);
    
    httpUtils.start();
    encoder.setup(CAMERA_HEIGHT / 4, CAMERA_WIDTH / 4, .25, 256);
    ofAddListener(ofxGifEncoder::OFX_GIF_SAVE_FINISHED, this, &CaptureState::onGifSaved);
  };
  
  void update() {
    ofLog(OF_LOG_VERBOSE, "capture:update");
    
    openNIDevice.update();
    videoGrabber.grabFrame();
  };
  
  void draw() {
    ofLog(OF_LOG_VERBOSE, "capture:draw");

    // capturedFromCamera = videoGrabber.getTextureReference();
    // capturedFromCamera.setFromPixels(videoGrabber.getPixels(), CAMERA_WITH, 1080, OF_IMAGE_COLOR);
    // capturedFromCamera.resize(192, 108);
    // capturedFromCamera.mirror(false, true);
    // capturedFromCamera.rotate90(3);
    // capturedFromCamera.draw(0, 0);
    videoGrabber.draw(0, 0);
    
    ofPushMatrix();
    openNIDevice.drawDebug(0.5, 0.5);
    ofPopMatrix();
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("capture", 15, 15);
    
    if (!isCapturing) return;
    
    ofPushMatrix();
    
    int numUsers = openNIDevice.getNumTrackedUsers();
    cout << numUsers << " users found." << endl;
    
    for (int i = 0; i < numUsers; i++) {
      ofxOpenNIUser &user = openNIDevice.getTrackedUser(i);
      
      if (!user.isSkeleton()) break;
      
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
      
      if (lp < 0 || rp < 0) break;
      
      char c = chars[lp][rp];
      
      if (c == target[cursor]) {  // captured!
        capturedChar = c;
        
        ofImage image;
        image.setFromPixels(videoGrabber.getPixels(), CAMERA_WIDTH, CAMERA_HEIGHT, OF_IMAGE_COLOR);
        image.rotate90(1);
        images.push_back(image);
        
        ofDirectory::createDirectory(ofToDataPath(getSharedData().timestamp));
        image.saveImage(getSharedData().timestamp + "/" + ofGetTimestampString() + ".jpg");
        
        if (++cursor >= target.length()) {
          isCapturing = false;
          vector<ofImage>::iterator it = images.begin();
          while (it != images.end()) {
            it->resize(CAMERA_HEIGHT / 4, CAMERA_WIDTH / 4);
            encoder.addFrame(*it, .1f);
            ++it;
          }
          encoder.save(getSharedData().timestamp + ".gif");
          return;
        } else {
          ofxOscMessage message;
          message.setAddress("/captured");
          message.addStringArg(ofToString(capturedChar));
          message.addStringArg(ofToString(target[cursor]));
          getSharedData().sender.sendMessage(message);
        }
      }
      
      ofDrawBitmapString("captured: " + ofToString(c), 20, 100);
      ofDrawBitmapString("    next: " + ofToString(target[cursor]), 20, 120);
      ofDrawBitmapString("          " + ofToString(lp) + ":" + ofToString(rp), 20, 80);
    }
    
    ofPopMatrix();
  };
  
  void stateExit() {
    ofLog(OF_LOG_NOTICE, "capture:stateExit");
  };
  
  void exit() {
    openNIDevice.stop();
  };
  
  string getName() {
    return "capture";
  };
  
private:
  
  string captureSemaphore(ofxOpenNIUser & user) {
    
  };
  
  void
  handEvent(ofxOpenNIHandEvent & event) {
    ofLogNotice()
      << getHandStatusAsString(event.handStatus)
      << "for hand" << event.id << "from device" << event.deviceID;
  };
  
  void
  keyPressed(int key) {
    switch (key) {
      case 'r': {
        int numUsers = openNIDevice.getNumTrackedUsers();
        for (int i = 0; i < numUsers; ++i) {
          openNIDevice.resetUserTracking(i);
        }
        break;
      }
      case ' ': {
        ofImage image;
        image.setFromPixels(videoGrabber.getPixels(), CAMERA_WIDTH, CAMERA_HEIGHT, OF_IMAGE_COLOR);
        image.rotate90(1);
        images.push_back(image);
        image.saveImage(getSharedData().timestamp + "/" + ofGetTimestampString() + ".jpg");
        break;
      }
      case 's': {
        ofxHttpForm form;
        form.action = "http://localhost:3000/plays/" + getSharedData().timestamp + "/pictures";
        cout << form.action << endl;
        form.method = OFX_HTTP_POST;
        form.addFormField("id", getSharedData().timestamp);
        form.addFile("file", getSharedData().timestamp + ".gif");
        httpUtils.addForm(form);
        break;
      }
      case 'g': {
        vector<ofImage>::iterator it = images.begin();
        while (it != images.end()) {
          it->resize(CAMERA_HEIGHT / 4, CAMERA_WIDTH / 4);
          encoder.addFrame(*it, .1f);
          ++it;
        }
        encoder.save(getSharedData().timestamp + ".gif");
        break;
      }
    }
  };
  
  void onGifSaved(string &filename) {
    ofLog(OF_LOG_NOTICE, "gif saved as " + filename);
    
    ofxHttpForm form;
    form.action = "http://localhost:3000/plays/" + getSharedData().timestamp + "/pictures";
    // /plays/:timestamp/pictures
    form.method = OFX_HTTP_POST;
    form.addFormField("id", getSharedData().timestamp);
    form.addFile("file", getSharedData().timestamp + ".gif");
    httpUtils.addForm(form);
    
    encoder.reset();
  };
  
  void onResponse(ofxHttpResponse &response) {
    ofLog(OF_LOG_NOTICE, ofToString(response.status));
    ofLog(OF_LOG_NOTICE, ofToString(response.responseBody));
    
    SharedData sharedData = getSharedData();
    
    ofxOscMessage message;
    message.setAddress("/captured");
    message.addStringArg(sharedData.timestamp);
    message.addStringArg(target);
    sharedData.sender.sendMessage(message);

    changeState("share");
  };
};

#endif
