#pragma once

#include "ofApp.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"

using namespace cv;

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
  bool              bHide;
  int morphologySize; // 使用したカーネルのサイズ
  
  Mat eyeMat, mask, gray_eyeMat, blackEyeMat, thre_img, Iteration_img, Iteration_img2, circle_img;
  Mat createMask(float sx, float sy, int width, int height, double centerX, double centerY);
  tuple<Mat, vector<cv::Point>> processImage(Mat img);
  vector<cv::Point> eyeCenters;
  
  ofxPanel gui;
  ofxVec2Slider sigma, center;
  ofxFloatSlider threnum;
  ofxIntSlider morphologyIteration, elementnum;
  
};
