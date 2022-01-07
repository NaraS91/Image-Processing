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

namespace std {
  template<>
  struct hash<Pixel>
  {
    std::size_t operator()(const Pixel &p) const
    {
      return 3 * p.x + 7 * p.y * 13 * p.color[0] * 17 * p.color[1] + 19 * p.color[3];
    }
  };

  template<>
  struct hash<utils::Node<Pixel>> 
  {
    std::size_t operator()(const utils::Node<Pixel> &n) const
    {
    return 3 * intptr_t(n.parent) + 7 * hash<Pixel>()(n.elem);
    }
  };
}


struct Edge {
  Pixel a;
  Pixel b;
};

void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method);

#endif