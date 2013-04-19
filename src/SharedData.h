//
//  SharedData.h
//  Signalman
//
//  Created by nulltask on 2013/04/10.
//
//

#ifndef Signalman_SharedData_h
#define Signalman_SharedData_h

class SharedData {

public:
  
  string timestamp;
  string target;
  ofxOscSender sender;
  ofxOscReceiver receiver;
  map<string, ofImage> guides;
  map<string, ofImage> *captured;
};

#endif
