#ifndef HELPER_HPP
#define HELPER_HPP

#include <vector>
#include <opencv2/core.hpp>
namespace utils
{
  std::vector<size_t> colorHistogram (const cv::InputArray &image, int bins,
    size_t height, size_t width, size_t x = 0, size_t y = 0);
}
#endif HELPER_HPP