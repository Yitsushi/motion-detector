#include "opencv2/opencv.hpp"
#include <iostream>

#define FPS 12
#define DEBUG false
#define OUTPUT_RATE 0.75

using namespace std;
using namespace cv;


int main() {
  VideoCapture vcap(0);
  if(!vcap.isOpened()) {
    cout << "Error opening video stream or file" << endl;
    return -1;
  }

  int frame_width  = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
  int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);

  VideoWriter video(
    "out.avi",
    CV_FOURCC('M','J','P','G'),
    FPS,
    Size(frame_width * OUTPUT_RATE, frame_height * OUTPUT_RATE),
    true
  );

  bool hasFirstFrame = false;
  Mat firstFrame;

  int initFrames = FPS / 2;

  for(;;){
    Mat original;
    Mat frame;
    Mat normalized;
    Mat fdelta;
    Mat thresh;

    vcap >> original;

    if (initFrames > 0) {
      initFrames--;
      continue;
    }

    // Normalize/Optimize for processing
    resize(original, frame, Size(), OUTPUT_RATE, OUTPUT_RATE);
    cvtColor(frame, normalized, COLOR_BGR2GRAY);
    GaussianBlur(normalized, normalized, Size(13, 13), 0);

    // Save first frame
    if (!hasFirstFrame) {
      if (DEBUG) {
        char c = (char)waitKey(1000/FPS);
        if (c == 32) {
          hasFirstFrame = true;
          firstFrame = normalized;
        }
        imshow("Frame", frame);
      } else {
        hasFirstFrame = true;
        firstFrame = normalized;
      }
      continue;
    }

    absdiff(firstFrame, normalized, fdelta);
    threshold(fdelta, thresh, 30, 255, THRESH_BINARY);

    // Mat *target = &normalized;
    Mat *target = &frame;

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( thresh, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    bool detectedAtLeastOne = false;
    for (int i = 0; i < contours.size(); i++) {
      if (contourArea(contours[i]) < 500) {
        continue;
      }
      if (DEBUG) {
        Rect bounding = boundingRect(contours[i]);
        rectangle(*target, bounding.tl(), bounding.br(), Scalar(255, 0, 0), 2, 8, 0);
      }
      detectedAtLeastOne = true;
    }

    if (detectedAtLeastOne) {
      // Add Timestamp
      time_t now = time(NULL);
      tm * ptm = localtime(&now);
      char buffer[32];
      strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
      putText(*target, buffer, Point(5, target->rows - 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 255));

      if (!DEBUG) {
        video.write(*target);
      }
    }

    if (DEBUG) {
      imshow("Frame", *target);
    }

    char c = (char)waitKey(1000/FPS);
    if (c == 27) break;
  }
  return 0;
}
