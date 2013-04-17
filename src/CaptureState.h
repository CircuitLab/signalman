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
#define CAMERA_DEVICE_ID 1

class CaptureState : public Apex::ofxState<SharedData> {
  
  string target;
  int flash;
  int cursor;
  char chars[8][8];
  bool isCapturing;
  char capturedChar;
  char nextChar;
  map<string, ofImage> images;
  ofImage logo;
  ofImage guide;
  ofSoundPlayer shutter;
  ofTrueTypeFont courier;
  ofVideoGrabber videoGrabber;
  ofxOpenNI openNIDevice;
  ofxHttpUtils httpUtils;
  ofxGifEncoder encoder;

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
    openNIDevice.setMaxNumUsers(100);
    openNIDevice.addUserGenerator();
    
    logo.loadImage("futuresemaphore_logo.png");
    logo.resize(500, 400);
    
    shutter.loadSound("shutter.wav");
    
    courier.loadFont("Courier New Bold.ttf", 192);
    
    ofAddListener(httpUtils.newResponseEvent, this, &CaptureState::onResponse);
    ofAddListener(ofxGifEncoder::OFX_GIF_SAVE_FINISHED, this, &CaptureState::onGifSaved);
  };
  
  void stateEnter() {
    ofLog(OF_LOG_NOTICE, "capture:stateEnter");
    flash = 0;
    cursor = 0;
    target = getSharedData().target;
    isCapturing = true;

    openNIDevice.start();
    
    videoGrabber.setVerbose(true);
    videoGrabber.setDeviceID(CAMERA_DEVICE_ID);
    videoGrabber.initGrabber(CAMERA_WIDTH, CAMERA_HEIGHT);
    
    httpUtils.start();
    encoder.setup(CAMERA_HEIGHT / 4, CAMERA_WIDTH / 4, .25, 128);
  };
  
  void update() {
    ofLog(OF_LOG_VERBOSE, "capture:update");
    
    openNIDevice.update();
    videoGrabber.grabFrame();
  };
  
  void draw() {
    ofLog(OF_LOG_VERBOSE, "capture:draw");

    // capturedFromCamera = videoGrabber.getTextureReference();
    // capturedFromCamera.setFromPixels(videoGrabber.getPixels(), CAMERA_WITH, CAMERA_HEIGHT, OF_IMAGE_COLOR);
    // capturedFromCamera.resize(192, 108);
    // capturedFromCamera.mirror(false, true);
    // capturedFromCamera.rotate90(3);
    // capturedFromCamera.draw(0, 0);
    videoGrabber.draw(0, 0);
    
    ofPushMatrix();
    openNIDevice.drawDebug(0.5, 0.5);
    ofPopMatrix();
    
    ofEnableAlphaBlending();
    logo.draw(ofGetWidth() - logo.getWidth(), ofGetHeight() - logo.getHeight());
    ofDisableAlphaBlending();
        
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("capture", 15, 15);
        
    if (flash > 0) {
      ofPushStyle();
      ofEnableAlphaBlending();
      ofSetColor(255, 255, 255, flash);
      ofRect(0, 0, ofGetWidth(), ofGetHeight());
      ofDisableAlphaBlending();
      ofPopStyle();
      flash -= 3;
      return;
    }
    
    if (!isCapturing) return;
    
    guide = getSharedData().images[ofToLower(ofToString(target[cursor]))];
    guide.draw(500, 0);
    
    ofPushMatrix();
    
    int numUsers = openNIDevice.getNumTrackedUsers();
    // cout << numUsers << " users found." << endl;
    
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
        flash = 255;
        capturedChar = c;
        
        shutter.play();
        
        ofImage image;
        image.setFromPixels(videoGrabber.getPixels(), CAMERA_WIDTH, CAMERA_HEIGHT, OF_IMAGE_COLOR);
        image.rotate90(1);
        images.insert(map<string, ofImage>::value_type(ofGetTimestampString(), image));
        
        if (++cursor >= target.length()) {
          isCapturing = false;
          map<string, ofImage>::iterator it = images.begin();
          while (it != images.end()) {
            (*it).second.resize(CAMERA_HEIGHT / 4, CAMERA_WIDTH / 4);
            encoder.addFrame((*it).second, .2f);
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
      
      ofPushStyle();
      ofSetColor(255, 255, 255, 192);
      courier.drawString(ofToString(target[cursor]), 200, 200);
      ofPopStyle();
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
    }
  };
  
  void onGifSaved(string &filename) {
    ofLog(OF_LOG_NOTICE, "gif saved as " + filename);
    
    ofxHttpForm form;
    form.action = "http://localhost:3000/plays/" + getSharedData().timestamp + "/pictures";
    form.method = OFX_HTTP_POST;
    form.addFormField("id", getSharedData().timestamp);
    form.addFile("file:gif", getSharedData().timestamp + ".gif");
    
    ofDirectory::createDirectory(ofToDataPath(getSharedData().timestamp));
    map<string, ofImage>::iterator it = images.begin();
    
    while (it != images.end()) {
      string path = getSharedData().timestamp + "/" + (*it).first + ".jpg";
      (*it).second.saveImage(path);
      form.addFile("file:jpg:" + path, path);
      ++it;
    }
     
    httpUtils.addForm(form);
    encoder.reset();
  };
  
  void onResponse(ofxHttpResponse &response) {
    ofLog(OF_LOG_NOTICE, ofToString(response.status));
    ofLog(OF_LOG_NOTICE, ofToString(response.responseBody));
    
    ofxOscMessage message;
    message.setAddress("/captured");
    message.addStringArg(getSharedData().timestamp);
    message.addStringArg(target);
    getSharedData().sender.sendMessage(message);

    changeState("share");
  };
};

#endif
