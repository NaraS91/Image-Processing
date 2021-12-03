#include <array>
#include <algorithm>
#include <vector>
#include <tuple>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "../helper.hpp"

class segment {
  public:
    segment(size_t x, size_t y, size_t height, size_t width, cv::Mat3b *img){
      _segment = cv::Mat3b(*img, cv::Range(y, y + height - 1), cv::Range(x, x + width - 1));
    }
    std::vector<size_t> colorHistogram(){
      return utils::colorHistogram(_segment, 25, Height(), Width(), 0, 0);
    }

    //simplified texture histogram with only 2 directions of derivative
    std::vector<size_t> textureHistogram(){
      int bins = 10;
      auto gaussianKernelSize = cv::Size2i(5, 5);
      std::vector<size_t> result;

      cv::Mat3b blured(Height(), Width());
      cv::GaussianBlur(_segment, blured, gaussianKernelSize, 0, 0);

      cv::Mat3b derivated(Height(), Width());
      cv::Mat1f kernels[2] = {
          cv::Mat1f({1., 0., -1.}),
          cv::Mat1f({{-1.}, {0.}, {1.}})};
      for (int i = 0; i < 2; i++){
        cv::filter2D(blured, derivated, -1, kernels[i]);
        auto histogram = utils::colorHistogram(derivated, bins, Height(), Width(), 0, 0);
        result.insert(result.end(), histogram.begin(), histogram.end());
      }

      return result;
    }

    size_t Width(){
      return _segment.size().width;
    }

    size_t Height(){
      return _segment.size().height;
    }

    size_t Size(){
      return Height() * Width();
    }

  private:
    cv::Mat3b _segment;
};