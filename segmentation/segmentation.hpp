#ifndef SEGMENTATION_HPP
#define SEGMENTATION_HPP

#include <vector>
#include <unordered_map>
#include <opencv2/core.hpp>
#include <algorithm>
#include <stdlib.h>
#include <list>
#include "../utils/disjoint-set.hpp"
#include "pixel.hpp"

enum EvalMethod {
  color,
  featureDistance,
};

struct Edge {
  Pixel a;
  Pixel b;
  int w;
};

//concurrent_edges - number of threads for concurrent creation of edges, default = 1
//parallel_segmentation - if true, colors will be segmented in parallel and the edge vector will be coppied twice
//sigma - for gaussian smoothing
//for reasonably big images parllel segmentation and concurrent creation of edges will slightly outperform the sequential solution
//for small images, concurrent edges can slow down the algorithm, parallel segmentation should be about the same speed
//independent evaluation can be easily run by changing DEBUG definition to true in segmentation.cpp
void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method, double sigma, int concurrent_edges, bool parallel_segmentation);
void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method, double sigma, int concurrent_edges);
void segment(cv::Mat& img, cv::Mat& out, int k, EvalMethod method, double sigma);

#endif