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
  
  unsigned int timestamp;
  ofxOscSender sender;
  ofxOscReceiver receiver;
};

#endif
