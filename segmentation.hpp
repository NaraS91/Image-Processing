#ifndef SEGMENTATION_HPP
#define SEGMENTATION_HPP

#include <vector>
#include <unordered_map>
#include <opencv2/core.hpp>
#include <algorithm>
#include "utils/disjoint-set.hpp"

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