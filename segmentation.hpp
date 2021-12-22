#ifndef SEGMENTATION_HPP
#define SEGMENTATION_HPP

#include <vector>
#include <set>
#include <opencv2/core.hpp>

enum EvalMethod {
  color,
  featureDistance,
};

struct Pixel {
  size_t x;
  size_t y;
  cv::Vec3b color;
};

struct Edge {
  Pixel a;
  Pixel b;
};

std::vector<Pixel> segment(cv::Mat img, int k, EvalMethod method);

#endif