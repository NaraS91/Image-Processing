#include "helper.hpp"

std::vector<size_t> colorHistogram (const cv::Mat &image, int bins, size_t height,
                               size_t width, size_t x, size_t y){
  std::vector<size_t> hist = std::vector<size_t>(bins * 3, 0);
  int binSize = 255 / bins;
  for (size_t i = x; i < x + height; i++){
    for (size_t j = y; j < y + width; j++){
        cv::Vec3b row = image.at<cv::Vec3b>(i, j);
        hist[row[0] / binSize]++;
        hist[row[1] / binSize + bins]++;
        hist[row[2] / binSize + bins * 2]++;
    }
  }

  return hist;
}