#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofBackground(255,255,255);
  ofSetVerticalSync(true);
  frameByframe = false;
  
  gui.setup(); // most of the time you don't need a name
  
  gui.add(sigma.setup("sigma", ofVec2f(100, 100), ofVec2f(0, 0), ofVec2f(100, 100)));
  gui.add(center.setup("center", ofVec2f(100, 100), ofVec2f(0, 0), ofVec2f(100, 100)));
  
  bHide = false;
  
  // Uncomment this to show movies with alpha channels
  // fingerMovie.setPixelFormat(OF_PIXELS_RGBA);
  
  eyeVideo.load("movies/MyMovie2017-12-05-16-35-55-017.mp4");
  eyeVideo.setLoopState(OF_LOOP_NORMAL);

  eyeVideo.play();
}

//--------------------------------------------------------------
void ofApp::update(){
  eyeVideo.update();
  
  cv::Mat eyeMat = ofxCv::toCv(eyeVideo);
  // グレースケールに変換する
  cvtColor(eyeMat, gray_eyeMat, CV_RGB2GRAY);
  mask = createMask(sigma->x, sigma->y, gray_eyeMat.cols/2, gray_eyeMat.rows, center->x, center->y);

  cv::add(gray_eyeMat, mask, eyeOnly);
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofSetHexColor(0xFFFFFF);
  
  eyeVideo.draw(20, 20);

  ofxCv::drawMat(mask, 20, 70);
  ofxCv::drawMat(eyeOnly, 20, 120);
  
  ofSetHexColor(0x000000);
  ofPixels & pixels = eyeVideo.getPixels();
  
  if(!bHide){
    gui.draw();
  }
  
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  switch(key){
    case 'h':
      if(key == 'h'){
        bHide = !bHide;
      }
      break;
    case 'f':
      frameByframe=!frameByframe;
      eyeVideo.setPaused(frameByframe);
      break;
    case OF_KEY_LEFT:
      eyeVideo.previousFrame();
      break;
    case OF_KEY_RIGHT:
      eyeVideo.nextFrame();
      break;
    case '0':
      eyeVideo.firstFrame();
      break;
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
  
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
  if(!frameByframe){
    int width = ofGetWidth();
    float pct = (float)x / (float)width;
    float speed = (2 * pct - 1) * 5.0f;
    eyeVideo.setSpeed(speed);
  }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  if(!frameByframe){
    int width = ofGetWidth();
    float pct = (float)x / (float)width;
    eyeVideo.setPosition(pct);
  }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  if(!frameByframe){
    eyeVideo.setPaused(true);
  }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
  if(!frameByframe){
    eyeVideo.setPaused(false);
  }
}
//--------------------------------------------------------------
cv::Mat ofApp::createMask(float sx, float sy, int width, int height, double centerX, double centerY)
{
  cv::Mat eye(height, width, CV_8UC1);
  cv::Mat kernel(height, width, CV_64F);

  // Calculate the normal distribution
  for (int i = 0; i < height; i++)
    {
      for (int j = 0; j < width; j++)
        {
          double x = (double)j - centerX;
          double y = (double)i - centerY;
          double value = (1.0 / (2.0 * PI*pow(sx,2)*pow(sy,2))) * exp( (-0.5) * ( pow(x, 2) / pow(sx, 2) + pow(y, 2) / pow(sy, 2)));
          kernel.at<double>(i, j) = value;
          }
     }

  // Normalize
  cv::normalize(kernel, eye, 0, 255, cv::NORM_MINMAX, CV_8UC1);
  cv::hconcat(eye, eye, eye);
  
  // Invert color
  cv::bitwise_not(eye, eye);
  return eye;
}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
  
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
  
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
  
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
  
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
  
}
