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
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdexcept>
#include "recorder.hpp"

namespace MotionDetector {
  Recorder::Recorder() {}

  void Recorder::openCamera(int index) {
    vcap = new cv::VideoCapture(index);

    if(!vcap->isOpened()) {
      throw std::runtime_error("Error opening video stream or file");
    }
  }

  int Recorder::fps() {
    int fps = vcap->get(CV_CAP_PROP_FPS);
    if (fps < 1) {
      std::cout << "Cam doesn't support ioctl for getting fps" << std::endl;
      fps = DEFAULT_FPS;
    }

    return fps;
  }

  void Recorder::openWriter(std::string filename, int fourCC) {
    cv::Size targetSize(
        vcap->get(CV_CAP_PROP_FRAME_WIDTH), //* OUTPUT_RATE,
        vcap->get(CV_CAP_PROP_FRAME_HEIGHT) //* OUTPUT_RATE
    );

    writer = new cv::VideoWriter(
        filename,
        fourCC,
        fps() / 2,
        targetSize,
        true
    );
  }

  cv::Mat* Recorder::nextFrame() {
    *vcap >> currentFrame;

    return &currentFrame;
  }

  void Recorder::showImage() {
    cv::imshow("Frame", currentFrame);
  }

  void Recorder::showImage(cv::Mat* frame) {
    cv::imshow("Frame", *frame);
  }

  char Recorder::waitKey() {
    return (char)cv::waitKey(1000/fps());
  }

  void Recorder::saveCurrentFrame(bool withTimestamp) {
    if (withTimestamp) {
      // Add Timestamp
      time_t now = time(NULL);
      tm * ptm = localtime(&now);
      char buffer[32];
      strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
      cv::putText(
          currentFrame,
          buffer,
          cv::Point(5, currentFrame.rows - 5),
          cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255)
      );
    }

    writer->write(currentFrame);
  }
}
