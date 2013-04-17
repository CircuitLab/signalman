
#include "ofApp.h"
#include "ofMain.h"
#include "ofAppGlutWindow.h"

int
main() {
  ofAppGlutWindow window;
  ofSetupOpenGL(&window, 1920, 1080, OF_WINDOW);
  ofRunApp(new ofApp());
}
