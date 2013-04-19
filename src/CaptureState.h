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

#define CAMERA_WIDTH 1280
#define CAMERA_HEIGHT 1024

class CaptureState : public Apex::ofxState<SharedData> {
  
  string target;
  int flash;
  int cursor;
  char chars[8][8];
  bool isCapturing;
  char capturedChar;
  char nextChar;
  map<string, ofImage> captured;
  ofImage logo;
  ofImage guide;
  ofSoundPlayer shutter;
  ofTrueTypeFont currentFont;
  ofTrueTypeFont guideFont;
  ofRectangle currentRect;
  ofRectangle guideRect;
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
    chars[5][7] = 'I';
    chars[0][2] = 'J';
    chars[5][0] = 'K';
    chars[5][1] = 'L';
    chars[5][2] = 'M';
    chars[5][3] = 'N';
    chars[6][7] = 'O';
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
    openNIDevice.setResolution(1280, 1024, 30);
    openNIDevice.addDepthGenerator();
    openNIDevice.addImageGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.setMaxNumUsers(100);
    openNIDevice.addUserGenerator();
    
    logo.loadImage("futuresemaphore_logo.png");
    logo.resize(500, 400);
    
    shutter.loadSound("shutter.wav");
    
    currentFont.loadFont("Courier New.ttf", 1024);
    guideFont.loadFont("Courier New Bold.ttf", 192);
    
    ofAddListener(httpUtils.newResponseEvent, this, &CaptureState::onResponse);
    ofAddListener(ofxGifEncoder::OFX_GIF_SAVE_FINISHED, this, &CaptureState::onGifSaved);
  };
  
  void stateEnter() {
    ofLog(OF_LOG_NOTICE, "capture:stateEnter");
    flash = 0;
    cursor = 0;
    target = getSharedData().target;
    captured.clear();
    isCapturing = true;

    openNIDevice.start();
    
    httpUtils.start();
    encoder.setup(CAMERA_WIDTH / 4, CAMERA_HEIGHT / 4, .25, 64);
  };
  
  void update() {
    ofLog(OF_LOG_VERBOSE, "capture:update");
    
    openNIDevice.update();
  };
  
  void draw() {
    ofLog(OF_LOG_VERBOSE, "capture:draw");
    
    // ofPushMatrix();
    // openNIDevice.drawDebug();
    // ofPopMatrix();
    
    openNIDevice.drawImage((1980 - 1440) / 2, 0, 1440, 1080);
    
    ofEnableAlphaBlending();
    logo.draw((1920 - 1440) / 2, -50);
    ofDisableAlphaBlending();
    
    ofPushStyle();
    ofSetColor(192, 192, 192);
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
    
    guide = getSharedData().guides[ofToLower(ofToString(target[cursor]))];
    
    // ofPushStyle();
    // ofSetColor(255, 255, 255);
    // guideRect = guideFont.getStringBoundingBox(ofToString(target[cursor]), 0, 0);
    // guideFont.drawString(ofToString(target[cursor]), ofGetWidth() - guideRect.getWidth(), guideRect.getHeight());
    // ofPopStyle();
    
    ofPushMatrix();
    
    int numUsers = openNIDevice.getNumTrackedUsers();
    
    for (int i = 0; i < numUsers; i++) {
      openNIDevice.drawSkeleton((1980 - 1440) / 2, 0, 1440, 1080, i);
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
      
      if (rd < 0) {
        rd = 360 + rd;
      }
      
      int lp = int(ld + 22.5) / 45 % 8;
      int rp = int(rd + 22.5) / 45 % 8;
      
      ofDrawBitmapString("        L: " + ofToString(lp) + " / " + ofToString(ld), 20, ofGetHeight() - 100);
      ofDrawBitmapString("        R: " + ofToString(rp) + " / " + ofToString(rd), 20, ofGetHeight() - 80);
      
      if (lp < 0 || rp < 0) break;
      
      char c = chars[lp][rp];
      
      if (c == target[cursor]) {  // captured!
        flash = 255;
        capturedChar = c;
        
        shutter.play();
        
        ofImage image;
        image.setFromPixels(openNIDevice.getImagePixels());
        captured.insert(map<string, ofImage>::value_type(ofGetTimestampString() + "-" + ofToString(c), image));
        
        if (++cursor >= target.length()) {
          isCapturing = false;
          map<string, ofImage>::iterator it = captured.begin();
          while (it != captured.end()) {
            (*it).second.resize(CAMERA_WIDTH / 4, CAMERA_HEIGHT / 4);
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
      
      ofDrawBitmapString("captured: " + ofToString(c), 20, ofGetHeight() - 60);
      ofDrawBitmapString("    next: " + ofToString(target[cursor]), 20, ofGetHeight() - 40);
      ofDrawBitmapString("          " + ofToString(lp) + ":" + ofToString(rp), 20, ofGetHeight() - 20);
      
      guide.draw(ofGetWidth() - guide.getWidth(), ofGetHeight() - guide.getHeight());
      
      ofPushStyle();
      ofSetColor(255, 255, 255, 128);
      currentRect = currentFont.getStringBoundingBox(ofToString(c), 0, 0);
      currentFont.drawString(ofToString(c),
        (ofGetWidth() / 2) - (currentRect.getWidth() / 2),
        ofGetHeight() - currentRect.getHeight() / 3 + 50);
      ofPopStyle();
    }
    
    ofPopStyle();
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
    map<string, ofImage>::iterator it = captured.begin();
    
    while (it != captured.end()) {
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
    
    getSharedData().captured = &captured;
    changeState("share");
  };
};

#endif
