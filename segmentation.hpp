#ifndef SEGMENTATION_HPP
#define SEGMENTATION_HPP

#include <vector>
#include <unordered_map>
#include <opencv2/core.hpp>
#include <algorithm>
#include <stdlib.h>
#include <list>
#include "utils/disjoint-set.hpp"
#include "pixel.hpp"
//main includes
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

enum EvalMethod {
  color,
  featureDistance,
};

struct Edge {
  Pixel a;
  Pixel b;
};

void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method);

#endif