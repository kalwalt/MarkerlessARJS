/*
---------------------------------------------------------------------
--- Author         : Ahmet Özlü
--- Mail           : ahmetozlu93@gmail.com
--- Date           : 1st August 2017
--- Version        : 1.0
--- OpenCV Version : 2.4.10
--- Demo Video     : https://www.youtube.com/watch?v=nPfR5ACrqu0
---------------------------------------------------------------------
*/

#ifndef EXAMPLE_MARKERLESS_AR_PATTERN_HPP
#define EXAMPLE_MARKERLESS_AR_PATTERN_HPP

// File includes:
#include "GeometryTypes.hpp"
#include "CameraCalibration.hpp"

#include <opencv2/opencv.hpp>

/**
 * Store the image data and computed descriptors of target pattern
 */
struct Pattern
{
  cv::Size                  size;
  cv::Mat                   frame;
  cv::Mat                   grayImg;

  std::vector<cv::KeyPoint> keypoints;
  cv::Mat                   descriptors;

  std::vector<cv::Point2f>  points2d;
  std::vector<cv::Point3f>  points3d;
};

/**
 * Intermediate pattern tracking info structure
 */
struct PatternTrackingInfo
{
  cv::Mat                   homography;
  std::vector<cv::Point2f>  points2d;
  Transformation            pose3d;

  void draw2dContour(cv::Mat& image, cv::Scalar color) const;

  /**
   * Compute pattern pose using PnP algorithm
   */
  void computePose(const Pattern& pattern, const CameraCalibration& calibration);
};

#endif