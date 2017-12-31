#include "ofApp.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup(){

  ofSetVerticalSync(true);
  frameByframe = false;
  bHide = false;
  
  gui.setup(); // most of the time you don't need a name
  
  // 初期設定
  gui.add(sigma.setup("Sigma (x, y)", ofVec2f(20, 11), ofVec2f(0, 0), ofVec2f(100, 100)));
  gui.add(center.setup("Center (x, y)", ofVec2f(31, 25), ofVec2f(0, 0), ofVec2f(100, 100)));
  gui.add(threnum.setup("Threshold", 128, 0, 255));
  gui.add(elementnum.setup("Element Size", 6, 0, 15));
  gui.add(morphologyIteration.setup("Iteration", 2, 0, 15));
  
  eyeVideo.load("movies/MyMovie.mp4");

  
  eyeVideo.setLoopState(OF_LOOP_NORMAL);
  eyeVideo.play();
}

//--------------------------------------------------------------
void ofApp::update(){
  eyeVideo.update();
  
  eyeMat = toCv(eyeVideo);
  cvtColor(eyeMat, gray_eyeMat, CV_RGB2GRAY);
  mask = createMask(sigma->x, sigma->y, gray_eyeMat.cols/2, gray_eyeMat.rows, center->x, center->y);
  add(gray_eyeMat, mask, blackEyeMat);
  
  threshold(blackEyeMat, thre_img, threnum, 255, 0);
  
  Mat element = getStructuringElement(cv::MORPH_RECT,
                                          cv::Size(2 * elementnum + 1, 2 * elementnum + 1),
                                          cv::Point(elementnum, elementnum));
  
  erode(thre_img, Iteration_img, element, cv::Point(-1, -1), morphologyIteration);
  dilate(Iteration_img, Iteration_img, element, cv::Point(-1, -1), morphologyIteration);
  
  threshold(Iteration_img, Iteration_img2, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);
  
  tie(circle_img, eyeCenters) = processImage(Iteration_img2);
  
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofSetHexColor(0xFFFFFF);
  // 元の動画
  drawMat(eyeMat, 0, 0);
  // マスク処理
  drawMat(mask, 0, eyeMat.rows);
  // 目のみ描写
  drawMat(blackEyeMat, 0, eyeMat.rows+mask.rows);
  // 黒目のみ取得
  drawMat(thre_img, 0, eyeMat.rows+mask.rows+blackEyeMat.rows);
  // 黒目のみ取得（塗りつぶし）
  drawMat(Iteration_img, 0, eyeMat.rows+mask.rows+blackEyeMat.rows+thre_img.rows);
  // 黒目の円周を取得
  drawMat(circle_img, 0, eyeMat.rows+mask.rows+blackEyeMat.rows+thre_img.rows+Iteration_img.rows);
  // 黒目の中心点を抽出
  if(eyeCenters.size() > 0) {
    ofDrawCircle(eyeCenters[0].x, eyeCenters[0].y, 2); // 瞳の中心に点を打つ
    ofDrawCircle(eyeCenters[1].x, eyeCenters[1].y, 2);
    
  }
  
  if(!bHide){
    gui.draw();
  }
}

//--------------------------------------------------------------
Mat ofApp::createMask(float sx, float sy, int width, int height, double centerX, double centerY)
{
  Mat eye(height, width, CV_8UC1);
  Mat kernel(height, width, CV_64F);

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
  normalize(kernel, eye, 0, 255, cv::NORM_MINMAX, CV_8UC1);
  hconcat(eye, eye, eye);
  
  // Invert color
  bitwise_not(eye, eye);
  
  return eye;
}

//--------------------------------------------------------------
tuple<Mat, vector<cv::Point>> ofApp::processImage(Mat img) {
  
  vector<vector<cv::Point>> contours;
  findContours(img, contours, RETR_LIST, CHAIN_APPROX_NONE);
  Mat cimage = Mat::zeros(img.size(), CV_8UC3);
  vector<cv::Point> centers;
  
  for(size_t i = 0; i < contours.size(); i++)
  {
    if (contours[i][0].x == 1 || contours[i][0].y == 1)
      continue;
    
    size_t count = contours[i].size();
    if(count < 6)
      continue;
  
    Mat pointsf;
    Mat(contours[i]).convertTo(pointsf, CV_32F);
    RotatedRect box = fitEllipse(pointsf);
    if( MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height)*30 )
      continue;
    
    centers.push_back(box.center);
    ellipse(cimage, box.center, box.size*0.5f, box.angle, 0, 360, Scalar(0,255,255), 1, CV_AA);
  }
  // 黒目の円周を縁取った線と黒目の中心点を抽出した値を返す
  return forward_as_tuple(cimage, centers);
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
