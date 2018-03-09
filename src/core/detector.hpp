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
#ifndef _CORE_DETECTOR_HPP_
#define _CORE_DETECTOR_HPP_

#include "recorder.hpp"

namespace MotionDetector {
  using ContourVectors = std::vector<std::vector<cv::Point> >;

  class Detector {
    cv::Mat firstFrame;
    Recorder* recorder;

    public:
      Detector(Recorder*);
      void start(bool);

    private:
      cv::Mat processFrame(cv::Mat*);
      void resize(cv::Mat*, cv::Mat*);
      void normalize(cv::Mat*, cv::Mat*);
      ContourVectors detect(cv::Mat*);
      ContourVectors filterContours(ContourVectors);
      void drawBoundaryBoxes(ContourVectors, cv::Mat*);
  };
}

#endif
