#pragma once

#include "ofApp.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"


class ofApp : public ofBaseApp {

public:
  
  void setup();
  void update();
  void draw();
  
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseEntered(int x, int y);
  void mouseExited(int x, int y);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);
  
  ofVideoPlayer     eyeVideo;
  bool              frameByframe;
  bool bHide;
  int morphologySize; // 使用したカーネルのサイズ
  
  cv::Mat createMask(float sx, float sy, int width, int height, double centerX, double centerY);
  cv::Mat mask;
  cv::Mat eyeMat, gray_eyeMat, eyeOnly;
  cv::Mat thre_img, Iteration_img;
  cv::Mat element;
  
  ofxPanel gui;
  ofxVec2Slider sigma;
  ofxVec2Slider center;
  ofxFloatSlider threnum;
  ofxIntSlider morphologyIteration, elementnum;

  
};
