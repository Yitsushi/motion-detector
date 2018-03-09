/*
 * Original code by Balazs Nadasdi (http://github.com/Yitsushi/)
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 * not claim that you wrote the original software. If you use this
 * software in a product, an acknowledgment in the product documentation
 * would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 */
#include "detector.hpp"
#include "recorder.hpp"

namespace MotionDetector {
  Detector::Detector(Recorder* rec) : recorder(rec) {
    int initFrames = recorder->fps() / 2;

    // Skip first N frames (turnOnDelay causes black screen)
    while(initFrames > 0) {
      recorder->nextFrame();
      initFrames--;
    }
  }

  void Detector::start(bool showOutput) {
    bool hasFirstFrame = false;
    for (;;) {
      cv::Mat frame = processFrame(recorder->nextFrame());
      cv::Mat* normalized = new cv::Mat();
      normalize(&frame, normalized);

      if (!hasFirstFrame) {
        firstFrame = *normalized;
        hasFirstFrame = true;
        recorder->waitKey();
        continue;
      }

      ContourVectors contours = filterContours(detect(normalized));
      if (contours.size() > 0) {
        if (showOutput) {
          drawBoundaryBoxes(contours, &frame);
        }
        recorder->saveCurrentFrame(true);
      }

      if (showOutput) {
        recorder->showImage(&frame);
      }

      char c = recorder->waitKey();
      if (c == 27) break;
    }
  }

  cv::Mat Detector::processFrame(cv::Mat* original) {
    cv::Mat frame;
    resize(original, &frame);
    return frame;
  }

  void Detector::resize(cv::Mat* original, cv::Mat* target) {
    cv::resize(*original, *target, cv::Size(), OUTPUT_RATE, OUTPUT_RATE);
  }

  void Detector::normalize(cv::Mat* original, cv::Mat* target) {
    cv::cvtColor(*original, *target, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(*target, *target, cv::Size(13, 13), 0);
  }

  ContourVectors Detector::detect(cv::Mat* frame) {
    cv::Mat fdelta, thresh;
    cv::absdiff(firstFrame, *frame, fdelta);
    cv::threshold(fdelta, thresh, 30, 255, cv::THRESH_BINARY);

    ContourVectors contours;
    std::vector<cv::Vec4i> hierarchy;
    findContours(thresh, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    return contours;
  }

  ContourVectors Detector::filterContours(ContourVectors contours) {
    ContourVectors validContours;
    for (int i = 0; i < contours.size(); i++) {
      if (cv::contourArea(contours[i]) >= 500) {
        validContours.push_back(contours[i]);
      }
    }

    return validContours;
  }

  void Detector::drawBoundaryBoxes(ContourVectors contours, cv::Mat* frame) {
    for (int i = 0; i < contours.size(); i++) {
      cv::Rect bounding = cv::boundingRect(contours[i]);
      cv::rectangle(*frame, bounding.tl(), bounding.br(), cv::Scalar(255, 0, 0), 2, 8, 0);
    }
  }
}
